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
// $Id: parser.hh,v 3.1 2002/12/11 00:47:34 breholee Exp $
// ---------------------------------------------------------------------------

// $Id: parser.hh,v 3.1 2002/12/11 00:47:34 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_FED_PARSER_TYPES_HH
#define _CERTI_FED_PARSER_TYPES_HH

namespace certi {
namespace fedparser {

union Object;

struct List {
  int type;
  union Object *first; // List(nested list) or Atom or String
  union Object *next; // List(if there are other elements) or Nil
};
// A list is composed of a first element 'first', that can be an
// Atom or a List, and of the rest of the elements, pointed by
// Next. Next can be a List or a Nil object.

struct Atom {
  int type;
  int length;
  char *name;
};

struct Nil {
  int type;
};

struct String {
  int type;
  int length;
  char *name;
};

union Object {
  int type;
  struct List list;
  struct Atom atom;
  struct Nil nil;
  struct String str;
};

typedef union Object Object;

#define ATOM_TYPE 0
#define LIST_TYPE 1
#define STRING_TYPE 2
#define NIL_TYPE 3

typedef enum {
  NONE,
  FED, // Reading a FED list
  OBJ, // Reading an object class definition
  INT, // Reading an interaction class definition
  CLASSOBJ, // Reading an object subclass definition
  CLASSINT, // Reading an interaction subclass definition
  ATTRIB, // Reading an attribute definition
  PARAM, // Reading a parameter definition
  SECLEVEL, // Waiting for a security level name
  FEDERATE_NAME, // Waiting for a federate name
  FEDERATE_LEVEL // Waiting for a federate level
} AtomType;

// --------------------------
// -- Free Object Function --(cf. CRead_Store.cc)
// --------------------------

void FreeObject(Object *x);
}
}

#endif // _CERTI_FED_PARSER_TYPES_HH

// $Id: parser.hh,v 3.1 2002/12/11 00:47:34 breholee Exp $

