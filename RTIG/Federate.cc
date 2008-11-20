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
// $Id: Federate.cc,v 3.16 2008/11/20 18:21:55 approx Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "Federate.hh"
#include "PrettyDebug.hh"

#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <cstring>
#include <string>
#endif

static PrettyDebug G("GENDOC",__FILE__);

using std::string ;

namespace certi {
namespace rtig {

// ----------------------------------------------------------------------------
//! A new FederateName is allocated. theLink must have been opened before.
Federate::Federate(const char *the_name, FederateHandle the_handle)
    throw (RTIinternalError)
    : handle(the_handle), name(the_name), regulator(false), constrained(false),
      cras(true), iras(true), aras(false), asas(false), 
      saving(false), restoring(false)
{
    if (handle == 0)
        throw RTIinternalError("Bad initialization parameter for Federate.");
}

// ----------------------------------------------------------------------------
//! Add a synchronization label to federate.
void
Federate::addSynchronizationLabel(const char *label)
    throw (RTIinternalError)
{
    G.Out(pdGendoc,"enter Federate::addSynchronizationLabel");

    string s = label ;
    SyncList::iterator it = std::find(syncLabels.begin(), syncLabels.end(), s);
    if (it == syncLabels.end())
	syncLabels.push_back(s);
    else
	throw RTIinternalError("Synchronization label pending in federate.");

    G.Out(pdGendoc,"exit  Federate::addSynchronizationLabel");

}

// ----------------------------------------------------------------------------
//! Removes a synchronization label from federate.
void
Federate::removeSynchronizationLabel(const char *label)
    throw (RTIinternalError)
{
    string s = label ;
    SyncList::iterator it = std::find(syncLabels.begin(), syncLabels.end(), s);
    if (it == syncLabels.end())
	throw RTIinternalError("Synch. label not in federate.");
    else
	syncLabels.erase(it);    
}

// ----------------------------------------------------------------------------
//! Returns whether the federate is already synchronized with this label.
bool
Federate::isSynchronizationLabel(const char *label) const
{
    return std::find(syncLabels.begin(), syncLabels.end(), string(label)) != syncLabels.end();
}

}}

// $Id: Federate.cc,v 3.16 2008/11/20 18:21:55 approx Exp $
