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
#include "XmlParser2010.hh"

using std::string;
using std::cerr;
using std::endl;

namespace certi {

static PrettyDebug D("XMLPARSER", "(XmlParser) ");

XmlParser2010::XmlParser2010(RootObject* root, const bool is_parsing_modules) : XmlParser(root, is_parsing_modules)
{
}

XmlParser2010::~XmlParser2010()
{
}

#ifdef HAVE_XML
xmlChar* getText(xmlNodePtr node)
{
    if (node->children && node->children->content) {
        return node->children->content;
    }
    else {
        throw CouldNotOpenFED("Invalid FOM file structure - Attribute Class not found");
    }
}

std::string XmlParser2010::getName()
{
    xmlNodePtr tempNode = cur->children;
    while (tempNode != NULL) {
        if ((!xmlStrcmp(tempNode->name, ATTRIBUTE_NAME))) {
            return std::string((const char*) getText(tempNode));
        }
        tempNode = tempNode->next;
    }
    return std::string();
}

void XmlParser2010::parseNTOS(HLAntos_t* ntos_p)
{
    //try to load a 1516e FOM file instead
    xmlNodePtr tempNode = cur->children;
    while (tempNode != NULL) {
        if ((!xmlStrcmp(tempNode->name, ATTRIBUTE_NAME))) {
            ntos_p->name = getText(tempNode);
        }
        else // ATTRIBUTE_TRANSPORTATION

            if ((!xmlStrcmp(tempNode->name, ATTRIBUTE_TRANSPORTATION))) {
            ntos_p->transportation = getText(tempNode);
        }
        else if ((!xmlStrcmp(tempNode->name, ATTRIBUTE_ORDER))) {
            ntos_p->order = getText(tempNode);
        }
        else if ((!xmlStrcmp(tempNode->name, ATTRIBUTE_SPACE))) {
            ntos_p->space = getText(tempNode);
        }
        tempNode = tempNode->next;
    }
}

#else // !HAVE_XML

void XmlParser2010::parseNTOS(HLAntos_t* /*ntos_p*/)
{
}

std::string XmlParser2010::getName()
{
    return std::string("");
}
#endif // HAVE_XML

} // namespace certi
