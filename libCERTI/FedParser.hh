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
// $Id: FedParser.hh,v 3.9 2003/10/20 11:56:54 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_FED_PARSER_HH
#define _CERTI_FED_PARSER_HH

#include "RootObject.hh"

#include <vector>

#define ZEOF ~0u // -1 for unsigned type

#define CREAD_MAX_BUF_LENGTH 4096 // File buffer length
#define CREAD_MAX_ATOM_LENGTH 256 // Max length of an atom in the FED file.
#define CREAD_MAX_OBJ_COUNT 100 // Max number of Object classes, attributes,
// Interaction Classes, etc.

#define FED_STR_FED "fed"
#define FED_STR_OBJECTS "objects"
#define FED_STR_INTERACTIONS "interactions"
#define FED_STR_CLASS "class"
#define FED_STR_ATTRIBUTE "attribute"
#define FED_STR_PARAMETER "parameter"
#define FED_STR_SECLEVEL "sec_level"
#define FED_STR_FEDERATE "federate"

#define FED_STR_RELIABLE "FED_RELIABLE"
#define FED_STR_BESTEFFORT "FED_BEST_EFFORT"

#define FED_STR_TIMESTAMP "FED_TIMESTAMP"
#define FED_STR_RECEIVE "FED_RECEIVE"

namespace certi {
namespace fedparser {

union Object ;

// A list is composed of a first element 'first', that can be an
// Atom or a List, and of the rest of the elements, pointed by
// Next. Next can be a List or a Nil object.
struct List {
    int type ;
    union Object *first ; // List(nested list) or Atom or String
    union Object *next ; // List(if there are other elements) or Nil
};

struct Atom {
    int type ;
    int length ;
    char *name ;
};

struct Nil {
    int type ;
};

struct String {
    int type ;
    int length ;
    char *name ;
};

union Object {
    int type ;
    struct List list ;
    struct Atom atom ;
    struct Nil nil ;
    struct String str ;
};

typedef union Object Object ;

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
} AtomType ;

class FedParser
{
public:
    // --------------------------------
    // -- Constructor and Destructor --
    // --------------------------------
    FedParser(RootObject *);
    ~FedParser();

    // ------------------------
    // -- Public Entry Point --
    // ------------------------
    void readFile(const char *fed_file)
        throw (CouldNotOpenRID,
               ErrorReadingRID,
               SecurityError,
               RTIinternalError);

    static void freeObject(Object *x);
private:

    // ------------------------------------------
    // -- Private Methods : Read-Build Methods --
    // ------------------------------------------

    void advance()
        throw (ErrorReadingRID);

    void skipLine()
        throw (ErrorReadingRID); // Inherited from advance().

    void readSeparators()
        throw (ErrorReadingRID); // Inherited from advance().

    Object *readListFirst()
        throw (ErrorReadingRID, MemoryExhausted);

    Object *readList()
        throw (ErrorReadingRID, MemoryExhausted);

    Object *readAtom()
        throw (ErrorReadingRID, MemoryExhausted);

    Object *readString()
        throw (ErrorReadingRID, MemoryExhausted);

    Object *makeList(Object *x, Object *y)
        throw (MemoryExhausted);

    Object *makeNil()
        throw (MemoryExhausted);

    // ---------------------------------------
    // -- Private Methods : Storage Methods --(cf. FedParser_Store.cc)
    // ---------------------------------------

    // -- Object and Interaction Classes Init --

    void allocateAndRegisterObjectClass()
        throw (RTIinternalError);

    void allocateAndRegisterInteractionClass()
        throw (RTIinternalError);

    ObjectClass * findObjectParent() const
        throw (RTIinternalError);

    Interaction * findInteractionParent() const
        throw (RTIinternalError);

    // -- Atom Processing --
    void processAttributeAtom(fedparser::Atom *x)
        throw (RTIinternalError);

    void processFederateString(fedparser::String *x)
        throw (RTIinternalError);

    void processClassAtom(fedparser::Atom *x)
        throw (RTIinternalError);

    void processParameterAtom(fedparser::Atom *x)
        throw (RTIinternalError);

    void processSecLevelAtom(fedparser::Atom *x)
        throw (SecurityError,
               RTIinternalError);

    void processSecLevelString(fedparser::String *x)
        throw (SecurityError,
               RTIinternalError);

    void processTransportOrderAtom(fedparser::Atom *x)
        throw (RTIinternalError);

    // -- Store* Methods --
    void store(Object *root)
        throw (SecurityError, RTIinternalError);

    void storeAtom(fedparser::Atom *x)
        throw (SecurityError, RTIinternalError);

    void storeList(fedparser::List *x, const char *header)
        throw (SecurityError, RTIinternalError);

    void storeObject(Object *x, const char *header)
        throw (SecurityError, RTIinternalError);

    void storeString(fedparser::String *x)
        throw (SecurityError, RTIinternalError);

    // ------------------------
    // -- Private Attributes --
    // ------------------------

    RootObject *RootObj ;

    Boolean ErrorWhileReading ; //<! Used to know when to display
    //RootObj struct.

    // ------------------------------
    // -- Stacks and Stack Indexes --(used in the Store part)
    // ------------------------------
    // Stacks Doc : For example, the current Object Class is pointed by
    // objStack.back(). It is the last Object Class discovered.

    std::vector<ObjectClass *> objStack ; //!< Object Class Stack.
    std::vector<Interaction *> intStack ; //!< Interaction Class Stack.
    std::vector<ObjectClassAttribute *> attStack ; //!< Object Attribute Stack.
    std::vector<Parameter *> parStack ; //!< Interaction Parameter Stack.

    //! Current Atom Type Stack.
    fedparser::AtomType TypeStack[CREAD_MAX_OBJ_COUNT] ;
    int Depth ; //<! Current Depth in the FED tree.

    // ---------------------------------
    // -- FED File objects and buffer --(used in the read part)
    // ---------------------------------

    std::ifstream *FEDFile ; //!< Pointer to file stream for reading data.

    char InBuffer[CREAD_MAX_BUF_LENGTH] ; //!< Buffer used to store
    //piece of file.
    int InBufferPos ; //!< Position in InBuffer currently being treated.
    int InBufferLength ; //!< Size of the useful part of InBuffer buffer.
    int CurrentChar ; //!< char value being currently treated by file reading.

    // To temporaly store a federate name before giving it a sec level.
    char FederateNameBuffer[MAX_FEDERATE_NAME_LENGTH+1] ;
};

}} // namespace certi/fedparser

#endif // _CERTI_FED_PARSER_HH

// $Id: FedParser.hh,v 3.9 2003/10/20 11:56:54 breholee Exp $
