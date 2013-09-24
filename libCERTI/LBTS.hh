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

#ifndef LIBCERTI_LBTS_HH
#define LIBCERTI_LBTS_HH

#include "certi.hh"
#include "FedTimeD.hh"

#include <map>
#include <vector>

namespace certi {

/**
 * The Lower Bound on TimeStamp class.
 */
class CERTI_EXPORT LBTS
{
public:
    typedef std::pair<FederateHandle, FederationTime> FederateClock ;

    /**
     * LBTS constructor.
     * LBTS is set to infinite in case of constrained
     * federate without any regulating federate.
     * @post LBTS is set to positive infinite.
     */
    LBTS();

    /**
     * LBTS Destructor.
     */
    ~LBTS();

    /**
     *  Compute the LBTS from the federate clocks value.
     */
    void compute();

    /** Return the current LBTS value */
    FederationTime getLBTSValue() const {return _LBTS;};

    /**
     * Check if a federate exists.
     * @return true is the corresponding federate exists.
     */
    bool exists(FederateHandle) const ;
    void get(std::vector<FederateClock> &) const ;
    void insert(FederateHandle num_fed, FederationTime the_time);
    void remove(FederateHandle num_fed);
    void setFederate(FederateHandle handle) { MyFederateNumber = handle ; };
    size_t size() const { return clocks.size(); };

    /**
     * Update the logical time of one federate.
     * @param[in] federateHandle the handle of the federate whose logical
     *            time will be updated. The handle may be 0 meaning that
     *            this is an "anonymous" update for everyone.
     */
    void update(FederateHandle federateHandle, FederationTime logicalTime);

    /**
     * Return true is the last call to update was done with an "anonymous"
     * federate handle. I.e. was the consequence of the NULL PRIME message
     * algorithm.
     */
    bool hasReceivedAnonymousUpdate() const {return anonymousUpdateReceived;};

    /**
     * The reception of anonymous update have been taken into account.
     */
    void resetAnonymousUpdate() {anonymousUpdateReceived=false;};

protected:
    FederationTime _LBTS ;
    FederateHandle MyFederateNumber ;
    bool           anonymousUpdateReceived;

private:
    typedef std::map<FederateHandle, FederationTime> ClockSet ;

    ClockSet clocks ;
};

}

#endif // LIBCERTI_LBTS_HH
