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
// $Id: FedParser.cc,v 3.0 2003/06/27 13:44:45 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "FedParser.hh"

#include "PrettyDebug.hh"
#include "ObjectClassAttribute.hh"

#include <fstream>
#include <iostream>

using std::ifstream ;
using std::ios ;
using std::cout ;
using std::endl ;
using std::vector ;

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
FedParser::advance()
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
    RootObj = theRootObj ;
    ErrorWhileReading = RTI_FALSE ;
    FEDFile = NULL ;

    InBuffer[0] = '\0' ;
    CurrentChar = ' ' ;
    InBufferPos = 0 ;
    InBufferLength = 0 ;

    Depth = 1 ;

    TypeStack[Depth] = fedparser::NONE ;

    D.Out(pdInit, "FedParser Object created.");
}

// ----------------------------------------------------------------------------
/*! Closes the file and display tree structure created in RootObject class
  instance.
*/
FedParser::~FedParser()
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
FedParser::makeNil()
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
FedParser::readAtom()
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
            throw MemoryExhausted("Could not store tmp string in readAtom.");
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

    return (fedparser::Object *) TempAtom ;
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
FedParser::readList()
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
FedParser::readListFirst()
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
FedParser::readSeparators()
    throw (ErrorReadingRID) // Inherited from advance()
{
    while (true) {
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
}

// ----------------------------------------------------------------------------
//! Called when a quote is encountered.
/*! This module is used to read a string when a quote is encountered until an
  unquote is encoutered.
*/
fedparser::Object*
FedParser::readString()
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

    struct String *result = (struct String *) malloc(sizeof(struct String));

    if (result == NULL) {
        D.Out(pdError, "Memory Exhausted in ReadString.");
        throw MemoryExhausted("Could not allow String Object.");
    }

    result->type = STRING_TYPE ;
    result->length = Length+1 ;
    result->name = strdup(TempString);
    if (result->name == NULL) {
        D.Out(pdError, "Memory Exhausted in ReadString for string name.");
        throw MemoryExhausted("Could not allow String Object.");
    }

    return (fedparser::Object *) result ;
}

// ----------------------------------------------------------------------------
//! Called when a comment (semicolon) is encountered.
/*! A semicolom marks a comment. A comment ends when a newline is encountered.
 */
void
FedParser::skipLine()
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

// ----------------------------------------------------------------------------
//! Allocate, initialize and register(add to RootObj tree) new Object Class.
void
FedParser::allocateAndRegisterObjectClass()
    throw (RTIinternalError)
{
    objStack.push_back(new ObjectClass);

    objStack.back()->setHandle(objStack.size());
    objStack.back()->Depth = Depth ;

    RootObj->ObjectClasses->addClass(objStack.back());
}

// ----------------------------------------------------------------------------
/*! Allocate, initialize and register (add to RootObj tree) new Interaction
  Class.
*/
void
FedParser::allocateAndRegisterInteractionClass()
    throw (RTIinternalError)
{
    intStack.push_back(new Interaction);

    intStack.back()->handle = intStack.size();
    intStack.back()->depth = Depth ;

    RootObj->Interactions->addClass(intStack.back());
}

// ----------------------------------------------------------------------------
//! Frees the allocated memory for object.
/*! Atom and String objects store a name. In these case, memory allocated to
  object name is freed. Object memory is freed.
*/
void
FedParser::freeObject(Object *x)
{
    switch(x->type) {
      case ATOM_TYPE: {
          Atom *y = (Atom *) x ;
          if (y->name != NULL) {
              free(y->name);
              y->name = NULL ;
          }
          break ;
      }
      case STRING_TYPE: {
          String *y = (String *) x ;
          if (y->name != NULL) {
              free(y->name);
              y->name = NULL ;
          }
          break ;
      }
    }

    free(x);
}

// ----------------------------------------------------------------------------
/*! Return the last registered Object Class handle whose Depth is equal to
  (Depth-1).
*/
ObjectClass *
FedParser::findObjectParent() const
    throw (RTIinternalError)
{
    // The parent class of the current Interaction is the last Interaction
    // class whose Depth attribute is equal to(current Depth minus one).
    vector<ObjectClass *>::const_reverse_iterator i ;
    for (i = objStack.rbegin(); i != objStack.rend(); i++) {
        if ((*i)->Depth == (Depth - 1))
            return *i ;
    }

    D.Out(pdError, "Parent Class not found.");
    throw RTIinternalError("Parent Class not found.");
}

