// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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
// ----------------------------------------------------------------------------

#include "RTIA.hh"

#include <assert.h>
#include <memory>

#include <config.h>

#include <libCERTI/M_Classes.hh>
#include <libCERTI/RoutingSpace.hh>
#include <libCERTI/XmlParser.hh>
#include <libCERTI/fed.hh>
#include <libHLA/Clock.hh>

using std::string;
using std::ifstream;

namespace certi {
namespace rtia {

static PrettyDebug D("RTIA", __FILE__);
static PrettyDebug G("GENDOC", __FILE__);

// ----------------------------------------------------------------------------
//! Verify that federate is not in saving or restoring state.
void RTIA::saveAndRestoreStatus(Message::Type type) throw(SaveInProgress, RestoreInProgress)
{
    G.Out(pdGendoc, "enter RTIA::saveAndRestoreStatus");
    //D.Mes(pdMessage, 'N', type);

    switch (type) {
    case Message::OPEN_CONNEXION:
    case Message::CLOSE_CONNEXION:
    case Message::RESIGN_FEDERATION_EXECUTION:
    case Message::TICK_REQUEST:
    case Message::TICK_REQUEST_NEXT:
    case Message::TICK_REQUEST_STOP:
    case Message::GET_OBJECT_CLASS_HANDLE:
    case Message::GET_OBJECT_CLASS_NAME:
    case Message::GET_ATTRIBUTE_HANDLE:
    case Message::GET_ATTRIBUTE_NAME:
    case Message::GET_INTERACTION_CLASS_HANDLE:
    case Message::GET_INTERACTION_CLASS_NAME:
    case Message::GET_PARAMETER_HANDLE:
    case Message::GET_PARAMETER_NAME:
    case Message::GET_OBJECT_INSTANCE_HANDLE:
    case Message::GET_OBJECT_INSTANCE_NAME:
    case Message::GET_SPACE_HANDLE:
    case Message::GET_SPACE_NAME:
    case Message::GET_DIMENSION_HANDLE:
    case Message::GET_DIMENSION_NAME:
    case Message::GET_ATTRIBUTE_SPACE_HANDLE:
    case Message::GET_OBJECT_CLASS:
    case Message::GET_INTERACTION_SPACE_HANDLE:
    case Message::GET_TRANSPORTATION_HANDLE:
    case Message::GET_TRANSPORTATION_NAME:
    case Message::GET_ORDERING_HANDLE:
    case Message::GET_ORDERING_NAME:
        break;
    case Message::FEDERATE_SAVE_BEGUN:
    case Message::FEDERATE_SAVE_COMPLETE:
    case Message::FEDERATE_SAVE_NOT_COMPLETE:
        fm->checkFederationRestoring();
        break;
    case Message::FEDERATE_RESTORE_COMPLETE:
    case Message::FEDERATE_RESTORE_NOT_COMPLETE:
        fm->checkFederationSaving();
        break;
    default:
        fm->checkFederationSaving();
        fm->checkFederationRestoring();
        break;
    }
    G.Out(pdGendoc, "exit  RTIA::saveAndRestoreStatus");
}

// ----------------------------------------------------------------------------
//! Choose federate processing.
void RTIA::chooseFederateProcessing(Message* request, Message* answer, Exception::Type& e)
{
    G.Out(pdGendoc,
          "enter RTIA::chooseFederateProcessing for msg <%s> (type=%d)",
          request->getMessageName(),
          request->getMessageType());

    // Verify not in saving or restoring state.
    // May throw SaveInProgress or RestoreInProgress
    saveAndRestoreStatus(request->getMessageType());

    e = Exception::Type::NO_EXCEPTION;

    switch (request->getMessageType()) {
    case Message::CLOSE_CONNEXION:
        D.Out(pdTrace, "Receiving Message from Federate, type CloseConnexion.");
        fm->_connection_state = FederationManagement::CONNECTION_FIN;
        // the this->comm can now be used only to sent the CLOSE_CONNEXION response
        break;

    case Message::CREATE_FEDERATION_EXECUTION: {
        M_Create_Federation_Execution *CFEq, *CFEr;
        CFEr = static_cast<M_Create_Federation_Execution*>(answer);
        CFEq = static_cast<M_Create_Federation_Execution*>(request);
        D.Out(pdTrace, "Receiving Message from Federate, type CreateFederation.");
        // Store FEDid for future usage (JOIN_FEDERATION_EXECUTION) into fm
        fm->createFederationExecution(CFEq->getFederationName(), CFEq->getFEDid(), e);
        if (e == Exception::Type::RTIinternalError) {
            answer->setException(e, "Federate is yet a creator or a member !");
        }
        D.Out(pdTrace,
              "Receiving Message from Federate, "
              "type CreateFederation done.");
        // RTIA needs FEDid into the answer (rep Message) to federate
        CFEr->setFEDid(CFEq->getFEDid());
        // RTIA needs federation name into the answer (rep Message) to federate
        CFEr->setFederationName(CFEq->getFederationName());
    } break;

    case Message::DESTROY_FEDERATION_EXECUTION: {
        M_Destroy_Federation_Execution *DFEq, *DFEr;
        DFEr = static_cast<M_Destroy_Federation_Execution*>(answer);
        DFEq = static_cast<M_Destroy_Federation_Execution*>(request);
        D.Out(pdTrace, "Receiving Message from Federate, type DestroyFederation.");

        fm->destroyFederationExecution(DFEq->getFederationName(), e);
        if (e == Exception::Type::RTIinternalError) {
            answer->setException(e, "Illegal federation handle");
        }
        // RTIA needs federation name into the answer (rep Message) to federate
        DFEr->setFederationName(DFEq->getFederationName());
    } break;

    case Message::JOIN_FEDERATION_EXECUTION: {
        M_Join_Federation_Execution *JFEq, *JFEr;
        JFEr = static_cast<M_Join_Federation_Execution*>(answer);
        JFEq = static_cast<M_Join_Federation_Execution*>(request);
        D.Out(pdTrace, "Receiving Message from Federate, type JoinFederation.");
        JFEr->setFederate(
            fm->joinFederationExecution(JFEq->getFederateName(), JFEq->getFederationName(), rootObject, e));
        if (e == Exception::Type::NO_EXCEPTION) {
            /// Set RTIA PrettyDebug federate name
            PrettyDebug::setFederateName("RTIA::" + JFEq->getFederateName());
            // Set federation name for the answer message (rep)
            JFEr->setFederationName(JFEq->getFederationName());
            JFEr->setFederateName(JFEq->getFederateName());
        }
        else {
            // JOIN FAILED
            switch (e) {
            case Exception::Type::FederateAlreadyExecutionMember:
                throw FederateAlreadyExecutionMember("Federate yet joined or same name");
                break;
            case Exception::Type::FederationExecutionDoesNotExist:
                throw FederationExecutionDoesNotExist("Federation does not exist [yet]");
                break;
            case Exception::Type::SaveInProgress:
                throw SaveInProgress("Save in progress");
                break;
            case Exception::Type::RestoreInProgress:
                throw RestoreInProgress("Restore in progress");
                break;
            case Exception::Type::RTIinternalError:
            default:
                throw RTIinternalError("Internal error");
                break;
            }
        }
    } break;
    case Message::RESIGN_FEDERATION_EXECUTION: {
        M_Resign_Federation_Execution* RFEq;
        RFEq = static_cast<M_Resign_Federation_Execution*>(request);
        D.Out(pdTrace, "Receiving Message from Federate, type ResignFederation.");

        switch (RFEq->getResignAction()) {
        case RELEASE_ATTRIBUTES:
            // TODO: Add attribute handling here
            break;
        case DELETE_OBJECTS:
            om->deleteAllObjects(e);
            break;
        case DELETE_OBJECTS_AND_RELEASE_ATTRIBUTES:
            // TODO: Add attribute handling here
            om->deleteAllObjects(e);
            break;
        case NO_ACTION:
            // NOTHING to DO
            break;
        }

        fm->resignFederationExecution(RFEq->getResignAction(), e);
    } break;

    case Message::REGISTER_FEDERATION_SYNCHRONIZATION_POINT: {
        M_Register_Federation_Synchronization_Point* RFSPq;
        RFSPq = static_cast<M_Register_Federation_Synchronization_Point*>(request);
        D.Out(pdTrace, "Receiving Message from Federate, type RegisterFederationSynchronizationPoint.");

        // Check whether if the with federates set size is strictly positive,
        // which means with federate set.
        if (RFSPq->getFederateSetSize() > 0) {
            fm->registerSynchronization(request->getLabel(),
                                        request->getTag(),
                                        static_cast<unsigned short>(RFSPq->getFederateSetSize()),
                                        RFSPq->getFederateSet(),
                                        e);
        }
        else {
            fm->registerSynchronization(request->getLabel(), request->getTag(), e);
        }
    } break;

    case Message::SYNCHRONIZATION_POINT_ACHIEVED:
        D.Out(pdTrace, "Receiving Message from Federate, type SynchronizationPointAchieved.");
        fm->unregisterSynchronization(request->getLabel(), e);
        break;

    case Message::REQUEST_FEDERATION_SAVE:
        if (request->isDated()) {
            D.Out(pdTrace,
                  "Receiving Message from Federate, type"
                  " RequestFederationSave with time.");
            fm->requestFederationSave(request->getLabel(), request->getDate(), e);
            answer->setDate(request->getDate());
        }
        else {
            D.Out(pdTrace,
                  "Receiving Message from Federate, type"
                  " RequestFederationSave without time.");
            fm->requestFederationSave(request->getLabel(), e);
        }
        break;

    case Message::FEDERATE_SAVE_BEGUN:
        D.Out(pdTrace,
              "Receiving Message from Federate, type"
              " FederateSaveBegun.");
        G.Out(pdGendoc, "chooseFederateProcessing FEDERATE_SAVE_BEGUN");
        fm->federateSaveBegun(e);
        break;

    case Message::FEDERATE_SAVE_COMPLETE:
    case Message::FEDERATE_SAVE_NOT_COMPLETE: {
        D.Out(pdTrace,
              "Receiving Message from Federate, type"
              " FederateSave(Not)Complete.");
        bool result = request->getMessageType() == Message::FEDERATE_SAVE_COMPLETE;
        if (result) {
            G.Out(pdGendoc, "chooseFederateProcessing FEDERATE_SAVE_COMPLETE");
        }
        else {
            G.Out(pdGendoc, "chooseFederateProcessing FEDERATE_SAVE_NOT_COMPLETE");
        }
        fm->federateSaveStatus(result, e);
    } break;

    case Message::REQUEST_FEDERATION_RESTORE: {
        D.Out(pdTrace,
              "Receiving Message from Federate, type"
              " RequestFederationRestore.");
        G.Out(pdGendoc, "chooseFederateProcessing REQUEST_FEDERATION_RESTORE");

        fm->requestFederationRestore(request->getLabel(), e);
    } break;

    case Message::FEDERATE_RESTORE_COMPLETE:
        fm->federateRestoreStatus(true, e);
        break;
    case Message::FEDERATE_RESTORE_NOT_COMPLETE:
        fm->federateRestoreStatus(false, e);
        break;

    case Message::PUBLISH_OBJECT_CLASS: {
        M_Publish_Object_Class* POCq;
        POCq = static_cast<M_Publish_Object_Class*>(request);
        D.Out(pdTrace, "Receiving Message from Federate, type PublishObjectClass.");

        dm->publishObjectClass(POCq->getObjectClass(), POCq->getAttributes(), e);
    } break;

    case Message::UNPUBLISH_OBJECT_CLASS: {
        M_Unpublish_Object_Class* UOCq;
        UOCq = static_cast<M_Unpublish_Object_Class*>(request);
        D.Out(pdTrace, "Receiving Message from Federate, type UnpublishObjectClass.");
        dm->unpublishObjectClass(UOCq->getObjectClass(), e);
    } break;

    case Message::PUBLISH_INTERACTION_CLASS: {
        M_Publish_Interaction_Class* PICq;
        PICq = static_cast<M_Publish_Interaction_Class*>(request);
        D.Out(pdTrace, "Receiving Message from Federate, type PublishInteractionClass.");

        dm->publishInteractionClass(PICq->getInteractionClass(), e);
    } break;

    case Message::UNPUBLISH_INTERACTION_CLASS: {
        M_Unpublish_Interaction_Class* UICq;
        UICq = static_cast<M_Unpublish_Interaction_Class*>(request);
        D.Out(pdTrace,
              "Receiving Message from Federate, "
              "type UnpublishInteractionClass");

        dm->unpublishInteractionClass(UICq->getInteractionClass(), e);
    } break;

    case Message::SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES: {
        M_Subscribe_Object_Class_Attributes* SOCAq;
        SOCAq = static_cast<M_Subscribe_Object_Class_Attributes*>(request);
        D.Out(pdTrace, "Receiving Message from Federate, type SubscribeObjectClass.");
        dm->subscribeObjectClassAttribute(
            SOCAq->getObjectClass(), SOCAq->getAttributes(), SOCAq->getAttributesSize(), e);
    } break;

    case Message::UNSUBSCRIBE_OBJECT_CLASS: {
        M_Unsubscribe_Object_Class* UsOCq;
        UsOCq = static_cast<M_Unsubscribe_Object_Class*>(request);
        D.Out(pdTrace, "Receiving Message from Federate, type UnsubscribeObjectClass.");

        // FIXME BUG: Why attributes are not transmitted ?
        dm->unsubscribeObjectClassAttribute(UsOCq->getObjectClass(), e);
    } break;

    case Message::SUBSCRIBE_INTERACTION_CLASS: {
        M_Subscribe_Interaction_Class* SICq;
        SICq = static_cast<M_Subscribe_Interaction_Class*>(request);
        D.Out(pdTrace, "Receiving Message from Federate, type SubscribeInteraction.");

        dm->subscribeInteractionClass(SICq->getInteractionClass(), e);
    } break;

    case Message::UNSUBSCRIBE_INTERACTION_CLASS: {
        M_Unsubscribe_Interaction_Class* UsICq;
        UsICq = static_cast<M_Unsubscribe_Interaction_Class*>(request);
        D.Out(pdTrace, "Receiving Message from Federate, type UnsubscribeInteraction.");

        dm->unsubscribeInteractionClass(UsICq->getInteractionClass(), e);
    } break;

    case Message::RESERVE_OBJECT_INSTANCE_NAME: {
        M_Reserve_Object_Instance_Name *ROINq, *ROINr;
        ROINq = static_cast<M_Reserve_Object_Instance_Name*>(request);
        ROINr = static_cast<M_Reserve_Object_Instance_Name*>(answer);

        ROINr->setObjectName(ROINq->getObjectName());
        om->reserveObjectName(ROINq->getObjectName(), e);
        break;
    }

    case Message::REGISTER_OBJECT_INSTANCE: {
        M_Register_Object_Instance *ROIq, *ROIr;
        ROIq = static_cast<M_Register_Object_Instance*>(request);
        ROIr = static_cast<M_Register_Object_Instance*>(answer);
        FederationTime date = tm->requestFederateTime();
        FederationTime heure = date + tm->requestLookahead();

        D.Out(pdTrace, "Receiving Message from Federate, type RegisterObject.");

        ROIr->setObject(om->registerObject(ROIq->getObjectClass(), ROIq->getObjectName(), date, heure, e));
        break;
    }

    case Message::UPDATE_ATTRIBUTE_VALUES: {
        M_Update_Attribute_Values *UAVq, *UAVr;
        EventRetraction event;
        UAVq = static_cast<M_Update_Attribute_Values*>(request);
        UAVr = static_cast<M_Update_Attribute_Values*>(answer);
        if (request->isDated()) {
            D.Out(pdTrace,
                  "Receiving Message from Federate, type "
                  "UpdateAttribValues with TIMESTAMP.");
            event.setSN(om->updateAttributeValues(UAVq->getObject(),
                                                  UAVq->getAttributes(),
                                                  UAVq->getValues(),
                                                  UAVq->getAttributesSize(),
                                                  UAVq->getDate(),
                                                  UAVq->getTag(),
                                                  e));
            //FIXME event.setSendingFederate()
            UAVr->setEventRetraction(event);
            // answer should contains the date too
            UAVr->setDate(UAVq->getDate());
        }
        else {
            D.Out(pdTrace,
                  "Receiving Message from Federate, type "
                  "UpdateAttribValues without TIMESTAMP.");
            om->updateAttributeValues(UAVq->getObject(),
                                      UAVq->getAttributes(),
                                      UAVq->getValues(),
                                      UAVq->getAttributesSize(),
                                      UAVq->getTag(),
                                      e);
        }
    } break;

    case Message::SEND_INTERACTION: {
        M_Send_Interaction *SIr, *SIq;
        EventRetraction event;
        SIq = static_cast<M_Send_Interaction*>(request);
        SIr = static_cast<M_Send_Interaction*>(answer);
        G.Out(pdGendoc, "S_I into RTIA::chooseFederateProcessing");

        if (SIq->isDated()) {
            D.Out(pdTrace, "Receiving Message from Federate, type SendInteraction with TIMESTAMP.");
            event.setSN(om->sendInteraction(SIq->getInteractionClass(),
                                            SIq->getParameters(),
                                            SIq->getValues(),
                                            SIq->getParametersSize(),
                                            SIq->getDate(),
                                            SIq->getTag(),
                                            SIq->getRegion(),
                                            e));
            SIr->setEventRetraction(event);
        }
        else {
            D.Out(pdTrace, "Receiving Message from Federate, type SendInteraction without TIMESTAMP.");
            om->sendInteraction(SIq->getInteractionClass(),
                                SIq->getParameters(),
                                SIq->getValues(),
                                SIq->getParametersSize(),
                                SIq->getTag(),
                                SIq->getRegion(),
                                e);
        }
    } break;

    case Message::DELETE_OBJECT_INSTANCE: {
        M_Delete_Object_Instance *DOIr, *DOIq;
        EventRetraction event;
        DOIq = static_cast<M_Delete_Object_Instance*>(request);
        DOIr = static_cast<M_Delete_Object_Instance*>(answer);
        G.Out(pdGendoc, "D_O_I into RTIA::chooseFederateProcessing");

        if (DOIq->isDated()) {
            D.Out(pdTrace, "Receiving Message from Federate, type DeleteObjectInstance with \
            TIMESTAMP.");
            event.setSN(om->deleteObject(DOIq->getObject(), DOIq->getDate(), DOIq->getTag(), e));
            // FIXME event.setSendingFederate();
            DOIr->setEventRetraction(event);
        }
        else {
            D.Out(pdTrace, "Receiving Message from Federate, type DeleteObjectInstance without \
            TIMESTAMP.");
            om->deleteObject(DOIq->getObject(), DOIq->getTag(), e);
        }
    } break;

    case Message::CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE: {
        M_Change_Attribute_Transportation_Type* CATTq;
        CATTq = static_cast<M_Change_Attribute_Transportation_Type*>(request);
        D.Out(pdTrace, "Receiving Message from Federate, type ChangeAttribTransport.");

        om->changeAttributeTransportType(
            CATTq->getObject(), CATTq->getAttributes(), CATTq->getAttributesSize(), CATTq->getTransportationType(), e);
    } break;

    case Message::CHANGE_ATTRIBUTE_ORDER_TYPE: {
        M_Change_Attribute_Order_Type* CAOTq;
        CAOTq = static_cast<M_Change_Attribute_Order_Type*>(request);

        D.Out(pdTrace, "Receiving Message from Federate, type ChangeAttribOrder.");

        om->changeAttributeOrderType(
            CAOTq->getObject(), CAOTq->getAttributes(), CAOTq->getAttributesSize(), CAOTq->getOrder(), e);
    } break;

    case Message::CHANGE_INTERACTION_TRANSPORTATION_TYPE: {
        M_Change_Interaction_Transportation_Type* CITTq;
        CITTq = static_cast<M_Change_Interaction_Transportation_Type*>(request);

        D.Out(pdTrace, "Receiving Message from Federate, type ChangeInterTransport.");

        om->changeInteractionTransportType(CITTq->getInteractionClass(), CITTq->getTransportationType(), e);
    } break;

    case Message::CHANGE_INTERACTION_ORDER_TYPE: {
        M_Change_Interaction_Order_Type* CIOTq;
        CIOTq = static_cast<M_Change_Interaction_Order_Type*>(request);

        D.Out(pdTrace, "Receiving Message from Federate, type ChangeInterOrder.");

        om->changeInteractionOrderType(CIOTq->getInteractionClass(), CIOTq->getOrder(), e);
    } break;

    case Message::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE: {
        M_Request_Object_Attribute_Value_Update* ROAVUq;
        ROAVUq = static_cast<M_Request_Object_Attribute_Value_Update*>(request);

        D.Out(pdTrace,
              "Receiving Message from Federate, type "
              "RequestAttributeValueUpadate.");
        om->requestObjectAttributeValueUpdate(
            ROAVUq->getObject(), ROAVUq->getAttributes(), ROAVUq->getAttributesSize(), e);
    } break;

    case Message::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE: {
        M_Request_Class_Attribute_Value_Update* RCAVUq;
        RCAVUq = static_cast<M_Request_Class_Attribute_Value_Update*>(request);

        D.Out(pdTrace,
              "Receiving Message from Federate, type "
              "RequestClassAttributeValueUpadate.");
        om->requestClassAttributeValueUpdate(
            RCAVUq->getObjectClass(), RCAVUq->getAttributes(), RCAVUq->getAttributesSize(), e);
    } break;

    case Message::RETRACT:
        std::cout << "retract not yet implemented in chooseFederateProcessing" << std::endl;
        e = Exception::Type::UnimplementedService;
        break;

    case Message::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE: {
        M_Unconditional_Attribute_Ownership_Divestiture* UAODq;
        UAODq = static_cast<M_Unconditional_Attribute_Ownership_Divestiture*>(request);

        D.Out(pdTrace,
              "Receiving Message from Federate, "
              "type unconditionalAttributeOwnershipDivestiture.");
        D.Out(pdTrace, "Object %u number of attributes %u ", UAODq->getObject(), UAODq->getAttributesSize());
        owm->unconditionalAttributeOwnershipDivestiture(
            UAODq->getObject(), UAODq->getAttributes(), UAODq->getAttributesSize(), e);
    } break;

    case Message::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE: {
        M_Negotiated_Attribute_Ownership_Divestiture* NAODq;
        NAODq = static_cast<M_Negotiated_Attribute_Ownership_Divestiture*>(request);
        D.Out(pdTrace,
              "Receiving Message from Federate, "
              "type negotiatedAttributeOwnershipDivestiture.");
        D.Out(pdTrace, "Object %u ; %u nb Attribute ", NAODq->getObject(), NAODq->getAttributesSize());
        owm->negotiatedAttributeOwnershipDivestiture(
            NAODq->getObject(), NAODq->getAttributes(), NAODq->getAttributesSize(), request->getTag(), e);
    } break;

    case Message::ATTRIBUTE_OWNERSHIP_ACQUISITION: {
        M_Attribute_Ownership_Acquisition* AOAq;
        AOAq = static_cast<M_Attribute_Ownership_Acquisition*>(request);

        D.Out(pdTrace,
              "Receiving Message from Federate, "
              "type attributeOwnershipAcquisition.");
        D.Out(pdTrace, "Object %u ; Nb d'Attributs ", AOAq->getObject(), AOAq->getAttributesSize());
        owm->attributeOwnershipAcquisition(
            AOAq->getObject(), AOAq->getAttributes(), AOAq->getAttributesSize(), request->getTag(), e);
    } break;

    case Message::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE: {
        M_Attribute_Ownership_Release_Response *AORRq, *AORRr;
        AORRq = static_cast<M_Attribute_Ownership_Release_Response*>(request);
        AORRr = static_cast<M_Attribute_Ownership_Release_Response*>(answer);

        D.Out(pdTrace,
              "Receiving Message from Federate, "
              "type attributeOwnershipRealeaseResponse.");
        D.Out(pdTrace, "Object %u nb Attribute %u ", AORRq->getObject(), AORRq->getAttributesSize());

        AttributeHandleSet* theAttributes = owm->attributeOwnershipRealeaseResponse(
            AORRq->getObject(), AORRq->getAttributes(), AORRq->getAttributesSize(), e);
        if (e == Exception::Type::NO_EXCEPTION) {
            AORRr->setAttributesSize(theAttributes->size());

            for (unsigned int i = 0; i < theAttributes->size(); i++) {
                AORRr->setAttributes(theAttributes->getHandle(i), i);
            }
        }
    } break;

    case Message::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE: {
        M_Cancel_Negotiated_Attribute_Ownership_Divestiture* CNAODq;
        CNAODq = static_cast<M_Cancel_Negotiated_Attribute_Ownership_Divestiture*>(request);
        D.Out(pdTrace,
              "Receiving Message from Federate, "
              "type cancelNegociatedAttributeOwnershipDivestiture.");
        D.Out(pdTrace, "Object %u nb Attribute %u ", CNAODq->getObject(), CNAODq->getAttributesSize());
        owm->cancelnegotiatedAttributeOwnershipDivestiture(
            CNAODq->getObject(), CNAODq->getAttributes(), CNAODq->getAttributesSize(), e);
    } break;

    case Message::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION: {
        M_Cancel_Attribute_Ownership_Acquisition* CAOAq;
        CAOAq = static_cast<M_Cancel_Attribute_Ownership_Acquisition*>(request);
        D.Out(pdTrace,
              "Receiving Message from Federate, "
              "type cancelAttributeOwnershipAcquisition.");
        D.Out(pdTrace, "Object %u nb Attribute %u ", CAOAq->getObject(), CAOAq->getAttributesSize());
        owm->cancelattributeOwnershipAcquisition(
            CAOAq->getObject(), CAOAq->getAttributes(), CAOAq->getAttributesSize(), e);
    } break;

    case Message::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE: {
        M_Attribute_Ownership_Acquisition_If_Available* AOQIAq;
        AOQIAq = static_cast<M_Attribute_Ownership_Acquisition_If_Available*>(request);

        D.Out(pdTrace,
              "Receiving Message from Federate, "
              "type attributeOwnershipAcquisitionIfAvailable.");
        D.Out(pdTrace, "Object %u nb Attribute %u ", AOQIAq->getObject(), AOQIAq->getAttributesSize());
        owm->attributeOwnershipAcquisitionIfAvailable(
            AOQIAq->getObject(), AOQIAq->getAttributes(), AOQIAq->getAttributesSize(), e);
    } break;

    case Message::QUERY_ATTRIBUTE_OWNERSHIP: {
        M_Query_Attribute_Ownership* QAOq;
        QAOq = static_cast<M_Query_Attribute_Ownership*>(request);
        D.Out(pdTrace,
              "Receiving Message from Federate, "
              "type queryAttributeOwnership.");
        D.Out(pdTrace, "Object %u Attribute %u ", QAOq->getObject(), QAOq->getAttribute());
        owm->queryAttributeOwnership(QAOq->getObject(), QAOq->getAttribute(), e);
    } break;

    case Message::IS_ATTRIBUTE_OWNED_BY_FEDERATE: {
        M_Is_Attribute_Owned_By_Federate* IAWBFq;
        IAWBFq = static_cast<M_Is_Attribute_Owned_By_Federate*>(request);
        D.Out(pdTrace,
              "Receiving Message from Federate, "
              "type isAttributeOwnedByFederate.");
        D.Out(pdTrace, "Object %u Attribute %u ", IAWBFq->getObject(), IAWBFq->getAttribute());
        answer->setTag(owm->attributeOwnedByFederate(IAWBFq->getObject(), IAWBFq->getAttribute(), e));
    } break;

    case Message::QUERY_LBTS:
        D.Out(pdTrace, "Receiving Message from Federate, type RequestLBTS.");

        answer->setDate(tm->requestLBTS());
        break;

    case Message::ENABLE_ASYNCHRONOUS_DELIVERY:
        D.Out(pdTrace,
              "Receiving Message from Federate, "
              "type EnableAsynchronousDelivery.");
        if (!tm->_asynchronous_delivery) {
            tm->_asynchronous_delivery = true;
            
            NM_Enable_Asynchronous_Delivery req;
        
            req.setFederation(fm->_numero_federation);
            req.setFederate(fm->federate);
            comm->sendMessage(&req);
        }
        else {
            e = Exception::Type::AsynchronousDeliveryAlreadyEnabled;
        }
        break;

    case Message::DISABLE_ASYNCHRONOUS_DELIVERY:
        D.Out(pdTrace,
              "Receiving Message from Federate, "
              "type DisableAsynchronousDelivery.");
        if (tm->_asynchronous_delivery) {
            tm->_asynchronous_delivery = false;
            
            NM_Disable_Asynchronous_Delivery req;
            
            req.setFederation(fm->_numero_federation);
            req.setFederate(fm->federate);
            comm->sendMessage(&req);
        }
        else {
            e = Exception::Type::AsynchronousDeliveryAlreadyDisabled;
        }
        break;

    case Message::QUERY_FEDERATE_TIME:
        D.Out(pdTrace, "Receiving Message from Federate, type RequestFederateTime.");

        answer->setDate(tm->requestFederateTime());
        break;

    case Message::QUERY_MIN_NEXT_EVENT_TIME:
        D.Out(pdTrace, "Receiving Message from Federate, type QueryMinNextEventTime.");

        answer->setDate(tm->requestMinNextEventTime());
        break;

    case Message::MODIFY_LOOKAHEAD: {
        M_Modify_Lookahead *MLr, *MLq;
        MLr = static_cast<M_Modify_Lookahead*>(answer);
        MLq = static_cast<M_Modify_Lookahead*>(request);
        D.Out(pdTrace, "Receiving Message from Federate, type SetLookAhead.");

        tm->setLookahead(MLq->getLookahead(), e);
        MLr->setLookahead(MLq->getLookahead());
    } break;

    case Message::QUERY_LOOKAHEAD: {
        M_Query_Lookahead* QLr;
        QLr = static_cast<M_Query_Lookahead*>(answer);
        // The query part is not necessary.
        //M_Query_Lookahead *QLq;
        //QLq = static_cast<M_Query_Lookahead*>(req);
        D.Out(pdTrace, "Receiving Message from Federate, type RequestLookAhead.");

        QLr->setLookahead(tm->requestLookahead().getTime());
    } break;

    case Message::TIME_ADVANCE_REQUEST:
        D.Out(pdTrace, "Receiving Message from Federate, type TimeAdvanceRequest.");

        tm->timeAdvanceRequest(request->getDate(), e);
        break;

    case Message::TIME_ADVANCE_REQUEST_AVAILABLE:
        D.Out(pdTrace, "Receiving Message from Federate, type TimeAdvanceRequestAvailable.");

        tm->timeAdvanceRequestAvailable(request->getDate(), e);
        break;

    case Message::NEXT_EVENT_REQUEST:
        D.Out(pdTrace, "Receiving Message from Federate, type NestEventRequest.");

        tm->nextEventRequest(request->getDate(), e);
        break;

    case Message::NEXT_EVENT_REQUEST_AVAILABLE:
        D.Out(pdTrace, "Receiving Message from Federate, type NestEventRequestAvailable.");

        tm->nextEventRequestAvailable(request->getDate(), e);
        break;

    case Message::FLUSH_QUEUE_REQUEST:
        e = Exception::Type::UnimplementedService;
        break;

    // May throw NameNotFound
    // Exception catched and stored in rep Message for answer
    case Message::GET_OBJECT_CLASS_HANDLE: {
        M_Get_Object_Class_Handle *GOCHq, *GOCHr;
        GOCHq = static_cast<M_Get_Object_Class_Handle*>(request);
        GOCHr = static_cast<M_Get_Object_Class_Handle*>(answer);
        D.Out(pdTrace, "Receiving Message from Federate, type GetObjectClassHandle.");
        try {
            GOCHr->setObjectClass(om->getObjectClassHandle(GOCHq->getClassName()));
            GOCHr->setClassName(GOCHq->getClassName());
        }
        catch (Exception& egoch) {
            answer->setException(static_cast<Exception::Type>(egoch.type()), egoch.reason());
        }
    } break;

    case Message::GET_OBJECT_CLASS_NAME: {
        M_Get_Object_Class_Name *GOCNq, *GOCNr;
        GOCNq = static_cast<M_Get_Object_Class_Name*>(request);
        GOCNr = static_cast<M_Get_Object_Class_Name*>(answer);
        D.Out(pdTrace, "Receiving Message from Federate, type GetObjectClassName.");

        try {
            GOCNr->setClassName(om->getObjectClassName(GOCNq->getObjectClass()));
            GOCNr->setObjectClass(GOCNq->getObjectClass());
        }
        catch (ObjectClassNotDefined&) {
            answer->setException(Exception::Type::ObjectClassNotDefined);
        }
        catch (RTIinternalError&) {
            answer->setException(Exception::Type::RTIinternalError);
        }
    } break;
    case Message::GET_OBJECT_INSTANCE_HANDLE: {
        M_Get_Object_Instance_Handle *GOIHq, *GOIHr;
        GOIHq = static_cast<M_Get_Object_Instance_Handle*>(request);
        GOIHr = static_cast<M_Get_Object_Instance_Handle*>(answer);
        D.Out(pdTrace, "Receiving Message from Federate, type getObjectInstanceHandle.");
        GOIHr->setObject(om->getObjectInstanceHandle(GOIHq->getObjectInstanceName()));
        GOIHr->setObjectInstanceName(GOIHq->getObjectInstanceName());
    } break;

    case Message::GET_OBJECT_INSTANCE_NAME: {
        M_Get_Object_Instance_Name *GOINq, *GOINr;
        GOINq = static_cast<M_Get_Object_Instance_Name*>(request);
        GOINr = static_cast<M_Get_Object_Instance_Name*>(answer);

        D.Out(pdTrace, "Receiving Message from Federate, type getObjectInstanceName.");
        GOINr->setObjectInstanceName(om->getObjectInstanceName(GOINq->getObject()));
        GOINr->setObject(GOINq->getObject());
    } break;

    // May throw NameNotFound
    // Exception catched and stored in rep Message for answer
    case Message::GET_ATTRIBUTE_HANDLE: {
        M_Get_Attribute_Handle *GAHq, *GAHr;
        GAHq = static_cast<M_Get_Attribute_Handle*>(request);
        GAHr = static_cast<M_Get_Attribute_Handle*>(answer);
        D.Out(pdTrace, "Receiving Message from Federate, type GetAttributeHandle.");
        try {
            GAHr->setAttribute(om->getAttributeHandle(GAHq->getAttributeName(), GAHq->getObjectClass()));
            GAHr->setAttributeName(GAHq->getAttributeName());
            GAHr->setObjectClass(GAHq->getObjectClass());
        }
        catch (Exception& egah) {
            answer->setException(static_cast<Exception::Type>(egah.type()), egah.reason());
        }
    } break;

    case Message::GET_ATTRIBUTE_NAME: {
        M_Get_Attribute_Name *GANq, *GANr;
        GANq = static_cast<M_Get_Attribute_Name*>(request);
        GANr = static_cast<M_Get_Attribute_Name*>(answer);
        D.Out(pdTrace, "Receiving Message from Federate, type GetAttributeName.");

        GANr->setAttributeName(om->getAttributeName(GANq->getAttribute(), GANq->getObjectClass()));
    } break;

    case Message::GET_INTERACTION_CLASS_HANDLE: {
        M_Get_Interaction_Class_Handle *GICHq, *GICHr;
        GICHr = static_cast<M_Get_Interaction_Class_Handle*>(answer);
        GICHq = static_cast<M_Get_Interaction_Class_Handle*>(request);

        D.Out(pdTrace, "Receiving Message from Federate, type GetInteractionHandle.");

        GICHr->setInteractionClass(om->getInteractionClassHandle(GICHq->getClassName()));
        GICHr->setClassName(GICHq->getClassName());
    } break;

    case Message::GET_INTERACTION_CLASS_NAME: {
        M_Get_Interaction_Class_Name *GICNq, *GICNr;
        GICNr = static_cast<M_Get_Interaction_Class_Name*>(answer);
        GICNq = static_cast<M_Get_Interaction_Class_Name*>(request);

        D.Out(pdTrace, "Receiving Message from Federate, type GetInteractionName.");

        GICNr->setClassName(om->getInteractionClassName(GICNq->getInteractionClass()));
        GICNr->setInteractionClass(GICNq->getInteractionClass());
    } break;

    case Message::GET_PARAMETER_HANDLE: {
        M_Get_Parameter_Handle *GPHq, *GPHr;
        GPHr = static_cast<M_Get_Parameter_Handle*>(answer);
        GPHq = static_cast<M_Get_Parameter_Handle*>(request);
        D.Out(pdTrace, "Receiving Message from Federate, type GetParameterHandle.");

        GPHr->setParameter(om->getParameterHandle(GPHq->getParameterName(), GPHq->getInteractionClass()));
        GPHr->setParameterName(GPHq->getParameterName());
        GPHr->setInteractionClass(GPHq->getInteractionClass());
    } break;

    case Message::GET_PARAMETER_NAME: {
        M_Get_Parameter_Name *GPNq, *GPNr;
        GPNr = static_cast<M_Get_Parameter_Name*>(answer);
        GPNq = static_cast<M_Get_Parameter_Name*>(request);
        D.Out(pdTrace, "Receiving Message from Federate, type GetParameterName.");

        GPNr->setParameterName(om->getParameterName(GPNq->getParameter(), GPNq->getInteractionClass()));
        GPNr->setParameter(GPNq->getParameter());
        GPNr->setInteractionClass(GPNq->getInteractionClass());
    } break;

    case Message::GET_SPACE_HANDLE: {
        M_Get_Space_Handle *GSHq, *GSHr;
        GSHr = static_cast<M_Get_Space_Handle*>(answer);
        GSHq = static_cast<M_Get_Space_Handle*>(request);
        D.Out(pdTrace, "Receiving Message from Federate, type GetSpaceHandle.");
        GSHr->setSpace(ddm->getRoutingSpaceHandle(GSHq->getSpaceName()));
    } break;

    case Message::GET_SPACE_NAME: {
        M_Get_Space_Name *GSNq, *GSNr;
        GSNr = static_cast<M_Get_Space_Name*>(answer);
        GSNq = static_cast<M_Get_Space_Name*>(request);
        D.Out(pdTrace, "Receiving Message from Federate, type GetSpaceName.");
        GSNr->setSpaceName(ddm->getRoutingSpaceName(GSNq->getSpace()));
    } break;

    case Message::GET_DIMENSION_HANDLE: {
        M_Get_Dimension_Handle *GDHq, *GDHr;
        GDHr = static_cast<M_Get_Dimension_Handle*>(answer);
        GDHq = static_cast<M_Get_Dimension_Handle*>(request);
        D.Out(pdTrace, "Receiving Message from Federate, type GetDimensionHandle");
        GDHr->setDimension(ddm->getDimensionHandle(string(GDHq->getDimensionName()), GDHq->getSpace()));
    } break;

    case Message::GET_DIMENSION_NAME: {
        M_Get_Dimension_Name *GDNq, *GDNr;
        GDNr = static_cast<M_Get_Dimension_Name*>(answer);
        GDNq = static_cast<M_Get_Dimension_Name*>(request);
        D.Out(pdTrace, "Receiving Message from Federate, type GetDimensionName");
        GDNr->setDimensionName(ddm->getDimensionName(GDNq->getDimension(), GDNq->getSpace()));
    } break;

    case Message::GET_ATTRIBUTE_SPACE_HANDLE: {
        M_Get_Attribute_Space_Handle *GASHq, *GASHr;
        GASHr = static_cast<M_Get_Attribute_Space_Handle*>(answer);
        GASHq = static_cast<M_Get_Attribute_Space_Handle*>(request);
        D.Out(pdTrace, "Receiving Message from Federate, type GetAttributeSpaceHandle");
        GASHr->setSpace(ddm->getAttributeSpace(GASHq->getAttribute(), GASHq->getObjectClass()));
    } break;

    case Message::GET_INTERACTION_SPACE_HANDLE: {
        M_Get_Interaction_Space_Handle *GISHq, *GISHr;
        GISHr = static_cast<M_Get_Interaction_Space_Handle*>(answer);
        GISHq = static_cast<M_Get_Interaction_Space_Handle*>(request);
        D.Out(pdTrace, "Receiving Message from Federate: GetInteractionSpaceHandle");
        GISHr->setSpace(ddm->getInteractionSpace(GISHq->getInteractionClass()));
    } break;

    case Message::GET_OBJECT_CLASS: {
        M_Get_Object_Class *GOCq, *GOCr;
        GOCq = static_cast<M_Get_Object_Class*>(request);
        GOCr = static_cast<M_Get_Object_Class*>(answer);
        Debug(D, pdTrace) << "Message from Federate: getObjectClass" << std::endl;
        GOCr->setObjectClass(om->getObjectClass(GOCq->getObject()));
    } break;

    case Message::GET_TRANSPORTATION_HANDLE: {
        M_Get_Transportation_Handle *GTHq, *GTHr;
        GTHr = static_cast<M_Get_Transportation_Handle*>(answer);
        GTHq = static_cast<M_Get_Transportation_Handle*>(request);
        Debug(D, pdTrace) << "Message from Federate: getTransportationHandle" << std::endl;
        GTHr->setTransportation(om->getTransportationHandle(GTHq->getTransportationName()));
    } break;

    case Message::GET_TRANSPORTATION_NAME: {
        M_Get_Transportation_Name *GTNq, *GTNr;
        GTNr = static_cast<M_Get_Transportation_Name*>(answer);
        GTNq = static_cast<M_Get_Transportation_Name*>(request);
        Debug(D, pdTrace) << "Message from Federate: getTransportationName" << std::endl;
        GTNr->setTransportationName(om->getTransportationName(GTNq->getTransportation()));
    } break;

    case Message::GET_ORDERING_HANDLE: {
        M_Get_Ordering_Handle *GOHq, *GOHr;
        GOHr = static_cast<M_Get_Ordering_Handle*>(answer);
        GOHq = static_cast<M_Get_Ordering_Handle*>(request);
        Debug(D, pdTrace) << "Message from Federate: getOrderingHandle" << std::endl;
        GOHr->setOrdering(om->getOrderingHandle(GOHq->getOrderingName()));
    } break;

    case Message::GET_ORDERING_NAME: {
        M_Get_Ordering_Name *GONq, *GONr;
        GONr = static_cast<M_Get_Ordering_Name*>(answer);
        GONq = static_cast<M_Get_Ordering_Name*>(request);
        Debug(D, pdTrace) << "Message from Federate: getOrderingName" << std::endl;
        GONr->setOrderingName(om->getOrderingName(GONq->getOrdering()));
    } break;

    case Message::DDM_CREATE_REGION: {
        M_Ddm_Create_Region *DDMCRq, *DDMCRr;
        DDMCRr = static_cast<M_Ddm_Create_Region*>(answer);
        DDMCRq = static_cast<M_Ddm_Create_Region*>(request);
        Debug(D, pdTrace) << "Receiving Message from Federate: CreateRegion" << std::endl;
        DDMCRr->setRegion(ddm->createRegion(DDMCRq->getSpace(), DDMCRq->getExtentSetSize(), e));
        DDMCRr->setExtentSetSize(rootObject->getRoutingSpace(DDMCRq->getSpace()).size());
    } break;

    case Message::DDM_MODIFY_REGION: {
        M_Ddm_Modify_Region* DDMMRq;
        DDMMRq = static_cast<M_Ddm_Modify_Region*>(request);
        Debug(D, pdTrace) << "Receiving Message from Federate: Modify Region" << std::endl;
        ddm->modifyRegion(DDMMRq->getRegion(), DDMMRq->getExtents(), e);
    } break;

    case Message::DDM_DELETE_REGION: {
        M_Ddm_Delete_Region* DDMDRq;
        DDMDRq = static_cast<M_Ddm_Delete_Region*>(request);
        Debug(D, pdTrace) << "Receiving Message from Federate: DeleteRegion" << std::endl;
        ddm->deleteRegion(DDMDRq->getRegion(), e);
    } break;

    case Message::DDM_ASSOCIATE_REGION: {
        M_Ddm_Associate_Region* DDMARq;
        DDMARq = static_cast<M_Ddm_Associate_Region*>(request);

        Debug(D, pdTrace) << "Receiving Message from Federate: Associate Region" << std::endl;
        ddm->associateRegion(
            DDMARq->getObject(), DDMARq->getRegion(), DDMARq->getAttributes(), DDMARq->getAttributesSize(), e);
    } break;

    case Message::DDM_REGISTER_OBJECT: {
        M_Ddm_Register_Object *DDMROq, *DDMROr;
        DDMROq = static_cast<M_Ddm_Register_Object*>(request);
        DDMROr = static_cast<M_Ddm_Register_Object*>(answer);
        Debug(D, pdTrace) << "Receiving Message from Federate: Register with Region" << std::endl;
        DDMROr->setObject(ddm->registerObject(DDMROq->getObjectClass(),
                                              DDMROq->getObjectInstanceName(),
                                              DDMROq->getAttributes(),
                                              DDMROq->getAttributesSize(),
                                              DDMROq->getRegions(),
                                              e));
    } break;

    case Message::DDM_UNASSOCIATE_REGION: {
        M_Ddm_Unassociate_Region* DDMURq;
        DDMURq = static_cast<M_Ddm_Unassociate_Region*>(request);

        Debug(D, pdTrace) << "Receiving Message from Federate: Unassociate Region" << std::endl;
        ddm->unassociateRegion(DDMURq->getObject(), DDMURq->getRegion(), e);
    } break;

    case Message::DDM_SUBSCRIBE_ATTRIBUTES: {
        M_Ddm_Subscribe_Attributes* DDMSAq;
        DDMSAq = static_cast<M_Ddm_Subscribe_Attributes*>(request);
        Debug(D, pdTrace) << "Receiving Message from Federate: Subscribe Attributes" << std::endl;
        ddm->subscribe(
            DDMSAq->getObjectClass(), DDMSAq->getRegion(), DDMSAq->getAttributes(), DDMSAq->getAttributesSize(), e);
    } break;

    case Message::DDM_UNSUBSCRIBE_ATTRIBUTES: {
        M_Ddm_Unsubscribe_Attributes* DDMUAq;
        DDMUAq = static_cast<M_Ddm_Unsubscribe_Attributes*>(request);

        Debug(D, pdTrace) << "Receiving Message from Federate: Unsubscribe class " << DDMUAq->getObjectClass()
                          << std::endl;
        ddm->unsubscribeAttributes(DDMUAq->getObjectClass(), DDMUAq->getRegion(), e);
    } break;

    case Message::DDM_SUBSCRIBE_INTERACTION: {
        M_Ddm_Subscribe_Interaction* DSIq;
        DSIq = static_cast<M_Ddm_Subscribe_Interaction*>(request);
        Debug(D, pdTrace) << "Receiving Message from Federate: Subscribe Interaction" << std::endl;
        ddm->subscribeInteraction(DSIq->getInteractionClass(), DSIq->getRegion(), e);
    } break;

    case Message::DDM_UNSUBSCRIBE_INTERACTION: {
        M_Ddm_Unsubscribe_Interaction* DUIq;
        DUIq = static_cast<M_Ddm_Unsubscribe_Interaction*>(request);
        Debug(D, pdTrace) << "Receiving Message from Federate: Unsubscribe Interaction" << std::endl;
        ddm->unsubscribeInteraction(DUIq->getInteractionClass(), DUIq->getRegion(), e);
    } break;

    case Message::ENABLE_TIME_REGULATION:
        tm->setTimeRegulating(
            true, request->getDate(), static_cast<M_Enable_Time_Regulation*>(request)->getLookahead(), e);
        break;
    case Message::DISABLE_TIME_REGULATION:
        tm->setTimeRegulating(
            false, request->getDate(), static_cast<M_Disable_Time_Regulation*>(request)->getLookahead(), e);
        break;

    case Message::ENABLE_TIME_CONSTRAINED:
        tm->setTimeConstrained(true, e);
        break;
    case Message::DISABLE_TIME_CONSTRAINED:
        tm->setTimeConstrained(false, e);
        break;
    case Message::TICK_REQUEST:
        M_Tick_Request* TRq;
        TRq = static_cast<M_Tick_Request*>(request);
        // called when tick() is invoked
        if (tm->_tick_state != TimeManagement::NO_TICK) {
            throw RTIinternalError(stringize() << "TICK_REQUEST cannot be called recursively (state="
                                               << static_cast<int>(tm->_tick_state)
                                               << ")");
        }

        tm->_tick_multiple = TRq->getMultiple();
        tm->_tick_result = false; // default return value

        if (TRq->getMinTickTime() >= 0.0) {
            tm->_tick_timeout = TRq->getMinTickTime();
            tm->_tick_max_tick = TRq->getMaxTickTime();
            tm->_tick_clock_start = clock->getCurrentTicksValue();
            tm->_tick_state = TimeManagement::TICK_BLOCKING;
        }
        else {
            tm->_tick_state = TimeManagement::TICK_CALLBACK;
        }
        processOngoingTick();
        break;

    case Message::TICK_REQUEST_NEXT:
        // called when tick() queries result, or next callback
        if (tm->_tick_state != TimeManagement::TICK_CALLBACK && tm->_tick_state != TimeManagement::TICK_RETURN)
            throw RTIinternalError("unexpected TICK_REQUEST_NEXT");

        processOngoingTick();
        break;

    case Message::TICK_REQUEST_STOP:
        // called to terminate tick() when error occured
        // do not invoke any callbacks, reset _tick_state and return
        tm->_tick_state = TimeManagement::TICK_RETURN;

        processOngoingTick();
        break;

    case Message::ENABLE_CLASS_RELEVANCE_ADVISORY_SWITCH:
        D.Out(pdTrace, "Receiving Message from Federate, type EnableClassRelevanceAdvisorySwitch.");
        dm->setClassRelevanceAdvisorySwitch(true, e);
        break;

    case Message::DISABLE_CLASS_RELEVANCE_ADVISORY_SWITCH:
        D.Out(pdTrace, "Receiving Message from Federate, type DisableClassRelevanceAdvisorySwitch.");
        dm->setClassRelevanceAdvisorySwitch(false, e);
        break;

    case Message::ENABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH:
        D.Out(pdTrace, "Receiving Message from Federate, type SetInteractionRelevanceAdvisorySwitch.");
        dm->setInteractionRelevanceAdvisorySwitch(true, e);
        break;

    case Message::DISABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH:
        D.Out(pdTrace, "Receiving Message from Federate, type SetInteractionRelevanceAdvisorySwitch.");
        dm->setInteractionRelevanceAdvisorySwitch(false, e);
        break;

    case Message::ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH:
        D.Out(pdTrace, "Receiving Message from Federate, type SetAttributeRelevanceAdvisorySwitch.");
        om->setAttributeRelevanceAdvisorySwitch(true, e);
        break;

    case Message::DISABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH:
        D.Out(pdTrace, "Receiving Message from Federate, type SetAttributeRelevanceAdvisorySwitch.");
        om->setAttributeRelevanceAdvisorySwitch(false, e);
        break;

    case Message::ENABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH:
        D.Out(pdTrace, "Receiving Message from Federate, type SetAttributeScopeAdvisorySwitch.");
        om->setAttributeScopeAdvisorySwitch(true, e);
        break;

    case Message::DISABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH:
        D.Out(pdTrace, "Receiving Message from Federate, type SetAttributeScopeAdvisorySwitch.");
        om->setAttributeScopeAdvisorySwitch(false, e);
        break;

    default:
        D.Out(pdExcept,
              "Receiving Message from Federate, Unknown Type %s/%d.",
              request->getMessageName(),
              request->getMessageType());
        throw RTIinternalError("");
    }
    stat.federateService(request->getMessageType());
    G.Out(pdGendoc, "exit  chooseFederateProcessing");
} /* end of RTIA::chooseFederateProcessing */

void RTIA::processOngoingTick()
{
    Exception::Type exc = Exception::Type::NO_EXCEPTION;

    while (1) {
        switch (tm->_tick_state) {
        case TimeManagement::TICK_BLOCKING:
            /*
			 * blocking tick() waits for an event to come:
			 * try to evoke a single callback
			 * the tm->tick(exc) will update _tick_state
			 */
            tm->_tick_result = tm->tick(exc);
            // if a callback has not been evoked
            if (tm->_tick_state != TimeManagement::TICK_NEXT)
                return; // keep waiting
        // else goto TICK_NEXT
        // NO break;

        case TimeManagement::TICK_NEXT:
            /* a callback was evoked decide how to continue */
            if (tm->_tick_result && tm->_tick_multiple
                && 1e-9 * clock->getDeltaNanoSecond(tm->_tick_clock_start) < tm->_tick_max_tick) {
                tm->_tick_state = TimeManagement::TICK_CALLBACK;
            }
            else {
                tm->_tick_state = TimeManagement::TICK_RETURN;
            }
            /* Unconditional return */
            return;

        case TimeManagement::TICK_CALLBACK:
            /* tick() waits until a federate callback finishes
			 *   try to evoke a single callback
			 */
            tm->_tick_result = tm->tick(exc);
            // if a callback has been evoked
            if (tm->_tick_state == TimeManagement::TICK_NEXT)
                break; // goto TICK_NEXT
        // else goto TICK_RETURN
        // NO break;

        case TimeManagement::TICK_RETURN: {
            M_Tick_Request msg_ack;
            /* send TICK_REQUEST response
			 */
            if (exc != Exception::Type::RTIinternalError)
                msg_ack.setException(exc);
            // terminate __tick() call in the federate
            msg_ack.setMultiple(tm->_tick_result);
            msg_ack.setMinTickTime(0); // unused
            msg_ack.setMaxTickTime(0); // unused
            comm->requestFederateService(&msg_ack);

            tm->_tick_state = TimeManagement::NO_TICK;
        }
            return;

        default:
            std::cerr << "Unknown state: " << tm->_tick_state << std::endl;
            assert(false);
            break;
        }
    }
} /* RTIA::processOngoingTick() */

void RTIA::initFederateProcessing(Message* request, Message* answer)
{
    if (request->getMessageType() == Message::OPEN_CONNEXION) {
        M_Open_Connexion *OCq, *OCr;
        OCq = static_cast<M_Open_Connexion*>(request);
        OCr = static_cast<M_Open_Connexion*>(answer);

        if (OCq->getVersionMajor() == CERTI_Message::versionMajor) {
            uint32_t minorEffective = OCq->getVersionMinor() <= CERTI_Message::versionMinor
                ? OCq->getVersionMinor()
                : CERTI_Message::versionMinor;
            OCr->setVersionMajor(CERTI_Message::versionMajor);
            OCr->setVersionMinor(minorEffective);

            fm->_connection_state = FederationManagement::CONNECTION_READY;
        }
        else {
            answer->setException(Exception::Type::RTIinternalError,
                                 stringize() << "RTIA protocol version mismatch"
                                             << "; federate "
                                             << OCq->getVersionMajor()
                                             << "."
                                             << OCq->getVersionMinor()
                                             << ", RTIA "
                                             << CERTI_Message::versionMajor
                                             << "."
                                             << CERTI_Message::versionMinor);
        }
    }
    else {
        answer->setException(Exception::Type::RTIinternalError,
                             "RTIA protocol version mismatch; expecting OPEN_CONNECTION first.");
    }
    stat.federateService(request->getMessageType());
}

void RTIA::processFederateRequest(Message* request)
{
    /* use virtual constructor in order to build  *
	 * appropriate answer message.                */
    std::unique_ptr<Message> rep(M_Factory::create(request->getMessageType()));

    G.Out(pdGendoc, "enter RTIA::processFederateRequest");

    try {
        switch (fm->_connection_state) {
        case FederationManagement::CONNECTION_PRELUDE:
            initFederateProcessing(request, rep.get());
            break;

        case FederationManagement::CONNECTION_READY: {
            Exception::Type exc;
            chooseFederateProcessing(request, rep.get(), exc);
            if (exc != Exception::Type::RTIinternalError && exc != Exception::Type::NO_EXCEPTION) {
                rep->setException(exc);
            }
            break;
        }

        case FederationManagement::CONNECTION_FIN:
        default:
            rep->setException(Exception::Type::RTIinternalError, "RTIA connection already closed.");
            break;
        }
    }
    // FIXME should
    // catch (Exception &e) {
    //  rep->setException(static_cast<Exception::Type>(e.getMessageType()),e.reason());
    catch (ArrayIndexOutOfBounds& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::ArrayIndexOutOfBounds);
        rep->setException(static_cast<Exception::Type>(e.type()), e.reason());
    }
    catch (AttributeAlreadyOwned& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::AttributeAlreadyOwned);
    }
    catch (AttributeAlreadyBeingDivested& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::AttributeAlreadyBeingDivested);
    }
    catch (AttributeAlreadyBeingAcquired& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::AttributeAlreadyBeingAcquired);
    }
    catch (AttributeAcquisitionWasNotRequested& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::AttributeAcquisitionWasNotRequested);
    }
    catch (AttributeDivestitureWasNotRequested& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::AttributeDivestitureWasNotRequested);
    }
    catch (AttributeNotDefined& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::AttributeNotDefined);
    }
    catch (AttributeNotKnown& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::AttributeNotKnown);
    }
    catch (AttributeNotOwned& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::AttributeNotOwned);
    }
    catch (AttributeNotPublished& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::AttributeNotPublished);
    }
    catch (AttributeNotSubscribed& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::AttributeNotSubscribed);
    }
    catch (ConcurrentAccessAttempted& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::ConcurrentAccessAttempted);
    }
    catch (CouldNotDiscover& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::CouldNotDiscover);
    }
    catch (CouldNotOpenRID& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::CouldNotOpenRID);
    }
    catch (CouldNotOpenFED& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::CouldNotOpenFED, e.reason());
    }
    catch (CouldNotRestore& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::CouldNotRestore);
    }
    catch (DeletePrivilegeNotHeld& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::DeletePrivilegeNotHeld);
    }
    catch (ErrorReadingRID& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::ErrorReadingRID, e.reason());
    }
    catch (ErrorReadingFED& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::ErrorReadingFED, e.reason());
    }
    catch (EventNotKnown& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::EventNotKnown);
    }
    catch (FederateAlreadyPaused& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::FederateAlreadyPaused);
    }
    catch (FederateAlreadyExecutionMember& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::FederateAlreadyExecutionMember, e.reason());
    }
    catch (FederateDoesNotExist& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::FederateDoesNotExist);
    }
    catch (FederateInternalError& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::FederateInternalError);
    }
    catch (FederateNameAlreadyInUse& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::FederateNameAlreadyInUse);
    }
    catch (FederateNotExecutionMember& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::FederateNotExecutionMember);
    }
    catch (FederateNotPaused& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::FederateNotPaused);
    }
    catch (FederateNotPublishing& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::FederateNotPublishing);
    }
    catch (FederateNotSubscribing& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::FederateNotSubscribing);
    }
    catch (FederateOwnsAttributes& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::FederateOwnsAttributes);
    }
    catch (FederatesCurrentlyJoined& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::FederatesCurrentlyJoined);
    }
    catch (FederateWasNotAskedToReleaseAttribute& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::FederateWasNotAskedToReleaseAttribute);
    }
    catch (FederationAlreadyPaused& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::FederationAlreadyPaused);
    }
    catch (FederationExecutionAlreadyExists& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::FederationExecutionAlreadyExists, e.reason());
    }
    catch (FederationExecutionDoesNotExist& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::FederationExecutionDoesNotExist);
    }
    catch (FederationNotPaused& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::FederationNotPaused);
    }
    catch (FederationTimeAlreadyPassed& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::FederationTimeAlreadyPassed);
    }
    catch (IDsupplyExhausted& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::IDsupplyExhausted);
    }
    catch (InteractionClassNotDefined& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::InteractionClassNotDefined);
    }
    catch (InteractionClassNotKnown& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::InteractionClassNotKnown);
    }
    catch (InteractionClassNotPublished& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::InteractionClassNotPublished);
    }
    catch (InteractionParameterNotDefined& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::InteractionParameterNotDefined);
    }
    catch (InteractionParameterNotKnown& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::InteractionParameterNotKnown);
    }
    catch (InvalidDivestitureCondition& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::InvalidDivestitureCondition);
    }
    catch (InvalidExtents& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::InvalidExtents);
    }
    catch (InvalidFederationTime& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::InvalidFederationTime);
    }
    catch (InvalidFederationTimeDelta& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::InvalidFederationTimeDelta);
    }
    catch (InvalidObjectHandle& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::InvalidObjectHandle);
    }
    catch (InvalidOrderingHandle& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::InvalidOrderingHandle);
    }
    catch (InvalidResignAction& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::InvalidResignAction);
    }
    catch (InvalidRetractionHandle& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::InvalidRetractionHandle);
    }
    catch (InvalidRoutingSpace& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::InvalidRoutingSpace);
    }
    catch (InvalidTransportationHandle& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::InvalidTransportationHandle);
    }
    catch (MemoryExhausted& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::MemoryExhausted);
    }
    catch (NameNotFound& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::NameNotFound);
    }
    catch (NoPauseRequested& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::NoPauseRequested);
    }
    catch (NoResumeRequested& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::NoResumeRequested);
    }
    catch (ObjectClassNotDefined& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::ObjectClassNotDefined);
    }
    catch (ObjectClassNotKnown& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::ObjectClassNotKnown);
    }
    catch (ObjectClassNotPublished& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::ObjectClassNotPublished);
    }
    catch (ObjectClassNotSubscribed& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::ObjectClassNotSubscribed);
    }
    catch (ObjectNotKnown& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::ObjectNotKnown);
    }
    catch (ObjectAlreadyRegistered& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::ObjectAlreadyRegistered);
    }
    catch (RegionNotKnown& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::RegionNotKnown);
    }
    catch (RestoreInProgress& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::RestoreInProgress);
    }
    catch (RestoreNotRequested& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::RestoreNotRequested);
    }
    catch (RTIinternalError& e) {
        Debug(D, pdError) << "RTIA sends InternalError to Fed., ";
        if (!e.reason().empty()) {
            Debug(D, pdError) << "reason : " << e.reason() << std::endl;
        }
        else {
            Debug(D, pdError) << "no reason given." << std::endl;
        }
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::RTIinternalError);
    }
    catch (SpaceNotDefined& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::SpaceNotDefined);
    }
    catch (SaveInProgress& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::SaveInProgress);
    }
    catch (SaveNotInitiated& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::SaveNotInitiated);
    }
    catch (SpecifiedSaveLabelDoesNotExist& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::SpecifiedSaveLabelDoesNotExist);
    }
    catch (TimeAdvanceAlreadyInProgress& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::TimeAdvanceAlreadyInProgress);
    }
    catch (TimeAdvanceWasNotInProgress& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::TimeAdvanceWasNotInProgress);
    }
    catch (TooManyIDsRequested& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::TooManyIDsRequested);
    }
    catch (UnableToPerformSave& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::UnableToPerformSave);
    }
    catch (UnimplementedService& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::UnimplementedService);
    }
    catch (UnknownLabel& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::UnknownLabel);
    }
    catch (ValueCountExceeded& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::ValueCountExceeded);
    }
    catch (ValueLengthExceeded& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << std::endl;
        rep->setException(Exception::Type::ValueLengthExceeded);
    }

    // Default Handler
    catch (Exception& e) {
        Debug(D, pdExcept) << "Unknown Exception :" << e.name() << std::endl;
        rep->setException(Exception::Type::RTIinternalError);
    }

    delete request;

    if (rep->getMessageType() != Message::TICK_REQUEST && rep->getMessageType() != Message::TICK_REQUEST_NEXT
        && rep->getMessageType() != Message::TICK_REQUEST_STOP) {
        // generic federate service acknowledgment
        // the TICK_REQUEST confirmation is generated in processOngoingTick()
        comm->sendUN(rep.get());
        D.Out(pdDebug, "Reply send to Unix socket.");
    }
    G.Out(pdGendoc, "exit  RTIA::processFederateRequest");
} /* end of RTIA::processFederateRequest(Message *req) */

} // namespace rtia
} // namespace certi
