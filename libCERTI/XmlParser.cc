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
// $Id: XmlParser.cc,v 3.12 2003/06/27 17:26:29 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "XmlParser.hh"

#include "ObjectClassAttribute.hh"
#include "RoutingSpace.hh"
#include "PrettyDebug.hh"

using std::string ;

#ifdef HAVE_XML

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

static pdCDebug D("XMLPARSER", "(XmlParser) ");

XmlParser::XmlParser(RootObject* r)
{
    root = r ;

    freeObjectClassHandle = 1 ;
    freeInteractionClassHandle = 1 ;
    freeAttributeHandle = 1 ;
    freeParameterHandle = 1 ;
    freeSpaceHandle = 1 ;
}

// ----------------------------------------------------------------------------
//! Main method to parse .xml file
RootObject*
XmlParser::parse(string s)
{
    D.Out(pdTrace, "Starting to parse XML file");
    filename = s ;

    // transportation = HLAreliable
    // order = TimeStamp

    doc = xmlParseFile(filename.c_str());

    // Did libXML manage to parse the file ?
    if (doc == 0) {
        cerr << "XML file not parsed successfully" << endl ;
        xmlFreeDoc(doc);
        return 0 ;
    }

    // Is there a root element ?
    cur = xmlDocGetRootElement(doc);
    if (cur == 0) {
        cerr << "XML file is empty" << endl ;
        xmlFreeDoc(doc);
        return 0 ;
    }

    // Is this root element an objectModel ?
    if (xmlStrcmp(cur->name, (const xmlChar *) NODE_OBJECT_MODEL)) {
        cerr << "Wrong XML file: not the expected root node" << endl ;
        return 0 ;
    }
    D.Out(pdTrace, "XML file looks ok, starting main loop");

    // First Loop (Routing Spaces)
    cur = xmlDocGetRootElement(doc);
    cur = cur->xmlChildrenNode ;
    while (cur != NULL) {
        if ((!xmlStrcmp(cur->name, NODE_ROUTING_SPACE))) {
            D.Out(pdTrace, "Found a routing space");
            xmlNodePtr prev = cur ;
            this->parseRoutingSpace();
            cur = prev ;
        }
        cur = cur->next ;
    }

    // Second Loop (Object and Interaction classes)
    cur = xmlDocGetRootElement(doc);
    cur = cur->xmlChildrenNode ;
    while (cur != NULL) {
        if ((!xmlStrcmp(cur->name, NODE_OBJECTS))) {
            D.Out(pdTrace, "Found a group of object classes");
            xmlNodePtr prev = cur ;
            cur = cur->xmlChildrenNode ;
            while (cur != NULL) {
                if ((!xmlStrcmp(cur->name, NODE_OBJECT_CLASS))) {
                    this->parseClass(0);
                }
                cur = cur->next ;
            }
            cur = prev ;
        }
        if ((!xmlStrcmp(cur->name, NODE_INTERACTIONS))) {
            D.Out(pdTrace, "Found a group of interaction classes");
            xmlNodePtr prev = cur ;
            cur = cur->xmlChildrenNode ;
            while (cur != NULL) {
                if ((!xmlStrcmp(cur->name, NODE_INTERACTION_CLASS))) {
                    this->parseInteraction(0);
                }
                cur = cur->next ;
            }
            cur = prev ;
        }
        cur = cur->next ;
    }

    xmlFreeDoc(doc);

    D[pdTrace] << "XmlParser: finished parsing" << endl ;
    return root ;
}

// ----------------------------------------------------------------------------
//! Parse the current class node
void
XmlParser::parseClass(ObjectClass* parent)
{
    D[pdTrace] << "New Object Class" << endl ;

    xmlNodePtr prev = cur ;
    ObjectClass* current = new ObjectClass();
    current->setName((char *) xmlGetProp(cur, ATTRIBUTE_NAME));
    current->setHandle(freeObjectClassHandle++);
    root->ObjectClasses->addClass(current);
    if (parent != 0)
        root->ObjectClasses->buildParentRelation(current, parent);

    cur = cur->xmlChildrenNode ;
    while (cur != NULL) {
        // Attributes
        if ((!xmlStrcmp(cur->name, NODE_ATTRIBUTE))) {
            ObjectClassAttribute *attr = new ObjectClassAttribute();

            // Name
            attr->setName((char *) xmlGetProp(cur, ATTRIBUTE_NAME));

            // Handle
            attr->setHandle(freeAttributeHandle++);

            // Transportation
            if (!xmlStrcmp(xmlGetProp(cur, ATTRIBUTE_TRANSPORTATION),
                           VALUE_RELIABLE))
                attr->Transport = RELIABLE ;
            else if (!xmlStrcmp(xmlGetProp(cur, ATTRIBUTE_TRANSPORTATION),
                                VALUE_BESTEFFORT))
                attr->Transport = BEST_EFFORT ;

            // Order
            if (!xmlStrcmp(xmlGetProp(cur, ATTRIBUTE_ORDER), VALUE_TSO))
                attr->Order = TIMESTAMP ;
            else if (!xmlStrcmp(xmlGetProp(cur, ATTRIBUTE_ORDER), VALUE_RO))
                attr->Order = RECEIVE ;

            // Routing space
            char *space = (char *) xmlGetProp(cur, ATTRIBUTE_SPACE);
            if (space) {
                SpaceHandle h ;
                try {
                    h = root->getRoutingSpaceHandle(string(space));
                }
                catch (Exception &e) {
                    cerr << "warning: Incorrect space name for attribute"
                         << endl ;
                }
                attr->setSpace(h);
            }

            // Attribute complete, adding to the class
            current->addAttribute(attr);
        }
        // Subclass
        if ((!xmlStrcmp(cur->name, NODE_OBJECT_CLASS))) {
            this->parseClass(current);
        }
        cur = cur->next ;
    }
    cur = prev ;
}

