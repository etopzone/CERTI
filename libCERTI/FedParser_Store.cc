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
// $Id: FedParser_Store.cc,v 3.0.2.1 2002/11/22 00:52:56 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "FedParser.hh"

namespace certi {
  namespace fedparser {

static pdCDebug D("CREAD", "(cread)    - ");

// ------------------------------------
// -- AllocateAndRegisterObjectClass --
// ------------------------------------

void FedParser::allocateAndRegisterObjectClass(int index)
  throw(RTIinternalError)
{
  ObjStack [index] = new ObjectClass;
  if(ObjStack [index] == NULL) {
    D.Out(pdError, "Memory Exhausted in ObjectClass allocation.");
    throw RTIinternalError("Memory Exhausted in ObjectClass allocation.");
  }

  ObjStack [index]->Handle = index;
  ObjStack [index]->Depth  = Depth;

  RootObj->ObjectClasses->addClass(ObjStack [index]);  
}


// -----------------------------------------
// -- AllocateAndRegisterInteractionClass --
// -----------------------------------------

void FedParser::allocateAndRegisterInteractionClass(int index)
  throw(RTIinternalError)
{
  IntStack [index] = new Interaction;
  if(IntStack [index] == NULL) {
    D.Out(pdError, "Memory Exhausted in InteractionClass allocation.");
    throw RTIinternalError("Memory Exhausted.");
  }
    
  IntStack [index]->handle = index;
  IntStack [index]->depth  = Depth;

  RootObj->Interactions->addClass(IntStack [index]);
}


// ----------------
// -- FreeObject --
// ----------------

void FreeObject(Object *x)
{
  switch(x->type) {
  case ATOM_TYPE: {
    Atom *y =(Atom *) x;
    if(y->name != NULL) {
      free(y->name);
      y->name = NULL;
    }
    break;
  }
  case STRING_TYPE: {
    String *y =(String *) x;
    if(y->name != NULL) {
      free(y->name);
      y->name = NULL;
    } 
    break;
  }
  }

  free(x);
}

// ---------------------------
// -- FindObjectParentIndex --
// ---------------------------

int FedParser::findObjectParentIndex()
  throw(RTIinternalError)
{
  int index = ObjIndex;

  // The parent class of the current Interaction is the last Interaction class
  // whose Depth attribute is equal to(current Depth minus one).
  for(index = ObjIndex; index > 0; index--)
    if(ObjStack [index]->Depth ==(Depth - 1))
      return index; // index is the same as ObjStack[index]->Handle

  D.Out(pdError, "Parent Class not found.");
  throw RTIinternalError("Parent Class not found.");
}


// --------------------------------
// -- FindInteractionParentIndex --
// --------------------------------

int FedParser::FindInteractionParentIndex()
  throw(RTIinternalError)
{
  int index = IntIndex;
  
  // The parent class of the current Interaction is the last Interaction class
  // whose Depth attribute is equal to(current Depth minus one).
  for(index = IntIndex; index > 0; index--)
    if(IntStack [index]->depth ==(Depth - 1))
      return index; // Index is the same as IntStack[index]->Handle 

  D.Out(pdError, "Interaction Parent Class not found.");
  throw RTIinternalError("Parent Class not found.");
}


// --------------------------
// -- ProcessAttributeAtom --
// --------------------------

void FedParser::processAttributeAtom(Atom *)
  throw(RTIinternalError)
{
  AttStack [AttIndex] = new ObjectClassAttribute();

  AttStack [AttIndex]->Handle 
    = ObjStack [ObjIndex - 1]->addAttribute(AttStack [AttIndex]);

  D.Out(pdRegister, "Adding new attribute %u to ObjectClass %u.",
	 AttStack [AttIndex]->Handle, ObjStack [ObjIndex - 1]->Handle);
  
  AttIndex++;
  TypeStack [Depth] = ATTRIB;
}


// ----------------------
// -- ProcessClassAtom --
// ----------------------

void FedParser::processClassAtom(Atom *)
  throw(RTIinternalError)
{
  int                ParentIndex = 0; // Index of the Parent Class(if needed)
  
  switch(TypeStack [Depth - 1])
    {
    case OBJ: // Root Object Class(no parent class)
      D.Out(pdRegister, "Allocating Root Object Class.");
      allocateAndRegisterObjectClass(ObjIndex);

      ObjIndex ++;
      if(ObjIndex >= CREAD_MAX_OBJ_COUNT) {
	D.Out(pdError, "Maximum Object Class count reached.");
	throw RTIinternalError("Maximum Object Class count reached in FED.");
      }

      TypeStack [Depth] = CLASSOBJ;
      break;
      
    case INT: // Root Interaction Class(no parent class)
      D.Out(pdRegister, "Allocating Root Interaction Class.");
      allocateAndRegisterInteractionClass(IntIndex);

      IntIndex ++;
      if(IntIndex >= CREAD_MAX_OBJ_COUNT) {
	D.Out(pdError, "Maximum Interaction Class count reached.");
	throw RTIinternalError("Maximum Inter. Class count reached in FED.");
      }

      TypeStack [Depth] = CLASSINT;
      break;
      
    case CLASSOBJ: // Object Class(with a parent class)
      D.Out(pdRegister, "Allocating Child Object Class.");
      allocateAndRegisterObjectClass(ObjIndex);

      // The parent Class is the last one with a Depth attribute equal to
      //(the current Depth - 1)
      ParentIndex = findObjectParentIndex();

      // Build the Parent-Child relation(mutual register, copy attributes...)
      RootObj->ObjectClasses->buildParentRelation(ObjStack [ObjIndex],
						       ObjStack [ParentIndex]);

      ObjIndex++;
      if(ObjIndex >= CREAD_MAX_OBJ_COUNT) {
	D.Out(pdError, "Maximum Object Class count reached.");
	throw RTIinternalError("Maximum Object Class count reached in FED.");
      }

      TypeStack [Depth] = CLASSOBJ;
      break;
      
    case CLASSINT: // Interaction Class(with parent)
      D.Out(pdRegister, "Allocating Child Interaction Class.");
      allocateAndRegisterInteractionClass(IntIndex);

      // The Parent Interaction Class is the last Interaction Class whose
      // Depth is equal to(current Depth minus one)
      ParentIndex = FindInteractionParentIndex();

      // Build the Parent-Child relation(mutual register, copy attributes...)
      RootObj->Interactions->buildParentRelation(IntStack [IntIndex],
						      IntStack [ParentIndex]);
      IntIndex ++;
      if(IntIndex >= CREAD_MAX_OBJ_COUNT) {
	D.Out(pdError, "Maximum Interaction Class count reached.");
	throw RTIinternalError("Maximum Inter. Class count reached in FED.");
      }

      TypeStack [Depth] = CLASSINT;
      break;
      
    default:
      D.Out(pdError, "Unknown type ProcessClassAtom's switch.");
      throw RTIinternalError("Unknown type ProcessClassAtom's switch.");
      break;
    }

  return;
}


// ---------------------------
// -- ProcessFederateString --
// ---------------------------

void FedParser::processFederateString(String *x)
  throw(RTIinternalError)
{
  SecurityLevelID LevelID;

  switch(TypeStack[Depth]) {

  case FEDERATE_NAME:
    // Check and store federate name
    if(strlen(x->name) > MAX_FEDERATE_NAME_LENGTH) {
      printf("Federate Name too long in FED file : %s\n", x->name);
      throw RTIinternalError("Federate Name too long in FED file.");
    }

    strcpy(FederateNameBuffer, x->name);
    
    // Wait for federate level string
    TypeStack [Depth] = FEDERATE_LEVEL;
    break;

  case FEDERATE_LEVEL:
    // 1- Translate string into SecurityLevelID
    LevelID = RootObj->GetSecurityLevelID(x->name);

    // 2- Register Federate to Security Server
    RootObj->registerFederate(FederateNameBuffer, LevelID);

    TypeStack [Depth] = NONE;
    break;

  default:
    throw RTIinternalError("Unknown TypeStack value in ProcessFederateSt.");
  }

}

// --------------------------
// -- ProcessParameterAtom --
// --------------------------

void FedParser::processParameterAtom(Atom *)
  throw(RTIinternalError)
{
  ParStack [ParIndex] = new Parameter();

  ParStack [ParIndex]->Handle 
    = IntStack [IntIndex - 1]->addParameter(ParStack [ParIndex]);

  D.Out(pdRegister, "Adding new parameter %u to Interaction %u.",
	 ParStack [ParIndex]->Handle, IntStack [IntIndex - 1]->handle);
  
  ParIndex++;
  TypeStack [Depth] = PARAM;
}


// -------------------------
// -- ProcessSecLevelAtom -- 
// -------------------------

void FedParser::processSecLevelAtom(Atom *)
  throw(SecurityError,
	 RTIinternalError)
{
  TypeStack [Depth] = SECLEVEL;
}


// ---------------------------
// -- ProcessSecLevelString -- 
// ---------------------------

void FedParser::processSecLevelString(String *x)
  throw(SecurityError,
	 RTIinternalError)
{
  SecurityLevelID  LevelID;

  // 1- Translate string into SecurityLevelID
  LevelID = RootObj->GetSecurityLevelID(x->name);

  // 2- Is it for an object class or an Interaction Class
  switch(TypeStack [Depth - 1]) {

  case OBJ:
  case CLASSOBJ:
    D.Out(pdTrace, "Setting SecLevel of ObjClass %d to %d.",
	   ObjStack [ObjIndex - 1]->Handle, LevelID);
    ObjStack [ObjIndex - 1]->setLevelId(LevelID);
    break;

  case INT:
  case CLASSINT:
    D.Out(pdTrace, "Setting SecLevel of IntClass %d to %d.",
	   IntStack [IntIndex - 1]->handle, LevelID);
    IntStack [IntIndex - 1]->setLevelId(LevelID);
    break;

  default:
    D.Out(pdError, "Should set security level for unknowm type %d.",
	   TypeStack [Depth - 1]);
    throw RTIinternalError("Unknown Type for Security Level in FED file.");
  }

}


// -------------------------------
// -- ProcessTransportOrderAtom --
// -------------------------------

void FedParser::processTransportOrderAtom(Atom *x)
  throw(RTIinternalError)
{
  if(TypeStack [Depth] == ATTRIB) {
    
    if(strcmp(x->name, FED_STR_RELIABLE) == 0)
      AttStack [AttIndex - 1]->Transport = RELIABLE;
    
    else if(strcmp(x->name, FED_STR_BESTEFFORT) == 0)
      AttStack [AttIndex - 1]->Transport = BEST_EFFORT;
    
    else if(strcmp(x->name, FED_STR_RECEIVE) == 0)
      AttStack [AttIndex - 1]->Order = RECEIVE;
    
    else if(strcmp(x->name, FED_STR_TIMESTAMP) == 0)
      AttStack [AttIndex - 1]->Order = TIMESTAMP;
    
    else {
      D.Out(pdError, 
	     "Unknown atom string %s in StoreAtom's switch.", x->name);
      throw RTIinternalError("Unknown Atom string in FED file.");
    }
  } // TypeStack [Depth] == ATTRIB
  
  else if(TypeStack [Depth] == CLASSINT) {
    
    if(strcmp(x->name, FED_STR_RELIABLE) == 0)
      IntStack [IntIndex - 1]->transport = RELIABLE;
    
    else if(strcmp(x->name, FED_STR_BESTEFFORT) == 0)
      IntStack [IntIndex - 1]->transport = BEST_EFFORT;
    
    else if(strcmp(x->name, FED_STR_RECEIVE) == 0)
      IntStack [IntIndex - 1]->order = RECEIVE;
    
    else if(strcmp(x->name, FED_STR_TIMESTAMP) == 0)
      IntStack [IntIndex - 1]->order = TIMESTAMP;
    
    else {
      D.Out(pdError, 
	     "Unknown atom string %s in StoreAtom's switch.", x->name);
      throw RTIinternalError("Unknown Atom string in FED file.");
    }
  } // TypeStack [Depth] == CLASSINT

  else {
    D.Out(pdError, 
	   "Unknown atom string %s in StoreAtom's switch.", x->name);
    throw RTIinternalError("Unknown Atom string in FED file.");
  }
}


// -----------
// -- Store --
// -----------

void FedParser::store(Object *Root)
  throw(SecurityError,
	 RTIinternalError)
{
  storeObject(Root, "");
  printf("\n");
}


// ---------------
// -- StoreAtom --
// ---------------

void FedParser::storeAtom(Atom *x)
  throw(SecurityError,
	 RTIinternalError)
{

  D.Out(pdDebug, "Storing Atom %s.", x->name);

  printf(" %s",x->name);
  if(strcmp(x->name, FED_STR_FED) == 0)
    TypeStack [Depth] = FED;

  else if(strcmp(x->name, FED_STR_OBJECTS) == 0)
    TypeStack [Depth] = OBJ;
    
  else if(strcmp(x->name, FED_STR_INTERACTIONS) == 0)
    TypeStack [Depth] = INT;
    
  else if(strcmp(x->name, FED_STR_CLASS) == 0)
    processClassAtom(x);
    
  else if(strcmp(x->name, FED_STR_ATTRIBUTE) == 0)
    processAttributeAtom(x);

  else if(strcmp(x->name, FED_STR_PARAMETER) == 0)
    processParameterAtom(x);
  
  else if(strcmp(x->name, FED_STR_SECLEVEL) == 0)
    processSecLevelAtom(x);

  else if(strcmp(x->name, FED_STR_FEDERATE) == 0)
    TypeStack [Depth] = FEDERATE_NAME;

  else
    processTransportOrderAtom(x);

  return;
}


// ---------------
// -- StoreList --
// ---------------

void FedParser::storeList(List *x, const char *Header)
  throw(SecurityError,
	 RTIinternalError)
{
  Object *next;
  
  storeObject(x->first, Header);

  next = x->next;
  // Next can be either a Nil object(then the list end is reached)
  // a another list(if there are remaining list elements).

  if(next->type == NIL_TYPE) {
    Depth--;
    printf(" ]");
  }
  // There are remaining elements in the list.
  else if(next->type == LIST_TYPE)
    storeList((struct List *)next, Header);

  else {
    D.Out(pdError, "Illegal type for next in List Object.");
    throw RTIinternalError("Illegal type for next in List Object.");
  }

  FreeObject(next);
  return;
}


// -----------------
// -- StoreObject --
// -----------------

void FedParser::storeObject(Object *x, const char *Header)
  throw(SecurityError,
	 RTIinternalError)
{
  char *NewHeader;

  switch(x->type) {
  case LIST_TYPE:
    printf("\n%s[", Header);
    Depth++;

    // Increment Header
    NewHeader =(char *) malloc(strlen(Header) + 4);
    if(NewHeader == NULL)
      storeList((struct List *) x, Header);
    else {
      strcpy(NewHeader, Header);
      strcat(NewHeader, "   ");
      storeList((struct List *) x, NewHeader);
      free(NewHeader);
    }

    break;

  case ATOM_TYPE:
    storeAtom((struct Atom *) x);
    break;
  case STRING_TYPE:
    storeString((struct String *) x);
    break;
  case NIL_TYPE:
    printf("%s[]\n", Header);
    break;
  }

  FreeObject(x);
  return;

}


// -----------------
// -- StoreString --
// -----------------

void FedParser::storeString(String *x)
  throw(SecurityError,
	 RTIinternalError)
{

  D.Out(pdDebug, "Storing String %s.", x->name);

  printf(" \"%s\"",x->name);

  try {
    switch(TypeStack [Depth]) {
      
    case OBJ:
    case CLASSOBJ:
      ObjStack [ObjIndex - 1]->setName(x->name);
      break;

    case INT:
    case CLASSINT:
      IntStack [IntIndex - 1]->setName(x->name);
      break;

    case ATTRIB:
      AttStack [AttIndex - 1]->setName(x->name);
      break;

    case PARAM:
      ParStack [ParIndex - 1]->setName(x->name);
      break;

    case SECLEVEL:
      processSecLevelString(x);
      break;

    case FEDERATE_NAME:
    case FEDERATE_LEVEL:
      processFederateString(x);
      break;

    default:
      D.Out(pdError, "Unknown type in StoreString's switch.");
      throw RTIinternalError("CREAD: Unknown type in StoreString's switch.");
    }
  }
  catch(ValueLengthExceeded) {
    printf("\n\nName Token too long : %s.\n", x->name);
    printf("You must shorten your string or increase the MAX_USER_TAG_LENGTH constant.\n");
    throw RTIinternalError("Name too long.");
  }

  return;
}

}}

// $Id: FedParser_Store.cc,v 3.0.2.1 2002/11/22 00:52:56 breholee Exp $
