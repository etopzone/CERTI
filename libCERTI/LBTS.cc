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
// ----------------------------------------------------------------------------



#include "LBTS.hh"
#include "Exception.hh"
#include "PrettyDebug.hh"
#include <limits>

using std::vector ;  

namespace certi {

static PrettyDebug D("LBTS", __FILE__);

// ----------------------------------------------------------------------------
/** Constructor.  */
LBTS::LBTS()
    : MyFederateNumber(0)
{
  anonymousUpdateReceived = false;
  _LBTS.setPositiveInfinity();
}

LBTS::~LBTS()
{
}

void
LBTS::compute()
{
    FederationTime hl ;

    // LBTS = + l'infini
    _LBTS.setPositiveInfinity();

    ClockSet::iterator i ;
    for (i = clocks.begin(); i != clocks.end(); ++i) {
        if (i->first != MyFederateNumber) {
            hl = i->second ;
            if (hl < _LBTS)
                _LBTS = hl ;
        }
    }
} /* end of compute */

bool
LBTS::exists(FederateHandle federate) const
{
    return clocks.find(federate) != clocks.end();
} /* end of exists */

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
void
LBTS::update(FederateHandle federateHandle, FederationTime time)
{
    D.Out(pdDebug, "LBTS.update: Updating federate %d (time=%f).", federateHandle, time.getTime());
    ClockSet::iterator it;
    ClockSet::iterator itend = clocks.end();

    /*
     * num fed will be 0 if it is an 'anonymous' Null Message
     * which was sent because some Null Prime Message were
     * sent after NERx (NMRx) calls.
     */
    if (federateHandle!=0) {
    	it = clocks.find(federateHandle);
    	if (it == clocks.end()) {
    		throw RTIinternalError(stringize() << "LBTS: Federate <" << federateHandle << "> not found.");
    	} else {
    		itend = it;
    	}
    } else {
    	anonymousUpdateReceived = true;
    	it = clocks.begin();
    }

    do {
    	// Coherence test.
    	if (it->second > time)
    		D.Out(pdDebug,
    				"LBTS.update: federate-%u, new time lower than oldest one.",federateHandle);
    	else {
    		D.Out(pdDebug, "LBTS.update: federate-%u, time %f --> %f (old-->new)",
    				it->first, it->second.getTime(),time.getTime());
    		it->second = time;
    	}
    	if (it!=itend) ++it;
    } while (it!=itend);
    /* now update LBTS */
	compute();
} /* end of update */

// ----------------------------------------------------------------------------
//! Remove a federate
void
LBTS::remove(FederateHandle num_fed)
{
    ClockSet::iterator it = clocks.find(num_fed);

    if (it == clocks.end())
        throw RTIinternalError(stringize() << "LBTS: Federate <"<< num_fed << "not found.");

    clocks.erase(it);
    compute();
}

} // namespace certi
