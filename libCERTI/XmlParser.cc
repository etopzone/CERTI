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
#include <algorithm>

using std::string ;
using std::cerr ;
using std::endl ;

#ifdef HAVE_XML

namespace certi {

static PrettyDebug D("XMLPARSER", "(XmlParser) ");

// ----------------------------------------------------------------------------
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
XmlParser::~XmlParser()
{
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
    // JLB just for debug
    // root->display();
    return root ;
}

// ----------------------------------------------------------------------------
void
XmlParser::parseClass(ObjectClass* parent)
{
    Debug(D, pdTrace) << "New Object Class" << endl ;

    xmlNodePtr prev = cur ;

    /* note how objectHandle counter is incremented */
    std::string tmpName = getName();
    if (0==tmpName.length()) {
    	throw CouldNotOpenFED("Current node Name is NULL!!");
    }

    ObjectClass* current = new ObjectClass(tmpName,freeObjectClassHandle++);

    root->addObjectClass(current, parent);
    cur = cur->xmlChildrenNode ;
    while (cur != NULL) {
        // Attributes
        if ((!xmlStrcmp(cur->name, NODE_ATTRIBUTE))) {
            //std::string name = std::string(CleanXmlGetProp(cur,ATTRIBUTE_NAME));

            HLAntos_t objClassProp ;
            objClassProp.name=NULL;
            objClassProp.transportation=NULL;
            objClassProp.order=NULL;
            objClassProp.space=NULL;
            parseNTOS(&objClassProp) ;

            AttributeHandle attributeHandle = current->getHandleClassAttributeMap().size() + 1;
            ObjectClassAttribute *attr = new ObjectClassAttribute( (const char* ) objClassProp.name , attributeHandle);

            // Transportation

            if (!xmlStrcmp(objClassProp.transportation,VALUE_RELIABLE)) {
                attr->transport = RELIABLE ;
            }
            else {
            	if (!xmlStrcmp(objClassProp.transportation,VALUE_BESTEFFORT)) {
            		attr->transport = BEST_EFFORT ;
            	}
            }

            // Order

            if (!xmlStrcmp(objClassProp.order, VALUE_TSO)) {
                attr->order = TIMESTAMP ;
            }
            else {
            	if (!xmlStrcmp(objClassProp.order, VALUE_RO)) {
            		attr->order = RECEIVE ;
            	}
            }

            // Routing space
            char *space = (char *) objClassProp.space ;
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
void
XmlParser::parseInteraction(Interaction* parent)
{
    Debug(D, pdTrace) << "New Interaction Class" << endl;
    std::string name;
    TransportType transport;
    OrderType order;

    xmlNodePtr prev = cur ;

    HLAntos_t intClassProp ;
    intClassProp.name=NULL;
    intClassProp.transportation=NULL;
    intClassProp.order=NULL;
    intClassProp.space=NULL;
    parseNTOS(&intClassProp);
    // Name
    //name = std::string(CleanXmlGetProp(cur,ATTRIBUTE_NAME));

    // Transportation

    if (!xmlStrcmp(intClassProp.transportation, VALUE_RELIABLE)) {
        transport = RELIABLE ;
    }
    else {
    	if (!xmlStrcmp(intClassProp.transportation,VALUE_BESTEFFORT))  {
    		transport = BEST_EFFORT ;
    	}
    }

    // Order
    if (!xmlStrcmp(intClassProp.order, VALUE_TSO)) {
        order = TIMESTAMP ;
    }
    else {
    	if (!xmlStrcmp(intClassProp.order, VALUE_RO)) {
    		order = RECEIVE ;
    	}
    }
    Interaction* current = new Interaction(reinterpret_cast<char*>(intClassProp.name),freeInteractionClassHandle++,transport,order);

    // Routing space
    char *space = (char *) intClassProp.space ;
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
            std::string tmpName = getName();
            Parameter *param = new Parameter(tmpName.c_str(), freeParameterHandle++);
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
    std::string tmpName = getName();
    current.setName(tmpName);

    // Dimensions
    cur = cur->xmlChildrenNode ;
    while (cur != NULL) {
        if ((!xmlStrcmp(cur->name, NODE_DIMENSION))) {
            Dimension dimension(freeDimensionHandle++);
            std::string tmpName = getName();
            dimension.setName(tmpName);
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

XmlParser::HLAXmlStdVersion_t
XmlParser::version (std::string pathToXmlFile){

    // transportation = HLAreliable
    // order = TimeStamp

    xmlDocPtr doc = xmlParseFile(pathToXmlFile.c_str());

    // Did libXML manage to parse the file ?
    if (doc == 0) {
        cerr << "XML file not parsed successfully" << endl ;
        xmlFreeDoc(doc);
        throw CouldNotOpenFED(stringize() << "Could not open File:" << pathToXmlFile) ;
    }

    // Is there a root element ?
    xmlNodePtr cur = xmlDocGetRootElement(doc);
    if (cur == 0) {
        cerr << "XML file is empty" << endl ;
        xmlFreeDoc(doc);
        throw CouldNotOpenFED(stringize() << "XML file:" << pathToXmlFile << "is empty") ;
    }

    // Is this root element an objectModel ?
    if (xmlStrcmp(cur->name, (const xmlChar *) NODE_OBJECT_MODEL)) {
        cerr << "Wrong XML file: not the expected root node" << endl ;
        throw CouldNotOpenFED(stringize() << "XML File:" << pathToXmlFile<< " has an invalid root node") ;
    }
    D.Out(pdTrace, "XML file looks ok, starting main loop");

    // xmlns is not a standard property we read in a different way using cur->ns->href
    if ( cur->ns != NULL ) {
    	if ( xmlStrstr(cur->ns->href, VERSION1516_2010)) {
        	return XmlParser::XML_IEEE1516_2010;
    	}
    }

    // Which XML FOM version
    xmlChar* version  = xmlGetProp(cur, ATTRIBUTE_DTDVERSION);
    if ( version != NULL) {
		if (!xmlStrcmp(version, VERSION1516)) {
			return XmlParser::XML_IEEE1516_2000;
		}
    }

    // If no good, what we do ?
    //std::cout << "found nothing donc XML_LEGACY" << endl ;
    //return XmlParser::XML_LEGACY;

    cur = xmlDocGetRootElement(doc);
    cur = cur->xmlChildrenNode ;
    while (cur != NULL) {
        if ((!xmlStrcmp(cur->name, NODE_OBJECTS))) {
            //xmlNodePtr prev = cur ;
            xmlNodePtr suite = cur->xmlChildrenNode ;
            while (suite != NULL) {
                if ((!xmlStrcmp(suite->name, NODE_OBJECT_CLASS))) {
                	// JLB : method used to recognize the xml version used
                	xmlChar* name  = xmlGetProp(suite, ATTRIBUTE_NAME);
                	if ( name != NULL) {
                		return XmlParser::XML_IEEE1516_2000;
                	}
                	else
                	{
                		// JLB default version if no property found
                		return XmlParser::XML_IEEE1516_2010;
//                		std::cout <<  "trying to found a name class" << endl ;
//                		xmlNodePtr hijo = suite->xmlChildrenNode ;
//                		while (hijo != NULL)
//                		{
//                			std::cout << "debut ++ " << hijo->name << "++" << endl;
//                			//if ((!xmlStrcmp(hijo->name, NODE_NAME))) {
//                			if(!xmlStrcmp(hijo->name,(const xmlChar*)"text")) {
//                				std::cout <<  "Found object name 2010 format " << endl;
//                				std::string strcontent = (const char* )hijo->content;
//                				std::cout << " content:<" << strcontent <<">";
//                				//strcontent.erase(remove_if(strcontent.begin(), strcontent.end(), isspace), strcontent.end());
//                				//if (strcontent.length()>0) std::cout << " content:<" << strcontent <<">";
//                				//return XmlParser::XML_IEEE1516_2010;
//                			}
//                			//}
//                			hijo = hijo->next ;
//                		}
//                		return  XmlParser::XML_LEGACY;
                	}

                }
                suite = suite->next ;
            }
        }
        cur = cur->next ;
    }
    return  XmlParser::XML_LEGACY;
}

//void displayCurrent(xmlNodePtr curNode) {
//
//    xmlNodePtr cur = curNode;
//    while (cur != NULL) {
//    	std::cout << indent();
//    	if(xmlStrcmp(cur->name,(const xmlChar*)"text")) {
//         std::cout << cur->name;
//    	} else {
//    		std::string strcontent = (const char* )cur->content;
//			strcontent.erase(remove_if(strcontent.begin(), strcontent.end(), isspace), strcontent.end());
//          if (strcontent.length()>0) std::cout << " content:<" << strcontent <<">";
//    	}
//        std::cout << std::endl;
//        if ((cur->children) != NULL) {
//            indentLevel++;
//            displayCurrent(cur->children);
//            indentLevel--;
//        }
//        cur = cur->next;
//    }
//}

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

XmlParser::~XmlParser()
{
}

} // namespace certi

#endif // HAVE_XML