// ----------------------------------------------------------------------------
/*! Return the last registered Interaction class handle whose Depth is equal
  to (Depth-1).
*/
Interaction *
FedParser::findInteractionParent() const
    throw (RTIinternalError)
{
    // The parent class of the current Interaction is the last Interaction
    // class whose Depth attribute is equal to(current Depth minus one).
    vector<Interaction *>::const_reverse_iterator i ;
    for (i = intStack.rbegin(); i != intStack.rend(); i++) {
        if ((*i)->depth == (Depth - 1))
            return *i ;
    }

    D.Out(pdError, "Interaction Parent Class not found.");
    throw RTIinternalError("Parent Class not found.");
}

// ----------------------------------------------------------------------------
/*! When a 'attribute' atom is encountered, it means that a new object class
  attribute definition is starting. This function process this event.
*/
void
FedParser::processAttributeAtom(Atom *)
    throw (RTIinternalError)
{
    attStack.push_back(new ObjectClassAttribute());

    attStack.back()->setHandle(objStack.back()->addAttribute(attStack.back()));

    D.Out(pdRegister, "Adding new attribute %u to ObjectClass %u.",
          attStack.back()->getHandle(), objStack.back()->getHandle());

    TypeStack[Depth] = ATTRIB ;
}

// ----------------------------------------------------------------------------
//! Called by storeAtom to process class atom.
/*! When a 'class' atom is encountered, it means that a new object or
  interaction class definition is starting. This function process this kind
  of event.
*/
void
FedParser::processClassAtom(Atom *)
    throw (RTIinternalError)
{
    switch(TypeStack[Depth - 1]) {
      case OBJ: // Root Object Class(no parent class)
        D.Out(pdRegister, "Allocating Root Object Class.");
        allocateAndRegisterObjectClass();

        if (objStack.size() >= CREAD_MAX_OBJ_COUNT) {
            D.Out(pdError, "Maximum Object Class count reached.");
            throw RTIinternalError("Maximum Object Class count reached in FED");
        }

        TypeStack[Depth] = CLASSOBJ ;
        break ;

      case INT: // Root Interaction Class(no parent class)
        D.Out(pdRegister, "Allocating Root Interaction Class.");
        allocateAndRegisterInteractionClass();

        if (intStack.size() >= CREAD_MAX_OBJ_COUNT) {
            D.Out(pdError, "Maximum Interaction Class count reached.");
            throw RTIinternalError("Maximum Inter. Class count reached in FED");
        }

        TypeStack[Depth] = CLASSINT ;
        break ;

      case CLASSOBJ: { // Object Class(with a parent class)
          D.Out(pdRegister, "Allocating Child Object Class.");
          allocateAndRegisterObjectClass();

          if (objStack.size() >= CREAD_MAX_OBJ_COUNT) {
              D.Out(pdError, "Maximum Object Class count reached.");
              throw RTIinternalError("Maximum Object Class count reached");
          }

          // The parent Class is the last one with a Depth attribute equal to
          //(the current Depth - 1)

          ObjectClass * oc = findObjectParent();

          // Build the Parent-Child relation(mutual register, copy attributes)
          RootObj->ObjectClasses->buildParentRelation(objStack.back(), oc);

          TypeStack[Depth] = CLASSOBJ ;
      }
        break ;

      case CLASSINT: { // Interaction Class(with parent)
          D.Out(pdRegister, "Allocating Child Interaction Class.");
          allocateAndRegisterInteractionClass();

          // The Parent Interaction Class is the last Interaction Class whose
          // Depth is equal to(current Depth minus one)
          Interaction * inter = findInteractionParent();

          // Build the Parent-Child relation(mutual register, copy attributes)
          RootObj->Interactions->buildParentRelation(intStack.back(), inter);

          if (intStack.size() >= CREAD_MAX_OBJ_COUNT) {
              D.Out(pdError, "Maximum Interaction Class count reached.");
              throw RTIinternalError("Maximum Interaction Class count reached");
          }

          TypeStack[Depth] = CLASSINT ;
      }
        break ;

      default:
        D.Out(pdError, "Unknown type ProcessClassAtom's switch.");
        throw RTIinternalError("Unknown type ProcessClassAtom's switch.");
        break ;
    }
}

// ----------------------------------------------------------------------------
/*! Process a 'federate' list token, containing a Federate Name and a Security
  Level associated with it. This method is called twice, for the Federate
  Name string and then for the Federate Level string.
*/
void
FedParser::processFederateString(String *x)
    throw (RTIinternalError)
{
    SecurityLevelID LevelID ;

    switch(TypeStack[Depth]) {

      case FEDERATE_NAME:
        // Check and store federate name
        if (strlen(x->name) > MAX_FEDERATE_NAME_LENGTH) {
            cout << "Federate Name too long in FED file : " << x->name << endl ;
            throw RTIinternalError("Federate Name too long in FED file.");
        }

        strcpy(FederateNameBuffer, x->name);

        // Wait for federate level string
        TypeStack[Depth] = FEDERATE_LEVEL ;
        break ;

      case FEDERATE_LEVEL:
        // 1- Translate string into SecurityLevelID
        LevelID = RootObj->GetSecurityLevelID(x->name);

        // 2- Register Federate to Security Server
        RootObj->registerFederate(FederateNameBuffer, LevelID);

        TypeStack[Depth] = NONE ;
        break ;

      default:
        throw RTIinternalError("Unknown TypeStack value in ProcessFederateSt.");
    }

}

