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

#include <config.h>
#include "RTIA.hh"

#include "Clock.hh"
#include "fed.hh"
#include "RoutingSpace.hh"
#include "XmlParser.hh"
#include "M_Classes.hh"

#include <assert.h>
#include <memory>

using std::string ;
using std::ifstream ;

namespace certi {
namespace rtia {

static PrettyDebug D("RTIA", __FILE__);
static PrettyDebug G("GENDOC",__FILE__) ;

// ----------------------------------------------------------------------------
//! Verify that federate is not in saving or restoring state.
void
RTIA::saveAndRestoreStatus(Message::Type type)
throw (SaveInProgress, RestoreInProgress)
{
	G.Out(pdGendoc,"enter RTIA::saveAndRestoreStatus");
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
		break ;
	case Message::FEDERATE_SAVE_BEGUN:
	case Message::FEDERATE_SAVE_COMPLETE:
	case Message::FEDERATE_SAVE_NOT_COMPLETE:
		fm->checkFederationRestoring();
		break ;
	case Message::FEDERATE_RESTORE_COMPLETE:
	case Message::FEDERATE_RESTORE_NOT_COMPLETE:
		fm->checkFederationSaving();
		break ;
	default:
		fm->checkFederationSaving();
		fm->checkFederationRestoring();
		break;
	}
	G.Out(pdGendoc,"exit  RTIA::saveAndRestoreStatus");
}

// ----------------------------------------------------------------------------
//! Choose federate processing.
void
RTIA::chooseFederateProcessing(Message *req, Message* rep, TypeException &e)
{
	G.Out(pdGendoc,"enter RTIA::chooseFederateProcessing for msg <%s> (type=%d)",req->getMessageName(), req->getMessageType());

	// Verify not in saving or restoring state.
	// May throw SaveInProgress or RestoreInProgress
	saveAndRestoreStatus(req->getMessageType());

	e = e_NO_EXCEPTION ;

	switch(req->getMessageType()) {

	case Message::CLOSE_CONNEXION:
		D.Out(pdTrace,
				"Receiving Message from Federate, type CloseConnexion.");
		fm->_connection_state = FederationManagement::CONNECTION_FIN;
		// the this->comm can now be used only to sent the CLOSE_CONNEXION response
		break ;

	case Message::CREATE_FEDERATION_EXECUTION: {
		M_Create_Federation_Execution *CFEq, *CFEr;
		CFEr = static_cast<M_Create_Federation_Execution *>(rep);
		CFEq = static_cast<M_Create_Federation_Execution *>(req);
		D.Out(pdTrace,
				"Receiving Message from Federate, type CreateFederation.");
		// Store FEDid for future usage (JOIN_FEDERATION_EXECUTION) into fm
		fm->createFederationExecution(CFEq->getFederationName(),
				CFEq->getFEDid(), e);
		if ( e == e_RTIinternalError )
		{
			rep->setException(e,"Federate is yet a creator or a member !");
		}
		D.Out(pdTrace, "Receiving Message from Federate, "
				"type CreateFederation done.");
		// RTIA needs FEDid into the answer (rep Message) to federate
		CFEr->setFEDid(CFEq->getFEDid()) ;
		// RTIA needs federation name into the answer (rep Message) to federate
		CFEr->setFederationName(CFEq->getFederationName());
	}
	break ;

	case Message::DESTROY_FEDERATION_EXECUTION: {
		M_Destroy_Federation_Execution *DFEq, *DFEr;
		DFEr = static_cast<M_Destroy_Federation_Execution *>(rep);
		DFEq = static_cast<M_Destroy_Federation_Execution *>(req);
		D.Out(pdTrace,
				"Receiving Message from Federate, type DestroyFederation.");

		fm->destroyFederationExecution(DFEq->getFederationName(), e);
		if ( e == e_RTIinternalError )
		{
			rep->setException(e,"Illegal federation handle");
		}
		// RTIA needs federation name into the answer (rep Message) to federate
		DFEr->setFederationName(DFEq->getFederationName());
	}
	break ;

	case Message::JOIN_FEDERATION_EXECUTION: {
		M_Join_Federation_Execution *JFEq, *JFEr;
		JFEr = static_cast<M_Join_Federation_Execution *>(rep);
		JFEq = static_cast<M_Join_Federation_Execution *>(req);
		D.Out(pdTrace,
				"Receiving Message from Federate, type JoinFederation.");
		JFEr->setFederate(fm->joinFederationExecution(JFEq->getFederateName(),
				JFEq->getFederationName(),
				rootObject, e));
		if ( e == e_NO_EXCEPTION ) {
			/// Set RTIA PrettyDebug federate name
			PrettyDebug::setFederateName("RTIA::"+JFEq->getFederateName());
			// Set federation name for the answer message (rep)
			JFEr->setFederationName(JFEq->getFederationName());
			JFEr->setFederateName(JFEq->getFederateName());
		}
		else {
			// JOIN FAILED
			switch (e)
			{
			case e_FederateAlreadyExecutionMember: 
				throw FederateAlreadyExecutionMember("Federate yet joined or same name");
				break;
			case e_FederationExecutionDoesNotExist:
				throw FederationExecutionDoesNotExist("Federation does not exist [yet]");
				break;
			case e_SaveInProgress:
				throw SaveInProgress("Save in progress");
				break;
			case e_RestoreInProgress:
				throw RestoreInProgress("Restore in progress");
				break;
			case e_RTIinternalError:
			default:
				throw RTIinternalError("Internal error");
				break;
			}
		}
	}
	break;
	case Message::RESIGN_FEDERATION_EXECUTION: {
		M_Resign_Federation_Execution *RFEq;
		RFEq = static_cast<M_Resign_Federation_Execution *>(req);
		D.Out(pdTrace,
				"Receiving Message from Federate, type ResignFederation.");
		
		switch( RFEq->getResignAction() )
		{
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
	}
	break ;

	case Message::REGISTER_FEDERATION_SYNCHRONIZATION_POINT: {
		M_Register_Federation_Synchronization_Point* RFSPq;
		RFSPq = static_cast<M_Register_Federation_Synchronization_Point *>(req);
		D.Out(pdTrace, "Receiving Message from Federate, type RegisterFederationSynchronizationPoint.");

		// Check whether if the with federates set size is strictly positive,
		// which means with federate set.
		if ( RFSPq->getFederateSetSize()>0 ) {
			fm->registerSynchronization(req->getLabel(),
										req->getTag(),
										static_cast<unsigned short>(RFSPq->getFederateSetSize()),
										RFSPq->getFederateSet(), e);
		}
		else {
			fm->registerSynchronization(req->getLabel(), req->getTag(), e);
		}
	}
	break ;

	case Message::SYNCHRONIZATION_POINT_ACHIEVED:
		D.Out(pdTrace, "Receiving Message from Federate, type SynchronizationPointAchieved.");
		fm->unregisterSynchronization(req->getLabel(), e);
		break ;

	case Message::REQUEST_FEDERATION_SAVE:
		if ( req->isDated() ) {
			D.Out(pdTrace, "Receiving Message from Federate, type"
					" RequestFederationSave with time.");
			fm->requestFederationSave(req->getLabel(),req->getDate(),
					e);
			rep->setDate(req->getDate());
		}
		else {
			D.Out(pdTrace, "Receiving Message from Federate, type"
					" RequestFederationSave without time.");
			fm->requestFederationSave(req->getLabel(),e);
		}
		break ;

	case Message::FEDERATE_SAVE_BEGUN:
		D.Out(pdTrace, "Receiving Message from Federate, type"
				" FederateSaveBegun.");
		G.Out(pdGendoc,"chooseFederateProcessing FEDERATE_SAVE_BEGUN");
		fm->federateSaveBegun(e);
		break ;

	case Message::FEDERATE_SAVE_COMPLETE:
	case Message::FEDERATE_SAVE_NOT_COMPLETE: {
		D.Out(pdTrace, "Receiving Message from Federate, type"
				" FederateSave(Not)Complete.");
		bool result = req->getMessageType() == Message::FEDERATE_SAVE_COMPLETE ;
		if (result) {
			G.Out(pdGendoc,"chooseFederateProcessing FEDERATE_SAVE_COMPLETE");
		}
		else {
			G.Out(pdGendoc,"chooseFederateProcessing FEDERATE_SAVE_NOT_COMPLETE");
		}
		fm->federateSaveStatus(result, e);
	}
	break ;

	case Message::REQUEST_FEDERATION_RESTORE: {
		D.Out(pdTrace, "Receiving Message from Federate, type"
				" RequestFederationRestore.");
		G.Out(pdGendoc,"chooseFederateProcessing REQUEST_FEDERATION_RESTORE");

		fm->requestFederationRestore(req->getLabel(), e);
	}
	break ;

	case Message::FEDERATE_RESTORE_COMPLETE:
		fm->federateRestoreStatus(true, e);
		break;
	case Message::FEDERATE_RESTORE_NOT_COMPLETE:
		fm->federateRestoreStatus(false, e);
		break ;

	case Message::PUBLISH_OBJECT_CLASS: {
		M_Publish_Object_Class* POCq;
		POCq = static_cast<M_Publish_Object_Class *>(req);
		D.Out(pdTrace,
				"Receiving Message from Federate, type PublishObjectClass.");

		dm->publishObjectClass(POCq->getObjectClass(),POCq->getAttributes(),e);
	}
	break ;

	case Message::UNPUBLISH_OBJECT_CLASS: {
		M_Unpublish_Object_Class* UOCq;
		UOCq = static_cast<M_Unpublish_Object_Class *>(req);
		D.Out(pdTrace,
				"Receiving Message from Federate, type UnpublishObjectClass.");
		dm->unpublishObjectClass(UOCq->getObjectClass(), e);
	}
	break ;

	case Message::PUBLISH_INTERACTION_CLASS: {
		M_Publish_Interaction_Class* PICq;
		PICq = static_cast<M_Publish_Interaction_Class *>(req);
		D.Out(pdTrace,
				"Receiving Message from Federate, type PublishInteractionClass.");

		dm->publishInteractionClass(PICq->getInteractionClass(), e);
	}
	break ;

	case Message::UNPUBLISH_INTERACTION_CLASS: {
		M_Unpublish_Interaction_Class* UICq;
		UICq = static_cast<M_Unpublish_Interaction_Class *>(req);
		D.Out(pdTrace,
				"Receiving Message from Federate, "
				"type UnpublishInteractionClass");

		dm->unpublishInteractionClass(UICq->getInteractionClass(), e);
	}
	break ;

	case Message::SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES: {
		M_Subscribe_Object_Class_Attributes* SOCAq;
		SOCAq = static_cast<M_Subscribe_Object_Class_Attributes *>(req);
		D.Out(pdTrace,
				"Receiving Message from Federate, type SubscribeObjectClass.");
		dm->subscribeObjectClassAttribute(SOCAq->getObjectClass(),
				SOCAq->getAttributes(),
				SOCAq->getAttributesSize(),
				e);
	}
	break ;

	case Message::UNSUBSCRIBE_OBJECT_CLASS: {
		M_Unsubscribe_Object_Class* UsOCq;
		UsOCq = static_cast<M_Unsubscribe_Object_Class *>(req);
		D.Out(pdTrace,
				"Receiving Message from Federate, type UnsubscribeObjectClass.");

		// FIXME BUG: Why attributes are not transmitted ?
		dm->unsubscribeObjectClassAttribute(UsOCq->getObjectClass(), e);
	}
	break ;

	case Message::SUBSCRIBE_INTERACTION_CLASS: {
		M_Subscribe_Interaction_Class* SICq;
		SICq = static_cast<M_Subscribe_Interaction_Class *>(req);
		D.Out(pdTrace,
				"Receiving Message from Federate, type SubscribeInteraction.");

		dm->subscribeInteractionClass(SICq->getInteractionClass(), e);
	}
	break ;

	case Message::UNSUBSCRIBE_INTERACTION_CLASS:  {
		M_Unsubscribe_Interaction_Class* UsICq;
		UsICq = static_cast<M_Unsubscribe_Interaction_Class *>(req);
		D.Out(pdTrace,
				"Receiving Message from Federate, type UnsubscribeInteraction.");

		dm->unsubscribeInteractionClass(UsICq->getInteractionClass(), e);
	}
	break ;

	case Message::RESERVE_OBJECT_INSTANCE_NAME: {
		M_Reserve_Object_Instance_Name *ROINq, *ROINr;
		ROINq = static_cast<M_Reserve_Object_Instance_Name *>(req);
		ROINr = static_cast<M_Reserve_Object_Instance_Name *>(rep);
		
		ROINr->setObjectName(ROINq->getObjectName());
		om->reserveObjectName(ROINq->getObjectName(), e);
		break;
	}

	case Message::REGISTER_OBJECT_INSTANCE: {
		M_Register_Object_Instance *ROIq, *ROIr;
		ROIq = static_cast<M_Register_Object_Instance *>(req);
		ROIr = static_cast<M_Register_Object_Instance *>(rep);
		FederationTime date = tm->requestFederateTime();
		FederationTime heure = date + tm->requestLookahead();

		D.Out(pdTrace,
				"Receiving Message from Federate, type RegisterObject.");

		ROIr->setObject(om->registerObject(ROIq->getObjectClass(),
										   ROIq->getObjectName(),
										   date, 
										   heure, 
										   e)
						);
		break ;
	}

	case Message::UPDATE_ATTRIBUTE_VALUES: {
		M_Update_Attribute_Values *UAVq,*UAVr;
		EventRetraction   event;
		UAVq = static_cast<M_Update_Attribute_Values *>(req);
		UAVr = static_cast<M_Update_Attribute_Values *>(rep);
		try {
			if (req->isDated() )
			{
				D.Out(pdTrace,"Receiving Message from Federate, type "
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
			else
			{
				D.Out(pdTrace,"Receiving Message from Federate, type "
						"UpdateAttribValues without TIMESTAMP.");
				om->updateAttributeValues(UAVq->getObject(),
						UAVq->getAttributes(),
						UAVq->getValues(),
						UAVq->getAttributesSize(),
						UAVq->getTag(),
						e);
			}
		} catch (Exception *e) {
			throw e ;
		}
	}
	break ;

	case Message::SEND_INTERACTION: {
		M_Send_Interaction *SIr, *SIq;
		EventRetraction    event;
		SIq = static_cast<M_Send_Interaction *>(req);
		SIr = static_cast<M_Send_Interaction *>(rep);
		G.Out(pdGendoc,"S_I into RTIA::chooseFederateProcessing") ;

		if (SIq->isDated() ) {
			D.Out(pdTrace,
					"Receiving Message from Federate, type SendInteraction with TIMESTAMP.");
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
			D.Out(pdTrace,
					"Receiving Message from Federate, type SendInteraction without TIMESTAMP.");
			om->sendInteraction(SIq->getInteractionClass(),
					SIq->getParameters(),
					SIq->getValues(),
					SIq->getParametersSize(),
					SIq->getTag(),
					SIq->getRegion(),
					e);
		}
	}
	break ;

	case Message::DELETE_OBJECT_INSTANCE: {
		M_Delete_Object_Instance *DOIr, *DOIq;
		EventRetraction   event;
		DOIq = static_cast<M_Delete_Object_Instance *>(req);
		DOIr = static_cast<M_Delete_Object_Instance *>(rep);
		G.Out(pdGendoc,"D_O_I into RTIA::chooseFederateProcessing") ;

		try {
			if ( DOIq->isDated() ) {
				D.Out(pdTrace,
						"Receiving Message from Federate, type DeleteObjectInstance with \
				TIMESTAMP.");
				event.setSN(om->deleteObject(
						DOIq->getObject(),
						DOIq->getDate(),
						DOIq->getTag(),
						e));
				// FIXME event.setSendingFederate();
				DOIr->setEventRetraction(event);
			}
			else {
				D.Out(pdTrace,
						"Receiving Message from Federate, type DeleteObjectInstance without \
				TIMESTAMP.");
				om->deleteObject(
						DOIq->getObject(),
						DOIq->getTag(),
						e);
			}
		} catch (Exception *e) {
			// FIXME what's the purpose of this catch?
			throw e;
		}
	}
	break ;

	case Message::CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE: {
		M_Change_Attribute_Transportation_Type* CATTq;
		CATTq = static_cast<M_Change_Attribute_Transportation_Type *>(req);
		D.Out(pdTrace,
				"Receiving Message from Federate, type ChangeAttribTransport.");

		om->changeAttributeTransportType(CATTq->getObject(),
				CATTq->getAttributes(),
				CATTq->getAttributesSize(),
				CATTq->getTransportationType(),
				e);
	}
	break ;

	case Message::CHANGE_ATTRIBUTE_ORDER_TYPE: {
		M_Change_Attribute_Order_Type* CAOTq;
		CAOTq = static_cast<M_Change_Attribute_Order_Type *>(req);

		D.Out(pdTrace,
				"Receiving Message from Federate, type ChangeAttribOrder.");

		om->changeAttributeOrderType(CAOTq->getObject(),
				CAOTq->getAttributes(),
				CAOTq->getAttributesSize(),
				CAOTq->getOrder(),
				e);
	}
	break ;

	case Message::CHANGE_INTERACTION_TRANSPORTATION_TYPE: {
		M_Change_Interaction_Transportation_Type* CITTq;
		CITTq = static_cast<M_Change_Interaction_Transportation_Type *>(req);

		D.Out(pdTrace,
				"Receiving Message from Federate, type ChangeInterTransport.");

		om->changeInteractionTransportType(CITTq->getInteractionClass(),
				CITTq->getTransportationType(),
				e);
	}
	break ;

	case Message::CHANGE_INTERACTION_ORDER_TYPE: {
		M_Change_Interaction_Order_Type* CIOTq;
		CIOTq = static_cast<M_Change_Interaction_Order_Type *>(req);

		D.Out(pdTrace,
				"Receiving Message from Federate, type ChangeInterOrder.");

		om->changeInteractionOrderType(CIOTq->getInteractionClass(),
				CIOTq->getOrder(), e);
	}
	break ;

	case Message::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE: {
		M_Request_Object_Attribute_Value_Update* ROAVUq;
		ROAVUq = static_cast<M_Request_Object_Attribute_Value_Update *>(req);

		D.Out(pdTrace,"Receiving Message from Federate, type "
				"RequestAttributeValueUpadate.");
		om->requestObjectAttributeValueUpdate(ROAVUq->getObject(),
				ROAVUq->getAttributes(),
				ROAVUq->getAttributesSize(),
				e);
	}
	break ;

	case Message::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE: {
		M_Request_Class_Attribute_Value_Update* RCAVUq;
		RCAVUq = static_cast<M_Request_Class_Attribute_Value_Update *>(req);

		D.Out(pdTrace,"Receiving Message from Federate, type "
				"RequestClassAttributeValueUpadate.");
		om->requestClassAttributeValueUpdate(RCAVUq->getObjectClass(),
				RCAVUq->getAttributes(),
				RCAVUq->getAttributesSize(),
				e);
	}
	break ;

	case Message::RETRACT:
		std::cout<<"retract not yet implemented in chooseFederateProcessing"<<std::endl;
		e = e_UnimplementedService ;
		break ;

	case Message::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE: {
		M_Unconditional_Attribute_Ownership_Divestiture *UAODq;
		UAODq =  static_cast<M_Unconditional_Attribute_Ownership_Divestiture*>(req);

		D.Out(pdTrace, "Receiving Message from Federate, "
				"type unconditionalAttributeOwnershipDivestiture.");
		D.Out(pdTrace, "Object %u number of attributes %u ",
				UAODq->getObject(), UAODq->getAttributesSize());
		owm->unconditionalAttributeOwnershipDivestiture(UAODq->getObject(),
				UAODq->getAttributes(),
				UAODq->getAttributesSize(),
				e);
	}
	break ;

	case Message::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE: {
		M_Negotiated_Attribute_Ownership_Divestiture *NAODq;
		NAODq =  static_cast<M_Negotiated_Attribute_Ownership_Divestiture*>(req);
		D.Out(pdTrace, "Receiving Message from Federate, "
				"type negotiatedAttributeOwnershipDivestiture.");
		D.Out(pdTrace, "Object %u ; %u nb Attribute ", NAODq->getObject(),
				NAODq->getAttributesSize());
		owm->negotiatedAttributeOwnershipDivestiture(NAODq->getObject(),
				NAODq->getAttributes(),
				NAODq->getAttributesSize(),
				req->getTag(),
				e);
	}
	break ;

	case Message::ATTRIBUTE_OWNERSHIP_ACQUISITION: {
		M_Attribute_Ownership_Acquisition *AOAq;
		AOAq =  static_cast<M_Attribute_Ownership_Acquisition*>(req);

		D.Out(pdTrace, "Receiving Message from Federate, "
				"type attributeOwnershipAcquisition.");
		D.Out(pdTrace, "Object %u ; Nb d'Attributs ", AOAq->getObject(),
				AOAq->getAttributesSize());
		owm->attributeOwnershipAcquisition(AOAq->getObject(),
				AOAq->getAttributes(),
				AOAq->getAttributesSize(),
				req->getTag(), e);
	}
	break ;

	case Message::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE: {
		M_Attribute_Ownership_Release_Response *AORRq, *AORRr;
		AORRq =  static_cast<M_Attribute_Ownership_Release_Response*>(req);
		AORRr =  static_cast<M_Attribute_Ownership_Release_Response*>(rep);

		D.Out(pdTrace, "Receiving Message from Federate, "
				"type attributeOwnershipRealeaseResponse.");
		D.Out(pdTrace, "Object %u nb Attribute %u ",
				AORRq->getObject(), AORRq->getAttributesSize());

		AttributeHandleSet* theAttributes =
				owm->attributeOwnershipRealeaseResponse(AORRq->getObject(),
						AORRq->getAttributes(),
						AORRq->getAttributesSize(),
						e);
		if (e == e_NO_EXCEPTION) {
			AORRr->setAttributesSize(theAttributes->size());

			for (unsigned int i=0 ; i<theAttributes->size(); i++) {
				AORRr->setAttributes(theAttributes->getHandle(i),i);
			}
		}
	}
	break ;

	case Message::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE: {
		M_Cancel_Negotiated_Attribute_Ownership_Divestiture *CNAODq;
		CNAODq =  static_cast<M_Cancel_Negotiated_Attribute_Ownership_Divestiture*>(req);
		D.Out(pdTrace, "Receiving Message from Federate, "
				"type cancelNegociatedAttributeOwnershipDivestiture.");
		D.Out(pdTrace, "Object %u nb Attribute %u ",
				CNAODq->getObject(), CNAODq->getAttributesSize());
		owm->cancelnegotiatedAttributeOwnershipDivestiture(CNAODq->getObject(),
				CNAODq->getAttributes(),
				CNAODq->getAttributesSize(),
				e);
	}
	break ;


	case Message::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION: {
		M_Cancel_Attribute_Ownership_Acquisition *CAOAq;
		CAOAq =  static_cast<M_Cancel_Attribute_Ownership_Acquisition*>(req);
		D.Out(pdTrace, "Receiving Message from Federate, "
				"type cancelAttributeOwnershipAcquisition.");
		D.Out(pdTrace, "Object %u nb Attribute %u ",
				CAOAq->getObject(), CAOAq->getAttributesSize());
		owm->cancelattributeOwnershipAcquisition(CAOAq->getObject(),
				CAOAq->getAttributes(),
				CAOAq->getAttributesSize(),
				e);
	}
	break ;

	case Message::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE: {
		M_Attribute_Ownership_Acquisition_If_Available *AOQIAq;
		AOQIAq =  static_cast<M_Attribute_Ownership_Acquisition_If_Available*>(req);

		D.Out(pdTrace, "Receiving Message from Federate, "
				"type attributeOwnershipAcquisitionIfAvailable.");
		D.Out(pdTrace, "Object %u nb Attribute %u ",
				AOQIAq->getObject(), AOQIAq->getAttributesSize());
		owm->attributeOwnershipAcquisitionIfAvailable(AOQIAq->getObject(),
				AOQIAq->getAttributes(),
				AOQIAq->getAttributesSize(),
				e);
	}
	break ;

	case Message::QUERY_ATTRIBUTE_OWNERSHIP: {
		M_Query_Attribute_Ownership *QAOq;
		QAOq = static_cast<M_Query_Attribute_Ownership *>(req);
		D.Out(pdTrace, "Receiving Message from Federate, "
				"type queryAttributeOwnership.");
		D.Out(pdTrace, "Object %u Attribute %u ",
				QAOq->getObject(), QAOq->getAttribute());
		owm->queryAttributeOwnership(QAOq->getObject(), QAOq->getAttribute(), e);
	}
	break ;

	case Message::IS_ATTRIBUTE_OWNED_BY_FEDERATE: {
		M_Is_Attribute_Owned_By_Federate *IAWBFq;
		IAWBFq = static_cast<M_Is_Attribute_Owned_By_Federate *>(req);
		D.Out(pdTrace, "Receiving Message from Federate, "
				"type isAttributeOwnedByFederate.");
		D.Out(pdTrace, "Object %u Attribute %u ",
				IAWBFq->getObject(), IAWBFq->getAttribute());
		rep->setTag(owm->attributeOwnedByFederate(IAWBFq->getObject(),
				IAWBFq->getAttribute(), e));
	}
	break ;

	case Message::QUERY_LBTS:
		D.Out(pdTrace, "Receiving Message from Federate, type RequestLBTS.");

		rep->setDate(tm->requestLBTS());
		break ;

	case Message::ENABLE_ASYNCHRONOUS_DELIVERY:
		D.Out(pdTrace, "Receiving Message from Federate, "
				"type EnableAsynchronousDelivery.");
		if (! tm->_asynchronous_delivery)
			tm->_asynchronous_delivery = true ;
		else
			e = e_AsynchronousDeliveryAlreadyEnabled;
		break ;

	case Message::DISABLE_ASYNCHRONOUS_DELIVERY:
		D.Out(pdTrace, "Receiving Message from Federate, "
				"type DisableAsynchronousDelivery.");
		if (tm->_asynchronous_delivery)
			tm->_asynchronous_delivery = false ;
		else
			e = e_AsynchronousDeliveryAlreadyDisabled;
		break ;


	case Message::QUERY_FEDERATE_TIME:
		D.Out(pdTrace,
				"Receiving Message from Federate, type RequestFederateTime.");

		rep->setDate(tm->requestFederateTime());
		break ;

	case Message::QUERY_MIN_NEXT_EVENT_TIME:
		D.Out(pdTrace,
				"Receiving Message from Federate, type QueryMinNextEventTime.");

		rep->setDate(tm->requestMinNextEventTime());
		break ;

	case Message::MODIFY_LOOKAHEAD: {
		M_Modify_Lookahead *MLr, *MLq;
				MLr = static_cast<M_Modify_Lookahead*>(rep);
				MLq = static_cast<M_Modify_Lookahead*>(req);
		D.Out(pdTrace, "Receiving Message from Federate, type SetLookAhead.");

		tm->setLookahead(MLq->getLookahead(), e);
		MLr->setLookahead(MLq->getLookahead());
	}
		break ;

	case Message::QUERY_LOOKAHEAD: {
		M_Query_Lookahead *QLr;
		QLr = static_cast<M_Query_Lookahead*>(rep);
		// The query part is not necessary.
		//M_Query_Lookahead *QLq;
		//QLq = static_cast<M_Query_Lookahead*>(req);
		D.Out(pdTrace,
				"Receiving Message from Federate, type RequestLookAhead.");

		QLr->setLookahead(tm->requestLookahead().getTime());
	}
	break ;

	case Message::TIME_ADVANCE_REQUEST:
		D.Out(pdTrace,
				"Receiving Message from Federate, type TimeAdvanceRequest.");

		tm->timeAdvanceRequest(req->getDate(), e);
		break ;

	case Message::TIME_ADVANCE_REQUEST_AVAILABLE:
		D.Out(pdTrace,
				"Receiving Message from Federate, type TimeAdvanceRequestAvailable.");

		tm->timeAdvanceRequestAvailable(req->getDate(), e);
		break ;

	case Message::NEXT_EVENT_REQUEST:
		D.Out(pdTrace,
				"Receiving Message from Federate, type NestEventRequest.");

		tm->nextEventRequest(req->getDate(), e);
		break ;

	case Message::NEXT_EVENT_REQUEST_AVAILABLE:
		D.Out(pdTrace,
				"Receiving Message from Federate, type NestEventRequestAvailable.");

		tm->nextEventRequestAvailable(req->getDate(), e);
		break ;

	case Message::FLUSH_QUEUE_REQUEST:
		e = e_UnimplementedService ;
		break ;

		// May throw NameNotFound
		// Exception catched and stored in rep Message for answer
	case Message::GET_OBJECT_CLASS_HANDLE: {
		M_Get_Object_Class_Handle *GOCHq, *GOCHr;
		GOCHq = static_cast<M_Get_Object_Class_Handle *>(req);
		GOCHr = static_cast<M_Get_Object_Class_Handle *>(rep);
		D.Out(pdTrace,
				"Receiving Message from Federate, type GetObjectClassHandle.");
		try {
			GOCHr->setObjectClass(om->getObjectClassHandle(GOCHq->getClassName()));
			GOCHr->setClassName(GOCHq->getClassName());
		}
		catch (Exception &egoch)
		{
			rep->setException(static_cast<TypeException>(egoch.getType()),egoch._reason);
		}
	}
	break ;

	case Message::GET_OBJECT_CLASS_NAME: {
		M_Get_Object_Class_Name *GOCNq, *GOCNr;
		GOCNq = static_cast<M_Get_Object_Class_Name *>(req);
		GOCNr = static_cast<M_Get_Object_Class_Name *>(rep);
		D.Out(pdTrace,
				"Receiving Message from Federate, type GetObjectClassName.");

		try {
			GOCNr->setClassName(om->getObjectClassName(GOCNq->getObjectClass()));
			GOCNr->setObjectClass(GOCNq->getObjectClass());
		}
		catch (ObjectClassNotDefined&) {
			rep->setException(e_ObjectClassNotDefined);
		}
		catch (RTIinternalError&) {
			rep->setException(e_RTIinternalError);
		}
	}
	break ;
	case Message::GET_OBJECT_INSTANCE_HANDLE: {
		M_Get_Object_Instance_Handle *GOIHq, *GOIHr;
		GOIHq = static_cast<M_Get_Object_Instance_Handle *>(req);
		GOIHr = static_cast<M_Get_Object_Instance_Handle *>(rep);
		D.Out(pdTrace,
				"Receiving Message from Federate, type getObjectInstanceHandle.");
		GOIHr->setObject(om->getObjectInstanceHandle(GOIHq->getObjectInstanceName()));
		GOIHr->setObjectInstanceName(GOIHq->getObjectInstanceName());
	}
	break ;

	case Message::GET_OBJECT_INSTANCE_NAME: {
		M_Get_Object_Instance_Name *GOINq, *GOINr;
		GOINq = static_cast<M_Get_Object_Instance_Name *>(req);
		GOINr = static_cast<M_Get_Object_Instance_Name *>(rep);

		D.Out(pdTrace,
				"Receiving Message from Federate, type getObjectInstanceName.");
		GOINr->setObjectInstanceName(om->getObjectInstanceName(GOINq->getObject()));
		GOINr->setObject(GOINq->getObject());
	}
	break ;

	// May throw NameNotFound
	// Exception catched and stored in rep Message for answer
	case Message::GET_ATTRIBUTE_HANDLE: {
		M_Get_Attribute_Handle *GAHq,*GAHr;
		GAHq = static_cast<M_Get_Attribute_Handle *>(req);
		GAHr = static_cast<M_Get_Attribute_Handle *>(rep);
		D.Out(pdTrace,
				"Receiving Message from Federate, type GetAttributeHandle.");
		try {
			GAHr->setAttribute(om->getAttributeHandle(GAHq->getAttributeName(),
					GAHq->getObjectClass()));
			GAHr->setAttributeName(GAHq->getAttributeName());
			GAHr->setObjectClass(GAHq->getObjectClass());
		}
		catch (Exception &egah)
		{
			rep->setException(static_cast<TypeException>(egah.getType()),egah._reason);
		}
	}
	break ;

	case Message::GET_ATTRIBUTE_NAME: {
		M_Get_Attribute_Name *GANq,*GANr;
		GANq = static_cast<M_Get_Attribute_Name *>(req);
		GANr = static_cast<M_Get_Attribute_Name *>(rep);
		D.Out(pdTrace,
				"Receiving Message from Federate, type GetAttributeName.");

		GANr->setAttributeName(om->getAttributeName(GANq->getAttribute(),
				GANq->getObjectClass()));
	}
	break ;

	case Message::GET_INTERACTION_CLASS_HANDLE: {
		M_Get_Interaction_Class_Handle *GICHq,*GICHr;
		GICHr = static_cast<M_Get_Interaction_Class_Handle*>(rep);
		GICHq = static_cast<M_Get_Interaction_Class_Handle*>(req);

		D.Out(pdTrace,
				"Receiving Message from Federate, type GetInteractionHandle.");

		GICHr->setInteractionClass(om->getInteractionClassHandle(GICHq->getClassName()));
		GICHr->setClassName(GICHq->getClassName());
	}
	break ;

	case Message::GET_INTERACTION_CLASS_NAME: {
		M_Get_Interaction_Class_Name *GICNq,*GICNr;
		GICNr = static_cast<M_Get_Interaction_Class_Name*>(rep);
		GICNq = static_cast<M_Get_Interaction_Class_Name*>(req);

		D.Out(pdTrace,
				"Receiving Message from Federate, type GetInteractionName.");

		GICNr->setClassName(om->getInteractionClassName(GICNq->getInteractionClass()));
		GICNr->setInteractionClass(GICNq->getInteractionClass());
	}
	break ;

	case Message::GET_PARAMETER_HANDLE: {
		M_Get_Parameter_Handle *GPHq,*GPHr;
		GPHr = static_cast<M_Get_Parameter_Handle*>(rep);
		GPHq = static_cast<M_Get_Parameter_Handle*>(req);
		D.Out(pdTrace,
				"Receiving Message from Federate, type GetParameterHandle.");

		GPHr->setParameter(om->getParameterHandle(GPHq->getParameterName(),
				GPHq->getInteractionClass()));
		GPHr->setParameterName(GPHq->getParameterName());
		GPHr->setInteractionClass(GPHq->getInteractionClass());
	}
	break ;

	case Message::GET_PARAMETER_NAME: {
		M_Get_Parameter_Name *GPNq,*GPNr;
		GPNr = static_cast<M_Get_Parameter_Name*>(rep);
		GPNq = static_cast<M_Get_Parameter_Name*>(req);
		D.Out(pdTrace,
				"Receiving Message from Federate, type GetParameterName.");

		GPNr->setParameterName(om->getParameterName(GPNq->getParameter(),
				GPNq->getInteractionClass()));
		GPNr->setParameter(GPNq->getParameter());
		GPNr->setInteractionClass(GPNq->getInteractionClass());
	}
	break ;

	case Message::GET_SPACE_HANDLE: {
		M_Get_Space_Handle *GSHq,*GSHr;
		GSHr = static_cast<M_Get_Space_Handle*>(rep);
		GSHq = static_cast<M_Get_Space_Handle*>(req);
		D.Out(pdTrace, "Receiving Message from Federate, type GetSpaceHandle.");
		GSHr->setSpace(ddm->getRoutingSpaceHandle(GSHq->getSpaceName()));
	}
	break ;

	case Message::GET_SPACE_NAME: {
		M_Get_Space_Name *GSNq,*GSNr;
		GSNr = static_cast<M_Get_Space_Name*>(rep);
		GSNq = static_cast<M_Get_Space_Name*>(req);
		D.Out(pdTrace, "Receiving Message from Federate, type GetSpaceName.");
		GSNr->setSpaceName(ddm->getRoutingSpaceName(GSNq->getSpace()));
	}
	break ;

	case Message::GET_DIMENSION_HANDLE: {
		M_Get_Dimension_Handle *GDHq,*GDHr;
		GDHr = static_cast<M_Get_Dimension_Handle*>(rep);
		GDHq = static_cast<M_Get_Dimension_Handle*>(req);
		D.Out(pdTrace,
				"Receiving Message from Federate, type GetDimensionHandle");
		GDHr->setDimension(ddm->getDimensionHandle(string(GDHq->getDimensionName()),
				GDHq->getSpace()));
	}
	break ;

	case Message::GET_DIMENSION_NAME: {
		M_Get_Dimension_Name *GDNq,*GDNr;
		GDNr = static_cast<M_Get_Dimension_Name*>(rep);
		GDNq = static_cast<M_Get_Dimension_Name*>(req);
		D.Out(pdTrace,
				"Receiving Message from Federate, type GetDimensionName");
		GDNr->setDimensionName(ddm->getDimensionName(GDNq->getDimension(),
				GDNq->getSpace()));
	}
	break ;

	case Message::GET_ATTRIBUTE_SPACE_HANDLE:{
		M_Get_Attribute_Space_Handle *GASHq,*GASHr;
		GASHr = static_cast<M_Get_Attribute_Space_Handle*>(rep);
		GASHq = static_cast<M_Get_Attribute_Space_Handle*>(req);
		D.Out(pdTrace,
				"Receiving Message from Federate, type GetAttributeSpaceHandle");
		GASHr->setSpace(ddm->getAttributeSpace(GASHq->getAttribute(),
				GASHq->getObjectClass()));
	}
	break ;

	case Message::GET_INTERACTION_SPACE_HANDLE: {
		M_Get_Interaction_Space_Handle *GISHq,*GISHr;
		GISHr = static_cast<M_Get_Interaction_Space_Handle*>(rep);
		GISHq = static_cast<M_Get_Interaction_Space_Handle*>(req);
		D.Out(pdTrace,
				"Receiving Message from Federate: GetInteractionSpaceHandle");
		GISHr->setSpace(ddm->getInteractionSpace(GISHq->getInteractionClass()));
	}
	break ;

	case Message::GET_OBJECT_CLASS: {
		M_Get_Object_Class *GOCq,*GOCr;
		GOCq = static_cast<M_Get_Object_Class *>(req);
		GOCr = static_cast<M_Get_Object_Class *>(rep);
		Debug(D, pdTrace) << "Message from Federate: getObjectClass" << std::endl ;
		GOCr->setObjectClass(om->getObjectClass(GOCq->getObject()));
	}
	break ;

	case Message::GET_TRANSPORTATION_HANDLE: {
		M_Get_Transportation_Handle *GTHq,*GTHr;
		GTHr = static_cast<M_Get_Transportation_Handle*>(rep);
		GTHq = static_cast<M_Get_Transportation_Handle*>(req);
		Debug(D, pdTrace) << "Message from Federate: getTransportationHandle" << std::endl ;
		GTHr->setTransportation(om->getTransportationHandle(GTHq->getTransportationName()));
	}
	break ;

	case Message::GET_TRANSPORTATION_NAME: {
		M_Get_Transportation_Name *GTNq,*GTNr;
		GTNr = static_cast<M_Get_Transportation_Name*>(rep);
		GTNq = static_cast<M_Get_Transportation_Name*>(req);
		Debug(D, pdTrace) << "Message from Federate: getTransportationName" << std::endl ;
		GTNr->setTransportationName(om->getTransportationName(GTNq->getTransportation()));
	}
	break ;

	case Message::GET_ORDERING_HANDLE: {
		M_Get_Ordering_Handle *GOHq,*GOHr;
		GOHr = static_cast<M_Get_Ordering_Handle*>(rep);
		GOHq = static_cast<M_Get_Ordering_Handle*>(req);
		Debug(D, pdTrace) << "Message from Federate: getOrderingHandle" << std::endl ;
		GOHr->setOrdering(om->getOrderingHandle(GOHq->getOrderingName()));
	}
	break ;

	case Message::GET_ORDERING_NAME: {
		M_Get_Ordering_Name *GONq,*GONr;
		GONr = static_cast<M_Get_Ordering_Name*>(rep);
		GONq = static_cast<M_Get_Ordering_Name*>(req);
		Debug(D, pdTrace) << "Message from Federate: getOrderingName" << std::endl ;
		GONr->setOrderingName(om->getOrderingName(GONq->getOrdering()));
	}
	break ;

	case Message::DDM_CREATE_REGION: {
		M_Ddm_Create_Region *DDMCRq,*DDMCRr;
		DDMCRr = static_cast<M_Ddm_Create_Region*>(rep);
		DDMCRq = static_cast<M_Ddm_Create_Region*>(req);
		Debug(D, pdTrace) << "Receiving Message from Federate: CreateRegion" << std::endl ;
		DDMCRr->setRegion(ddm->createRegion(DDMCRq->getSpace(), DDMCRq->getExtentSetSize(), e));
		DDMCRr->setExtentSetSize(rootObject->getRoutingSpace(DDMCRq->getSpace()).size());
	}
	break ;

	case Message::DDM_MODIFY_REGION: {
		M_Ddm_Modify_Region *DDMMRq;
		DDMMRq = static_cast<M_Ddm_Modify_Region*>(req);
		Debug(D, pdTrace) << "Receiving Message from Federate: Modify Region" << std::endl ;
		ddm->modifyRegion(DDMMRq->getRegion(), DDMMRq->getExtents(), e);
	}
		break ;

	case Message::DDM_DELETE_REGION: {
		M_Ddm_Delete_Region *DDMDRq;
		DDMDRq = static_cast<M_Ddm_Delete_Region*>(req);
		Debug(D, pdTrace) << "Receiving Message from Federate: DeleteRegion" << std::endl ;
		ddm->deleteRegion(DDMDRq->getRegion(), e);
	}
		break ;

	case Message::DDM_ASSOCIATE_REGION: {
		M_Ddm_Associate_Region *DDMARq;
		DDMARq = static_cast<M_Ddm_Associate_Region*>(req);

		Debug(D, pdTrace) << "Receiving Message from Federate: Associate Region"
				<< std::endl ;
		ddm->associateRegion(DDMARq->getObject(), DDMARq->getRegion(),
		DDMARq->getAttributes(), DDMARq->getAttributesSize(), e);
	}
	break ;

	case Message::DDM_REGISTER_OBJECT: {
		M_Ddm_Register_Object *DDMROq,*DDMROr;
		DDMROq = static_cast<M_Ddm_Register_Object*>(req);
		DDMROr = static_cast<M_Ddm_Register_Object*>(rep);
		Debug(D, pdTrace) << "Receiving Message from Federate: Register with Region"
				<< std::endl ;
		DDMROr->setObject(ddm->registerObject(DDMROq->getObjectClass(),
				DDMROq->getObjectInstanceName(),
				DDMROq->getAttributes(),
				DDMROq->getAttributesSize(),
				DDMROq->getRegions(),
				e));
	}
	break ;

	case Message::DDM_UNASSOCIATE_REGION: {
		M_Ddm_Unassociate_Region *DDMURq;
		DDMURq = static_cast<M_Ddm_Unassociate_Region*>(req);

		Debug(D, pdTrace) << "Receiving Message from Federate: Unassociate Region" << std::endl ;
		ddm->unassociateRegion(DDMURq->getObject(), DDMURq->getRegion(), e);
	}
	break ;

	case Message::DDM_SUBSCRIBE_ATTRIBUTES: {
		M_Ddm_Subscribe_Attributes *DDMSAq;
		DDMSAq = static_cast<M_Ddm_Subscribe_Attributes*>(req);
		Debug(D, pdTrace) << "Receiving Message from Federate: Subscribe Attributes"
				<< std::endl ;
		ddm->subscribe(DDMSAq->getObjectClass(), DDMSAq->getRegion(),
				DDMSAq->getAttributes(), DDMSAq->getAttributesSize(), e);
	}
	break ;

	case Message::DDM_UNSUBSCRIBE_ATTRIBUTES: {
		M_Ddm_Unsubscribe_Attributes *DDMUAq;
		DDMUAq = static_cast<M_Ddm_Unsubscribe_Attributes*>(req);

		Debug(D, pdTrace) << "Receiving Message from Federate: Unsubscribe class "
				<< DDMUAq->getObjectClass() << std::endl ;
		ddm->unsubscribeAttributes(DDMUAq->getObjectClass(), DDMUAq->getRegion(), e);
	}
	break ;

	case Message::DDM_SUBSCRIBE_INTERACTION: {
		M_Ddm_Subscribe_Interaction* DSIq;
				DSIq = static_cast<M_Ddm_Subscribe_Interaction* >(req);
		Debug(D, pdTrace) << "Receiving Message from Federate: Subscribe Interaction"
		<< std::endl ;
		ddm->subscribeInteraction(DSIq->getInteractionClass(),
				DSIq->getRegion(), e);
	}
		break ;

	case Message::DDM_UNSUBSCRIBE_INTERACTION: {
		M_Ddm_Unsubscribe_Interaction* DUIq;
		DUIq = static_cast<M_Ddm_Unsubscribe_Interaction* >(req);
		Debug(D, pdTrace) << "Receiving Message from Federate: Unsubscribe Interaction"
				<< std::endl ;
		ddm->unsubscribeInteraction(DUIq->getInteractionClass(),
				DUIq->getRegion(), e);
	}
	break ;

	case Message::ENABLE_TIME_REGULATION:
		tm->setTimeRegulating(true,req->getDate(),static_cast<M_Enable_Time_Regulation *>(req)->getLookahead(),e);
		break;
	case Message::DISABLE_TIME_REGULATION:
		tm->setTimeRegulating(false,req->getDate(),static_cast<M_Disable_Time_Regulation *>(req)->getLookahead(),e);
		break;

	case Message::ENABLE_TIME_CONSTRAINED:
		tm->setTimeConstrained(true, e);
		break;
	case Message::DISABLE_TIME_CONSTRAINED:
		tm->setTimeConstrained(false, e);
		break ;
	case Message::TICK_REQUEST:
		M_Tick_Request* TRq;
		TRq = static_cast<M_Tick_Request *>(req);
		// called when tick() is invoked
		if (tm->_tick_state != TimeManagement::NO_TICK) {

			throw RTIinternalError(stringize() << "TICK_REQUEST cannot be called recursively (state=" << static_cast<int>(tm->_tick_state) <<")");
		}

		tm->_tick_multiple = TRq->getMultiple();
		tm->_tick_result   = false; // default return value

		if (TRq->getMinTickTime() >= 0.0) {
			tm->_tick_timeout     = TRq->getMinTickTime();
			tm->_tick_max_tick    = TRq->getMaxTickTime();
			tm->_tick_clock_start = clock->getCurrentTicksValue();
			tm->_tick_state       = TimeManagement::TICK_BLOCKING;
		}
		else {
			tm->_tick_state = TimeManagement::TICK_CALLBACK;
		}
		processOngoingTick();
		break ;

	case Message::TICK_REQUEST_NEXT:
		// called when tick() queries result, or next callback
		if (tm->_tick_state != TimeManagement::TICK_CALLBACK &&
				tm->_tick_state != TimeManagement::TICK_RETURN)
			throw RTIinternalError("unexpected TICK_REQUEST_NEXT");

		processOngoingTick();
		break ;

	case Message::TICK_REQUEST_STOP:
		// called to terminate tick() when error occured
		// do not invoke any callbacks, reset _tick_state and return
		tm->_tick_state = TimeManagement::TICK_RETURN;

		processOngoingTick();
		break ;

	case Message::ENABLE_CLASS_RELEVANCE_ADVISORY_SWITCH:
		D.Out(pdTrace,
				"Receiving Message from Federate, type EnableClassRelevanceAdvisorySwitch.");
		dm->setClassRelevanceAdvisorySwitch(true, e);
		break;

	case Message::DISABLE_CLASS_RELEVANCE_ADVISORY_SWITCH:
		D.Out(pdTrace,
				"Receiving Message from Federate, type DisableClassRelevanceAdvisorySwitch.");
		dm->setClassRelevanceAdvisorySwitch(false, e);
		break;

	case Message::ENABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH:
		D.Out(pdTrace,
				"Receiving Message from Federate, type SetInteractionRelevanceAdvisorySwitch.");
		dm->setInteractionRelevanceAdvisorySwitch(true, e);
		break;

	case Message::DISABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH:
		D.Out(pdTrace,
				"Receiving Message from Federate, type SetInteractionRelevanceAdvisorySwitch.");
		dm->setInteractionRelevanceAdvisorySwitch(false, e);
		break;

	case Message::ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH:
		D.Out(pdTrace,
				"Receiving Message from Federate, type SetAttributeRelevanceAdvisorySwitch.");
		om->setAttributeRelevanceAdvisorySwitch(true, e);
		break;

	case Message::DISABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH:
		D.Out(pdTrace,
				"Receiving Message from Federate, type SetAttributeRelevanceAdvisorySwitch.");
		om->setAttributeRelevanceAdvisorySwitch(false, e);
		break;

	case Message::ENABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH:
		D.Out(pdTrace,
				"Receiving Message from Federate, type SetAttributeScopeAdvisorySwitch.");
		om->setAttributeScopeAdvisorySwitch(true, e);
		break;

	case Message::DISABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH:
		D.Out(pdTrace,
				"Receiving Message from Federate, type SetAttributeScopeAdvisorySwitch.");
		om->setAttributeScopeAdvisorySwitch(false, e);
		break;

	default:
		D.Out(pdExcept,
				"Receiving Message from Federate, Unknown Type %s/%d.", req->getMessageName(), req->getMessageType());
		throw RTIinternalError("");
	}
	stat.federateService(req->getMessageType());
	G.Out(pdGendoc,"exit  chooseFederateProcessing");
} /* end of RTIA::chooseFederateProcessing */

void
RTIA::processOngoingTick() {
	TypeException exc = e_NO_EXCEPTION;

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
			if (tm->_tick_result   &&
				tm->_tick_multiple &&
				1e-9*clock->getDeltaNanoSecond(tm->_tick_clock_start) < tm->_tick_max_tick) {


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
			if ( exc != e_RTIinternalError )
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

void
RTIA::initFederateProcessing(Message *req, Message* rep)
{
    if(req->getMessageType() == Message::OPEN_CONNEXION) {
        M_Open_Connexion *OCq, *OCr;
        OCq = static_cast<M_Open_Connexion *>(req);
        OCr = static_cast<M_Open_Connexion *>(rep);

        if(OCq->getVersionMajor() == CERTI_Message::versionMajor) {
            uint32_t minorEffective = OCq->getVersionMinor() <= CERTI_Message::versionMinor
                    ? OCq->getVersionMinor() : CERTI_Message::versionMinor;
            OCr->setVersionMajor(CERTI_Message::versionMajor);
            OCr->setVersionMinor(minorEffective);

            fm->_connection_state = FederationManagement::CONNECTION_READY;
        }
        else {
            rep->setException(e_RTIinternalError, stringize()
                    << "RTIA protocol version mismatch"
                    << "; federate " << OCq->getVersionMajor() << "." << OCq->getVersionMinor()
                    << ", RTIA " << CERTI_Message::versionMajor << "." << CERTI_Message::versionMinor);
        }
    }
    else {
        rep->setException(e_RTIinternalError,
                "RTIA protocol version mismatch; expecting OPEN_CONNECTION first.");
    }
    stat.federateService(req->getMessageType());
}

void
RTIA::processFederateRequest(Message *req)
{
	/* use virtual constructor in order to build  *
	 * appropriate answer message.                */
	std::auto_ptr<Message> rep(M_Factory::create(req->getMessageType()));

	G.Out(pdGendoc,"enter RTIA::processFederateRequest");

	try {
		switch(fm->_connection_state) {
		case FederationManagement::CONNECTION_PRELUDE:
			initFederateProcessing(req, rep.get());
			break;

		case FederationManagement::CONNECTION_READY:
		{
			TypeException exc ;
			chooseFederateProcessing(req, rep.get(), exc);
			if ( exc != e_RTIinternalError && exc != e_NO_EXCEPTION) {
				rep->setException(exc);
			}
			break;
		}

		case FederationManagement::CONNECTION_FIN:
		default:
			rep->setException(e_RTIinternalError,
				"RTIA connection already closed.");
			break;
		}
	}
	// FIXME should
	// catch (Exception &e) {
	//  rep->setException(static_cast<TypeException>(e.getMessageType()),e._reason);
	catch (ArrayIndexOutOfBounds &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_ArrayIndexOutOfBounds);
		rep->setException(static_cast<TypeException>(e.getType()),e._reason);
	}
	catch (AttributeAlreadyOwned &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_AttributeAlreadyOwned);
	}
	catch (AttributeAlreadyBeingDivested &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_AttributeAlreadyBeingDivested);
	}
	catch (AttributeAlreadyBeingAcquired &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_AttributeAlreadyBeingAcquired);
	}
	catch (AttributeAcquisitionWasNotRequested &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_AttributeAcquisitionWasNotRequested);
	}
	catch (AttributeDivestitureWasNotRequested &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_AttributeDivestitureWasNotRequested);
	}
	catch (AttributeNotDefined &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_AttributeNotDefined);
	}
	catch (AttributeNotKnown &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_AttributeNotKnown);
	}
	catch (AttributeNotOwned &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_AttributeNotOwned);
	}
	catch (AttributeNotPublished &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_AttributeNotPublished);
	}
	catch (AttributeNotSubscribed &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_AttributeNotSubscribed);
	}
	catch (ConcurrentAccessAttempted &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_ConcurrentAccessAttempted);
	}
	catch (CouldNotDiscover &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_CouldNotDiscover);
	}
	catch (CouldNotOpenRID &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_CouldNotOpenRID);
	}
	catch (CouldNotOpenFED &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_CouldNotOpenFED,e._reason);
	}
	catch (CouldNotRestore &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_CouldNotRestore);
	}
	catch (DeletePrivilegeNotHeld &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_DeletePrivilegeNotHeld);
	}
	catch (ErrorReadingRID &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_ErrorReadingRID,e._reason);
	}
	catch (ErrorReadingFED &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_ErrorReadingFED,e._reason);
	}
	catch (EventNotKnown &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_EventNotKnown);
	}
	catch (FederateAlreadyPaused &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_FederateAlreadyPaused);
	}
	catch (FederateAlreadyExecutionMember &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_FederateAlreadyExecutionMember,e._reason);
	}
	catch (FederateDoesNotExist &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_FederateDoesNotExist);
	}
	catch (FederateInternalError &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_FederateInternalError);
	}
	catch (FederateNameAlreadyInUse &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_FederateNameAlreadyInUse);
	}
	catch (FederateNotExecutionMember &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_FederateNotExecutionMember);
	}
	catch (FederateNotPaused &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_FederateNotPaused);
	}
	catch (FederateNotPublishing &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_FederateNotPublishing);
	}
	catch (FederateNotSubscribing &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_FederateNotSubscribing);
	}
	catch (FederateOwnsAttributes &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_FederateOwnsAttributes);
	}
	catch (FederatesCurrentlyJoined &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_FederatesCurrentlyJoined);
	}
	catch (FederateWasNotAskedToReleaseAttribute &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_FederateWasNotAskedToReleaseAttribute);
	}
	catch (FederationAlreadyPaused &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_FederationAlreadyPaused);
	}
	catch (FederationExecutionAlreadyExists &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_FederationExecutionAlreadyExists,e._reason);
	}
	catch (FederationExecutionDoesNotExist &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_FederationExecutionDoesNotExist);
	}
	catch (FederationNotPaused &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_FederationNotPaused);
	}
	catch (FederationTimeAlreadyPassed &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_FederationTimeAlreadyPassed);
	}
	catch (IDsupplyExhausted &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_IDsupplyExhausted);
	}
	catch (InteractionClassNotDefined &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_InteractionClassNotDefined);
	}
	catch (InteractionClassNotKnown &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_InteractionClassNotKnown);
	}
	catch (InteractionClassNotPublished &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_InteractionClassNotPublished);
	}
	catch (InteractionParameterNotDefined &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_InteractionParameterNotDefined);
	}
	catch (InteractionParameterNotKnown &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_InteractionParameterNotKnown);
	}
	catch (InvalidDivestitureCondition &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_InvalidDivestitureCondition);
	}
	catch (InvalidExtents &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_InvalidExtents);
	}
	catch (InvalidFederationTime &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_InvalidFederationTime);
	}
	catch (InvalidFederationTimeDelta &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_InvalidFederationTimeDelta);
	}
	catch (InvalidObjectHandle &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_InvalidObjectHandle);
	}
	catch (InvalidOrderingHandle &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_InvalidOrderingHandle);
	}
	catch (InvalidResignAction &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_InvalidResignAction);
	}
	catch (InvalidRetractionHandle &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_InvalidRetractionHandle);
	}
	catch (InvalidRoutingSpace &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_InvalidRoutingSpace);
	}
	catch (InvalidTransportationHandle &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_InvalidTransportationHandle);
	}
	catch (MemoryExhausted &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_MemoryExhausted);
	}
	catch (NameNotFound &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_NameNotFound);
	}
	catch (NoPauseRequested &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_NoPauseRequested);
	}
	catch (NoResumeRequested &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_NoResumeRequested);
	}
	catch (ObjectClassNotDefined &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_ObjectClassNotDefined);
	}
	catch (ObjectClassNotKnown &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_ObjectClassNotKnown);
	}
	catch (ObjectClassNotPublished &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_ObjectClassNotPublished);
	}
	catch (ObjectClassNotSubscribed &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_ObjectClassNotSubscribed);
	}
	catch (ObjectNotKnown &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_ObjectNotKnown);
	}
	catch (ObjectAlreadyRegistered &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_ObjectAlreadyRegistered);
	}
	catch (RegionNotKnown &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_RegionNotKnown);
	}
	catch (RestoreInProgress &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_RestoreInProgress);
	}
	catch (RestoreNotRequested &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_RestoreNotRequested);
	}
	catch (RTIinternalError &e) {
		Debug(D,pdError) << "RTIA sends InternalError to Fed., " ;
		if (!e._reason.empty()) {
			Debug(D,pdError) << "reason : " << e._reason << std::endl ;
		}
		else {
			Debug(D,pdError) << "no reason given." << std::endl ;
		}
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_RTIinternalError);
	}
	catch (SpaceNotDefined &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_SpaceNotDefined);
	}
	catch (SaveInProgress &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_SaveInProgress);
	}
	catch (SaveNotInitiated &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_SaveNotInitiated);
	}
	catch (SpecifiedSaveLabelDoesNotExist &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_SpecifiedSaveLabelDoesNotExist);
	}
	catch (TimeAdvanceAlreadyInProgress &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_TimeAdvanceAlreadyInProgress);
	}
	catch (TimeAdvanceWasNotInProgress &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_TimeAdvanceWasNotInProgress);
	}
	catch (TooManyIDsRequested &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_TooManyIDsRequested);
	}
	catch (UnableToPerformSave &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_UnableToPerformSave);
	}
	catch (UnimplementedService &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_UnimplementedService);
	}
	catch (UnknownLabel &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_UnknownLabel);
	}
	catch (ValueCountExceeded &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_ValueCountExceeded);
	}
	catch (ValueLengthExceeded &e) {
		D.Out(pdExcept, "Catched %s Exception.", e._name);
		rep->setException(e_ValueLengthExceeded);
	}

	// Default Handler
	catch (Exception &e) {
		D.Out(pdExcept, "Unknown Exception : %s.", e._name);
		rep->setException(e_RTIinternalError);
	}

	delete req;

	if (rep->getMessageType() != Message::TICK_REQUEST &&
		rep->getMessageType() != Message::TICK_REQUEST_NEXT &&
		rep->getMessageType() != Message::TICK_REQUEST_STOP) {
		// generic federate service acknowledgment
		// the TICK_REQUEST confirmation is generated in processOngoingTick()
		comm->sendUN(rep.get());
		D.Out(pdDebug, "Reply send to Unix socket.");
	}
	G.Out(pdGendoc,"exit  RTIA::processFederateRequest");
} /* end of RTIA::processFederateRequest(Message *req) */

} // namespace rtia
} // namespace certi

