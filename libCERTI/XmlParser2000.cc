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
#include "XmlParser2000.hh"
#include "ObjectClassSet.hh"
#include "InteractionSet.hh"
#include "ObjectClassAttribute.hh"
#include "RoutingSpace.hh"
#include "PrettyDebug.hh"

using std::string ;
using std::cerr ;
using std::endl ;


namespace certi {

static PrettyDebug D("XMLPARSER", "(XmlParser) ");

XmlParser2000::XmlParser2000(RootObject* r): XmlParser(r)
{
}

XmlParser2000::~XmlParser2000()
{}

#ifdef HAVE_XML
void XmlParser2000::parseNTOS(HLAntos_t  *ntos_p)
{
	ntos_p->name = xmlGetProp(cur, ATTRIBUTE_NAME);
	ntos_p->transportation = xmlGetProp(cur, ATTRIBUTE_TRANSPORTATION);
	ntos_p->order = xmlGetProp(cur, ATTRIBUTE_ORDER);
	ntos_p->space = xmlGetProp(cur, ATTRIBUTE_SPACE);
}

std::string XmlParser2000::getName()
{
	return  std::string((const char*)xmlGetProp(cur, ATTRIBUTE_NAME));
}

#else // !HAVE_XML

void XmlParser2000::parseNTOS(HLAntos_t  *ntos_p)
{
}

std::string XmlParser2000::getName()
{
	return  std::string("");
}

#endif // HAVE_XML

} // namespace certi