// ----------------------------------------------------------------------------
/*! When a 'parameter' atom is encountered, it means that a new interaction
  parameter definition is starting. This function process this event.
*/
void
FedParser::processParameterAtom(Atom *)
    throw (RTIinternalError)
{
    parStack.push_back(new Parameter);

    parStack.back()->Handle = intStack.back()->addParameter(parStack.back());

    D.Out(pdRegister, "Adding new parameter %u to Interaction %u.",
          parStack.back()->Handle, intStack.back()->handle);

    TypeStack[Depth] = PARAM ;
}

// ----------------------------------------------------------------------------
//! Process security level atoms.
void
FedParser::processSecLevelAtom(Atom *)
    throw (SecurityError,
           RTIinternalError)
{
    TypeStack[Depth] = SECLEVEL ;
}

// ----------------------------------------------------------------------------
//! Guess whose security level the string describes, and set it correctly.
void
FedParser::processSecLevelString(String *x)
    throw (SecurityError,
           RTIinternalError)
{
    SecurityLevelID LevelID ;

    // 1- Translate string into SecurityLevelID
    LevelID = RootObj->GetSecurityLevelID(x->name);

    // 2- Is it for an object class or an Interaction Class
    switch(TypeStack[Depth - 1]) {

      case OBJ:
      case CLASSOBJ:
        D.Out(pdTrace, "Setting SecLevel of ObjClass %d to %d.",
              objStack.back()->getHandle(), LevelID);
        objStack.back()->setLevelId(LevelID);
        break ;

      case INT:
      case CLASSINT:
        D.Out(pdTrace, "Setting SecLevel of IntClass %d to %d.",
              intStack.back()->handle, LevelID);
        intStack.back()->setLevelId(LevelID);
        break ;

      default:
        D.Out(pdError, "Should set security level for unknowm type %d.",
              TypeStack[Depth - 1]);
        throw RTIinternalError("Unknown Type for Security Level in FED file.");
    }

}

// ----------------------------------------------------------------------------
//! Process FED_RELIABLE etc. atoms, related to Transport or Order type.
void
FedParser::processTransportOrderAtom(Atom *x)
    throw (RTIinternalError)
{
    if (TypeStack[Depth] == ATTRIB) {

        if (strcmp(x->name, FED_STR_RELIABLE) == 0)
            attStack.back()->Transport = RELIABLE ;

        else if (strcmp(x->name, FED_STR_BESTEFFORT) == 0)
            attStack.back()->Transport = BEST_EFFORT ;

        else if (strcmp(x->name, FED_STR_RECEIVE) == 0)
            attStack.back()->Order = RECEIVE ;

        else if (strcmp(x->name, FED_STR_TIMESTAMP) == 0)
            attStack.back()->Order = TIMESTAMP ;

        else {
            D.Out(pdError,
                  "Unknown atom string %s in StoreAtom's switch.", x->name);
            throw RTIinternalError("Unknown Atom string in FED file.");
        }
    } // TypeStack[Depth] == ATTRIB

    else if (TypeStack[Depth] == CLASSINT) {

        if (strcmp(x->name, FED_STR_RELIABLE) == 0)
            intStack.back()->transport = RELIABLE ;

        else if (strcmp(x->name, FED_STR_BESTEFFORT) == 0)
            intStack.back()->transport = BEST_EFFORT ;

        else if (strcmp(x->name, FED_STR_RECEIVE) == 0)
            intStack.back()->order = RECEIVE ;

        else if (strcmp(x->name, FED_STR_TIMESTAMP) == 0)
            intStack.back()->order = TIMESTAMP ;

        else {
            D.Out(pdError,
                  "Unknown atom string %s in StoreAtom's switch.", x->name);
            throw RTIinternalError("Unknown Atom string in FED file.");
        }
    } // TypeStack[Depth] == CLASSINT

    else {
        D.Out(pdError,
              "Unknown atom string %s in StoreAtom's switch.", x->name);
        throw RTIinternalError("Unknown Atom string in FED file.");
    }
}

// ----------------------------------------------------------------------------
//! Entry point for creating object instances (called by readFile module).
/*! Transfer the Object tree pointed by Root into a RootObj tree. It calls
  storeObject(Root) to start the recursive storage process.
  -- THIS METHOD IS THE STORE PART ENTRY POINT --
*/
void
FedParser::store(Object *Root)
    throw (SecurityError, RTIinternalError)
{
    storeObject(Root, "");
    cout << endl ;
}

