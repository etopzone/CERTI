// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
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
// $Id: RTIA_network.cc,v 3.6 2003/05/05 20:21:39 breholee Exp $
// ----------------------------------------------------------------------------

#include "RTIA.hh"

namespace certi {
namespace rtia {

static pdCDebug D("RTIA", "(RTIA net) ");

// ----------------------------------------------------------------------------
//! RTIA processes a message coming from network.
void
RTIA::processNetworkMessage(NetworkMessage *msg)
{
    switch(msg->type) {

      case m_MESSAGE_NULL:
      {
          nb_messages[0]++ ;
          D.Out(pdTrace,
                "Receving Message from RTIG, type m_MESSAGE_NULL(%f).",
                msg->date);

          tm->update(msg->federate, msg->date);
          delete msg ;
          break ;
      }

      case m_SET_TIME_REGULATING:
      {
          // Another federate is becoming regulating.
          nb_messages[1]++ ;
          D.Out(pdTrace,
                "Receving Message from RTIG, type m_SET_TIME_REGULATING.");

          if (msg->regulator)
              tm->insert(msg->federate, msg->date);
          else
              tm->remove(msg->federate);
          delete msg ;
          break ;
      }

      case m_SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
        //nb_messages[xx]++ ;
        D.Out(pdTrace, "Receiving Message from RTIG, type m_SYNCHRONIZATION_"
              "POINT_REGISTRATION_SUCCEEDED.");

        queues->insertBeginCommand(msg);
        break ;
      case m_ANNOUNCE_SYNCHRONIZATION_POINT:
        nb_messages[2]++ ;
        D.Out(pdTrace, "Receiving Message from RTIG, type m_ANNOUCE_"
              "SYNCHRONIZATION_POINT.");

        queues->insertBeginCommand(msg);
        break ;
      case m_FEDERATION_SYNCHRONIZED:
        nb_messages[3]++ ;
        D.Out(pdTrace,
              "Receiving Message from RTIG, type m_FEDERATION_SYNCHRONIZED.");
        queues->insertBeginCommand(msg);
        break ;

      case m_DISCOVER_OBJECT:
      {
          nb_messages[4]++ ;
          D.Out(pdTrace, "Receving Message from RTIG, "
                "type m_DISCOVER_OBJECT.");
          queues->insertFifoMessage(msg);
          break ;
      }

      case m_REFLECT_ATTRIBUTE_VALUES:
      {
          nb_messages[7]++ ;
          D.Out(pdTrace,
                "Receving Message from RTIG, "
                "type m_REFLECT_ATTRIBUTE_VALUES.");

          if (tm->requestContraintState())
              // Verify that received TSO timestamp is >= current
              // time + lookahead
              queues->insertTsoMessage(msg);
          else
              queues->insertFifoMessage(msg);

          break ;
      }

      case m_RECEIVE_INTERACTION:
      {
          nb_messages[8]++ ;
          D.Out(pdTrace,
                "Receving Message from RTIG, type m_RECEIVE_INTERACTION.");

          nb_evenements++ ;

          if (tm->requestContraintState()) {
              // Verify that received TSO timestamp is >= current
              // time + lookahead
              queues->insertTsoMessage(msg);
          }
          else
              queues->insertFifoMessage(msg);

          break ;
      }

      case m_REMOVE_OBJECT:
      {
          nb_messages[9]++ ;
          D.Out(pdTrace, "Receving Message from RTIG, type m_REMOVE_OBJECT.");

          if (tm->requestContraintState()) {
              // Verify that received TSO timestamp is >= current
              // time + lookahead
              queues->insertTsoMessage(msg);
          }
          else
              queues->insertFifoMessage(msg);

          break ;
      }

      case m_INFORM_ATTRIBUTE_OWNERSHIP:
      {
          nb_messages[10]++ ;
          D.Out(pdTrace,
                "Receving Message from RTIG, "
                "type m_INFORM_ATTRIBUTE_OWNERSHIP.");
          queues->insertFifoMessage(msg);
          break ;
      }

      case m_ATTRIBUTE_IS_NOT_OWNED:
      {
          nb_messages[11]++ ;
          D.Out(pdTrace,
                "Receving Message from RTIG, type m_ATTRIBUTE_IS_NOT_OWNED.");
          queues->insertFifoMessage(msg);
          break ;
      }


      case m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
      {
          nb_messages[12]++ ;
          D.Out(pdTrace, "Receving Message from RTIG, "
                "type m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION.");
          queues->insertFifoMessage(msg);
          break ;
      }

      case m_ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
      {
          nb_messages[13]++ ;
          D.Out(pdTrace, "Receving Message from RTIG, "
                "type m_ATTRIBUTE_OWNERSHIP_UNAVAILABLE.");
          queues->insertFifoMessage(msg);
          break ;
      }

      case m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
      {
          nb_messages[14]++ ;
          D.Out(pdTrace, "Receving Message from RTIG, "
                "type m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION.");
          queues->insertFifoMessage(msg);
          break ;
      }

      case m_ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
      {
          nb_messages[15]++ ;
          D.Out(pdTrace, "Receving Message from RTIG, "
                "type m_ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION.");
          queues->insertFifoMessage(msg);
          break ;
      }

      case m_REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
      {
          nb_messages[16]++ ;
          D.Out(pdTrace, "Receving Message from RTIG, "
                "type m_REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE.");
          queues->insertFifoMessage(msg);
          break ;
      }


      case m_CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
      {
          nb_messages[17]++ ;
          D.Out(pdTrace, "Receving Message from RTIG, "
                "type m_CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION.");
          queues->insertFifoMessage(msg);
          break ;
      }

      case m_INITIATE_FEDERATE_SAVE:
        //nb_messages[xx]++ ;
        D.Out(pdTrace, "Receiving Message from RTIG, "
              " type InitiateFederateSave.");
        queues->insertBeginCommand(msg);
        break ;

      case m_FEDERATION_SAVED:
      case m_FEDERATION_NOT_SAVED:
        //nb_messages[xx]++ ;
        D.Out(pdTrace, "Receiving Message from RTIG, "
              " type Federation(Not)Saved.");
        queues->insertBeginCommand(msg);
        break ;

      case m_REQUEST_FEDERATION_RESTORE_SUCCEEDED:
      case m_REQUEST_FEDERATION_RESTORE_FAILED:
        //nb_messages[xx]++ ;
        D.Out(pdTrace, "Receiving Message from RTIG, "
              " type RequestFederationRestoreStatus.");
        queues->insertBeginCommand(msg);
        break ;

      case m_FEDERATION_RESTORE_BEGUN:
        //nb_messages[xx]++ ;
        D.Out(pdTrace, "Receiving Message from RTIG, "
              " type FederationRestoreBegun.");
        queues->insertBeginCommand(msg);
        break ;

      case m_INITIATE_FEDERATE_RESTORE:
        //nb_messages[xx]++ ;
        D.Out(pdTrace, "Receiving Message from RTIG, "
              " type InitiateFederateRestore.");
        queues->insertBeginCommand(msg);
        break ;

      case m_FEDERATION_RESTORED:
      case m_FEDERATION_NOT_RESTORED:
        //nb_messages[xx]++ ;
        D.Out(pdTrace, "Receiving Message from RTIG, "
              " type Federation(Not)Restored.");
        queues->insertBeginCommand(msg);
        break ;

      default:
      {
          D.Out(pdTrace,
                "Receving Message from RTIG, unknown type %d.", msg->type);
          delete msg ;
          throw RTIinternalError("Unknown Message type received from RTIG.");
      }
    }

    TOTAL=TOTAL+1 ;
}

}} // namespace certi/rtia

// $Id: RTIA_network.cc,v 3.6 2003/05/05 20:21:39 breholee Exp $
