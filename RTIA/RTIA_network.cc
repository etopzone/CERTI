// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002  ONERA
//
// This file is part of CERTI
//
// CERTI is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// $Id: RTIA_network.cc,v 3.1 2002/12/11 00:47:33 breholee Exp $
// ---------------------------------------------------------------------------

#include "RTIA.hh"

namespace certi {
namespace rtia {

static pdCDebug D("RTIA", "(RTIA ) - ");

// --------------------------
// -- TraiterNetworkMessage --
// --------------------------

// le RTIA traite un message venant du reseau

void RTIA::processNetworkMessage(NetworkMessage *msg)
{ 
  switch(msg->Type) {

  case m_MESSAGE_NULL:
    {
      nb_messages [0] ++;
      D.Out(pdTrace, 
	    "Receving Message from RTIG, type m_MESSAGE_NULL(%f).",
	    msg->Date);

      _GT->update(msg->NumeroFedere, msg->Date);
      delete msg;
      break;
    }
 
  case m_SET_TIME_REGULATING:
    {
      // un autre federe devient regulateur
      nb_messages [1] ++;
      D.Out(pdTrace, 
	    "Receving Message from RTIG, type m_SET_TIME_REGULATING.");

      if(msg->EstRegulateur)
	_GT->insert(msg->NumeroFedere, msg->Date);
      else
	_GT->remove(msg->NumeroFedere);
      delete msg;
      break;
    }
 
  case m_REQUEST_PAUSE:
    { 
      nb_messages [2] ++;
      D.Out(pdTrace, "Receving Message from RTIG, type m_REQUEST_PAUSE.");

      _GQueues->insertBeginCommand(msg);
      break;
    }
 
  case m_REQUEST_RESUME:
    {
      D.Out(pdTrace, "Receving Message from RTIG, type m_REQUEST_RESUME.");
      nb_messages [3] ++;

      _GQueues->insertLastCommand(msg);
      break;
    }
 
  case m_DISCOVER_OBJECT:
    {
      D.Out(pdTrace, "Receving Message from RTIG, "
	    "type m_DISCOVER_OBJECT.");
      nb_messages [4] ++;
      _GQueues->insertFifoMessage(msg);
      break;
    }

  case m_REFLECT_ATTRIBUTE_VALUES:
    {
      D.Out(pdTrace, 
	    "Receving Message from RTIG, "
	    "type m_REFLECT_ATTRIBUTE_VALUES.");
      nb_messages [7] ++;

      if( _GT->requestContraintState())
	// verifier que estampille du TSO recu >= 
	// heure_courant + lookahead
	_GQueues->insertTsoMessage(msg);
      else
	_GQueues->insertFifoMessage(msg);
 
      break;
    }
 
  case m_RECEIVE_INTERACTION:
    { 
      D.Out(pdTrace, 
	    "Receving Message from RTIG, type m_RECEIVE_INTERACTION.");

      nb_evenements ++;
      nb_messages [8] ++;

      if(_GT->requestContraintState())
	{
	  // verifier que estampille du TSO recu >= 
	  // heure_courant + lookahead
	  _GQueues->insertTsoMessage(msg);
	}
      else
	_GQueues->insertFifoMessage(msg);
 
      break;
    }
 
  case m_REMOVE_OBJECT:
    {
      D.Out(pdTrace, "Receving Message from RTIG, type m_REMOVE_OBJECT.");
      nb_messages [9] ++;

      if(_GT->requestContraintState())
	{
	  // verifier que estampille du TSO recu >= 
	  // heure_courant + lookahead
	  _GQueues->insertTsoMessage(msg);
	}
      else
	_GQueues->insertFifoMessage(msg);
 
      break;
    }
 
  case m_INFORM_ATTRIBUTE_OWNERSHIP:
    {
      D.Out(pdTrace, 
	    "Receving Message from RTIG, "
	    "type m_INFORM_ATTRIBUTE_OWNERSHIP.");
      nb_messages [10] ++;
      _GQueues->insertFifoMessage(msg);
      break;
    }
 
  case m_ATTRIBUTE_IS_NOT_OWNED:
    {
      D.Out(pdTrace, 
	    "Receving Message from RTIG, type m_ATTRIBUTE_IS_NOT_OWNED.");
      nb_messages [11] ++;
      _GQueues->insertFifoMessage(msg);
      break;
    }
 
 
  case m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
    {
      D.Out(pdTrace, "Receving Message from RTIG, "
	    "type m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION.");
      nb_messages [12] ++;
      _GQueues->insertFifoMessage(msg);
      break;
    }
 
  case m_ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
    {
      D.Out(pdTrace, "Receving Message from RTIG, "
	    "type m_ATTRIBUTE_OWNERSHIP_UNAVAILABLE.");
      nb_messages [13] ++;
      _GQueues->insertFifoMessage(msg);
      break;
    }

  case m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
    {
      D.Out(pdTrace, "Receving Message from RTIG, "
	    "type m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION.");
      nb_messages [14] ++;
      _GQueues->insertFifoMessage(msg);
      break;
    }

  case m_ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
    {
      D.Out(pdTrace, "Receving Message from RTIG, "
	    "type m_ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION.");
      nb_messages [15] ++;
      _GQueues->insertFifoMessage(msg);
      break;
    }

  case m_REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
    {
      D.Out(pdTrace, "Receving Message from RTIG, "
	    "type m_REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE.");
      nb_messages [16] ++;
      _GQueues->insertFifoMessage(msg);
      break;
    }
 

  case m_CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
    {
      D.Out(pdTrace, "Receving Message from RTIG, "
	    "type m_CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION.");
      nb_messages [17] ++;
      _GQueues->insertFifoMessage(msg);
      break;
    }
 
  default:
    {
      D.Out(pdTrace, 
	    "Receving Message from RTIG, unknown type %d.", msg->Type);
      delete msg;
      throw RTIinternalError("Unknown Message type received from RTIG.");
    }
  }

  TOTAL=TOTAL+1;
}

}}

// $Id: RTIA_network.cc,v 3.1 2002/12/11 00:47:33 breholee Exp $
