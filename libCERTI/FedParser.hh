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
// $Id: FedParser.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_FED_PARSER_HH
#define _CERTI_FED_PARSER_HH

#include "RootObject.hh"
#include "fed_file.hh"
#include "parser.hh"
#include "PrettyDebug.hh"

#define ZEOF -1

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
  // Parameter : a pointer to the federation Root Object. It would better
  // be an empty Root Object...

  ~FedParser();
 
  // ------------------------
  // -- Public Entry Point --
  // ------------------------

  void readFile(char *FedFile)
    throw(CouldNotOpenRID,
	  ErrorReadingRID,
	  SecurityError,
	  RTIinternalError);
  // Read the 'FedFile' file and build the Root Object tree following the
  // FED file content. Call the 'Store' method after a completed file read.
  // -- THIS METHOD IS THE READ PART ENTRY POINT --

private:

  // ------------------------------------------
  // -- Private Methods : Read-Build Methods --
  // ------------------------------------------

  void advance()
    throw(ErrorReadingRID);
  // Put the next character of FEDFile in CurrentChar.
  // Uses InBuffer to temporaly store content of FEDFile.

  void skipLine()
    throw(ErrorReadingRID); // Inherited from advance().
  // Read a comment line.

  void readSeparators(void)
    throw(ErrorReadingRID); // Inherited from advance().
  // Read white spaces, new line, and also skip comment lines.

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
  // Allocate, initialize and register(add to RootObj tree) new
  // Object Class.

  void allocateAndRegisterInteractionClass(int index)
    throw(RTIinternalError);
  // See AllocateAndRegisterObjectClass

  int findObjectParentIndex()
    throw(RTIinternalError);
  // Return the last registered Object Class handle whose Depth is equal
  // to(Depth-1).

  int FindInteractionParentIndex()
    throw(RTIinternalError);
  // Return the last registered Interaction class handle whose Depth is 
  // equal to(Depth-1)

  // -- Atom Processing -- 
  void processAttributeAtom(fedparser::Atom *x)
    throw(RTIinternalError);
  // When a 'attribute' atom is encountered, it means that a new
  // object class attribute definition is starting. This function
  // process this event.

  void processFederateString(fedparser::String *x)
    throw(RTIinternalError);
  // Process a 'federate' list token, containing a Federate Name
  // and a Security Level associated with it. This method is
  // called twice, for the Federate Name string and then for the
  // Federate Level string.

  void processClassAtom(fedparser::Atom *x)
    throw(RTIinternalError);
  // When a 'class' atom is encountered, it means that a new
  // object or interaction class definition is starting. This
  // function process this kind of event.

  void processParameterAtom(fedparser::Atom *x)
    throw(RTIinternalError);
  // When a 'parameter' atom is encountered, it means that a new
  // interaction parameter definition is starting. This function
  // process this event.

  void processSecLevelAtom(fedparser::Atom *x)
    throw(SecurityError,
	  RTIinternalError);
  // Process security level atoms.

  void processSecLevelString(fedparser::String *x)
    throw(SecurityError,
	  RTIinternalError);
  // Guess whose security level the string describes, and set it
  // correctly.
 
  void processTransportOrderAtom(fedparser::Atom *x)
    throw(RTIinternalError);
  // Process FED_RELIABLE etc. atoms, related to Transport or
  // Order type.

  // -- Store* Methods --
  void store(Object *Root)
    throw(SecurityError,
	  RTIinternalError);
  // Transfer the Object tree pointed by Root into a RootObj tree.
  // It calls storeObject(Root) to start the recursive storage
  // process.  -- THIS METHOD IS THE STORE PART ENTRY POINT --

  void storeAtom(fedparser::Atom *x)
    throw(SecurityError, RTIinternalError);

  void storeList(fedparser::List *x, const char *Header)
    throw(SecurityError, RTIinternalError);
  // The main recursive part is here.
  // Header is used to print trace information.

  void storeObject(Object *x, const char *Header)
    throw(SecurityError, RTIinternalError);
  // Call one of the Store* method depending on 'x' type.
  // Header is used to print trace information.

  void storeString(fedparser::String *x)
    throw(SecurityError, RTIinternalError);

  // ------------------------
  // -- Private Attributes --
  // ------------------------

  RootObject *RootObj;
 
  Boolean ErrorWhileReading; // Used to know when to display RootObj struct

  // ------------------------------
  // -- Stacks and Stack Indexes --(used in the Store part)
  // ------------------------------

  ObjectClass *ObjStack[CREAD_MAX_OBJ_COUNT];
  // Object Class Stack
  Interaction *IntStack[CREAD_MAX_OBJ_COUNT];
  // Interaction Class Stack
  ObjectClassAttribute *AttStack[CREAD_MAX_OBJ_COUNT];
  // Object Attribute Stack
  Parameter *ParStack[CREAD_MAX_OBJ_COUNT];
  // Interaction Parameter Stack
  fedparser::AtomType TypeStack[CREAD_MAX_OBJ_COUNT];
  // Current Atom Type Stack

  int ObjIndex; // Object Class Stack Index
  int IntIndex; // Interaction Class Stack Index
  int AttIndex; // Object Attribute Stack Index
  int ParIndex; // Interaction Parameter Stack Index

  int Depth; // Current Depth in the FED tree.
  // TypeStack[Depth] is the type of the last read List.

  // Stacks Doc : For example, the current Object Class is pointed
  // by ObjStack[ObjIndex - 1]. It is the last Object Class discovered.

  // ObjIndex, IntIndex, AttIndex and ParIndex are incremented each time
  // a new Object Class(etc.) is put in the stack. Their respective
  // value is the NEXT EMPTY space in the stack.

  // ---------------------------------
  // -- FED File objects and buffer --(used in the read part)
  // ---------------------------------

  FILE *FEDFile;
 
  char InBuffer[CREAD_MAX_BUF_LENGTH];
  int InBufferPos;
  int InBufferLength;
  int CurrentChar;

  // To temporaly store a federate name before giving it a sec level.
  char FederateNameBuffer[MAX_FEDERATE_NAME_LENGTH+1];
};

}
}

#endif // _CERTI_FED_PARSER_HH

// $Id: FedParser.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
