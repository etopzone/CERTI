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
// $Id: XmlParser.hh,v 3.12 2007/08/31 13:43:20 erk Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_XML_PARSER_HH
#define _CERTI_XML_PARSER_HH

// Project
#include "RootObject.hh"
#include "ObjectClass.hh"
#include "Interaction.hh"

// Libraries
#ifdef HAVE_XML
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

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
#define NODE_NAME (const xmlChar*) "name"

#define ATTRIBUTE_DTDVERSION  (const xmlChar*) "DTDversion"
#define ATTRIBUTE_XMLNSVERSION  (const xmlChar*) "xmlns"
#define ATTRIBUTE_NAME (const xmlChar*) "name"
#define ATTRIBUTE_TRANSPORTATION (const xmlChar*) "transportation"
#define ATTRIBUTE_ORDER (const xmlChar*) "order"
#define ATTRIBUTE_SPACE (const xmlChar*) "space"

#define VALUE_RELIABLE (const xmlChar*) "HLAreliable"
#define VALUE_BESTEFFORT (const xmlChar*) "HLAbestEffort"
#define VALUE_TSO (const xmlChar*) "TimeStamp"
#define VALUE_RO (const xmlChar*) "Receive"

#define VERSION1516      (const xmlChar*)"1516.2"
#define VERSION1516_2010 (const xmlChar*)"1516-2010"
#endif


// Standard libraries
#include <string>

namespace certi {

/**
 * The CERTI XML federation Object Model parser.
 */
class CERTI_EXPORT XmlParser
{
public:
	/**
	 * Different type of XML files.
	 * The parse will guess which type of file it has been given.
	 */
	typedef enum HLAXmlStdVersion {XML_LEGACY, XML_IEEE1516_2000, XML_IEEE1516_2010}
	HLAXmlStdVersion_t;


	/**
	 * Build a parser.
	 * @param root the root object of the FOM.
	 */
	XmlParser(RootObject* root);
	
	/**
	 * Destructor.
	 */
	virtual ~XmlParser();

	/** 
	 * Main method to parse .xml FOM file
	 * @param[in] pathToXmlFile the path to the XML file.
	 * @return the RootObject resulting from the parse. 
	 */	   
    RootObject* parse(std::string pathToXmlFile);
  
    /**
     * Return true if the XML parser is available.
     * XML Parser may not be available if CERTI was
     * compiled without XML support. In this case you
     * should use the .fed file parser.
     * @return true if the XML parser is available
     */
    static bool exists(void);

    /**
     * give the version of the xml FOM file to be parsed
     * it could be 1516 ou 1516-2010
     * @param[in] pathToXmlFile the path to the XML file.
     * @return a string containing the version of the FOM file
     */
    static HLAXmlStdVersion_t version (std::string pathToXmlFile);
    
#if HAVE_XML
    /**
     * XmlParser Inner class used to "cleany"
     * get a property without memory leak.
     */
	class CleanXmlGetProp {
	public:
		/**
		 * Build a property object which
		 * may be casted out in a const char*
		 * @param[in] node the xml node
		 * @param[in] propName the property name 
		 */
		CleanXmlGetProp(_xmlNode* node, const xmlChar* propName) {
			prop = xmlGetProp(node,propName);
		}
		/**
		 * Cast operator from CleanXmlGetProp to const char*
		 */
		operator const char*() {
			return reinterpret_cast<const char*>(prop);
		}
		
		~CleanXmlGetProp(){
			xmlFree(prop);
		}

	protected:
		xmlChar* prop;
	}; // end class CleanXmlGetProp
#endif

protected:
#ifdef HAVE_XML
	/**
	 * Parse the current class node.
	 * @param[in,out] parent the parent object class
	 */
    virtual void parseClass(ObjectClass *parent);
    
    /**
     * Parse the current interaction node
     * @param[in,out] parent the parent interaction node
     */
    virtual void parseInteraction(Interaction *parent);
    
    /** 
     * Parse a routing space from current node.
     */
    virtual void parseRoutingSpace(void);

    /**
     *  Infos we need to retrieve in the xml file according to the xml version used
     *  ntos stand for name, transportation, order, space
     */
    typedef struct ntos {
        xmlChar* name;
        xmlChar* transportation;
        xmlChar* order;
        xmlChar* space;
    } HLAntos_t ;
#else
    /**
     *  Infos we need to retrieve in the xml file according to the xml version used
     *  ntos stand for name, transportation, order, space
     */
    typedef struct ntos {
        std::string name;
        std::string transportation;
        std::string order;
        std::string space;
    } HLAntos_t ;
#endif

    /**
     * ParseNTOS get name transportation order and space
     * @param[out] name transportation order and space
     * @return nothing
     */
	virtual void parseNTOS(HLAntos_t *ntos_p)=0;

	/**
	 * Get the name of the current node.
	 * @return the name
	 */
	virtual std::string getName()=0;

    int freeObjectClassHandle ;
    int freeInteractionClassHandle ;
    int freeAttributeHandle ;
    int freeParameterHandle ;
    int freeSpaceHandle ;

    std::string filename ;
    RootObject* root ;
    
#ifdef HAVE_XML
    xmlDocPtr  doc;
    xmlNodePtr cur;    
#endif 
};

} // namespace certi

#include "XmlParser2000.hh"
#include "XmlParser2010.hh"

#endif // _CERTI_XML_PARSER_HH

// $Id: XmlParser.hh,v 3.12 2007/08/31 13:43:20 erk Exp $