// ----------------------------------------------------------------------------
//! Parse the current interaction node
void
XmlParser::parseInteraction(Interaction* parent)
{
    D[pdTrace] << "New Interaction Class" << endl ;

    xmlNodePtr prev = cur ;
    Interaction* current = new Interaction();

    // Name
    current->setName((char *) xmlGetProp(cur, ATTRIBUTE_NAME));

    // Handle
    current->handle = freeInteractionClassHandle++ ;

    // Transportation
    if (!xmlStrcmp(xmlGetProp(cur, ATTRIBUTE_TRANSPORTATION), VALUE_RELIABLE))
        current->transport = RELIABLE ;
    else if (!xmlStrcmp(xmlGetProp(cur, ATTRIBUTE_TRANSPORTATION),
                        VALUE_BESTEFFORT))
        current->transport = BEST_EFFORT ;

    // Order
    if (!xmlStrcmp(xmlGetProp(cur, ATTRIBUTE_ORDER), VALUE_TSO))
        current->order = TIMESTAMP ;
    else if (!xmlStrcmp(xmlGetProp(cur, ATTRIBUTE_ORDER), VALUE_RO))
        current->order = RECEIVE ;

    // Routing space
    char *space = (char *) xmlGetProp(cur, ATTRIBUTE_SPACE);
    if (space) {
        SpaceHandle h ;
        try {
            h = root->getRoutingSpaceHandle(string(space));
        }
        catch (Exception &e) {
            cerr << "warning: Incorrect space name for interaction"
                 << endl ;
        }
        current->setSpace(h);
    }

    // Add to interactions list, and build inheritance relation
    root->Interactions->addClass(current);
    if (parent != 0)
        root->Interactions->buildParentRelation(current, parent);

    cur = cur->xmlChildrenNode ;
    while (cur != NULL) {
        if ((!xmlStrcmp(cur->name, NODE_PARAMETER))) {
            Parameter *param = new Parameter();
            param->setName((char *) xmlGetProp(cur, ATTRIBUTE_NAME));
            param->Handle = freeParameterHandle++ ;
            current->addParameter(param);
        }
        // Subinteraction
        if ((!xmlStrcmp(cur->name, NODE_INTERACTION_CLASS))) {
            this->parseInteraction(current);
        }
        cur = cur->next ;
    }
    cur = prev ;
}

// ----------------------------------------------------------------------------
//! Parse a routing space
void
XmlParser::parseRoutingSpace()
{
    D[pdTrace] << "New Routing Space" << endl ;

    DimensionHandle freeDimensionHandle = 1 ;
    xmlNodePtr prev = cur ;
    RoutingSpace *current = new RoutingSpace();
    current->setHandle(freeSpaceHandle++);
    current->setName((char *) xmlGetProp(cur, ATTRIBUTE_NAME));
    root->addRoutingSpace(current);

    // Dimensions
    cur = cur->xmlChildrenNode ;
    while (cur != NULL) {
        if ((!xmlStrcmp(cur->name, NODE_DIMENSION))) {
            Dimension* dimension = new Dimension(freeDimensionHandle++);
            dimension->setName((char *) xmlGetProp(cur, ATTRIBUTE_NAME));
            current->addDimension(dimension);
        }
        cur = cur->next ;
    }
    cur = prev ;
}

// ----------------------------------------------------------------------------
//! is the XML parser available ?
bool
XmlParser::exists()
{
    return true ;
}

} // namespace certi

#else // !HAVE_XML

namespace certi {

XmlParser::XmlParser(RootObject *)
{
};

RootObject *XmlParser::parse(string)
{
    return 0 ;
}

bool XmlParser::exists()
{
    return false ;
}

} // namespace certi

#endif // HAVE_XML

// $Id: XmlParser.cc,v 3.12 2003/06/27 17:26:29 breholee Exp $
