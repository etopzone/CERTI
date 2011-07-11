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
// $Id: RTIA_network.cc,v 3.43 2011/07/11 11:17:24 erk Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "NM_Classes.hh"
#include "RTIA.hh"
#include "ObjectClassAttribute.hh"
#include "Interaction.hh"
#include "InteractionSet.hh"
#include "ObjectClass.hh"
#include "ObjectClassSet.hh"
#include "ObjectSet.hh"
#include <assert.h>

namespace certi {
namespace rtia {

static PrettyDebug D("RTIA", "[RTIA net] ");
static PrettyDebug G("GENDOC",__FILE__);
static PrettyDebug DNULL("RTIA_NULLMSG","[RTIA NULL MSG] ");

// ----------------------------------------------------------------------------
//! RTIA processes a message coming from network.
void
RTIA::processNetworkMessage(NetworkMessage *msg)
{
NetworkMessage::Type msgType = msg->getMessageType();

	G.Out(pdGendoc,"enter RTIA::processNetworkMessage");

    switch(msgType) {

      case NetworkMessage::MESSAGE_NULL:
      {
          D.Out(pdTrace,
                "Receiving Message from RTIG, type NetworkMessage::MESSAGE_NULL(%f).",
                msg->getDate().getTime());

          //msg->show(std::cout);
          DNULL.Out(pdDebug, "NULL message received (Federate=%d, Time = %f)",
                            msg->getFederate(), msg->getDate().getTime()) ;
          tm->update(msg->getFederate(), msg->getDate());
          delete msg ;
          break ;
      }

      case NetworkMessage::SET_TIME_REGULATING:
      {
          // Another federate is becoming regulating.
          D.Out(pdTrace,
                "Receving Message from RTIG, type NetworkMessage::SET_TIME_REGULATING.");

          if (static_cast<NM_Set_Time_Regulating*>(msg)->isRegulatorOn())
              tm->insert(msg->getFederate(), msg->getDate());
          else
              tm->remove(msg->getFederate());
          delete msg ;
          break ;
      }

      case NetworkMessage::CONFIRM_SYNCHRONIZATION_POINT_REGISTRATION:
        D.Out(pdTrace, "Receiving Message from RTIG, type NetworkMessage::CONFIRM_"
              "SYNCHRONIZATION_POINT_REGISTRATION.");

        queues->insertLastCommand(msg);
        break ;
      case NetworkMessage::ANNOUNCE_SYNCHRONIZATION_POINT:
        D.Out(pdTrace, "Receiving Message from RTIG, type NetworkMessage::ANNOUCE_"
              "SYNCHRONIZATION_POINT.");

        queues->insertLastCommand(msg);
        break ;
      case NetworkMessage::FEDERATION_SYNCHRONIZED:
        D.Out(pdTrace,
              "Receiving Message from RTIG, type NetworkMessage::FEDERATION_SYNCHRONIZED.");
        queues->insertLastCommand(msg);
        break ;

      case NetworkMessage::DISCOVER_OBJECT:
      {
    	  NM_Discover_Object* DO = static_cast<NM_Discover_Object*>(msg);
          D.Out(pdTrace, "Receving Message from RTIG, type NetworkMessage::DISCOVER_OBJECT.");
          queues->insertFifoMessage(msg);
          	 /* FIXME
          	 *
          	 * discoverObject tries to auto-registrate a discovered object instance,
          	 * if the object is already discovered through a previous invocation of
          	 * discoverObject the exception ObjectAlreadyRegistered is raised.
          	 *
          	 * This behavior needs further investigation.
          	 * Is it useful to call registerObjectInstance within a federate service?
          	 */

          	try {
          		// Adding discovered object in federate internal object list.
          		// We MUST update RootObject here
          		rootObject->registerObjectInstance(fm->federate, DO->getObjectClass(), DO->getObject(),DO->getLabel());
          	}
          	catch (ObjectAlreadyRegistered&) {
          	}

      }
      break;

      case NetworkMessage::REFLECT_ATTRIBUTE_VALUES:
      {
    	  NM_Reflect_Attribute_Values *RAV = static_cast<NM_Reflect_Attribute_Values*>(msg);
          OrderType updateOrder  ;

          //RAV->show(std::cerr);
          D.Out(pdTrace,
                "Receiving Message from RTIG, "
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
         if ( !msg->isDated())
             {
             // without time
             updateOrder = RECEIVE ;
             }
         else
             {
             // Retrieve order type
             updateOrder = TIMESTAMP;
             // FIXME we may inspect rootObject for an unknown object which has
             // (not yet) been discovered...because DiscoverObject is a RO message
             ObjectClassHandle och = rootObject->objects->getObjectClass(RAV->getObject());
             //std::cerr << "FOUND och = " <<och << "  for object " << RAV->getObject() <<std::endl;
             for (uint32_t i=0; i< RAV->getAttributesSize(); ++i)
               {
            	// FIXME we need an object **CLASS** handle and not an **OBJECT* handle
                //if (rootObject->ObjectClasses->getObjectFromHandle(RAV->getObject())
            	 if (rootObject->ObjectClasses->getObjectFromHandle(och)->getAttribute(RAV->getAttributes(i))->order != TIMESTAMP)
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

    	 NM_Receive_Interaction* RI = static_cast<NM_Receive_Interaction*>(msg);
         OrderType interactionOrder ;

         D.Out(pdTrace,
            "Receving Message from RTIG, type NetworkMessage::RECEIVE_INTERACTION.");

         // Here we have to consider RAV without time
         if ( !msg->isDated())
             {
             // without time
             interactionOrder = RECEIVE ;
             }
         else
             {
             // Retrieve order type
             interactionOrder = rootObject->Interactions->
                getObjectFromHandle(RI->getInteractionClass())->order;
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

          if (tm->requestContraintState() && msg->isDated()) {
              // Verify that received TSO timestamp is >= current
              // time + lookahead
              queues->insertTsoMessage(msg);
          }
          else {
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

      case NetworkMessage::FEDERATION_SAVED:
      case NetworkMessage::FEDERATION_NOT_SAVED:
        D.Out(pdTrace, "Receiving Message from RTIG, "
              " type Federation(Not)Saved.");
        queues->insertBeginCommand(msg);
        break ;

      case NetworkMessage::REQUEST_FEDERATION_RESTORE_SUCCEEDED:
        D.Out(pdTrace, "Receiving Message from RTIG, "
              " type RequestFederationRestoreSucceeded.");
        G.Out(pdGendoc,"processNetworkMessage for REQUEST_FEDERATION_RESTORE_SUCCEEDED");
        queues->insertLastCommand(msg);
        break ;

      case NetworkMessage::REQUEST_FEDERATION_RESTORE_FAILED:
        D.Out(pdTrace, "Receiving Message from RTIG, "
              " type RequestFederationRestoreFailed.");
        G.Out(pdGendoc,"processNetworkMessage for REQUEST_FEDERATION_RESTORE_FAILED");
        queues->insertLastCommand(msg);
        break ;

      case NetworkMessage::FEDERATION_RESTORE_BEGUN:
        D.Out(pdTrace, "Receiving Message from RTIG, "
              " type FederationRestoreBegun.");
        queues->insertLastCommand(msg);
        break ;

      case NetworkMessage::INITIATE_FEDERATE_RESTORE:
        D.Out(pdTrace, "Receiving Message from RTIG, "
              " type InitiateFederateRestore.");
        G.Out(pdGendoc,"processNetworkMessage for INITIATE_FEDERATE_RESTORE");
        queues->insertLastCommand(msg);
        break ;

      case NetworkMessage::FEDERATION_RESTORED:
      case NetworkMessage::FEDERATION_NOT_RESTORED:
        D.Out(pdTrace, "Receiving Message from RTIG, "
              " type Federation(Not)Restored.");
        queues->insertLastCommand(msg);
        break ;

      case NetworkMessage::PROVIDE_ATTRIBUTE_VALUE_UPDATE:
        D.Out(pdTrace, "Receiving Message from RTIG, "
              " type ProvideAttributeValueUpdate.");
        queues->insertFifoMessage(msg);
        break ;

      case NetworkMessage::TIME_REGULATION_ENABLED:
    	  D.Out(pdTrace, "Receiving Message from RTIG, "
    			  " type TimeRegulationEnabled.");
    	  queues->insertLastCommand(msg);
    	  break ;
      case NetworkMessage::TIME_CONSTRAINED_ENABLED:
    	  D.Out(pdTrace, "Receiving Message from RTIG, "
    			  " type TimeConstrainedEnabled.");
    	  queues->insertLastCommand(msg);
    	  break;
      case NetworkMessage::SET_CLASS_RELEVANCE_ADVISORY_SWITCH:
    	  D.Out(pdTrace, "Receiving Message from RTIG, "
    			  " type setCRAS.");
	  break;
      case NetworkMessage::SET_INTERACTION_RELEVANCE_ADVISORY_SWITCH:
    	  D.Out(pdTrace, "Receiving Message from RTIG, "
    			  " type setIRAS.");
	  break;
      case NetworkMessage::SET_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH:
    	  D.Out(pdTrace, "Receiving Message from RTIG, "
    			  " type setARAS.");
	  break;
      case NetworkMessage::SET_ATTRIBUTE_SCOPE_ADVISORY_SWITCH:
    	  D.Out(pdTrace, "Receiving Message from RTIG, "
    			  " type setASAS.");
	  break;
      case NetworkMessage::START_REGISTRATION_FOR_OBJECT_CLASS:
    	  D.Out(pdTrace, "Receiving Message from RTIG, "
    			  " type StartRegForObjClass.");
    	  queues->insertLastCommand(msg);
	  break;
      case NetworkMessage::RESERVE_OBJECT_INSTANCE_NAME_SUCCEEDED:
          D.Out(pdTrace, "Receiving Message from RTIG, "
    			  " type reserveObjectInstanceNameSucceeded.");
    	  queues->insertLastCommand(msg);
	  break;
      case NetworkMessage::RESERVE_OBJECT_INSTANCE_NAME_FAILED:
          D.Out(pdTrace, "Receiving Message from RTIG, "
    			  " type reserveObjectInstanceNameFaild.");
    	  queues->insertLastCommand(msg);
	  break;
      	
      default:
      {
          D.Out(pdTrace,
                "Receving Message from RTIG, unknown type %d.", msgType);
          delete msg ;
          throw RTIinternalError(stringize() << "Unknown Message type <" << msgType << "> received from RTIG.");
      }
    }

    stat.rtiService(msgType);
    G.Out(pdGendoc,"exit  RTIA::processNetworkMessage");
}

}} // namespace certi/rtia

// $Id: RTIA_network.cc,v 3.43 2011/07/11 11:17:24 erk Exp $
