// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: FedParser_Build.cc,v 3.5 2003/02/20 13:49:31 breholee Exp $
// ----------------------------------------------------------------------------

// FedParser Class handles .fed files.
// This cc file concerns file reading and list building.

#include "FedParser.hh"

namespace certi {
namespace fedparser {

static pdCDebug D("CREAD", "(FedParser) - ");

// ----------------------------------------------------------------------------
//! Moves forward one character in file.
/*! advance moves forward one character in the file being read. Instead of
  advancing character by character in the file, a whole piece of
  CREAD_MAX_BUF_LENGTH is read into a buffer. When buffer is fully examined,
  a new piece is read from file.

  Put the next character of FEDFile in CurrentChar.
  Uses InBuffer to temporaly store content of FEDFile.
*/

void
FedParser::advance(void)
    throw (ErrorReadingRID)
{
    if (InBufferPos < InBufferLength) {
        CurrentChar = InBuffer[InBufferPos] ;
        InBufferPos++ ;
        return ;
    }

    FEDFile->read(InBuffer, CREAD_MAX_BUF_LENGTH);

    int LengthRead = FEDFile->gcount();
    if (LengthRead == 0) {
        CurrentChar = ZEOF ;
        return ;
    }

    if (LengthRead > 0) {
        InBufferLength = LengthRead ;
        InBufferPos = 1 ;
        CurrentChar = InBuffer[0] ;
        return ;
    }

    D.Out(pdError, "File error while reading FED file.");
    throw ErrorReadingRID("Negative Length returned by fread on FED file.");
}

// ----------------------------------------------------------------------------
//! FedParser is a class for reading a federate file and creating a structure.
/*! Parameter : a pointer to the federation Root Object. It would better be an
  empty Root Object...
*/
FedParser::FedParser(RootObject *theRootObj)
{
    for (int i=0 ; i < CREAD_MAX_OBJ_COUNT ; i++) {
        ObjStack[i] = NULL ;
        IntStack[i] = NULL ;
        AttStack[i] = NULL ;
        ParStack[i] = NULL ;
    }

    RootObj = theRootObj ;
    ErrorWhileReading = RTI_FALSE ;
    FEDFile = NULL ;

    InBuffer[0] = '\0' ;
    CurrentChar = ' ' ;
    InBufferPos = 0 ;
    InBufferLength = 0 ;

    ObjIndex = 1 ;
    IntIndex = 1 ;
    AttIndex = 1 ;
    ParIndex = 1 ;
    Depth = 1 ;

    TypeStack[Depth] = fedparser::NONE ;

    D.Out(pdInit, "FedParser Object created.");
}

// ----------------------------------------------------------------------------
/*! Closes the file and display tree structure created in RootObject class
  instance.
*/
FedParser::~FedParser(void)
{
    if (FEDFile != NULL)
        FEDFile->close();

#ifdef CREAD_DISPLAYS_ROOTOBJ
    if (ErrorWhileReading == RTI_FALSE)
        RootObj->display();
#endif

    D.Out(pdTerm, "FedParser object destroyed.");
}

// ----------------------------------------------------------------------------
//! Creates a new object which aggregates two objects.
fedparser::Object*
FedParser::makeList(Object *x, Object *y)
    throw (MemoryExhausted)
{
    struct List *p ;

    p = (struct List *) malloc(sizeof(struct List));
    if (p == NULL) {
        D.Out(pdError, "Memory exhausted in MakeList.");
        throw MemoryExhausted("Could not allow List Object.");
    }

    p->type = LIST_TYPE ;
    p->first = x ;
    p->next = y ;
    return((Object *)p);
}

// ----------------------------------------------------------------------------
//! Creates a nil object to detect the end of a list.
fedparser::Object*
FedParser::makeNil(void)
    throw (MemoryExhausted)
{
    struct Nil *p ;

    p = (struct Nil *) malloc(sizeof(struct Nil));
    if (p == NULL) {
        D.Out(pdError, "Memory exhausted in MakeNil.");
        throw MemoryExhausted("Could not allow Nil Object.");
    }

    p->type = NIL_TYPE ;
    return((fedparser::Object *)p);
}

// ----------------------------------------------------------------------------
//! read an atom string until a separator or a brace character is encountered.
fedparser::Object*
FedParser::readAtom(void)
    throw (ErrorReadingRID, MemoryExhausted)
{
    int Length = 0 ;
    char CompString[] = {' ', ZEOF, '\t', '\n', '(', ')', '\0'};
    char TempString[CREAD_MAX_ATOM_LENGTH] ;
    struct Atom *TempAtom ;

    // While CurrentChar not in CompString, read buffer for TempString.
    while (strchr(CompString, CurrentChar) == NULL) {
        TempString[Length] = CurrentChar ;
        Length++ ;
        advance();

        if (Length > CREAD_MAX_ATOM_LENGTH) {
            D.Out(pdError, "TempString too small to store string in readAtom.");
            throw MemoryExhausted("Could not store temporary string in readAtom.");
        }
    }
    TempString[Length] = '\0' ;

    TempAtom = (struct Atom *) malloc(sizeof(struct Atom));
    if (TempAtom == NULL) {
        D.Out(pdError, "Memory Exhausted in ReadAtom.");
        throw MemoryExhausted("Could not allow Atom Object.");
    }

    TempAtom->type = ATOM_TYPE ;
    TempAtom->length = Length+1 ;
    TempAtom->name = strdup(TempString);
    if (TempAtom->name == NULL) {
        D.Out(pdError, "Memory Exhausted in ReadAtom for atom name.");
        throw MemoryExhausted("Could not allow Atom Object.");
    }

    return((fedparser::Object *) TempAtom);
}

// ----------------------------------------------------------------------------
//! Open a file, read data, initialize data and close it.
/*! readFile takes a filename as parameter. This file is used to retrieve data
  used by federate. After opening it, datas are retrieved by the
  readListFirst module and stored by store module.

  Read the 'FedFile' file and build the Root Object tree following the
  FED file content. Call the 'Store' method after a completed file read.
  -- THIS METHOD IS THE READ PART ENTRY POINT --
*/

void
FedParser::readFile(const char *FedFile)
    throw (CouldNotOpenRID, ErrorReadingRID, SecurityError, RTIinternalError)
{
    FEDFile = new ifstream(FedFile, ios::in);

    if (!FEDFile->is_open())
        {
            D.Out(pdError, "Unable to open \"%s\".", FedFile);
            throw CouldNotOpenRID(FedFile);
        }

    try {
        store(readListFirst());
    }
    catch (MemoryExhausted &e) {
        FEDFile->close();
        ErrorWhileReading = RTI_TRUE ;
        D.Out(pdError, "FedParser throwing RTIinternalError.");
        throw RTIinternalError(e._serial, e._reason);
    }
    catch (Exception &e) {
        FEDFile->close();
        ErrorWhileReading = RTI_TRUE ;
        D.Out(pdError, "FedParser throwing exception %s.", e._name);
        if (e._reason != NULL)
            D.Out(pdError, "Exception reason : %s", e._reason);
        throw ErrorReadingRID(e._serial, e._reason);
    }

    FEDFile->close();
    FEDFile = NULL ;
}

// ----------------------------------------------------------------------------
//! called when an opening brace is encountered.
/*! Two objects are built by this module.
  Go deeper in tree structure until a closing brace is encountered.
*/
fedparser::Object*
FedParser::readList(void)
    throw (ErrorReadingRID, MemoryExhausted)
{
    fedparser::Object* object1 ;
    fedparser::Object* object2 ;

    readSeparators();
    if (CurrentChar == ')')
        {
            advance();
            return(makeNil());
        }

    object1 = readListFirst();
    object2 = readList();
    return(makeList(object1, object2));
}

// ----------------------------------------------------------------------------
//! Returns an Object which depends on character being read.
fedparser::Object*
FedParser::readListFirst(void)
    throw (ErrorReadingRID, MemoryExhausted)
{
    readSeparators();

    switch(CurrentChar) {

    case '(':
        advance();
        return(readList());

    case '"':
        advance();
        return(readString());

    case ZEOF:
        throw ErrorReadingRID("Premature EOF.");

    default:
        return(readAtom());
    }
}

// ----------------------------------------------------------------------------
//! advance in file if a comment (;) or a separator is read.
/*! This module advances in the file being read while an empty character or a
  ';' is read. Module returns when another character is read.
*/
void
FedParser::readSeparators(void)
    throw (ErrorReadingRID) // Inherited from advance()
{
    while (true)
        switch(CurrentChar) {

        case ' ': // Separators
        case '\t':
        case '\n':
            advance();
            break ;

        case ';': // Comment line
            skipLine();
            break ;

        default:
            return ;
        }
}

// ----------------------------------------------------------------------------
//! Called when a quote is encountered.
/*! This module is used to read a string when a quote is encountered until an
  unquote is encoutered.
*/
fedparser::Object*
FedParser::readString(void)
    throw (ErrorReadingRID, MemoryExhausted)
{
    int Length = 0 ;
    char TempString[CREAD_MAX_ATOM_LENGTH] ;

    bool end = false ;
    while (!end) {
        switch (CurrentChar) {
        case '"':
            advance();
            end = true ;
            break ;
        case ZEOF:
            TempString[Length] = '\0' ;
            D.Out(pdError, "EOF while reading string \"%s\".", TempString);
            throw ErrorReadingRID("Unexpected EOF while reading string.");
            break ;
        default:
            TempString[Length] = CurrentChar ;
            Length++ ;
            advance();
        }

        if (Length > CREAD_MAX_ATOM_LENGTH) {
            D.Out(pdError, "String too long for ReadString.");
            throw MemoryExhausted("String couldn't be read due to size.");
        }
    }
    TempString[Length] = '\0' ;

    struct String *ResultString = (struct String *) malloc(sizeof(struct String));

    if (ResultString == NULL) {
        D.Out(pdError, "Memory Exhausted in ReadString.");
        throw MemoryExhausted("Could not allow String Object.");
    }

    ResultString->type = STRING_TYPE ;
    ResultString->length = Length+1 ;
    ResultString->name = strdup(TempString);
    if (ResultString->name == NULL) {
        D.Out(pdError, "Memory Exhausted in ReadString for string name.");
        throw MemoryExhausted("Could not allow String Object.");
    }

    return((fedparser::Object *)ResultString);
}

// ----------------------------------------------------------------------------
//! Called when a comment (semicolon) is encountered.
/*! A semicolom marks a comment. A comment ends when a newline is encountered.
 */
void
FedParser::skipLine(void)
    throw (ErrorReadingRID) // Inherited from advance()
{
    while (true) {
        switch(CurrentChar) {
        case '\n':
            advance();
            return ;
            break ;
        case ZEOF:
            return ;
            break ;
        default:
            advance();
        }
    }
}

}}

// $Id: FedParser_Build.cc,v 3.5 2003/02/20 13:49:31 breholee Exp $
