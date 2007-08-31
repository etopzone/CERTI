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
	 * Build a parser.
	 * @param root the root object of the FOM.
	 */
	XmlParser(RootObject* root);
	
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
	private:
		xmlChar* prop;
	};
#endif
private:
	/**
	 * Parse the current class node.
	 * @param[in,out] parent the parent object class
	 */
    void parseClass(ObjectClass *parent);
    
    /**
     * Parse the current interaction node
     * @param[in,out] parent the parent interaction node
     */
    void parseInteraction(Interaction *parent);
    
    /** 
     * Parse a routing space from current node.
     */
    void parseRoutingSpace(void);

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

#endif // _CERTI_XML_PARSER_HH

// $Id: XmlParser.hh,v 3.12 2007/08/31 13:43:20 erk Exp $
