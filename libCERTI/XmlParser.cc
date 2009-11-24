// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003-2006  ONERA
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
// ----------------------------------------------------------------------------

#include "XmlParser.hh"
#include "ObjectClassSet.hh"
#include "InteractionSet.hh"
#include "ObjectClassAttribute.hh"
#include "RoutingSpace.hh"
#include "PrettyDebug.hh"

using std::string ;
using std::cerr ;
using std::endl ;

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

static PrettyDebug D("XMLPARSER", "(XmlParser) ");

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
RootObject*
XmlParser::parse(string pathToXmlFile)
{
    D.Out(pdTrace, "Starting to parse XML file");
    filename = pathToXmlFile;

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

    Debug(D, pdTrace) << "XmlParser: finished parsing" << endl ;
    return root ;
}

// ----------------------------------------------------------------------------
void
XmlParser::parseClass(ObjectClass* parent)
{
    Debug(D, pdTrace) << "New Object Class" << endl ;

    xmlNodePtr prev = cur ;
    /* note how objectHandle counter is incremented */
    ObjectClass* current = new ObjectClass(std::string(CleanXmlGetProp(cur,ATTRIBUTE_NAME)),freeObjectClassHandle++);

    root->addObjectClass(current, parent);
    cur = cur->xmlChildrenNode ;
    while (cur != NULL) {
        // Attributes
        if ((!xmlStrcmp(cur->name, NODE_ATTRIBUTE))) {
            std::string name = std::string(CleanXmlGetProp(cur,ATTRIBUTE_NAME));
            AttributeHandle attributeHandle = current->getHandleClassAttributeMap().size() + 1;

            ObjectClassAttribute *attr = new ObjectClassAttribute(name, attributeHandle);

            // Transportation
            xmlChar* xtransport = xmlGetProp(cur, ATTRIBUTE_TRANSPORTATION);
            if (!xmlStrcmp(xtransport,VALUE_RELIABLE)) {
                attr->transport = RELIABLE ;
            }
            else {
            	if (!xmlStrcmp(xtransport,VALUE_BESTEFFORT)) {
            		attr->transport = BEST_EFFORT ;
            	}
            }
            xmlFree(xtransport);

            // Order
            xmlChar* xorder = xmlGetProp(cur, ATTRIBUTE_ORDER);
            if (!xmlStrcmp(xorder, VALUE_TSO)) {
                attr->order = TIMESTAMP ;
            }
            else {
            	if (!xmlStrcmp(xorder, VALUE_RO)) {
            		attr->order = RECEIVE ;
            	}
            }
            xmlFree(xorder);

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
            xmlFree(space);

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
void
XmlParser::parseInteraction(Interaction* parent)
{
    Debug(D, pdTrace) << "New Interaction Class" << endl;
    std::string name;
    TransportType transport;
    OrderType order;

    xmlNodePtr prev = cur ;

    // Name
    name = std::string(CleanXmlGetProp(cur,ATTRIBUTE_NAME));

    // Transportation
    xmlChar* xtransport = xmlGetProp(cur, ATTRIBUTE_TRANSPORTATION);
    if (!xmlStrcmp(xtransport, VALUE_RELIABLE)) {
        transport = RELIABLE ;
    }
    else {
    	if (!xmlStrcmp(xtransport,VALUE_BESTEFFORT))  {
    		transport = BEST_EFFORT ;
    	}
    }
    xmlFree(xtransport);

    // Order
    xmlChar* xorder = xmlGetProp(cur, ATTRIBUTE_ORDER);
    if (!xmlStrcmp(xorder, VALUE_TSO)) {
        order = TIMESTAMP ;
    }
    else {
    	if (!xmlStrcmp(xorder, VALUE_RO)) {
    		order = RECEIVE ;
    	}
    }
    xmlFree(xorder);
    Interaction* current = new Interaction(name,freeInteractionClassHandle++,transport,order);

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
    xmlFree(space);

    // Add to interactions list, and build inheritance relation
    root->addInteractionClass(current,parent);

    cur = cur->xmlChildrenNode ;
    while (cur != NULL) {
        if ((!xmlStrcmp(cur->name, NODE_PARAMETER))) {
            std::string name(CleanXmlGetProp(cur,ATTRIBUTE_NAME));
            Parameter *param = new Parameter(name, freeParameterHandle++);
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
void
XmlParser::parseRoutingSpace()
{
    Debug(D, pdTrace) << "New Routing Space" << endl ;

    DimensionHandle freeDimensionHandle = 1 ;
    xmlNodePtr prev = cur ;
    RoutingSpace current ;
    current.setHandle(freeSpaceHandle++);
    current.setName(std::string(CleanXmlGetProp(cur,ATTRIBUTE_NAME)));

    // Dimensions
    cur = cur->xmlChildrenNode ;
    while (cur != NULL) {
        if ((!xmlStrcmp(cur->name, NODE_DIMENSION))) {
            Dimension dimension(freeDimensionHandle++);
            dimension.setName(std::string(CleanXmlGetProp(cur,ATTRIBUTE_NAME)));
            current.addDimension(dimension);
        }
        cur = cur->next ;
    }
    // Routing Space should be added after the
    // Dimension has been added since addRoutingSpace store a copy
    // of the object and not a reference
    // see bug #19534
    // https://savannah.nongnu.org/bugs/?19534
    root->addRoutingSpace(current);

    cur = prev ;
}

// ----------------------------------------------------------------------------
bool
XmlParser::exists()
{
    return true;
}

} // namespace certi

#else // !HAVE_XML

namespace certi {

XmlParser::XmlParser(RootObject *)
{
}

RootObject *XmlParser::parse(std::string)
{
    return 0 ;
}

bool XmlParser::exists()
{
    return false ;
}

} // namespace certi

#endif // HAVE_XML
