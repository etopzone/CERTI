// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI
//
// CERTI is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// $Id: Federate.cc,v 3.5 2003/03/21 15:06:46 breholee Exp $
// ----------------------------------------------------------------------------

#include "Federate.hh"

namespace certi {
namespace rtig {

// ----------------------------------------------------------------------------
//! A new FederateName is allocated. theLink must have been opened before.
Federate::Federate(const char *the_name, FederateHandle the_handle)
    throw (MemoryExhausted, RTIinternalError)
{
    if ((the_name == NULL) || (the_handle == 0))
        throw RTIinternalError("Bad initialization param for Federate.");

    name = strdup(the_name);
    if (name == NULL) throw MemoryExhausted("Unable to allocate Federate name.");

    handle = the_handle ;

    regulator = false ;
    constrained = false ;
}

// ----------------------------------------------------------------------------
// Destructor

Federate::~Federate(void)
{
    free(name);
}

// ----------------------------------------------------------------------------
// Get attributes

FederateHandle
Federate::getHandle(void) const
{
    return handle ;
}

// ----------------------------------------------------------------------------
//! getName.
const char *
Federate::getName(void) const
{
    return name ;
}

bool
Federate::isConstrained(void) const
{
    return constrained ;
}

bool
Federate::isRegulator(void) const
{
    return regulator ;
}

// ----------------------------------------------------------------------------
// Set attributes

void
Federate::setConstrained(bool c)
{
    constrained = c ;
}

void
Federate::setRegulator(bool r)
{
    regulator = r ;
}

// ---------------------------------------------------------------------------
void
Federate::addSynchronizationLabel(const char * label)
    throw (RTIinternalError)
{
    list<char *>::const_iterator i = synchronizationLabels.begin();
    for (; i != synchronizationLabels.end() ; i++) {
        if (!strcmp((*i), label)) {
            throw RTIinternalError("Synch. label pending in federate.");
        }
    }

    synchronizationLabels.push_back(strdup(label));
}

// ---------------------------------------------------------------------------
void
Federate::removeSynchronizationLabel(const char * label)
    throw (RTIinternalError)
{
    list<char *>::iterator i = synchronizationLabels.begin();
    for (; i != synchronizationLabels.end() ; i++) {
        if (!strcmp((*i), label)) {
            delete (*i) ;
            synchronizationLabels.erase(i);
            return ;
        }
    }

    throw RTIinternalError("Synch. label not in federate.");
}

// ---------------------------------------------------------------------------
bool
Federate::isSynchronizationLabel(const char * label) const
{
    list<char *>::const_iterator i = synchronizationLabels.begin();
    for (; i != synchronizationLabels.end() ; i++) {
        if (!strcmp((*i), label)) {
            return true;
        }
    }

    return false ;
}

}}

// $Id: Federate.cc,v 3.5 2003/03/21 15:06:46 breholee Exp $
