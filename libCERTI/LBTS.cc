// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: LBTS.cc,v 3.2 2003/01/17 23:54:04 breholee Exp $
// ---------------------------------------------------------------------------

#include "LBTS.hh"

namespace certi {

static pdCDebug D("LBTS", "(LBTS   ) - ");

// ---------------------------------------------------------------------------
//! Compute the LBTS.
void
LBTS::compute(void)
{
    FederationTime hl;
  
    // LBTS = + l'infini
    _LBTS = DBL_MAX;

    deque<FederateClock *>::iterator i ;
    for(i = begin(); i != end() ; i++) {
        if ((*i)->federate != MyFederateNumber) {
            hl = (*i)->lastNullMessageDate;
            if (hl < _LBTS)
                _LBTS = hl;
        }
    }
}

// ---------------------------------------------------------------------------
/*! Constructor.
    _LBTS is set to infinite in case of constrained federate without any
    regulating federate.
*/
LBTS::LBTS(void)
    : deque<FederateClock *>(), _LBTS(DBL_MAX), MyFederateNumber(0)
{
}

// ---------------------------------------------------------------------------
//! Empty deque and frees memory allocated to FederateClock.
LBTS::~LBTS(void)
{
    while (!empty()) {
        delete front();
        pop_front();
    }
}

// ---------------------------------------------------------------------------
//! Find a federate by federate handle.
void
LBTS::exists(FederateHandle fed_num, Boolean &found, int &rank) const
{
    found = RTI_FALSE;
    rank = 0;

    deque<FederateClock *>::const_iterator i = begin();
    for (int j = 1 ; i != end() ; i++, j++) {
        if ((*i)->federate == fed_num ) {
            found = RTI_TRUE;
            rank = j;
            return;
        }
    }
}

// ---------------------------------------------------------------------------
//! Get the federate handle and time at rank rank.
void
LBTS::get(int i, FederateHandle &num_fed, FederationTime &time) const
{
    deque<FederateClock *>::const_iterator k = begin();
    for (int j = 1; k != end() ; k++, j++) {
        if (j == i) {
            num_fed = (*k)->federate;
            time    = (*k)->lastNullMessageDate;
            return;
        }
    }
}

// ---------------------------------------------------------------------------
void
LBTS::insert(FederateHandle num_fed, FederationTime time)
{
  Boolean found;
  int rank;

  exists(num_fed, found, rank);

  if (found != RTI_FALSE)
    throw RTIinternalError("LBTS: Federate already present.");

  // BUG: We should verify that clock time is correct.
  push_front(new FederateClock(num_fed, time));
  compute();
}

// ---------------------------------------------------------------------------
//! update.
void
LBTS::update(FederateHandle num_fed, FederationTime time)
{
  D.Out(pdDebug, "LBTS.update: Updating federate %d(%f).", num_fed, time);

  Boolean found;
  int     rank;
  exists(num_fed, found, rank);

  if (found != RTI_TRUE)
    throw RTIinternalError("LBTS: Federate not found.");

  deque<FederateClock *>::const_iterator k = begin();
  for (int j = 1 ; k != end() ; k++, j++) {
      if (j == rank) {
          break;
      }
  }

  // Coherence test.
  if ((*k)->lastNullMessageDate > time)
      D.Out(pdDebug, 
            "LBTS.update: federate %u, new time lower than oldest one.",
            num_fed);
  else {
      D.Out(pdDebug, "before LBTS.update: federate %u, old time %f.",
            (*k)->federate, (*k)->lastNullMessageDate);
      (*k)->lastNullMessageDate = time;
      D.Out(pdDebug, "after LBTS.update: federate %u, new time %f.",
            (*k)->federate, (*k)->lastNullMessageDate);
      compute();
  }
}

// ---------------------------------------------------------------------------
//! Remove a FederateClock from list.
void
LBTS::remove(FederateHandle num_fed)
{
  Boolean found;
  int rank;

  exists(num_fed, found, rank);

  if (found != RTI_TRUE)
    throw RTIinternalError("LBTS: Federate not found.");

  deque<FederateClock *>::iterator k = begin();
  for (int j = 1 ; k != end() ; k++, j++) {
      if (j == rank) {
          delete (*k);
          erase(k);
          break;
      }
  }

  compute();
}

} // namespace certi

// $Id: LBTS.cc,v 3.2 2003/01/17 23:54:04 breholee Exp $
