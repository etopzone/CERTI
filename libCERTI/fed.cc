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
// ----------------------------------------------------------------------------

// CERTI header
#include "Dimension.hh"
#include "Interaction.hh"
#include "InteractionSet.hh"
#include "Object.hh"
#include "ObjectClass.hh"
#include "ObjectClassAttribute.hh"
#include "ObjectClassSet.hh"
#include "PrettyDebug.hh"
#include "RoutingSpace.hh"

#include "fed.hh"

// Standard headers
#include <iostream>
#include <list>
#include <string>
#include <utility>

using std::cout;
using std::endl;
using std::pair;
using std::string;
using std::list;

extern FILE* yyin;
int yyparse();
void yyrestart(FILE*);

namespace certi {
namespace fedparser {

extern std::string arg;
extern std::string federationname_arg;
extern std::string federatename_arg;
extern std::string spacename_arg;
extern std::string dimensionname_arg;
extern std::string objectclassname_arg;
extern std::string attributename_arg;
extern std::string interactionclassname_arg;
extern std::string parametername_arg;

extern OrderType order;
extern TransportType transport;
extern int line_number;
static PrettyDebug D("FEDPARSER", __FILE__);

static RootObject* root_object = 0;
static bool verbose = false;
static int indentation = 0;
const char* fed_filename;

/* this is the current object stack */
static std::list<ObjectClass*> objects;

/* this is the current interaction stack */
static std::list<Interaction*> interactions;
static string federate;
static ObjectClassAttribute* attribute;
static Parameter* parameter;
static RoutingSpace routing_space;

static int objectHandle = 1;
static int interactionHandle = 1;
static int attributeHandle = 1;
static int parameterHandle = 1;
static int spaceHandle = 1;
static int dimensionHandle = 1;

// ----------------------------------------------------------------------------
int build(const char* filename, RootObject* root, bool verboseArg)
{
    fed_filename = filename;
    line_number = 1;
    verbose = verboseArg;
    root_object = root;
    FILE* file = fopen(filename, "r");
    if (file == NULL)
        return 1;
    yyin = file;
    indentation = 0;
    objectHandle = 1;
    interactionHandle = 1;
    attributeHandle = 1;
    parameterHandle = 1;
    spaceHandle = 1;
    dimensionHandle = 1;
    objects.clear();
    interactions.clear();
    federate = "";
    attribute = 0;
    parameter = 0;
#ifndef _WIN32
    /* we may need to restart parsing after previous parse error */
    rewind(yyin);
    yyrestart(yyin);
#endif
    int result = yyparse();
    fclose(file);
    return result;
}

// ----------------------------------------------------------------------------
void indent()
{
    if (verbose) {
        cout << endl;
        for (int i = 0; i < indentation; ++i) {
            cout << " "
                 << " "
                 << " "
                 << " ";
        }
    }
}

// ----------------------------------------------------------------------------
void startFed()
{
    indent();
    if (verbose) {
        cout << "(FED";
    }
    ++indentation;
}

// ----------------------------------------------------------------------------
void endFed()
{
    --indentation;
    if (verbose) {
        indent();
        cout << ")" << endl;
    }
}

// ----------------------------------------------------------------------------
void addFederation()
{
    indent();
    if (verbose) {
        cout << "(Federation \"" << federationname_arg << "\")";
    }
}

// ----------------------------------------------------------------------------
void addFedVersion()
{
    indent();
    if (verbose) {
        cout << "(FEDversion \"" << arg << "\")";
    }
}

// ----------------------------------------------------------------------------
void startSpaces()
{
    indent();
    if (verbose) {
        cout << "(spaces";
    }
    ++indentation;
}

void checkSpaceName()
{
    // FIXME no check TDB
    if (verbose) {
        cout << spacename_arg;
    }
}

// ----------------------------------------------------------------------------
void end()
{
    --indentation;
    if (verbose) {
        indent();
        cout << ")";
    }
}

// ----------------------------------------------------------------------------
void startObjects()
{
    indent();
    if (verbose) {
        cout << "(objects";
    }
    ++indentation;
}

// ----------------------------------------------------------------------------
void startInteractions()
{
    indent();
    if (verbose) {
        cout << "(interactions";
    }
    ++indentation;
}

// ----------------------------------------------------------------------------
void startFederate()
{
    federate = federatename_arg;
}

// ----------------------------------------------------------------------------
void endFederate()
{
    SecurityLevelID level = root_object->getSecurityLevelID(arg);
    root_object->registerFederate(federate, level);
    indent();
    if (verbose) {
        cout << "(federate \"" << federate << "\" \"" << arg << "\")" << endl;
    }
}

// ----------------------------------------------------------------------------
void startObject()
{
    ObjectClass* parent = NULL;
    /* note how objectHandle counter is incremented */
    ObjectClass* object = new ObjectClass(objectclassname_arg, objectHandle++);

    /*
	 * Does this object class have a parent [object] class?
	 */
    if (objects.size() > 0) {
        parent = objects.back();
    }

    /* add current on object stack */
    objects.push_back(object);

    /* add new object to the RootObject */
    root_object->addObjectClass(object, parent);

    indent();
    if (verbose) {
        cout << "(class \"" << objectclassname_arg << "\" (id " << object->getHandle() << ")";
    }
    ++indentation;
}

// ----------------------------------------------------------------------------
void addObjectSecLevel(string name)
{
    SecurityLevelID level = root_object->getSecurityLevelID(name);
    objects.back()->setSecurityLevelId(level);
    indent();
    if (verbose) {
        cout << "(sec_level \"" << name << "\")";
    }
}

// ----------------------------------------------------------------------------
void addAttribute()
{
    AttributeHandle attributeHandle = objects.back()->getHandleClassAttributeMap().size() + 1;
    attribute = new ObjectClassAttribute(attributename_arg, attributeHandle);
    attribute->transport = transport;
    attribute->order = order;
    objects.back()->addAttribute(attribute);

    indent();
    if (verbose) {
        cout << "(attribute \"" << attributename_arg << "\" (id " << attribute->getHandle() << ")";
    }
    printTransport();
    printOrder();
    if (verbose) {
        cout << ")";
    }
}

// ----------------------------------------------------------------------------
void endObject()
{
    objects.pop_back();
    if (objects.size() == 0)
        attributeHandle = 1;
    --indentation;
    if (verbose) {
        indent();
        cout << ")";
    }
}

// ----------------------------------------------------------------------------
void startInteraction()
{
    Interaction* parent = NULL;
    Interaction* interaction = new Interaction(interactionclassname_arg, interactionHandle++, transport, order);

    /* does this interaction class have a parent ? */
    if (interactions.size() > 0) {
        parent = interactions.back();
    }

    root_object->addInteractionClass(interaction, parent);
    /* put the freshly created interaction on the interaction stack
     * (for inheritance handle)
     */
    interactions.push_back(interaction);

    indent();
    if (verbose)
        cout << "(interaction \"" << interactionclassname_arg << "\" (id " << interaction->getHandle() << ")";
    printTransport();
    printOrder();
    ++indentation;
}

// ----------------------------------------------------------------------------
void addInteractionSecurityLevel()
{
    SecurityLevelID level = root_object->getSecurityLevelID(arg);
    interactions.back()->setSecurityLevelId(level);
    indent();
    if (verbose)
        cout << "(sec_level \"" << arg << "\")";
}

// ----------------------------------------------------------------------------
void addObjectSecurityLevel()
{
    SecurityLevelID level = root_object->getSecurityLevelID(arg);
    objects.back()->setSecurityLevelId(level);
    indent();
    if (verbose) {
        cout << "(sec_level \"" << arg << "\")";
    }
}

// ----------------------------------------------------------------------------
void addParameter()
{
    parameter = new Parameter(parametername_arg, parameterHandle++);
    interactions.back()->addParameter(parameter);

    indent();
    if (verbose) {
        cout << "(parameter \"" << parametername_arg << "\" (id " << parameter->getHandle() << "))" << std::flush;
    }
}

// ----------------------------------------------------------------------------
void endInteraction()
{
    interactions.pop_back();
    if (interactions.size() == 0)
        parameterHandle = 1;
    --indentation;
    if (verbose) {
        indent();
        cout << ")";
    }
}

// ----------------------------------------------------------------------------
void printOrder()
{
    switch (order) {
    case RECEIVE:
        if (verbose)
            cout << " receive";
        break;
    case TIMESTAMP:
        if (verbose)
            cout << " timestamp";
        break;
    }
}

// ----------------------------------------------------------------------------
void printTransport()
{
    switch (transport) {
    case RELIABLE:
        if (verbose) {
            cout << " reliable";
        }
        break;
    case BEST_EFFORT:
        if (verbose) {
            cout << " best_effort";
        }
        break;
    }
}

// ----------------------------------------------------------------------------
void startSpace()
{
    routing_space = RoutingSpace();
    routing_space.setHandle(spaceHandle++);
    routing_space.setName(spacename_arg);
    dimensionHandle = 1;

    indent();
    if (verbose) {
        cout << "(space \"" << spacename_arg << "\" (id " << routing_space.getHandle() << ")";
    }
    ++indentation;
}

// ----------------------------------------------------------------------------
void endSpace()
{
    root_object->addRoutingSpace(routing_space);

    --indentation;
    if (verbose) {
        indent();
        cout << ")";
    }
}

// ----------------------------------------------------------------------------
void addDimension()
{
    Dimension dimension(dimensionHandle++);
    dimension.setName(dimensionname_arg);
    routing_space.addDimension(dimension);

    indent();
    if (verbose) {
        cout << "(dimension \"" << dimensionname_arg << "\" (id " << dimension.getHandle() << "))";
    }
}
}
} // namespaces
