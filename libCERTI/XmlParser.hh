// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003  ONERA
//
// This file is part of CERTI-libCERTI
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
// $Id: XmlParser.hh,v 3.1 2003/01/20 17:45:49 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_XML_PARSER_HH
#define _CERTI_XML_PARSER_HH

#ifdef HAVE_XML

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "RootObject.hh"
#include "ObjectClass.hh"
#include "Interaction.hh"

#include <string>
using std::string ;

#define NODE_OBJECT_MODEL (const xmlChar*) "objectModel"
#define NODE_OBJECTS (const xmlChar*) "objects"
#define NODE_OBJECT_CLASS (const xmlChar*) "objectClass"
#define NODE_INTERACTIONS (const xmlChar*) "interactions"
#define NODE_INTERACTION_CLASS (const xmlChar*) "interactionClass"
#define NODE_ATTRIBUTE (const xmlChar*) "attribute"
#define NODE_PARAMETER (const xmlChar*) "parameter"

#define ATTRIBUTE_NAME (const xmlChar*) "name"
#define ATTRIBUTE_TRANSPORTATION (const xmlChar*) "transportation"
#define ATTRIBUTE_ORDER (const xmlChar*) "order"

#define VALUE_RELIABLE (const xmlChar*) "HLAreliable"
#define VALUE_BESTEFFORT (const xmlChar*) "HLAbestEffort"
#define VALUE_TSO (const xmlChar*) "TimeStamp"
#define VALUE_RO (const xmlChar*) "Reveive"

namespace certi {

class XmlParser
{
public:
    XmlParser(RootObject*);

    RootObject* parse(string);

private:
    string filename ;
    RootObject* root ;

    xmlDocPtr doc ;
    xmlNodePtr cur ;

    int freeObjectClassHandle ;
    int freeInteractionClassHandle ;
    int freeAttributeHandle ;
    int freeParameterHandle ;

    void parseClass(ObjectClass *);
    void parseInteraction(Interaction *);
};

} // namespace certi

#endif // HAVE_XML
#endif // _CERTI_XML_PARSER_HH

// $Id: XmlParser.hh,v 3.1 2003/01/20 17:45:49 breholee Exp $
