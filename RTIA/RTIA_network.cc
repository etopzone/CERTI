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
// $Id: RTIA_network.cc,v 3.7 2003/06/07 22:24:12 breholee Exp $
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

      case NetworkMessage::MESSAGE_NULL:
      {
          D.Out(pdTrace,
                "Receving Message from RTIG, type NetworkMessage::MESSAGE_NULL(%f).",
                msg->date);

          tm->update(msg->federate, msg->date);
          delete msg ;
          break ;
      }

      case NetworkMessage::SET_TIME_REGULATING:
      {
          // Another federate is becoming regulating.
          D.Out(pdTrace,
                "Receving Message from RTIG, type NetworkMessage::SET_TIME_REGULATING.");

          if (msg->regulator)
              tm->insert(msg->federate, msg->date);
          else
              tm->remove(msg->federate);
          delete msg ;
          break ;
      }

      case NetworkMessage::SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED:
        D.Out(pdTrace, "Receiving Message from RTIG, type NetworkMessage::SYNCHRONIZATION_"
              "POINT_REGISTRATION_SUCCEEDED.");

        queues->insertBeginCommand(msg);
        break ;
      case NetworkMessage::ANNOUNCE_SYNCHRONIZATION_POINT:
        D.Out(pdTrace, "Receiving Message from RTIG, type NetworkMessage::ANNOUCE_"
              "SYNCHRONIZATION_POINT.");

        queues->insertBeginCommand(msg);
        break ;
      case NetworkMessage::FEDERATION_SYNCHRONIZED:
        D.Out(pdTrace,
              "Receiving Message from RTIG, type NetworkMessage::FEDERATION_SYNCHRONIZED.");
        queues->insertBeginCommand(msg);
        break ;

      case NetworkMessage::DISCOVER_OBJECT:
      {
          D.Out(pdTrace, "Receving Message from RTIG, "
                "type NetworkMessage::DISCOVER_OBJECT.");
          queues->insertFifoMessage(msg);
          break ;
      }

      case NetworkMessage::REFLECT_ATTRIBUTE_VALUES:
      {
          D.Out(pdTrace,
                "Receving Message from RTIG, "
                "type NetworkMessage::REFLECT_ATTRIBUTE_VALUES.");

          if (tm->requestContraintState())
              // Verify that received TSO timestamp is >= current
              // time + lookahead
              queues->insertTsoMessage(msg);
          else
              queues->insertFifoMessage(msg);

          break ;
      }

      case NetworkMessage::RECEIVE_INTERACTION:
      {
          D.Out(pdTrace,
                "Receving Message from RTIG, type NetworkMessage::RECEIVE_INTERACTION.");

          if (tm->requestContraintState()) {
              // Verify that received TSO timestamp is >= current
              // time + lookahead
              queues->insertTsoMessage(msg);
          }
          else
              queues->insertFifoMessage(msg);

          break ;
      }

      case NetworkMessage::REMOVE_OBJECT:
      {
          D.Out(pdTrace, "Receving Message from RTIG, type NetworkMessage::REMOVE_OBJECT.");

          if (tm->requestContraintState()) {
              // Verify that received TSO timestamp is >= current
              // time + lookahead
              queues->insertTsoMessage(msg);
          }
          else
              queues->insertFifoMessage(msg);

          break ;
      }

      case NetworkMessage::INFORM_ATTRIBUTE_OWNERSHIP:
      {
          D.Out(pdTrace,
                "Receving Message from RTIG, "
                "type NetworkMessage::INFORM_ATTRIBUTE_OWNERSHIP.");
          queues->insertFifoMessage(msg);
          break ;
      }

      case NetworkMessage::ATTRIBUTE_IS_NOT_OWNED:
      {
          D.Out(pdTrace,
                "Receving Message from RTIG, type NetworkMessage::ATTRIBUTE_IS_NOT_OWNED.");
          queues->insertFifoMessage(msg);
          break ;
      }


      case NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:
      {
          D.Out(pdTrace, "Receving Message from RTIG, "
                "type NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION.");
          queues->insertFifoMessage(msg);
          break ;
      }

      case NetworkMessage::ATTRIBUTE_OWNERSHIP_UNAVAILABLE:
      {
          D.Out(pdTrace, "Receving Message from RTIG, "
                "type NetworkMessage::ATTRIBUTE_OWNERSHIP_UNAVAILABLE.");
          queues->insertFifoMessage(msg);
          break ;
      }

      case NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:
      {
          D.Out(pdTrace, "Receving Message from RTIG, "
                "type NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION.");
          queues->insertFifoMessage(msg);
          break ;
      }

      case NetworkMessage::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:
      {
          D.Out(pdTrace, "Receving Message from RTIG, "
                "type NetworkMessage::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION.");
          queues->insertFifoMessage(msg);
          break ;
      }

      case NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:
      {
          D.Out(pdTrace, "Receving Message from RTIG, "
                "type NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE.");
          queues->insertFifoMessage(msg);
          break ;
      }


      case NetworkMessage::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:
      {
          D.Out(pdTrace, "Receving Message from RTIG, "
                "type NetworkMessage::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION.");
          queues->insertFifoMessage(msg);
          break ;
      }

      case NetworkMessage::INITIATE_FEDERATE_SAVE:
        D.Out(pdTrace, "Receiving Message from RTIG, "
              " type InitiateFederateSave.");
        queues->insertBeginCommand(msg);
        break ;

      case NetworkMessage::FEDERATION_SAVED:
      case NetworkMessage::FEDERATION_NOT_SAVED:
        D.Out(pdTrace, "Receiving Message from RTIG, "
              " type Federation(Not)Saved.");
        queues->insertBeginCommand(msg);
        break ;

      case NetworkMessage::REQUEST_FEDERATION_RESTORE_SUCCEEDED:
      case NetworkMessage::REQUEST_FEDERATION_RESTORE_FAILED:
        D.Out(pdTrace, "Receiving Message from RTIG, "
              " type RequestFederationRestoreStatus.");
        queues->insertBeginCommand(msg);
        break ;

      case NetworkMessage::FEDERATION_RESTORE_BEGUN:
        D.Out(pdTrace, "Receiving Message from RTIG, "
              " type FederationRestoreBegun.");
        queues->insertBeginCommand(msg);
        break ;

      case NetworkMessage::INITIATE_FEDERATE_RESTORE:
        D.Out(pdTrace, "Receiving Message from RTIG, "
              " type InitiateFederateRestore.");
        queues->insertBeginCommand(msg);
        break ;

      case NetworkMessage::FEDERATION_RESTORED:
      case NetworkMessage::FEDERATION_NOT_RESTORED:
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

    stat.rtiService(msg->type);
}

}} // namespace certi/rtia

// $Id: RTIA_network.cc,v 3.7 2003/06/07 22:24:12 breholee Exp $
