// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003  ONERA
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
// $Id: XmlParser.hh,v 3.5 2003/03/19 08:54:18 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_XML_PARSER_HH
#define _CERTI_XML_PARSER_HH

#ifdef HAVE_XML

// Project
#include <config.h>
#include "RoutingSpace.hh"
#include "RootObject.hh"
#include "ObjectClass.hh"
#include "Interaction.hh"
#include "PrettyDebug.hh"

// Libraries
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

// Standard libraries
#include <string>

using std::string ;

#define NODE_OBJECT_MODEL (const xmlChar*) "objectModel"
#define NODE_OBJECTS (const xmlChar*) "objects"
#define NODE_OBJECT_CLASS (const xmlChar*) "objectClass"
#define NODE_INTERACTIONS (const xmlChar*) "interactions"
#define NODE_INTERACTION_CLASS (const xmlChar*) "interactionClass"
#define NODE_ATTRIBUTE (const xmlChar*) "attribute"
#define NODE_PARAMETER (const xmlChar*) "parameter"
#define NODE_ROUTING_SPACE (const xmlChar*) "routingSpace"
#define NODE_DIMENSIONS (const xmlChar*) "dimensions"
#define NODE_DIMENSION (const xmlChar*) "dimension"

#define ATTRIBUTE_NAME (const xmlChar*) "name"
#define ATTRIBUTE_TRANSPORTATION (const xmlChar*) "transportation"
#define ATTRIBUTE_ORDER (const xmlChar*) "order"
#define ATTRIBUTE_SPACE (const xmlChar*) "space"

#define VALUE_RELIABLE (const xmlChar*) "HLAreliable"
#define VALUE_BESTEFFORT (const xmlChar*) "HLAbestEffort"
#define VALUE_TSO (const xmlChar*) "TimeStamp"
#define VALUE_RO (const xmlChar*) "Receive"

namespace certi {

class XmlParser
{
public:
    XmlParser(RootObject*);
    RootObject* parse(string);
    static bool exists(void);

private:
    string filename ;
    RootObject* root ;

    xmlDocPtr doc ;
    xmlNodePtr cur ;

    int freeObjectClassHandle ;
    int freeInteractionClassHandle ;
    int freeAttributeHandle ;
    int freeParameterHandle ;
    int freeSpaceHandle ;

    void parseClass(ObjectClass *);
    void parseInteraction(Interaction *);
    void parseRoutingSpace(void);
};

} // namespace certi

#endif // HAVE_XML
#endif // _CERTI_XML_PARSER_HH

// $Id: XmlParser.hh,v 3.5 2003/03/19 08:54:18 breholee Exp $
