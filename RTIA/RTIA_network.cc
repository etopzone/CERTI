// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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
// $Id: RTIA_network.cc,v 3.2 2003/01/16 17:55:33 breholee Exp $
// ---------------------------------------------------------------------------

#include "RTIA.hh"

namespace certi {
namespace rtia {

static pdCDebug D("RTIA", "(RTIA ) - ");

// ---------------------------------------------------------------------------
//! RTIA processes a message coming from network.
void
RTIA::processNetworkMessage(NetworkMessage *msg)
{ 
  switch(msg->Type) {

  case m_MESSAGE_NULL:
    {
      nb_messages[0]++;
      D.Out(pdTrace, 
	    "Receving Message from RTIG, type m_MESSAGE_NULL(%f).",
	    msg->Date);

      _GT->update(msg->NumeroFedere, msg->Date);
      delete msg;
      break;
    }
 
  case m_SET_TIME_REGULATING:
    {
      // Another federate is becoming regulating.
      nb_messages[1]++;
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
      nb_messages[2]++;
      D.Out(pdTrace, "Receving Message from RTIG, type m_REQUEST_PAUSE.");

      _GQueues->insertBeginCommand(msg);
      break;
    }
 
  case m_REQUEST_RESUME:
    {
      nb_messages[3]++;
      D.Out(pdTrace, "Receving Message from RTIG, type m_REQUEST_RESUME.");

      _GQueues->insertLastCommand(msg);
      break;
    }
 
  case m_DISCOVER_OBJECT:
    {
      nb_messages[4]++;
      D.Out(pdTrace, "Receving Message from RTIG, "
	    "type m_DISCOVER_OBJECT.");
      _GQueues->insertFifoMessage(msg);
      break;
    }

  case m_REFLECT_ATTRIBUTE_VALUES:
    {
      nb_messages[7]++;
      D.Out(pdTrace, 
	    "Receving Message from RTIG, "
	    "type m_REFLECT_ATTRIBUTE_VALUES.");

      if( _GT->requestContraintState())
          // Verify that received TSO timestamp is >= current time + lookahead
          _GQueues->insertTsoMessage(msg);
      else
          _GQueues->insertFifoMessage(msg);
 
      break;
    }
 
  case m_RECEIVE_INTERACTION:
    {
      nb_messages[8]++;
      D.Out(pdTrace, 
            "Receving Message from RTIG, type m_RECEIVE_INTERACTION.");

      nb_evenements++;

      if(_GT->requestContraintState()) {
          // Verify that received TSO timestamp is >= current time + lookahead
          _GQueues->insertTsoMessage(msg);
      }
      else
          _GQueues->insertFifoMessage(msg);
 
      break;
    }
 
  case m_REMOVE_OBJECT:
    {
      nb_messages[9]++;
      D.Out(pdTrace, "Receving Message from RTIG, type m_REMOVE_OBJECT.");

      if(_GT->requestContraintState()) {
          // Verify that received TSO timestamp is >= current time + lookahead
          _GQueues->insertTsoMessage(msg);
      }
      else
          _GQueues->insertFifoMessage(msg);
 
      break;
    }
 
  case m_INFORM_ATTRIBUTE_OWNERSHIP:
    {
      nb_messages[10]++;
      D.Out(pdTrace, 
	    "Receving Message from RTIG, "
	    "type m_INFORM_ATTRIBUTE_OWNERSHIP.");
      _GQueues->insertFifoMessage(msg);
      break;
    }
 
  case m_ATTRIBUTE_IS_NOT_OWNED:
    {
      nb_messages[11]++;
      D.Out(pdTrace, 
	    "Receving Message from RTIG, type m_ATTRIBUTE_IS_NOT_OWNED.");
      _GQueues->insertFifoMessage(msg);
      break;
    }
 
 
  case m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
    {
      nb_messages[12]++;
      D.Out(pdTrace, "Receving Message from RTIG, "
	    "type m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION.");
      _GQueues->insertFifoMessage(msg);
      break;
    }
 
  case m_ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
    {
      nb_messages[13]++;
      D.Out(pdTrace, "Receving Message from RTIG, "
	    "type m_ATTRIBUTE_OWNERSHIP_UNAVAILABLE.");
      _GQueues->insertFifoMessage(msg);
      break;
    }

  case m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
    {
      nb_messages[14]++;
      D.Out(pdTrace, "Receving Message from RTIG, "
	    "type m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION.");
      _GQueues->insertFifoMessage(msg);
      break;
    }

  case m_ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
    {
      nb_messages[15]++;
      D.Out(pdTrace, "Receving Message from RTIG, "
	    "type m_ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION.");
      _GQueues->insertFifoMessage(msg);
      break;
    }

  case m_REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
    {
      nb_messages[16]++;
      D.Out(pdTrace, "Receving Message from RTIG, "
	    "type m_REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE.");
      _GQueues->insertFifoMessage(msg);
      break;
    }
 

  case m_CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
    {
      nb_messages[17]++;
      D.Out(pdTrace, "Receving Message from RTIG, "
	    "type m_CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION.");
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

}} // namespaces

// $Id: RTIA_network.cc,v 3.2 2003/01/16 17:55:33 breholee Exp $
