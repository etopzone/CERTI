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
// $Id: Federate.cc,v 3.9 2003/05/23 16:40:04 breholee Exp $
// ----------------------------------------------------------------------------

#include "Federate.hh"

// Project
#include <config.h>

// Standard
#include <cstring>
#include <string>

using std::string ;
using std::list ;

namespace certi {
namespace rtig {

// ----------------------------------------------------------------------------
//! A new FederateName is allocated. theLink must have been opened before.
Federate::Federate(const char *the_name, FederateHandle the_handle)
    throw (MemoryExhausted, RTIinternalError)
    : handle(the_handle), regulator(false), constrained(false), saving(false),
      restoring(false)

{
    if ((the_name == NULL) || (handle == 0))
        throw RTIinternalError("Bad initialization param for Federate.");

    name = strdup(the_name);
    if (name == NULL) throw MemoryExhausted("Unable to allocate Federate name.");
}

// ----------------------------------------------------------------------------
// Destructor

Federate::~Federate()
{
    free(name);
}

// ----------------------------------------------------------------------------
//! Returns the federate handle.
FederateHandle
Federate::getHandle() const
{
    return handle ;
}

// ----------------------------------------------------------------------------
//! Changes the federate handle.
/*! This service can be used for a restore from previous federation save.
  Federates recover their previous handle.
*/
void
Federate::setHandle(FederateHandle the_handle)
{
    handle = the_handle ;
}

// ----------------------------------------------------------------------------
//! Returns the federate name pointer.
const char *
Federate::getName() const
{
    return name ;
}

// ----------------------------------------------------------------------------
//! Returns the federate constrained state.
bool
Federate::isConstrained() const
{
    return constrained ;
}

// ----------------------------------------------------------------------------
//! Returns the federate regulating state.
bool
Federate::isRegulator() const
{
    return regulator ;
}

// ----------------------------------------------------------------------------
//! Changes the federate constrained state.
void
Federate::setConstrained(bool c)
{
    constrained = c ;
}

// ----------------------------------------------------------------------------
//! Changes the federate regulating state.
void
Federate::setRegulator(bool r)
{
    regulator = r ;
}

// ----------------------------------------------------------------------------
//! Returns the federate saving state.
bool
Federate::isSaving() const
{
    return saving ;
}

// ----------------------------------------------------------------------------
//! Returns the federate restoring state.
bool
Federate::isRestoring() const
{
    return restoring ;
}

// ----------------------------------------------------------------------------
//! Put the federate in saving state.
/*! This service is called by Federation saving services 'Request Federation
  Save' and 'Federate Save Status'.
*/
void
Federate::setSaving(bool save_status)
{
    saving = save_status ;
}

// ----------------------------------------------------------------------------
//! Put the federate in saving state.
/*! This service is called by Federation restoring services 'Request Federation
  Restore' and 'Federate Restore Status'.
*/
void
Federate::setRestoring(bool restore_status)
{
    restoring = restore_status ;
}

// ----------------------------------------------------------------------------
//! Add a synchronization label to federate.
void
Federate::addSynchronizationLabel(const char *label)
    throw (RTIinternalError)
{
    list<char *>::const_iterator i = synchronizationLabels.begin();
    for (; i != synchronizationLabels.end(); i++) {
        if (!strcmp((*i), label)) {
            throw RTIinternalError("Synch. label pending in federate.");
        }
    }

    char *copy = new char[strlen(label) + 1] ;
    strcpy(copy, label);

    synchronizationLabels.push_back(copy);
}

// ----------------------------------------------------------------------------
//! Removes a synchronization label from federate.
void
Federate::removeSynchronizationLabel(const char *label)
    throw (RTIinternalError)
{
    list<char *>::iterator i = synchronizationLabels.begin();
    for (; i != synchronizationLabels.end(); i++) {
        if (!strcmp((*i), label)) {
            delete[] *i ;
            synchronizationLabels.erase(i);
            return ;
        }
    }

    throw RTIinternalError("Synch. label not in federate.");
}

// ----------------------------------------------------------------------------
//! Returns whether the federate is already synchronized with this label.
bool
Federate::isSynchronizationLabel(const char *label) const
{
    list<char *>::const_iterator i = synchronizationLabels.begin();
    for (; i != synchronizationLabels.end(); i++) {
        if (!strcmp((*i), label)) {
            return true ;
        }
    }

    return false ;
}

}}

// $Id: Federate.cc,v 3.9 2003/05/23 16:40:04 breholee Exp $
