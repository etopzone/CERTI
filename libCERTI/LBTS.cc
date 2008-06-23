// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003, 2004  ONERA
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
// $Id: LBTS.cc,v 3.11 2008/06/23 13:25:05 erk Exp $
// ----------------------------------------------------------------------------



#include "LBTS.hh"
#include "PrettyDebug.hh"

#ifndef _WIN32
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
	#include <stdlib.h>
#endif
#include <float.h>
#include <limits>

using std::vector ;  

namespace certi {

static pdCDebug D("LBTS", __FILE__);

// ----------------------------------------------------------------------------
/** Constructor.  LBTS is set to infinite in case of constrained
  federate without any regulating federate.
*/
LBTS::LBTS()
    : _LBTS(std::numeric_limits<double>::infinity()),
      MyFederateNumber(0)
{
}

// ----------------------------------------------------------------------------
LBTS::~LBTS()
{
}

// ----------------------------------------------------------------------------
//! Compute the LBTS.
void
LBTS::compute()
{
    FederationTime hl ;

    // LBTS = + l'infini
    _LBTS = std::numeric_limits<double>::infinity();

    ClockSet::iterator i ;
    for (i = clocks.begin(); i != clocks.end(); ++i) {
        if (i->first != MyFederateNumber) {
            hl = i->second ;
            if (hl < _LBTS)
                _LBTS = hl ;
        }
    }
}

// ----------------------------------------------------------------------------
/** Check if a federate exists
 */
bool
LBTS::exists(FederateHandle federate) const
{
    return clocks.find(federate) != clocks.end();
}

// ----------------------------------------------------------------------------
/** Get all the federate handle and time in a list of pairs
    @param v Output parameter, list receiving values
*/
void
LBTS::get(std::vector<FederateClock> &v) const
{
    v.reserve(v.size() + clocks.size());
    // append clocks to v
    // note, the ClockSet::value_type and FederateClock differ in const-ness
    for(ClockSet::const_iterator pos = clocks.begin();
        pos != clocks.end(); pos++)
        v.push_back(FederateClock(pos->first, pos->second));
}

// ----------------------------------------------------------------------------
/** Insert a new federate and time
 */
void
LBTS::insert(FederateHandle num_fed, FederationTime time)
{
    // TODO: is this check really required ?
    if (exists(num_fed))
        throw RTIinternalError("LBTS: Federate already present.");

    // BUG: We should verify that clock time is correct.
    clocks[num_fed] = time ;
    compute();
}

// ----------------------------------------------------------------------------
//! update a federate
void
LBTS::update(FederateHandle num_fed, FederationTime time)
{
    D.Out(pdDebug, "LBTS.update: Updating federate %d(%f).", num_fed, time);

    ClockSet::iterator it = clocks.find(num_fed);

    if (it == clocks.end())
        throw RTIinternalError("LBTS: Federate not found.");

    // Coherence test.
    if (it->second > time)
        D.Out(pdDebug,
              "LBTS.update: federate %u, new time lower than oldest one.",
              num_fed);
    else {
        D.Out(pdDebug, "before LBTS.update: federate %u, old time %f.",
              it->first, it->second);
        it->second = time ;
        D.Out(pdDebug, "after LBTS.update: federate %u, new time %f.",
              it->first, it->second) ;
        compute();
    }
}

// ----------------------------------------------------------------------------
//! Remove a federate
void
LBTS::remove(FederateHandle num_fed)
{
    ClockSet::iterator it = clocks.find(num_fed);

    if (it == clocks.end())
        throw RTIinternalError("LBTS: Federate not found.");

    clocks.erase(it);
    compute();
}

} // namespace certi

// $Id: LBTS.cc,v 3.11 2008/06/23 13:25:05 erk Exp $
