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

#include <assert.h>

#include <config.h>

#include <libCERTI/Interaction.hh>
#include <libCERTI/InteractionSet.hh>
#include <libCERTI/NM_Classes.hh>
#include <libCERTI/ObjectClass.hh>
#include <libCERTI/ObjectClassAttribute.hh>
#include <libCERTI/ObjectClassSet.hh>
#include <libCERTI/ObjectSet.hh>

#include "RTIA.hh"

namespace certi {
namespace rtia {

static PrettyDebug D("RTIA", "[RTIA net] ");
static PrettyDebug G("GENDOC", __FILE__);
static PrettyDebug DNULL("RTIA_NULLMSG", "[RTIA NULL MSG] ");

// ----------------------------------------------------------------------------
//! RTIA processes a message coming from network.
void RTIA::processNetworkMessage(NetworkMessage* request)
{
    NetworkMessage::Type msgType = request->getMessageType();

    Debug(G, pdGendoc) << "enter RTIA::processNetworkMessage" << std::endl;

    switch (msgType) {
    case NetworkMessage::Type::ADDITIONAL_FOM_MODULE: {
        Debug(D, pdTrace) << "Receiving Message from RTIG, type NetworkMessage::ADDITIONAL_FOM_MODULE "
                          << request->getDate().getTime() << std::endl;
        my_root_object.rebuildFromSerializedFOM(*static_cast<NM_Additional_Fom_Module*>(request));
        
        break;
    }
    
    case NetworkMessage::Type::MESSAGE_NULL: {
        Debug(D, pdTrace) << "Receiving Message from RTIG, type NetworkMessage::MESSAGE_NULL "
                          << request->getDate().getTime() << std::endl;

        //request->show(std::cout);
        Debug(DNULL, pdDebug) << "NULL message received (Federate=" << request->getFederate()
                              << ", Time = " << request->getDate().getTime() << ")" << std::endl;
        tm.update(request->getFederate(), request->getDate());
        delete request;
        break;
    }

    case NetworkMessage::Type::SET_TIME_REGULATING: {
        // Another federate is becoming regulating.
        Debug(D, pdTrace) << "Receving Message from RTIG, type NetworkMessage::SET_TIME_REGULATING." << std::endl;

        if (static_cast<NM_Set_Time_Regulating*>(request)->isRegulatorOn())
            tm.insert(request->getFederate(), request->getDate());
        else
            tm.remove(request->getFederate());
        delete request;
        break;
    }

    case NetworkMessage::Type::CONFIRM_SYNCHRONIZATION_POINT_REGISTRATION:
        Debug(D, pdTrace) << "Receiving Message from RTIG, type NetworkMessage::CONFIRM_"
                             "SYNCHRONIZATION_POINT_REGISTRATION."
                          << std::endl;

        queues.insertLastCommand(request);
        break;
    case NetworkMessage::Type::ANNOUNCE_SYNCHRONIZATION_POINT:
        Debug(D, pdTrace) << "Receiving Message from RTIG, type NetworkMessage::ANNOUCE_"
                             "SYNCHRONIZATION_POINT."
                          << std::endl;

        queues.insertLastCommand(request);
        break;
    case NetworkMessage::Type::FEDERATION_SYNCHRONIZED:
        Debug(D, pdTrace) << "Receiving Message from RTIG, type NetworkMessage::FEDERATION_SYNCHRONIZED." << std::endl;
        queues.insertLastCommand(request);
        break;

    case NetworkMessage::Type::DISCOVER_OBJECT: {
        NM_Discover_Object* DO = static_cast<NM_Discover_Object*>(request);
        Debug(D, pdTrace) << "Receving Message from RTIG, type NetworkMessage::DISCOVER_OBJECT." << std::endl;
        queues.insertFifoMessage(request);
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
            auto responses = my_root_object.registerObjectInstance(
                fm.getFederateHandle(), DO->getObjectClass(), DO->getObject(), DO->getLabel());
            std::cout << "==========================" << std::endl;
            std::cout << "RESPONSES FROM ROOT OBJECT" << std::endl;
            for (auto& rep : responses) {
                std::cout << (int) rep.message()->getMessageType() << std::endl;
            }
            std::cout << "==========================" << std::endl;
        }
        catch (ObjectAlreadyRegistered&) {
        }

    } break;

    case NetworkMessage::Type::REFLECT_ATTRIBUTE_VALUES: {
        NM_Reflect_Attribute_Values* RAV = static_cast<NM_Reflect_Attribute_Values*>(request);
        OrderType updateOrder;

        //RAV->show(std::cerr);
        Debug(D, pdTrace) << "Receiving Message from RTIG, "
                             "type NetworkMessage::REFLECT_ATTRIBUTE_VALUES."
                          << std::endl;

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
        if (!request->isDated()) {
            // without time
            updateOrder = RECEIVE;
        }
        else {
            // Retrieve order type
            updateOrder = TIMESTAMP;
            // FIXME we may inspect rootObject for an unknown object which has
            // (not yet) been discovered...because DiscoverObject is a RO message
            ObjectClassHandle och = my_root_object.objects->getObjectClass(RAV->getObject());
            //std::cerr << "FOUND och = " <<och << "  for object " << RAV->getObject() <<std::endl;
            for (uint32_t i = 0; i < RAV->getAttributesSize(); ++i) {
                // FIXME we need an object **CLASS** handle and not an **OBJECT* handle
                //if (rootObject->ObjectClasses->getObjectFromHandle(RAV->getObject())
                if (my_root_object.ObjectClasses->getObjectFromHandle(och)->getAttribute(RAV->getAttributes(i))->order
                    != TIMESTAMP) {
                    updateOrder = RECEIVE;
                    break;
                }
            }
        }

        // Decide which queue will be used
        if (updateOrder == TIMESTAMP && tm.requestContraintState()) {
            // Update is TSO
            queues.insertTsoMessage(request);
        }
        else {
            // Update is RO
            queues.insertFifoMessage(request);
        }

        break;
    }

    case NetworkMessage::Type::RECEIVE_INTERACTION: {
        NM_Receive_Interaction* RI = static_cast<NM_Receive_Interaction*>(request);
        OrderType interactionOrder;

        Debug(D, pdTrace) << "Receving Message from RTIG, type NetworkMessage::RECEIVE_INTERACTION." << std::endl;

        // Here we have to consider RAV without time
        if (!request->isDated()) {
            // without time
            interactionOrder = RECEIVE;
        }
        else {
            // Retrieve order type
            interactionOrder = my_root_object.Interactions->getObjectFromHandle(RI->getInteractionClass())->order;
        }

        // Decide which queue will be used
        if (interactionOrder == TIMESTAMP && tm.requestContraintState()) {
            // Interaction is TSO
            queues.insertTsoMessage(request);
        }
        else {
            // Interaction is RO
            queues.insertFifoMessage(request);
        }

        break;
    }

    case NetworkMessage::Type::REMOVE_OBJECT: {
        Debug(D, pdTrace) << "Receving Message from RTIG, \
	  		  type NetworkMessage::REMOVE_OBJECT."
                          << std::endl;

        if (tm.requestContraintState() && request->isDated()) {
            // Verify that received TSO timestamp is >= current
            // time + lookahead
            queues.insertTsoMessage(request);
        }
        else {
            queues.insertFifoMessage(request);
        }

        break;
    }

    case NetworkMessage::Type::INFORM_ATTRIBUTE_OWNERSHIP: {
        Debug(D, pdTrace) << "Receving Message from RTIG, "
                             "type NetworkMessage::INFORM_ATTRIBUTE_OWNERSHIP."
                          << std::endl;
        queues.insertFifoMessage(request);
        break;
    }

    case NetworkMessage::Type::ATTRIBUTE_IS_NOT_OWNED: {
        Debug(D, pdTrace) << "Receving Message from RTIG, type NetworkMessage::ATTRIBUTE_IS_NOT_OWNED." << std::endl;
        queues.insertFifoMessage(request);
        break;
    }

    case NetworkMessage::Type::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION: {
        Debug(D, pdTrace) << "Receving Message from RTIG, "
                             "type NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION."
                          << std::endl;
        queues.insertFifoMessage(request);
        break;
    }

    case NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_UNAVAILABLE: {
        Debug(D, pdTrace) << "Receving Message from RTIG, "
                             "type NetworkMessage::ATTRIBUTE_OWNERSHIP_UNAVAILABLE."
                          << std::endl;
        queues.insertFifoMessage(request);
        break;
    }

    case NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION: {
        Debug(D, pdTrace) << "Receving Message from RTIG, "
                             "type NetworkMessage::ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION."
                          << std::endl;
        queues.insertFifoMessage(request);
        break;
    }

    case NetworkMessage::Type::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION: {
        Debug(D, pdTrace) << "Receving Message from RTIG, "
                             "type NetworkMessage::ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION."
                          << std::endl;
        queues.insertFifoMessage(request);
        break;
    }

    case NetworkMessage::Type::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE: {
        Debug(D, pdTrace) << "Receving Message from RTIG, "
                             "type NetworkMessage::REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE."
                          << std::endl;
        queues.insertFifoMessage(request);
        break;
    }

    case NetworkMessage::Type::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION: {
        Debug(D, pdTrace) << "Receving Message from RTIG, "
                             "type NetworkMessage::CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION."
                          << std::endl;
        queues.insertFifoMessage(request);
        break;
    }

    case NetworkMessage::Type::INITIATE_FEDERATE_SAVE:
        Debug(D, pdTrace) << "Receiving Message from RTIG, "
                             " type InitiateFederateSave."
                          << std::endl;
        queues.insertBeginCommand(request);
        break;

    case NetworkMessage::Type::FEDERATION_SAVED:
    case NetworkMessage::Type::FEDERATION_NOT_SAVED:
        Debug(D, pdTrace) << "Receiving Message from RTIG, "
                             " type Federation(Not)Saved."
                          << std::endl;
        queues.insertBeginCommand(request);
        break;

    case NetworkMessage::Type::REQUEST_FEDERATION_RESTORE_SUCCEEDED:
        Debug(D, pdTrace) << "Receiving Message from RTIG, "
                             " type RequestFederationRestoreSucceeded."
                          << std::endl;
        Debug(G, pdGendoc) << "processNetworkMessage for REQUEST_FEDERATION_RESTORE_SUCCEEDED" << std::endl;
        queues.insertLastCommand(request);
        break;

    case NetworkMessage::Type::REQUEST_FEDERATION_RESTORE_FAILED:
        Debug(D, pdTrace) << "Receiving Message from RTIG, "
                             " type RequestFederationRestoreFailed."
                          << std::endl;
        Debug(G, pdGendoc) << "processNetworkMessage for REQUEST_FEDERATION_RESTORE_FAILED" << std::endl;
        queues.insertLastCommand(request);
        break;

    case NetworkMessage::Type::FEDERATION_RESTORE_BEGUN:
        Debug(D, pdTrace) << "Receiving Message from RTIG, "
                             " type FederationRestoreBegun."
                          << std::endl;
        queues.insertLastCommand(request);
        break;

    case NetworkMessage::Type::INITIATE_FEDERATE_RESTORE:
        Debug(D, pdTrace) << "Receiving Message from RTIG, "
                             " type InitiateFederateRestore."
                          << std::endl;
        Debug(G, pdGendoc) << "processNetworkMessage for INITIATE_FEDERATE_RESTORE" << std::endl;
        queues.insertLastCommand(request);
        break;

    case NetworkMessage::Type::FEDERATION_RESTORED:
    case NetworkMessage::Type::FEDERATION_NOT_RESTORED:
        Debug(D, pdTrace) << "Receiving Message from RTIG, "
                             " type Federation(Not)Restored."
                          << std::endl;
        queues.insertLastCommand(request);
        break;

    case NetworkMessage::Type::PROVIDE_ATTRIBUTE_VALUE_UPDATE:
        Debug(D, pdTrace) << "Receiving Message from RTIG, "
                             " type ProvideAttributeValueUpdate."
                          << std::endl;
        queues.insertFifoMessage(request);
        break;

    case NetworkMessage::Type::TIME_REGULATION_ENABLED:
        Debug(D, pdTrace) << "Receiving Message from RTIG, "
                             " type TimeRegulationEnabled."
                          << std::endl;
        queues.insertLastCommand(request);
        break;
    case NetworkMessage::Type::TIME_CONSTRAINED_ENABLED:
        Debug(D, pdTrace) << "Receiving Message from RTIG, "
                             " type TimeConstrainedEnabled."
                          << std::endl;
        queues.insertLastCommand(request);
        break;
    case NetworkMessage::Type::SET_CLASS_RELEVANCE_ADVISORY_SWITCH:
        Debug(D, pdTrace) << "Receiving Message from RTIG, "
                             " type setCRAS."
                          << std::endl;
        break;
    case NetworkMessage::Type::SET_INTERACTION_RELEVANCE_ADVISORY_SWITCH:
        Debug(D, pdTrace) << "Receiving Message from RTIG, "
                             " type setIRAS."
                          << std::endl;
        break;
    case NetworkMessage::Type::SET_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH:
        Debug(D, pdTrace) << "Receiving Message from RTIG, "
                             " type setARAS."
                          << std::endl;
        break;
    case NetworkMessage::Type::SET_ATTRIBUTE_SCOPE_ADVISORY_SWITCH:
        Debug(D, pdTrace) << "Receiving Message from RTIG, "
                             " type setASAS."
                          << std::endl;
        break;
    case NetworkMessage::Type::START_REGISTRATION_FOR_OBJECT_CLASS:
        Debug(D, pdTrace) << "Receiving Message from RTIG, "
                             " type StartRegForObjClass."
                          << std::endl;
        queues.insertLastCommand(request);
        break;
    case NetworkMessage::Type::RESERVE_OBJECT_INSTANCE_NAME_SUCCEEDED:
        Debug(D, pdTrace) << "Receiving Message from RTIG, "
                             " type reserveObjectInstanceNameSucceeded."
                          << std::endl;
        queues.insertLastCommand(request);
        break;
    case NetworkMessage::Type::RESERVE_OBJECT_INSTANCE_NAME_FAILED:
        Debug(D, pdTrace) << "Receiving Message from RTIG, "
                             " type reserveObjectInstanceNameFaild."
                          << std::endl;
        queues.insertLastCommand(request);
        break;

    case NetworkMessage::Type::TIME_ADVANCE_REQUEST: {
        Debug(D, pdTrace) << "Receiving Message from RTIG, type TimeAdvanceRequest." << std::endl;

        Exception::Type e;
        tm.timeAdvanceRequest(request->getDate(), e);
        if (e != Exception::Type::NO_EXCEPTION) {
            Debug(D, pdTrace) << "TAR threw, but it was forced by a MOM interaction. Discard." << std::endl;
        }
        break;
    }

    case NetworkMessage::Type::TIME_ADVANCE_REQUEST_AVAILABLE: {
        Debug(D, pdTrace) << "Receiving Message from RTIG, type TimeAdvanceRequestAvailable." << std::endl;

        Exception::Type e;
        tm.timeAdvanceRequestAvailable(request->getDate(), e);
        if (e != Exception::Type::NO_EXCEPTION) {
            Debug(D, pdTrace) << "TAR threw, but it was forced by a MOM interaction. Discard." << std::endl;
        }
        break;
    }

    case NetworkMessage::Type::NEXT_MESSAGE_REQUEST: {
        Debug(D, pdTrace) << "Receiving Message from RTIG, type NestEventRequest." << std::endl;

        Exception::Type e;
        tm.nextEventRequest(request->getDate(), e);
        if (e != Exception::Type::NO_EXCEPTION) {
            Debug(D, pdTrace) << "TAR threw, but it was forced by a MOM interaction. Discard." << std::endl;
        }
        break;
    }

    case NetworkMessage::Type::NEXT_MESSAGE_REQUEST_AVAILABLE: {
        Debug(D, pdTrace) << "Receiving Message from RTIG, type NestEventRequestAvailable." << std::endl;

        Exception::Type e;
        tm.nextEventRequestAvailable(request->getDate(), e);
        if (e != Exception::Type::NO_EXCEPTION) {
            Debug(D, pdTrace) << "TAR threw, but it was forced by a MOM interaction. Discard." << std::endl;
        }
        break;
    }
    
    case NetworkMessage::Type::MOM_STATUS: {
        NM_Mom_Status* status = static_cast<NM_Mom_Status*>(request);
        Debug(D, pdTrace) << "Received mom status, enable=" << status->getMomState() << ", period=" << status->getUpdatePeriod() << std::endl;

        if(status->getMomState() == false) {
            tm.setMomUpdateRate(std::chrono::seconds(0));
        }
        else {
            tm.setMomUpdateRate(std::chrono::seconds(status->getUpdatePeriod()));
        }
        
        break;
    }

    default: {
        Debug(D, pdTrace) << "Receving Message from RTIG, unknown type " << static_cast<int>(msgType) << std::endl;
        delete request;
        throw RTIinternalError("Unknown Message type <"
                               + std::to_string(static_cast<std::underlying_type<NetworkMessage::Type>::type>(msgType))
                               + "> received from RTIG.");
    }
    }

    stat.rtiService(msgType);
    Debug(G, pdGendoc) << "exit  RTIA::processNetworkMessage" << std::endl;
}
}
} // namespace certi/rtia

// $Id: RTIA_network.cc,v 3.43 2011/07/11 11:17:24 erk Exp $
