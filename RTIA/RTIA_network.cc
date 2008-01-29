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
// $Id: RTIA_network.cc,v 3.18 2008/01/29 14:30:51 rousse Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "RTIA.hh"
#include "ObjectClassAttribute.hh"
#include "Interaction.hh"
#include "InteractionSet.hh"
#include "ObjectClass.hh"
#include "ObjectClassSet.hh"
#include <assert.h>

namespace certi {
namespace rtia {

static pdCDebug D("RTIA", "(RTIA net) ");
static PrettyDebug G("GENDOC",__FILE__);

// ----------------------------------------------------------------------------
//! RTIA processes a message coming from network.
void
RTIA::processNetworkMessage(NetworkMessage *msg)
{
NetworkMessage::Type msgType = msg->type;

G.Out(pdGendoc,"enter RTIA::processNetworkMessage");

//D.Mes(pdMessage, 'N', msgType);
msg->trace("RTIA::processNetworkMessage ");

    switch(msgType) {

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
          OrderType updateOrder  ;

          D.Out(pdTrace,
                "Receving Message from RTIG, "
                "type NetworkMessage::REFLECT_ATTRIBUTE_VALUES.");

         // It is important to note that several attributes may be updated at
         // the same time. Each attribute has its own order type, region, etc.
         // So attributes which are meeting similar criteria should be sent
         // together. It means that case REFLECT_ATTRIBUTE_VALUES can generate
         // several messages in queues.
         //
         // At this point of development, we cannot assume this facility and
         // decided to store message as TSO only if all attributes meets TSO
         // criteria. Otherwise, a single message will be enqueue in FIFO.
 
         // Here we have to consider RAV without time
         if ( !msg->getBoolean() )
             {
             // without time
             updateOrder = RECEIVE ;
             }
         else
             {        
             // Retrieve order type
             updateOrder = TIMESTAMP;

             for (UShort i=0; i< msg->handleArraySize; ++i)
               {
                if (rootObject->ObjectClasses->getWithHandle( msg->objectClass)
                    ->getAttribute(msg->handleArray[i])->order != TIMESTAMP)
                {
                   updateOrder = RECEIVE;
                   break;
                }
               }
             }
         
         // Decide which queue will be used
         if(updateOrder == TIMESTAMP && tm->requestContraintState())    
         {
            // Update is TSO
            queues->insertTsoMessage(msg); 
         }
         else
         {
            // Update is RO
            queues->insertFifoMessage(msg);
         }

         break ;
      }

      case NetworkMessage::RECEIVE_INTERACTION:
      {
         OrderType interactionOrder ;

         D.Out(pdTrace,
            "Receving Message from RTIG, type NetworkMessage::RECEIVE_INTERACTION.");

         // Here we have to consider RAV without time
         if ( !msg->getBoolean() )
             {
             // without time
             interactionOrder = RECEIVE ;
             }
         else
             {      
             // Retrieve order type
             interactionOrder = rootObject->Interactions->
                getByHandle(msg->interactionClass)->order;
             }
         
         // Decide which queue will be used
         if (interactionOrder == TIMESTAMP && tm->requestContraintState())
         {
            // Interaction is TSO
            queues->insertTsoMessage(msg);
         }
         else
         {
            // Interaction is RO
            queues->insertFifoMessage(msg);
         }

         break ;
      }

      case NetworkMessage::REMOVE_OBJECT:
      {
          D.Out(pdTrace, "Receving Message from RTIG, \
	  		  type NetworkMessage::REMOVE_OBJECT.");

          if (tm->requestContraintState() && msg->getBoolean()) {
              // Verify that received TSO timestamp is >= current
              // time + lookahead
              queues->insertTsoMessage(msg);
          }
          else {
	      msg->setBoolean(false);
              queues->insertFifoMessage(msg);
	  }

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

      case NetworkMessage::FEDERATE_SAVE_BEGUN:
        D.Out(pdTrace, "Receiving Message from RTIG, "
              " type FederateSaveBegun.");
        queues->insertBeginCommand(msg);
        break ;

      case NetworkMessage::FEDERATE_SAVE_COMPLETE:
        D.Out(pdTrace, "Receiving Message from RTIG, "
              " type FederateSaveComplete.");
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
                "Receving Message from RTIG, unknown type %d.", msgType);
          delete msg ;
          throw RTIinternalError("Unknown Message type received from RTIG.");
      }
    }

    stat.rtiService(msgType);
    G.Out(pdGendoc,"exit  RTIA::processNetworkMessage");
}

}} // namespace certi/rtia

// $Id: RTIA_network.cc,v 3.18 2008/01/29 14:30:51 rousse Exp $
