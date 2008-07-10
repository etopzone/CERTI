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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: fed.cc,v 3.16 2008/07/10 14:05:30 erk Exp $
// ----------------------------------------------------------------------------

// CERTI header
#include "Object.hh"
#include "ObjectClass.hh"
#include "ObjectClassAttribute.hh"
#include "ObjectClassSet.hh"
#include "Interaction.hh"
#include "InteractionSet.hh"
#include "Dimension.hh"
#include "RoutingSpace.hh"
#include "PrettyDebug.hh"

#include "fed.hh"

// Standard headers
#include <iostream>
#include <utility>
#include <string>
#include <list>

using std::cout ;
using std::endl ;
using std::pair ;
using std::string ;
using std::list ;

extern FILE *yyin ;
int yyparse();
void yyrestart(FILE*);

namespace certi {
namespace fedparser {

extern std::string arg ;
extern OrderType order ;
extern TransportType transport ;
extern int line_number ;
static pdCDebug D("FEDPARSER", __FILE__);

static RootObject *root_object = 0 ;
static bool verbose = false ;
static int indentation = 0 ;
const char *fed_filename ;

static std::list<ObjectClass *> objects ;
static std::list<Interaction *> interactions ;
static string federate ;
static ObjectClassAttribute *attribute ;
static Parameter *parameter ;
static RoutingSpace routing_space ;

static int objectHandle = 1 ;
static int interactionHandle = 1 ;
static int attributeHandle = 1 ;
static int parameterHandle = 1 ;
static int spaceHandle = 1 ;
static int dimensionHandle = 1 ;

// ----------------------------------------------------------------------------
int
build(const char *filename, RootObject *root, bool v)
{
    fed_filename = filename ;
    line_number = 1 ;
    verbose = v ;
    root_object = root ;
    FILE *file = fopen(filename, "r");
    if (file == NULL)
	return 1 ;
    yyin = file ;
    indentation = 0 ;
    objectHandle = 1 ;
    interactionHandle = 1 ;
    attributeHandle = 1 ;
    parameterHandle = 1 ;
    spaceHandle = 1 ;
    dimensionHandle = 1 ;
    objects.clear();
    interactions.clear();
    federate = "" ;
    attribute = 0 ;
    parameter = 0 ;
    #ifndef _WIN32 
   /* we may need to restart parsing after previous parse error */
    rewind(yyin);
    yyrestart(yyin);
    #endif
    int result = yyparse();   
    fclose(file);
    return result ;
}

// ----------------------------------------------------------------------------
void
indent()
{
	if(verbose) {
		cout << endl ;
		for (int i = 0 ; i < indentation ; ++i) {
			cout << " " << " " << " " << " " ;
		}
	}
}

// ----------------------------------------------------------------------------
void
startFed()
{
    indent();
    if(verbose) {
	  cout << "(FED";
    }
    ++indentation ;
}

// ----------------------------------------------------------------------------
void
endFed()
{
    --indentation ;
    if(verbose) {
	  cout << ")" << endl << endl ;
    }
}

// ----------------------------------------------------------------------------
void
addFederation()
{
    indent();
    if(verbose) {
	  cout << "(federation \"" << arg.c_str() << "\")" ;
    }
}

// ----------------------------------------------------------------------------
void
addFedVersion()
{
    indent();
    if(verbose) {
	  cout << "(FEDversion \"" << arg.c_str() << "\")" ;
    }
}

// ----------------------------------------------------------------------------
void
startSpaces()
{
    indent();
    if(verbose) {
	  cout << "(spaces" ;
    }
    ++indentation ;
}

// ----------------------------------------------------------------------------
void
end()
{
    --indentation ;
    if(verbose) {
	  cout << ")" ;
    }
}

// ----------------------------------------------------------------------------
void
startObjects()
{
     indent();
     if(verbose) {
	   cout << "(objects" ;
     }
     ++indentation ;
}

// ----------------------------------------------------------------------------
void
startInteractions()
{
     indent(); 
     if(verbose) {
	   cout << "(interactions" ;
     }
     ++indentation ;
}

// ----------------------------------------------------------------------------
void
startFederate()
{
    federate = arg ;
}

// ----------------------------------------------------------------------------
void
endFederate()
{
    SecurityLevelID level = root_object->GetSecurityLevelID(arg.c_str());
    root_object->registerFederate(federate.c_str(), level);
    indent();
    if(verbose) {
	  cout <<"(federate \"" << federate.c_str() << "\" \""	 
		   << arg.c_str() << "\")" ;
    }
}

// ----------------------------------------------------------------------------
void
startObject()
{
    ObjectClass *object = new ObjectClass();
    /* note how objectHandle counter is incremented */
    object->setHandle(objectHandle++);
    object->setName(arg.c_str());

    if (objects.size() > 0) {
    	ObjectClass *parent = objects.back();
    	root_object->ObjectClasses->buildParentRelation(object, parent);
    }
    
    objects.push_back(object);
    root_object->ObjectClasses->addClass(object);

    indent();
    if(verbose) {
	  cout << "(class \"" << arg.c_str() << "\" (id "
		   << object->getHandle()	<< ")" ;
    }
    ++indentation ;
}

// ----------------------------------------------------------------------------
void
addObjectSecLevel(string name)
{
    SecurityLevelID level = root_object->GetSecurityLevelID(name.c_str());
    objects.back()->setLevelId(level);
    indent();
    if(verbose) {
	  cout << "(sec_level \"" << name.c_str() << "\")" ;
    }
}

// ----------------------------------------------------------------------------
void
addAttribute()
{
    attribute = new ObjectClassAttribute();
    attribute->setName(arg.c_str());
    attribute->transport = transport ;
    attribute->order = order ;
    objects.back()->addAttribute(attribute);
    
    indent();
    if(verbose) {
	  cout << "(attribute \"" << arg.c_str() << "\" (id "
		   << attribute->getHandle() << ")" ;
    }
    printTransport();
    printOrder();
    if(verbose) {
	  cout << ")" ;
    }
}

// ----------------------------------------------------------------------------
void
endObject()
{
    if(verbose) {
	  cout << ")" ;
    }
    objects.pop_back();    
    if (objects.size() == 0) attributeHandle = 1 ;
    --indentation ;
}

// ----------------------------------------------------------------------------
void
startInteraction()
{
    Interaction *interaction = new Interaction();
    interaction->setHandle(interactionHandle++);
    interaction->setName(arg.c_str());
    interaction->transport = transport ;
    interaction->order = order ;
    
    if (interactions.size() > 0) {
	Interaction *parent = interactions.back();
	root_object->Interactions->buildParentRelation(interaction, parent);
    }
    interactions.push_back(interaction);
    root_object->Interactions->addClass(interaction);

    indent();
    if(verbose)
	  cout << "(interaction \"" << arg.c_str() << "\" (id "
		   << interaction->getHandle() << ")" ;
    printTransport();
    printOrder();
    ++indentation ;
}

// ----------------------------------------------------------------------------
void
addInteractionSecurityLevel()
{
    SecurityLevelID level = root_object->GetSecurityLevelID(arg.c_str());
    interactions.back()->setLevelId(level);
    indent();
    if(verbose)
	  cout << "(sec_level \"" << arg.c_str() << "\")" ;
}

// ----------------------------------------------------------------------------
void
addObjectSecurityLevel()
{
    SecurityLevelID level = root_object->GetSecurityLevelID(arg.c_str());
    objects.back()->setLevelId(level);
    indent();
    if(verbose) {
	  cout << "(sec_level \"" << arg.c_str() << "\")" ;
    }
}

// ----------------------------------------------------------------------------
void
addParameter()
{
    parameter = new Parameter();
    parameter->setHandle(parameterHandle++);
    parameter->setName(arg.c_str());
    interactions.back()->addParameter(parameter);

    indent();
    if(verbose) {
	  cout << "(parameter \"" << arg.c_str() << "\" (id "
		   << parameter->getHandle() << "))" << std::flush;
    }
}

// ----------------------------------------------------------------------------
void
endInteraction()
{
    if(verbose) {
	  cout << ")" ;
    }
    interactions.pop_back();
    if (interactions.size() == 0) parameterHandle = 1 ;
    --indentation ;
}

// ----------------------------------------------------------------------------
void
printOrder()
{
    switch (order) {
      case RECEIVE: if(verbose)
		cout << " receive" ; break ;
      case TIMESTAMP: if(verbose)
		cout << " timestamp" ; break ;
    }
}

// ----------------------------------------------------------------------------
void
printTransport()
{
    switch (transport) {
      case RELIABLE: if(verbose)
		cout << " reliable" ; break ;
      case BEST_EFFORT: if(verbose)
		cout << " best_effort" ; break ;
    }
}

// ----------------------------------------------------------------------------
void
startSpace()
{
	routing_space = RoutingSpace();
	routing_space.setHandle(spaceHandle++);
	routing_space.setName(arg);
	dimensionHandle = 1 ;

	indent();
	if(verbose) {
		cout << "(space \"" << arg.c_str() << "\" (id " 
		     << routing_space.getHandle() << ")" ;
	}
	++indentation ;
}

// ----------------------------------------------------------------------------
void
endSpace()
{   
    root_object->addRoutingSpace(routing_space);
 
    --indentation ;
    if(verbose) {
	  cout << ")" ;
    }
}

// ----------------------------------------------------------------------------
void
addDimension()
{
    Dimension dimension(dimensionHandle++);
    dimension.setName(arg);
    routing_space.addDimension(dimension);

    indent();
    if(verbose) {
	  cout << "(dimension \"" << arg.c_str() << "\" (id "
		   << dimension.getHandle() << "))" ;
    }
}

}} // namespaces

// $Id: fed.cc,v 3.16 2008/07/10 14:05:30 erk Exp $
