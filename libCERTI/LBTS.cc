// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002  ONERA
//
// This file is part of CERTI-libcerti
//
// CERTI-libcerti is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libcerti is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: LBTS.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "LBTS.hh"

namespace certi {

static pdCDebug D("LBTS", "(LBTS   ) - ");

// ------------------
// -- CalculerLBTS --
// ------------------

void LBTS::compute()
{
  FederationTime hl;
  int i;
  
  // LBTS = + l'infini
  _LBTS = DBL_MAX;
  //_LBTS = POSITIVE_INFINITY;

  for(i=1; i<=lg; i++)
    {
      if(Ieme(i)->federate != MyFederateNumber)
	{
	  hl = Ieme(i)->lastNullMessageDate;
	  if(hl < _LBTS)
	      _LBTS = hl;
	}
    }
}


// ------------------
// -- LBTS --
// ------------------

LBTS::LBTS() : List<FederateClock*>()
{
  // + l'infini pour le cas ou federe contraint
  // et aucun autre federe regulateur
  _LBTS            = DBL_MAX;

  MyFederateNumber = 0;
}


// -------------------
// -- ~LBTS --
// -------------------

LBTS::~LBTS()
{
  FederateClock *Cell = NULL;

  while(lg > 0) {
    Cell = Ieme(1);
    Supprimer(1);
    delete Cell;
  }
}


// ----------------
// -- EstPresent --
// ----------------

void LBTS::exists(FederateHandle  num_fed,
			       Boolean        &trouve,
			       int            &rang)
{
  int i;
  
  trouve = RTI_FALSE;
  rang = 0;
  i = 1; 
  while(i<=lg && !trouve)
    {
      if(Ieme(i)->federate == num_fed)
	{
	  trouve = RTI_TRUE;
	  rang = i;
	}
      i++;
    }
}


// ----------------
// -- IemeFedere --
// ----------------

void LBTS::get(int             i, 
			      FederateHandle &num_fed, 
			      FederationTime &heure)
{
  FederateClock *tampon;

  tampon = Ieme(i);
  num_fed = tampon->federate;
  heure   = tampon->lastNullMessageDate;
}


// -------------------
// -- InsererFedere --
// -------------------

void LBTS::insert(FederateHandle num_fed,
                                 FederationTime heure)
{
  FederateClock *tampon;
  Boolean trouve;
  int rang;

  exists(num_fed, trouve, rang);

  if(trouve != RTI_FALSE)
    throw RTIinternalError("LBTS: Federate already present.");

  // BUG: On pourrait verifier si le temps est correct.
  tampon = new FederateClock(num_fed, heure);
  Inserer(1, tampon);
  compute();
}


// ---------------
// -- MAJfedere --
// ---------------

void LBTS::update(FederateHandle num_fed,
                             FederationTime heure)
{
  FederateClock *tampon;
  Boolean         trouve;
  int             rang;

  D.Out(pdDebug, "LBTS.MAJfedere: Mise a jour demandee pour fed. %d(%f).",
	  num_fed, heure);

  exists(num_fed, trouve, rang);

  if(trouve != RTI_TRUE)
    throw RTIinternalError("LBTS: Federate not found.");

  tampon = Ieme(rang);

  // test de coherence
  if(tampon->lastNullMessageDate > heure)
    D.Out(pdDebug, 
	   "LBTS.MAJfedere: federe %u, nelle heure inferieure a l'ancienne.",
	   num_fed);
  else
  {   
    D.Out(pdDebug, "avant LBTS.MAJfedere: federe %u, ancienne heure %f.",
	   tampon->federate, tampon->lastNullMessageDate);
    tampon->lastNullMessageDate = heure;
    D.Out(pdDebug, "apres LBTS.MAJfedere: federe %u, nvelle heure %f.",
	   tampon->federate, tampon->lastNullMessageDate);
    compute();
  }
}


// ---------------------
// -- SupprimerFedere --
// ---------------------

void LBTS::remove(FederateHandle num_fed)
{
  FederateClock *tampon;
  Boolean trouve;
  int rang;

  exists(num_fed, trouve, rang);

  if(trouve != RTI_TRUE)
    throw RTIinternalError("LBTS: Federate not found.");

  tampon = Ieme(rang);
  Supprimer(rang);
  delete tampon;
  compute();
}

}

// $Id: LBTS.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $

