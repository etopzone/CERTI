// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: FedParser.hh,v 3.4 2003/01/28 23:41:30 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_FED_PARSER_HH
#define _CERTI_FED_PARSER_HH

#include <config.h>

#include <fstream>
using std::ifstream;
using std::ios;

#include <iostream>
using std::cout;
using std::endl;

#include "RootObject.hh"
#include "fed_file.hh"
#include "parser.hh"
#include "PrettyDebug.hh"

#define ZEOF ~0u // -1 for unsigned type (Standard C)

#define CREAD_MAX_BUF_LENGTH 4096 // File buffer length
#define CREAD_MAX_ATOM_LENGTH 256 // Max length of an atom in the FED file.
#define CREAD_MAX_OBJ_COUNT 100 // Max number of Object classes, attributes,
// Interaction Classes, etc.

namespace certi {
namespace fedparser {

class FedParser
{
public:

  // --------------------------------
  // -- Constructor and Destructor --
  // --------------------------------
  FedParser(RootObject *);
  ~FedParser(void);
 
  // ------------------------
  // -- Public Entry Point --
  // ------------------------
  void readFile(const char *fed_file)
    throw(CouldNotOpenRID,
	  ErrorReadingRID,
	  SecurityError,
	  RTIinternalError);

private:

  // ------------------------------------------
  // -- Private Methods : Read-Build Methods --
  // ------------------------------------------

  void advance(void)
    throw(ErrorReadingRID);

  void skipLine(void)
    throw(ErrorReadingRID); // Inherited from advance().

  void readSeparators(void)
    throw(ErrorReadingRID); // Inherited from advance().

  Object *readListFirst()
    throw(ErrorReadingRID, MemoryExhausted);

  Object *readList()
    throw(ErrorReadingRID, MemoryExhausted);

  Object *readAtom()
    throw(ErrorReadingRID, MemoryExhausted);

  Object *readString()
    throw(ErrorReadingRID, MemoryExhausted);

  Object *makeList(Object *x, Object *y)
    throw(MemoryExhausted);

  Object *makeNil()
    throw(MemoryExhausted);

  // ---------------------------------------
  // -- Private Methods : Storage Methods --(cf. FedParser_Store.cc)
  // ---------------------------------------

  // -- Object and Interaction Classes Init --

  void allocateAndRegisterObjectClass(int index)
    throw(RTIinternalError);

  void allocateAndRegisterInteractionClass(int index)
    throw(RTIinternalError);

  int findObjectParentIndex(void)
    throw(RTIinternalError);

  int FindInteractionParentIndex(void)
    throw(RTIinternalError);

  // -- Atom Processing -- 
  void processAttributeAtom(fedparser::Atom *x)
    throw(RTIinternalError);

  void processFederateString(fedparser::String *x)
    throw(RTIinternalError);

  void processClassAtom(fedparser::Atom *x)
    throw(RTIinternalError);

  void processParameterAtom(fedparser::Atom *x)
    throw(RTIinternalError);

  void processSecLevelAtom(fedparser::Atom *x)
    throw(SecurityError,
	  RTIinternalError);

  void processSecLevelString(fedparser::String *x)
    throw(SecurityError,
	  RTIinternalError);
 
  void processTransportOrderAtom(fedparser::Atom *x)
    throw(RTIinternalError);

  // -- Store* Methods --
  void store(Object *root)
    throw(SecurityError, RTIinternalError);

  void storeAtom(fedparser::Atom *x)
    throw(SecurityError, RTIinternalError);

  void storeList(fedparser::List *x, const char *header)
    throw(SecurityError, RTIinternalError);

  void storeObject(Object *x, const char *header)
    throw(SecurityError, RTIinternalError);

  void storeString(fedparser::String *x)
    throw(SecurityError, RTIinternalError);

  // ------------------------
  // -- Private Attributes --
  // ------------------------

  RootObject *RootObj;
 
  Boolean ErrorWhileReading; //<! Used to know when to display RootObj struct.

  // ------------------------------
  // -- Stacks and Stack Indexes --(used in the Store part)
  // ------------------------------

  ObjectClass *ObjStack[CREAD_MAX_OBJ_COUNT]; //!< Object Class Stack.
  Interaction *IntStack[CREAD_MAX_OBJ_COUNT]; //!< Interaction Class Stack.

  //! Object Attribute Stack.
  ObjectClassAttribute *AttStack[CREAD_MAX_OBJ_COUNT];
  Parameter *ParStack[CREAD_MAX_OBJ_COUNT]; //!< Interaction Parameter Stack.
  //! Current Atom Type Stack.
  fedparser::AtomType TypeStack[CREAD_MAX_OBJ_COUNT];

  int ObjIndex; //!< Object Class Stack Index
  int IntIndex; //!< Interaction Class Stack Index
  int AttIndex; //!< Object Attribute Stack Index
  int ParIndex; //!< Interaction Parameter Stack Index

  int Depth;    //<! Current Depth in the FED tree.
  // TypeStack[Depth] is the type of the last read List.

  // Stacks Doc : For example, the current Object Class is pointed
  // by ObjStack[ObjIndex - 1]. It is the last Object Class discovered.

  // ObjIndex, IntIndex, AttIndex and ParIndex are incremented each time
  // a new Object Class(etc.) is put in the stack. Their respective
  // value is the NEXT EMPTY space in the stack.

  // ---------------------------------
  // -- FED File objects and buffer --(used in the read part)
  // ---------------------------------

  ifstream *FEDFile; //!< Pointer to file stream for reading data.
 
  char InBuffer[CREAD_MAX_BUF_LENGTH]; //!< Buffer used to store piece of file.
  int InBufferPos; //!< Position in InBuffer currently being treated.
  int InBufferLength; //!< Size of the useful part of InBuffer buffer.
  int CurrentChar; //!< char value being currently treated by file reading.

  // To temporaly store a federate name before giving it a sec level.
  char FederateNameBuffer[MAX_FEDERATE_NAME_LENGTH+1];
};

}
}

#endif // _CERTI_FED_PARSER_HH

// $Id: FedParser.hh,v 3.4 2003/01/28 23:41:30 breholee Exp $
