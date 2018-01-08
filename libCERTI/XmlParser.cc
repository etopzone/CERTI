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

#include "InteractionSet.hh"
#include "ObjectClassAttribute.hh"
#include "ObjectClassSet.hh"
#include "PrettyDebug.hh"
#include "RoutingSpace.hh"
#include "XmlParser.hh"
#include <algorithm>

using std::string;
using std::cerr;
using std::endl;

#ifdef HAVE_XML

namespace certi {

static PrettyDebug D("XMLPARSER", "(XmlParser) ");

// ----------------------------------------------------------------------------
XmlParser::XmlParser(RootObject* r, const bool is_parsing_module) : root{r}, my_is_parsing_module{is_parsing_module}
{
}

// ----------------------------------------------------------------------------
XmlParser::~XmlParser()
{
}

// ----------------------------------------------------------------------------
RootObject* XmlParser::parse(string pathToXmlFile)
{
    Debug(D, pdTrace) << "Starting to parse XML file" << std::endl;
    filename = pathToXmlFile;

    // transportation = HLAreliable
    // order = TimeStamp

    doc = xmlParseFile(filename.c_str());

    // Did libXML manage to parse the file ?
    if (doc == 0) {
        cerr << "XML file not parsed successfully" << endl;
        xmlFreeDoc(doc);
        return 0;
    }

    // Is there a root element ?
    cur = xmlDocGetRootElement(doc);
    if (cur == 0) {
        cerr << "XML file is empty" << endl;
        xmlFreeDoc(doc);
        return 0;
    }

    // Is this root element an objectModel ?
    if (xmlStrcmp(cur->name, (const xmlChar*) NODE_OBJECT_MODEL)) {
        cerr << "Wrong XML file: not the expected root node" << endl;
        return 0;
    }
    Debug(D, pdTrace) << "XML file looks ok, starting main loop" << std::endl;

    // First Loop (Routing Spaces)
    cur = xmlDocGetRootElement(doc);
    cur = cur->xmlChildrenNode;
    while (cur != nullptr) {
        if ((!xmlStrcmp(cur->name, NODE_ROUTING_SPACE))) {
            Debug(D, pdTrace) << "Found a routing space" << std::endl;
            xmlNodePtr prev = cur;
            this->parseRoutingSpace();
            cur = prev;
        }
        cur = cur->next;
    }

    // Second Loop (Object and Interaction classes)
    cur = xmlDocGetRootElement(doc);
    cur = cur->xmlChildrenNode;
    while (cur != nullptr) {
        if ((!xmlStrcmp(cur->name, NODE_OBJECTS))) {
            Debug(D, pdTrace) << "Found a group of object classes" << std::endl;
            xmlNodePtr prev = cur;
            cur = cur->xmlChildrenNode;
            while (cur != nullptr) {
                if ((!xmlStrcmp(cur->name, NODE_OBJECT_CLASS))) {
                    parseClass(nullptr);
                }
                cur = cur->next;
            }
            cur = prev;
        }
        if ((!xmlStrcmp(cur->name, NODE_INTERACTIONS))) {
            Debug(D, pdTrace) << "Found a group of interaction classes" << std::endl;
            xmlNodePtr prev = cur;
            cur = cur->xmlChildrenNode;
            while (cur != nullptr) {
                if ((!xmlStrcmp(cur->name, NODE_INTERACTION_CLASS))) {
                    parseInteraction(nullptr);
                }
                cur = cur->next;
            }
            cur = prev;
        }
        cur = cur->next;
    }

    xmlFreeDoc(doc);

    Debug(D, pdTrace) << "XmlParser: finished parsing" << endl;
    // JLB just for debug
    // root->display();
    return root;
}

// ----------------------------------------------------------------------------
void XmlParser::parseClass(ObjectClass* parent)
{
    Debug(D, pdTrace) << "New Object Class" << endl;

    bool class_already_exists = false;

    xmlNodePtr prev = cur;

    /* note how objectHandle counter is incremented */
    std::string name = getName();
    if (name.empty()) {
        throw CouldNotOpenFED("Current node Name is nullptr!!");
    }

    auto current = new ObjectClass(name, root->getFreeObjectClassHandle());

    try {
        root->addObjectClass(current, parent);
    }
    catch (RTIinternalError& e) {
        if (my_is_parsing_module) {
            // The class already exists. point to it and do not add attributes
            class_already_exists = true;
            current = root->getObjectClass(root->ObjectClasses->getObjectClassHandle(name));
        }
        else {
            throw;
        }
    }
    cur = cur->xmlChildrenNode;
    while (cur != nullptr) {
        // Attributes
        if ((!xmlStrcmp(cur->name, NODE_ATTRIBUTE))) {
            if (!class_already_exists) {
                //std::string name = std::string(CleanXmlGetProp(cur,ATTRIBUTE_NAME));

                HLAntos_t objClassProp;
                objClassProp.name = nullptr;
                objClassProp.transportation = nullptr;
                objClassProp.order = nullptr;
                objClassProp.space = nullptr;
                parseNTOS(&objClassProp);

                AttributeHandle attributeHandle = current->getHandleClassAttributeMap().size() + 1;
                ObjectClassAttribute* attr = new ObjectClassAttribute((const char*) objClassProp.name, attributeHandle);

                // Transportation

                if (!xmlStrcmp(objClassProp.transportation, VALUE_RELIABLE)) {
                    attr->transport = RELIABLE;
                }
                else {
                    if (!xmlStrcmp(objClassProp.transportation, VALUE_BESTEFFORT)) {
                        attr->transport = BEST_EFFORT;
                    }
                }

                // Order

                if (!xmlStrcmp(objClassProp.order, VALUE_TSO)) {
                    attr->order = TIMESTAMP;
                }
                else {
                    if (!xmlStrcmp(objClassProp.order, VALUE_RO)) {
                        attr->order = RECEIVE;
                    }
                }

                // Routing space
                char* space = (char*) objClassProp.space;
                if (space) {
                    try {
                        SpaceHandle h = root->getRoutingSpaceHandle(string(space));
                        attr->setSpace(h);
                    }
                    catch (Exception& e) {
                        cerr << "warning: Incorrect space name for attribute" << endl;
                    }
                }

                // Attribute complete, adding to the class
                current->addAttribute(attr);
            }
        }
        // Subclass
        if ((!xmlStrcmp(cur->name, NODE_OBJECT_CLASS))) {
            this->parseClass(current);
        }
        cur = cur->next;
    }
    cur = prev;
}

// ----------------------------------------------------------------------------
void XmlParser::parseInteraction(Interaction* parent)
{
    Debug(D, pdTrace) << "New Interaction Class" << endl;

    bool interaction_already_exists = false;

    std::string name; // FIXME never used?

    xmlNodePtr prev = cur;

    HLAntos_t intClassProp;
    intClassProp.name = nullptr;
    intClassProp.transportation = nullptr;
    intClassProp.order = nullptr;
    intClassProp.space = nullptr;
    parseNTOS(&intClassProp);
    // Name
    //name = std::string(CleanXmlGetProp(cur,ATTRIBUTE_NAME));

    // Transportation
    TransportType transport{RELIABLE};
    if (!xmlStrcmp(intClassProp.transportation, VALUE_RELIABLE)) {
        transport = RELIABLE;
    }
    else if (!xmlStrcmp(intClassProp.transportation, VALUE_BESTEFFORT)) {
        transport = BEST_EFFORT;
    }
    else {
        cerr << "warning: No transportation provided, defaulting to reliable" << endl;
    }

    // Order
    OrderType order{TIMESTAMP};
    if (!xmlStrcmp(intClassProp.order, VALUE_TSO)) {
        order = TIMESTAMP;
    }
    else if (!xmlStrcmp(intClassProp.order, VALUE_RO)) {
        order = RECEIVE;
    }
    else {
        cerr << "warning: No order provided, defaulting to TSO" << endl;
    }

    Interaction* current = new Interaction(
        reinterpret_cast<char*>(intClassProp.name), root->getFreeInteractionClassHandle(), transport, order);

    // Routing space
    char* space = (char*) intClassProp.space;
    if (space) {
        try {
            SpaceHandle h = root->getRoutingSpaceHandle(string(space));
            current->setSpace(h);
        }
        catch (Exception& e) {
            cerr << "warning: Incorrect space name for interaction" << endl;
        }
    }
    xmlFree(space);

    // Add to interactions list, and build inheritance relation

    try {
        root->addInteractionClass(current, parent);
    }
    catch (RTIinternalError& e) {
        if (my_is_parsing_module) {
            // The interaction already exists. point to it and do not add parameters
            interaction_already_exists = true;
            current = root->getInteractionClass(
                root->Interactions->getInteractionClassHandle(reinterpret_cast<char*>(intClassProp.name)));
        }
        else {
            throw;
        }
    }

    cur = cur->xmlChildrenNode;
    while (cur != nullptr) {
        if ((!xmlStrcmp(cur->name, NODE_PARAMETER))) {
            if (!interaction_already_exists) {
                std::string tmpName = getName();
                Parameter* param = new Parameter(tmpName.c_str(), root->getFreeParameterHandle());
                current->addParameter(param);
            }
        }
        // Subinteraction
        if ((!xmlStrcmp(cur->name, NODE_INTERACTION_CLASS))) {
            this->parseInteraction(current);
        }
        cur = cur->next;
    }
    cur = prev;
}

// ----------------------------------------------------------------------------
void XmlParser::parseRoutingSpace()
{
    Debug(D, pdTrace) << "New Routing Space" << endl;

    xmlNodePtr prev = cur;
    RoutingSpace current;
    current.setHandle(root->getFreeSpaceHandle());
    std::string tmpName = getName();
    current.setName(tmpName);

    // Dimensions
    cur = cur->xmlChildrenNode;
    while (cur != nullptr) {
        if ((!xmlStrcmp(cur->name, NODE_DIMENSION))) {
            Dimension dimension(root->getFreeDimensionHandle());
            std::string tmpName = getName();
            dimension.setName(tmpName);
            current.addDimension(dimension);
        }
        cur = cur->next;
    }
    // Routing Space should be added after the
    // Dimension has been added since addRoutingSpace store a copy
    // of the object and not a reference
    // see bug #19534
    // https://savannah.nongnu.org/bugs/?19534
    root->addRoutingSpace(current);

    cur = prev;
}

// ----------------------------------------------------------------------------
bool XmlParser::exists()
{
    return true;
}

XmlParser::HLAXmlStdVersion_t XmlParser::version(std::string pathToXmlFile)
{
    // transportation = HLAreliable
    // order = TimeStamp

    xmlDocPtr doc = xmlParseFile(pathToXmlFile.c_str());

    // Did libXML manage to parse the file ?
    if (doc == 0) {
        cerr << "XML file not parsed successfully" << endl;
        xmlFreeDoc(doc);
        throw CouldNotOpenFED("Could not open File:" + pathToXmlFile);
    }

    // Is there a root element ?
    xmlNodePtr cur = xmlDocGetRootElement(doc);
    if (cur == 0) {
        cerr << "XML file is empty" << endl;
        xmlFreeDoc(doc);
        throw CouldNotOpenFED("XML file:" + pathToXmlFile + "is empty");
    }

    // Is this root element an objectModel ?
    if (xmlStrcmp(cur->name, (const xmlChar*) NODE_OBJECT_MODEL)) {
        cerr << "Wrong XML file: not the expected root node" << endl;
        throw CouldNotOpenFED("XML File:" + pathToXmlFile + " has an invalid root node");
    }
    Debug(D, pdTrace) << "XML file looks ok, starting main loop" << std::endl;

    // xmlns is not a standard property we read in a different way using cur->ns->href
    if (cur->ns != nullptr) {
        if (xmlStrstr(cur->ns->href, VERSION1516_2010)) {
            return XmlParser::XML_IEEE1516_2010;
        }
    }

    // Which XML FOM version
    xmlChar* version = xmlGetProp(cur, ATTRIBUTE_DTDVERSION);
    if (version != nullptr) {
        if (!xmlStrcmp(version, VERSION1516)) {
            return XmlParser::XML_IEEE1516_2000;
        }
    }

    // If no good, what we do ?
    //std::cout << "found nothing donc XML_LEGACY" << endl ;
    //return XmlParser::XML_LEGACY;

    cur = xmlDocGetRootElement(doc);
    cur = cur->xmlChildrenNode;
    while (cur != nullptr) {
        if ((!xmlStrcmp(cur->name, NODE_OBJECTS))) {
            //xmlNodePtr prev = cur ;
            xmlNodePtr suite = cur->xmlChildrenNode;
            while (suite != nullptr) {
                if ((!xmlStrcmp(suite->name, NODE_OBJECT_CLASS))) {
                    // JLB : method used to recognize the xml version used
                    xmlChar* name = xmlGetProp(suite, ATTRIBUTE_NAME);
                    if (name != nullptr) {
                        return XmlParser::XML_IEEE1516_2000;
                    }
                    else {
                        // JLB default version if no property found
                        return XmlParser::XML_IEEE1516_2010;
                        //                		std::cout <<  "trying to found a name class" << endl ;
                        //                		xmlNodePtr hijo = suite->xmlChildrenNode ;
                        //                		while (hijo != nullptr)
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
                suite = suite->next;
            }
        }
        cur = cur->next;
    }
    return XmlParser::XML_LEGACY;
}

//void displayCurrent(xmlNodePtr curNode) {
//
//    xmlNodePtr cur = curNode;
//    while (cur != nullptr) {
//    	std::cout << indent();
//    	if(xmlStrcmp(cur->name,(const xmlChar*)"text")) {
//         std::cout << cur->name;
//    	} else {
//    		std::string strcontent = (const char* )cur->content;
//			strcontent.erase(remove_if(strcontent.begin(), strcontent.end(), isspace), strcontent.end());
//          if (strcontent.length()>0) std::cout << " content:<" << strcontent <<">";
//    	}
//        std::cout << std::endl;
//        if ((cur->children) != nullptr) {
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

XmlParser::XmlParser(RootObject*, const bool)
{
}

RootObject* XmlParser::parse(std::string)
{
    return 0;
}

bool XmlParser::exists()
{
    return false;
}

XmlParser::~XmlParser()
{
}

} // namespace certi

#endif // HAVE_XML
