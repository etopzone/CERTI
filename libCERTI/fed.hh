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
// ----------------------------------------------------------------------------

#ifndef _FED_HH
#define _FED_HH

#include "RootObject.hh"

namespace certi {
namespace fedparser {

/**
 * Main function for parsing FED file.
 * @param[in] filename the FED file to be parsed
 * @param[out] the RootObject to build during the FED file parse
 * @param[in] verboseArg, if true the parsed element will printed on stdout.
 */
CERTI_EXPORT
int build(const char* filename, RootObject* root, bool verboseArg);

// internal functions
void indent();
void printOrder();
void printTransport();

// parser requests
void startFed();
void startFederate();
void startInteraction();
void startInteractions();
void startObject();
void startObjects();
void startSpace();
void startSpaces();
void checkSpaceName();

void addAttribute();
void addDimension();
void addFederation();
void addFedVersion();
void addInteractionSecurityLevel();
void addObjectSecurityLevel();
void addParameter();

void end();
void endFed();
void endFederate();
void endInteraction();
void endObject();
void endSpace();
}
}

#endif // _FED_HH