// ----------------------------------------------------------------------------
//! Recognize atom name and store type or process atom.
void
FedParser::storeAtom(Atom *x)
    throw (SecurityError, RTIinternalError)
{

    D.Out(pdDebug, "Storing Atom %s.", x->name);
    cout << ' ' << x->name ;

    if (strcmp(x->name, FED_STR_FED) == 0)
        TypeStack[Depth] = FED ;

    else if (strcmp(x->name, FED_STR_OBJECTS) == 0)
        TypeStack[Depth] = OBJ ;

    else if (strcmp(x->name, FED_STR_INTERACTIONS) == 0)
        TypeStack[Depth] = INT ;

    else if (strcmp(x->name, FED_STR_CLASS) == 0)
        processClassAtom(x);

    else if (strcmp(x->name, FED_STR_ATTRIBUTE) == 0)
        processAttributeAtom(x);

    else if (strcmp(x->name, FED_STR_PARAMETER) == 0)
        processParameterAtom(x);

    else if (strcmp(x->name, FED_STR_SECLEVEL) == 0)
        processSecLevelAtom(x);

    else if (strcmp(x->name, FED_STR_FEDERATE) == 0)
        TypeStack[Depth] = FEDERATE_NAME ;

    else
        processTransportOrderAtom(x);
}

// ----------------------------------------------------------------------------
/*! The main recursive part is here.
  Header is used to print trace information.
*/
void FedParser::storeList(List *x, const char *Header)
    throw (SecurityError,
           RTIinternalError)
{
    Object *next ;

    storeObject(x->first, Header);

    next = x->next ;
    // Next can be either a Nil object(then the list end is reached)
    // a another list(if there are remaining list elements).

    if (next->type == NIL_TYPE) {
        Depth-- ;
        cout << "]" ;
    }
    // There are remaining elements in the list.
    else if (next->type == LIST_TYPE)
        storeList((struct List *)next, Header);

    else {
        D.Out(pdError, "Illegal type for next in List Object.");
        throw RTIinternalError("Illegal type for next in List Object.");
    }

    FedParser::freeObject(next);
}

// ----------------------------------------------------------------------------
//! storeObject calls the right store module depending on object type.
/*! Call one of the Store* method depending on 'x' type.
  Header is used to print trace information.
*/
void
FedParser::storeObject(Object *x, const char *Header)
    throw (SecurityError, RTIinternalError)
{
    switch(x->type) {
      case LIST_TYPE: {
          cout << endl << Header << '[' ;
          Depth++ ;

          // Increment Header
          char *NewHeader ;
          NewHeader = (char *) malloc(strlen(Header) + 4);
          if (NewHeader == NULL)
              storeList((struct List *) x, Header);
          else {
              strcpy(NewHeader, Header);
              strcat(NewHeader, " ");
              storeList((struct List *) x, NewHeader);
              free(NewHeader);
          }
      }
        break ;

      case ATOM_TYPE:
        storeAtom((struct Atom *) x);
        break ;
      case STRING_TYPE:
        storeString((struct String *) x);
        break ;
      case NIL_TYPE:
        cout << Header << "[]" << endl ;
        break ;
    }

    FedParser::freeObject(x);
}

// ----------------------------------------------------------------------------
void
FedParser::storeString(String *x)
    throw (SecurityError, RTIinternalError)
{

    D.Out(pdDebug, "Storing String %s.", x->name);

    cout << " \"" << x->name << '\"' ;

    try {
        switch(TypeStack[Depth]) {

          case OBJ:
          case CLASSOBJ:
            objStack.back()->setName(x->name);
            break ;

          case INT:
          case CLASSINT:
            intStack.back()->setName(x->name);
            break ;

          case ATTRIB:
            attStack.back()->setName(x->name);
            break ;

          case PARAM:
            parStack.back()->setName(x->name);
            break ;

          case SECLEVEL:
            processSecLevelString(x);
            break ;

          case FEDERATE_NAME:
          case FEDERATE_LEVEL:
            processFederateString(x);
            break ;

          default:
            D.Out(pdError, "Unknown type in StoreString's switch.");
            throw RTIinternalError("CREAD: Unknown type in StoreString");
        }
    }
    catch (ValueLengthExceeded) {
        cout << endl << endl << "Name Token too long : " << x->name << '.'
             << endl << "You must shorten your string or increase the "
             << "MAX_USER_TAG_LENGTH constant." << endl ;
        throw RTIinternalError("Name too long.");
    }
}

}}

// $Id: FedParser.cc,v 3.0 2003/06/27 13:44:45 breholee Exp $
