// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002  ONERA
//
// This file is part of CERTI-libcerti
//
// CERTI-libcerti is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libcerti is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: FedParser_Build.cc,v 3.1 2002/12/11 00:47:33 breholee Exp $
// ---------------------------------------------------------------------------

// Classe FedParser chargee de lire les fichiers .fed, partie lecture
// du fichier et construction de la liste.

#include <config.h>

#include "FedParser.hh"

namespace certi {
namespace fedparser {

static pdCDebug D("CREAD", "(FedParser   ) - ");

// ---------------------------------------------------------------------------
// advance
//
void FedParser::advance()
  throw(ErrorReadingRID)
{
  int LengthRead;
  if(InBufferPos < InBufferLength)
    {
      CurrentChar = InBuffer [InBufferPos];
      InBufferPos ++;
      return;
    }

  LengthRead = fread(InBuffer, 1, CREAD_MAX_BUF_LENGTH, FEDFile);

  if(LengthRead == 0)
    {
      CurrentChar = ZEOF;
      return;
    }

  if(LengthRead > 0)
    {
      InBufferLength = LengthRead;
      InBufferPos    = 1;
      CurrentChar    = InBuffer[0];
      return;
    }

  D.Out(pdError, "File error while reading FED file.");
  throw ErrorReadingRID("Negative Length returned by fread on FED file.");
}


// ---------------------------------------------------------------------------
// FedParser
// 
FedParser::FedParser(RootObject *theRootObj)
{
  for(int i=0; i < CREAD_MAX_OBJ_COUNT; i++)
    {
      ObjStack [i] = NULL;
      IntStack [i] = NULL;
      AttStack [i] = NULL;
      ParStack [i] = NULL;
    }

  RootObj           = theRootObj;
  ErrorWhileReading = RTI_FALSE;
  FEDFile           = NULL;

  InBuffer[0]    = '\0';
  CurrentChar    = ' ';
  InBufferPos    = 0;
  InBufferLength = 0;

  ObjIndex  = 1;
  IntIndex  = 1;
  AttIndex  = 1;
  ParIndex  = 1;
  Depth     = 1;

  TypeStack [Depth] = fedparser::NONE;

  D.Out(pdInit, "FedParser Object created.");
}


// ---------------------------------------------------------------------------
// ~FedParser
// 
FedParser::~FedParser()
{
  if(FEDFile != NULL)
    fclose(FEDFile);

#ifdef CREAD_DISPLAYS_ROOTOBJ
  if(ErrorWhileReading == RTI_FALSE)
    RootObj->display();
#endif

  D.Out(pdTerm, "FedParser object destroyed.");
}


// ---------------------------------------------------------------------------
// makeList
// 
fedparser::Object *FedParser::makeList(Object *x, Object *y)
  throw(MemoryExhausted)
{
  struct List *p;

  p =(struct List *) malloc(sizeof(struct List));
  if(p == NULL) {
    D.Out(pdError, "Memory exhausted in MakeList.");
    throw MemoryExhausted("Could not allow List Object.");
  }

  p->type  = LIST_TYPE;
  p->first = x;
  p->next  = y;
  return((Object *)p);
}


// ---------------------------------------------------------------------------
// makeNil
// 
fedparser::Object *FedParser::makeNil()
  throw(MemoryExhausted)
{
  struct Nil *p;

  p =(struct Nil *) malloc(sizeof(struct Nil));
  if(p == NULL) {
    D.Out(pdError, "Memory exhausted in MakeNil.");
    throw MemoryExhausted("Could not allow Nil Object.");
  }

  p->type = NIL_TYPE;
  return((fedparser::Object *)p);
}


// ---------------------------------------------------------------------------
// readAtom
// 
fedparser::Object *FedParser::readAtom()
  throw(ErrorReadingRID,
	MemoryExhausted)
{
  int Length = 0;
  char CompString [] = {' ', ZEOF, '\t', '\n', '(', ')', '\0'}; 
  char TempString [CREAD_MAX_ATOM_LENGTH];
  struct Atom *TempAtom;

  // While CurrentChar not in CompString, read buffer for TempString.
  while(strchr(CompString, CurrentChar) == NULL) {
    TempString[Length] = CurrentChar;
    Length++;
    advance();
  }
    
  TempString [Length] = '\0';

  TempAtom           =(struct Atom *) malloc(sizeof(struct Atom));
  if(TempAtom == NULL) {
    D.Out(pdError, "Memory Exhausted in ReadAtom.");
    throw MemoryExhausted("Could not allow Atom Object.");
  }

  TempAtom->type   = ATOM_TYPE;
  TempAtom->length = Length+1;
  TempAtom->name   =(char *) malloc(Length+1);
  if(TempAtom->name == NULL) {
    D.Out(pdError, "Memory Exhausted in ReadAtom for atom name.");
    throw MemoryExhausted("Could not allow Atom Object.");
  }
  strcpy(TempAtom->name,TempString);

  return((fedparser::Object *) TempAtom);
}


// ---------------------------------------------------------------------------
// readFile
// 
void FedParser::readFile(char *FedFile)
  throw(CouldNotOpenRID,
	ErrorReadingRID,
	SecurityError,
	RTIinternalError)
{
  FEDFile = fopen(FedFile, "r");
  
  if(FEDFile == NULL)
    {
      D.Out(pdError, "Unable to open \"%s\".", FedFile);
      throw CouldNotOpenRID(FedFile);
    }

  try {
    store(readListFirst());
  }
  catch(MemoryExhausted &e) {
    fclose(FEDFile);
    ErrorWhileReading = RTI_TRUE;
    D.Out(pdError, "FedParser throwing RTIinternalError.");
    throw RTIinternalError(e._serial, e._reason);
  }
  catch(Exception &e) {
    fclose(FEDFile);
    ErrorWhileReading = RTI_TRUE;
    D.Out(pdError, "FedParser throwing exception %s.", e._name);
    if(e._reason != NULL)
      D.Out(pdError, "Exception reason : %s", e._reason);
    throw ErrorReadingRID(e._serial, e._reason);
  }

  fclose(FEDFile);
  FEDFile = NULL;
  
}


// ---------------------------------------------------------------------------
// readList
// 
fedparser::Object *FedParser::readList()
  throw(ErrorReadingRID,
	MemoryExhausted)
{
  fedparser::Object* object1 ;
  fedparser::Object* object2 ;

  readSeparators();
  if(CurrentChar == ')')
    {
      advance();
      return(makeNil());
    }

  object1 = readListFirst();
  object2 = readList();
  return(makeList(object1, object2));
}


// ---------------------------------------------------------------------------
// readListFirst
// 
fedparser::Object *FedParser::readListFirst()
  throw(ErrorReadingRID,
	MemoryExhausted)
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


// ---------------------------------------------------------------------------
// readSeparators
// 
void FedParser::readSeparators()
  throw(ErrorReadingRID) // Inherited from advance()
{
  int EndSeparators = RTI_FALSE;

  while(! EndSeparators) 
    switch(CurrentChar) {
      
    case ' ':   // Separators
    case '\t':
    case '\n':
      advance();
      break;
      
    case ';':   // Comment line
      skipLine();
      break;
      
    default:
      EndSeparators = RTI_TRUE;
    }

  return;
}


// ---------------------------------------------------------------------------
// readString
// 
fedparser::Object *FedParser::readString()
  throw(ErrorReadingRID,
	MemoryExhausted)
{
  int            Length = 0; 
  char           TempString [CREAD_MAX_ATOM_LENGTH];
  struct String *ResultString;

  while((CurrentChar != '"') &&(CurrentChar != ZEOF)) {
    TempString [Length] = CurrentChar;
    Length ++;
    advance();
  }

  if(CurrentChar == '"')
    advance();

  if(CurrentChar == ZEOF) {
    TempString [Length] = '\0';
    D.Out(pdError, "EOF while reading string \"%s\".", TempString);
    throw ErrorReadingRID("Unexpected EOF while reading string.");
  }

  TempString [Length] = 0;

  ResultString =(struct String *) malloc(sizeof(struct String));
  if(ResultString == NULL) {
    D.Out(pdError, "Memory Exhausted in ReadString.");
    throw MemoryExhausted("Could not allow String Object.");
  }

  ResultString->type   = STRING_TYPE;
  ResultString->length = Length+1;
  ResultString->name   =(char *)malloc(Length+1);;
  if(ResultString->name == NULL) {
    D.Out(pdError, "Memory Exhausted in ReadString for string name.");
    throw MemoryExhausted("Could not allow String Object.");
  }
  strcpy(ResultString->name, TempString);

  return((fedparser::Object *)ResultString);
}


// ---------------------------------------------------------------------------
// skipLine
// 
void FedParser::skipLine()
  throw(ErrorReadingRID) // Inherited from advance()
{
  while((CurrentChar != '\n') &&(CurrentChar != ZEOF))
    advance();

  if(CurrentChar == '\n')
    advance();

  return;
}

}}

// $Id: FedParser_Build.cc,v 3.1 2002/12/11 00:47:33 breholee Exp $

