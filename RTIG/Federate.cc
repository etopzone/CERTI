// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This program is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// $Id: Federate.cc,v 3.19 2010/08/09 18:24:07 erk Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "Federate.hh"
#include "PrettyDebug.hh"

#include <algorithm>
#include <string>

static PrettyDebug G("GENDOC",__FILE__);

namespace certi {
namespace rtig {

// ----------------------------------------------------------------------------
//! A new FederateName is allocated. theLink must have been opened before.
Federate::Federate(const std::string& the_name, FederateHandle the_handle)
    throw (RTIinternalError)
    : handle(the_handle), name(the_name), regulator(false), constrained(false), usingNERx(false),
      cras(true), iras(true), aras(false), asas(false), 
      saving(false), restoring(false)
{
    if (handle == 0)
        throw RTIinternalError("Bad initialization parameter for Federate.");
}


void
Federate::addSynchronizationLabel(const std::string& label)
    throw (RTIinternalError)
{
    G.Out(pdGendoc,"enter Federate::addSynchronizationLabel");

    SyncList::iterator it = std::find(syncLabels.begin(), syncLabels.end(), label);
    if (it == syncLabels.end())
	syncLabels.push_back(label);
    else
	throw RTIinternalError("Synchronization label pending in federate.");

    G.Out(pdGendoc,"exit  Federate::addSynchronizationLabel");

}

// ----------------------------------------------------------------------------
//! Removes a synchronization label from federate.
void
Federate::removeSynchronizationLabel(const std::string& label)
    throw (RTIinternalError)
{
    SyncList::iterator it = std::find(syncLabels.begin(), syncLabels.end(), label);
    if (it == syncLabels.end())
	throw RTIinternalError("Synch. label not in federate.");
    else
	syncLabels.erase(it);    
}

// ----------------------------------------------------------------------------
//! Returns whether the federate is already synchronized with this label.
bool
Federate::isSynchronizationLabel(const std::string& label) const
{
    return std::find(syncLabels.begin(), syncLabels.end(), label) != syncLabels.end();
}

}}

// $Id: Federate.cc,v 3.19 2010/08/09 18:24:07 erk Exp $
