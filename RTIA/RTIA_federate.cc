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
// $Id: RTIA_federate.cc,v 3.72 2008/04/23 12:55:06 erk Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "RTIA.hh"


#include "fed.hh"
#include "RoutingSpace.hh"
#include "XmlParser.hh"

using std::string ;
using std::ifstream ;

namespace certi {
namespace rtia {

static pdCDebug D("RTIA", __FILE__);
static PrettyDebug G("GENDOC",__FILE__) ;

// ----------------------------------------------------------------------------
static RTI::TickTime currentTickTime()
{
    RTI::TickTime result;
#ifdef _WIN32 
    _timeb timev;
    _ftime(&timev);
    result = timev.time + timev.millitm/1000;
#else 
    struct timeval timev;
    gettimeofday(&timev, NULL);
    result = timev.tv_sec + timev.tv_usec/1000000;
#endif 
    return result;
}

// ----------------------------------------------------------------------------
//! Verify that federate is not in saving or restoring state.
void
RTIA::saveAndRestoreStatus(Message::Type type)
    throw (SaveInProgress, RestoreInProgress)
{
G.Out(pdGendoc,"enter RTIA::saveAndRestoreStatus");
//D.Mes(pdMessage, 'N', type);

    switch (type) {
      case Message::RESIGN_FEDERATION_EXECUTION:
      case Message::TICK_REQUEST:
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
    }
G.Out(pdGendoc,"exit  RTIA::saveAndRestoreStatus");
}

// ----------------------------------------------------------------------------
//! Choose federate processing.
void
RTIA::chooseFederateProcessing(Message *req, Message &rep, TypeException &e)
       throw (CouldNotOpenFED,FederationExecutionAlreadyExists,ErrorReadingFED,
              FederateAlreadyExecutionMember)
{
    G.Out(pdGendoc,"enter RTIA::chooseFederateProcessing for type = %d",req->type);

    // Verify not in saving or restoring state.
    saveAndRestoreStatus(req->type);

    e = e_NO_EXCEPTION ;

    switch(req->type) {

      case Message::CREATE_FEDERATION_EXECUTION:
        D.Out(pdTrace,
              "Receiving Message from Federate, type CreateFederation.");
        // Store FEDid for future usage (JOIN_FEDERATION_EXECUTION) into fm
        fm->_FEDid = new char[strlen(req->getFEDid())+1] ;
        strcpy(fm->_FEDid, req->getFEDid()) ;
        fm->createFederationExecution(req->getFederationName(), e);
        if ( e == e_RTIinternalError )
            {
            rep.setException(e,"Federate is yet a creator or a member !");
            }
        D.Out(pdTrace, "Receiving Message from Federate, "
              "type CreateFederation done.");
        // RTIA needs FEDid into the answer (rep Message) to federate
        rep.setFEDid(fm->_FEDid) ;
        // RTIA needs federation name into the answer (rep Message) to federate
        rep.setFederationName(req->getFederationName());
        break ;

      case Message::DESTROY_FEDERATION_EXECUTION:
        D.Out(pdTrace,
              "Receiving Message from Federate, type DestroyFederation.");

        fm->destroyFederationExecution(req->getFederationName(), e);
        if ( e == e_RTIinternalError )
            {
            rep.setException(e,"Illegal federation handle");
            }
        // RTIA needs federation name into the answer (rep Message) to federate
        rep.setFederationName(req->getFederationName());
        break ;

      case Message::JOIN_FEDERATION_EXECUTION: {
          D.Out(pdTrace,
                "Receiving Message from Federate, type JoinFederation.");
          rep.setFederate(fm->joinFederationExecution(req->getFederateName(),
                                                      req->getFederationName(),
                                                      e));
          if ( e == e_NO_EXCEPTION )              
              {
              /// Set RTIA PrettyDebug federate name
              PrettyDebug::setFederateName(req->getFederateName());
              // Set federation name for the answer message (rep)
              rep.setFederationName(req->getFederationName());

              string filename = fm->_FEDid ;
              int nbcar_filename=filename.length();
              bool is_a_fed = false ;
              bool is_an_xml = false ;

              // Hope there is a . before fed or xml
              if ( filename.at(nbcar_filename-4) != '.' )
                  throw CouldNotOpenFED(". missing or not in place");

              string extension = filename.substr(nbcar_filename-3,3) ;
              if ( !strcasecmp(extension.c_str(),"fed") )
                  {
                  is_a_fed = true ;
                  D.Out(pdTrace, "Trying to use .fed file");
                  }
              else if  ( !strcasecmp(extension.c_str(),"xml") )
                  {
                  is_an_xml = true ;
                  D.Out(pdTrace, "Trying to use .xml file");
                  } 
              else 
                  throw CouldNotOpenFED("nor .fed nor .xml");
    
              ifstream *fdd = new ifstream(filename.c_str());
              if (fdd->is_open())
                  {
                  if ( is_a_fed )
                      {        
	              int result = certi::fedparser::build(filename.c_str(),
		        				   rootObject, true);  
                      if (result != 0 ) throw ErrorReadingFED("invalid .fed");
                      }
                  else if ( is_an_xml )
                      {
                      if (XmlParser::exists())
                          {
                          XmlParser *parser = new XmlParser(rootObject);
                          parser->parse(filename);
                          delete fdd ;
                          delete parser ;
                          }
		       else 
                          throw CouldNotOpenFED("no XmlParser");
                      }
                  }
              else
                  throw CouldNotOpenFED("File not found");
              break ;
              }
          else
              {
              // JOIN FAILED
              throw FederateAlreadyExecutionMember("Federate yet joined or same name");
              break;
              }
      }
      case Message::RESIGN_FEDERATION_EXECUTION:
        D.Out(pdTrace,
              "Receiving Message from Federate, type ResignFederation.");

        fm->resignFederationExecution(req->getResignAction(), e);
        break ;

      case Message::REGISTER_FEDERATION_SYNCHRONIZATION_POINT:
        D.Out(pdTrace, "Receiving Message from Federate, type RegisterFederationSynchronizationPoint.");

        // boolean true means with federates set
        if ( req->getBoolean() )
            fm->registerSynchronization(req->getLabel(), req->getTag(), 
                (unsigned short)req->handleArraySize, req->handleArray, e);
        else
            fm->registerSynchronization(req->getLabel(), req->getTag(), e);
        // Don't forget boolean for the answer
        rep.setBoolean(req->getBoolean()) ;
        break ;

      case Message::SYNCHRONIZATION_POINT_ACHIEVED:
        D.Out(pdTrace, "Receiving Message from Federate, type SynchronizationPointAchieved.");

        fm->unregisterSynchronization(req->getLabel(), e);
        break ;

      case Message::REQUEST_FEDERATION_SAVE:
        // boolean true means with time
        if ( req->getBoolean() )
            {
            D.Out(pdTrace, "Receiving Message from Federate, type"
                           " RequestFederationSave with time.");
            fm->requestFederationSave(req->getLabel(),req->getFederationTime(),
                                      e);
            }
        else
            {
            D.Out(pdTrace, "Receiving Message from Federate, type"
                           " RequestFederationSave without time.");
            fm->requestFederationSave(req->getLabel(),e);
            }
        // Don't forget boolean value (req Message) for the answer (rep Message)
        rep.setBoolean(req->getBoolean()) ;
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
          bool result = req->type == Message::FEDERATE_SAVE_COMPLETE ;
          if (result)
              {
              G.Out(pdGendoc,"chooseFederateProcessing FEDERATE_SAVE_COMPLETE");
              }
          else
              {
              G.Out(pdGendoc,"chooseFederateProcessing FEDERATE_SAVE_NOT_COMPLETE");
              }
          fm->federateSaveStatus(result, e);
      }
        break ;

      case Message::REQUEST_FEDERATION_RESTORE:
        D.Out(pdTrace, "Receiving Message from Federate, type"
              " RequestFederationRestore.");
        G.Out(pdGendoc,"chooseFederateProcessing REQUEST_FEDERATION_RESTORE");

        fm->requestFederationRestore(req->getLabel(), e);
        break ;

      case Message::FEDERATE_RESTORE_COMPLETE:
      case Message::FEDERATE_RESTORE_NOT_COMPLETE: {
          D.Out(pdTrace, "Receiving Message from Federate, type"
                " FederateRestore(Not)Complete.");

          bool result = req->type == Message::FEDERATE_RESTORE_COMPLETE ;
          if (result)
              {
              G.Out(pdGendoc,"chooseFederateProcessing FEDERATE_RETORE_COMPLETE");
              }
          else
              {
              G.Out(pdGendoc,"chooseFederateProcessing FEDERATE_RESTORE_NOT_COMPLETE");
              }
          fm->federateRestoreStatus(result, e);
      }
        break ;

      case Message::PUBLISH_OBJECT_CLASS:
        D.Out(pdTrace,
              "Receiving Message from Federate, type PublishObjectClass.");

        dm->publishObjectClass(req->getObjectClass(),
                               req->handleArray,
                               req->handleArraySize,
                               e);
        break ;

      case Message::UNPUBLISH_OBJECT_CLASS:
        D.Out(pdTrace,
              "Receiving Message from Federate, type UnpublishObjectClass.");

        dm->unpublishObjectClass(req->getObjectClass(), e);
        break ;

      case Message::PUBLISH_INTERACTION_CLASS:
        D.Out(pdTrace,
              "Receiving Message from Federate, type PublishInteractionClass.");

        dm->publishInteractionClass(req->getInteractionClass(), e);
        break ;

      case Message::UNPUBLISH_INTERACTION_CLASS:
        D.Out(pdTrace,
              "Receiving Message from Federate, "
              "type UnpublishInteractionClass");

        dm->unpublishInteractionClass(req->getInteractionClass(), e);
        break ;

      case Message::SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES:
        D.Out(pdTrace,
              "Receiving Message from Federate, type SubscribeObjectClass.");
        dm->subscribeObjectClassAttribute(req->getObjectClass(),
                                          req->handleArray,
                                          req->handleArraySize,
                                          e);
        break ;

      case Message::UNSUBSCRIBE_OBJECT_CLASS:
        D.Out(pdTrace,
              "Receiving Message from Federate, type UnsubscribeObjectClass.");

        // BUG: Why attributes are not transmitted ?
        dm->unsubscribeObjectClassAttribute(req->getObjectClass(), e);
        break ;

      case Message::SUBSCRIBE_INTERACTION_CLASS:
        D.Out(pdTrace,
              "Receiving Message from Federate, type SubscribeInteraction.");

        dm->subscribeInteractionClass(req->getInteractionClass(), e);
        break ;

      case Message::UNSUBSCRIBE_INTERACTION_CLASS:
        D.Out(pdTrace,
              "Receiving Message from Federate, type UnsubscribeInteraction.");

        dm->unsubscribeInteractionClass(req->getInteractionClass(), e);
        break ;

      case Message::REGISTER_OBJECT_INSTANCE: {
          FederationTime date = tm->requestFederateTime();
          FederationTime heure = date + tm->requestLookahead();

          D.Out(pdTrace,
		"Receiving Message from Federate, type RegisterObject.");

          rep.setObject(om->registerObject(req->getObjectClass(),
                                           req->getName(),
                                           date, heure, e));
          break ;
      }

      case Message::UPDATE_ATTRIBUTE_VALUES: {

          ValueLengthPair *ValueArray = req->getValueArray();

          try {
              if (req->getBoolean() )
                  {
                  D.Out(pdTrace,"Receiving Message from Federate, type "
                                "UpdateAttribValues with TIMESTAMP.");
                  rep.setEventRetraction(
		  om->updateAttributeValues(req->getObject(),
                                            req->handleArray,
                                            ValueArray,
                                            req->handleArraySize,
                                            req->getFederationTime(),
                                            req->getTag(),
                                            e));
                  }
              else
                  {
                  D.Out(pdTrace,"Receiving Message from Federate, type "
                                "UpdateAttribValues without TIMESTAMP.");
		  om->updateAttributeValues(req->getObject(),
                                            req->handleArray,
                                            ValueArray,
                                            req->handleArraySize,
                                            req->getTag(),
                                            e);
                  }
              // Don't forget boolean value for the answer
              rep.setBoolean(req->getBoolean()) ;
              free(ValueArray);
          } catch (Exception *e) {
              free(ValueArray);
              throw e ;
          }
      }
        break ;

      case Message::SEND_INTERACTION: {

          ParameterLengthPair *ValueArray = (ParameterLengthPair *) req->getValueArray();
          G.Out(pdGendoc,"S_I into RTIA::chooseFederateProcessing") ;
          try {
              if (req->getBoolean() )
                  {
                  D.Out(pdTrace,
   "Receiving Message from Federate, type SendInteraction with TIMESTAMP.");
                  rep.setEventRetraction(
                      om->sendInteraction(req->getInteractionClass(),
                                      req->handleArray,
                                      ValueArray,
                                      req->handleArraySize,
                                      req->getFederationTime(),
                                      req->getTag(),
				      req->getRegion(),
                                      e));
                  }
            else
                  {
                  D.Out(pdTrace,
   "Receiving Message from Federate, type SendInteraction without TIMESTAMP.");
                      om->sendInteraction(req->getInteractionClass(),
                                      req->handleArray,
                                      ValueArray,
                                      req->handleArraySize,
                                      req->getTag(),
				      req->getRegion(),
                                      e);
                  }
              free(ValueArray);
          } catch (Exception *e) {
              free(ValueArray);
              throw e ;
          }
      }
        break ;

      case Message::DELETE_OBJECT_INSTANCE: {
          
	G.Out(pdGendoc,"D_O_I into RTIA::chooseFederateProcessing") ;

	try {
	    if ( req->getBoolean() ) {
        	D.Out(pdTrace, 
	"Receiving Message from Federate, type DeleteObjectInstance with \
	 TIMESTAMP.");
                rep.setEventRetraction(
			om->deleteObject(
				req->getObject(), 
				req->getFederationTime(),
				req->getTag(), 
				e));
	    }
	    else {
        	D.Out(pdTrace, 
	"Receiving Message from Federate, type DeleteObjectInstance without \
	 TIMESTAMP.");
		om->deleteObject(
			req->getObject(), 
			req->getTag(), 
			e);
	    }
	} catch (Exception *e) {
	    throw e;
	}
      }
        break ;

      case Message::CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE:
        D.Out(pdTrace,
              "Receiving Message from Federate, type ChangeAttribTransport.");

        om->changeAttributeTransportType(req->getObject(),
                                         req->handleArray,
                                         req->handleArraySize,
                                         req->getTransportType(),
                                         e);
        break ;

      case Message::CHANGE_ATTRIBUTE_ORDER_TYPE:
        D.Out(pdTrace,
              "Receiving Message from Federate, type ChangeAttribOrder.");

        om->changeAttributeOrderType(req->getObject(),
                                     req->handleArray,
                                     req->handleArraySize,
                                     req->getOrderType(),
                                     e);
        break ;

      case Message::CHANGE_INTERACTION_TRANSPORTATION_TYPE:
        D.Out(pdTrace,
              "Receiving Message from Federate, type ChangeInterTransport.");

        om->changeInteractionTransportType(req->getInteractionClass(),
                                           req->getTransportType(),
                                           e);
        break ;

      case Message::CHANGE_INTERACTION_ORDER_TYPE:
        D.Out(pdTrace,
              "Receiving Message from Federate, type ChangeInterOrder.");

        om->changeInteractionOrderType(req->getInteractionClass(),
                                       req->getOrderType(), e);
        break ;

      case Message::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE:
            D.Out(pdTrace,"Receiving Message from Federate, type "
                                "RequestAttributeValueUpadate.");
            om->requestObjectAttributeValueUpdate(req->getObject(),
                                            req->handleArray,
                                            req->handleArraySize,
                                            e);
        break ;

      case Message::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE:
        std::cout<<"request class attribute value update not yet implemented in chooseFederateProcessing"
        <<std::endl;
        e = e_UnimplementedService ;
        break ;

      case Message::RETRACT:
        e = e_UnimplementedService ;
        break ;

      case Message::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
        D.Out(pdTrace, "Receiving Message from Federate, "
              "type unconditionalAttributeOwnershipDivestiture.");
        D.Out(pdTrace, "Object %u number of attributes %u ",
              req->getObject(), req->handleArraySize);
        owm->unconditionalAttributeOwnershipDivestiture(req->getObject(),
                                                        req->handleArray,
                                                        req->handleArraySize,
                                                        e);
        break ;

      case Message::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
        D.Out(pdTrace, "Receiving Message from Federate, "
              "type negotiatedAttributeOwnershipDivestiture.");
        D.Out(pdTrace, "Object %u ; %u nb Attribute ", req->getObject(),
              req->handleArraySize);
        owm->negotiatedAttributeOwnershipDivestiture(req->getObject(),
                                                     req->handleArray,
                                                     req->handleArraySize,
                                                     req->getTag(),
                                                     e);
        break ;

      case Message::ATTRIBUTE_OWNERSHIP_ACQUISITION:
        D.Out(pdTrace, "Receiving Message from Federate, "
              "type attributeOwnershipAcquisition.");
        D.Out(pdTrace, "Object %u ; Nb d'Attributs ", req->getObject(),
              req->handleArraySize);
        owm->attributeOwnershipAcquisition(req->getObject(),
                                           req->handleArray,
                                           req->handleArraySize,
                                           req->getTag(), e);
        break ;

      case Message::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:
      {
          D.Out(pdTrace, "Receiving Message from Federate, "
                "type attributeOwnershipRealeaseResponse.");
          D.Out(pdTrace, "Object %u nb Attribute %u ",
                req->getObject(), req->handleArraySize);

          AttributeHandleSet* theAttributes =
              owm->attributeOwnershipRealeaseResponse(req->getObject(),
                                                      req->handleArray,
                                                      req->handleArraySize,
                                                      e);
          if (e == e_NO_EXCEPTION) {
              rep.handleArraySize = theAttributes->size();

              for (unsigned int i=0 ; i<theAttributes->size(); i++) {
                  rep.handleArray[i] = theAttributes->getHandle(i);
              }
          }
      }
      break ;

      case Message::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
        D.Out(pdTrace, "Receiving Message from Federate, "
              "type cancelNegociatedAttributeOwnershipDivestiture.");
        D.Out(pdTrace, "Object %u nb Attribute %u ",
              req->getObject(), req->handleArraySize);
        owm->cancelnegotiatedAttributeOwnershipDivestiture(req->getObject(),
                                                           req->handleArray,
                                                           req->handleArraySize,
                                                           e);
        break ;


      case Message::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:
        D.Out(pdTrace, "Receiving Message from Federate, "
              "type cancelAttributeOwnershipAcquisition.");
        D.Out(pdTrace, "Object %u nb Attribute %u ",
              req->getObject(), req->handleArraySize);
        owm->cancelattributeOwnershipAcquisition(req->getObject(),
                                                 req->handleArray,
                                                 req->handleArraySize,
                                                 e);
        break ;

      case Message::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
        D.Out(pdTrace, "Receiving Message from Federate, "
              "type attributeOwnershipAcquisitionIfAvailable.");
        D.Out(pdTrace, "Object %u nb Attribute %u ",
              req->getObject(), req->handleArraySize);
        owm->attributeOwnershipAcquisitionIfAvailable(req->getObject(),
                                                      req->handleArray,
                                                      req->handleArraySize,
                                                      e);
        break ;

      case Message::QUERY_ATTRIBUTE_OWNERSHIP:
        D.Out(pdTrace, "Receiving Message from Federate, "
              "type queryAttributeOwnership.");
        D.Out(pdTrace, "Object %u Attribute %u ",
              req->getObject(), req->getAttribute());
        owm->queryAttributeOwnership(req->getObject(), req->getAttribute(), e);

        break ;

      case Message::IS_ATTRIBUTE_OWNED_BY_FEDERATE:
        D.Out(pdTrace, "Receiving Message from Federate, "
              "type isAttributeOwnedByFederate.");
        D.Out(pdTrace, "Object %u Attribute %u ",
              req->getObject(), req->getAttribute());
        rep.setTag(owm->attributeOwnedByFederate(req->getObject(),
                                                 req->getAttribute(), e));
        break ;

      case Message::QUERY_LBTS:
        D.Out(pdTrace, "Receiving Message from Federate, type RequestLBTS.");

        rep.setFederationTime(tm->requestLBTS());
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

        rep.setFederationTime(tm->requestFederateTime());
        break ;

      case Message::QUERY_MIN_NEXT_EVENT_TIME:
        D.Out(pdTrace,
              "Receiving Message from Federate, type QueryMinNextEventTime."); 

        rep.setFederationTime(tm->requestMinNextEventTime());    
        break ;

      case Message::MODIFY_LOOKAHEAD:
        D.Out(pdTrace, "Receiving Message from Federate, type SetLookAhead.");

        tm->setLookahead(req->getFederationTimeDelta(), e);
        break ;

      case Message::QUERY_LOOKAHEAD:
        D.Out(pdTrace,
              "Receiving Message from Federate, type RequestLookAhead.");

        rep.setFederationTimeDelta(tm->requestLookahead());
        break ;

      case Message::TIME_ADVANCE_REQUEST:
        D.Out(pdTrace,
              "Receiving Message from Federate, type TimeAdvanceRequest.");

        tm->timeAdvanceRequest(req->getFederationTime(), e);
        break ;

      case Message::TIME_ADVANCE_REQUEST_AVAILABLE:
        D.Out(pdTrace,
              "Receiving Message from Federate, type TimeAdvanceRequestAvailable.");

        tm->timeAdvanceRequestAvailable(req->getFederationTime(), e);
        break ;

      case Message::NEXT_EVENT_REQUEST:
        D.Out(pdTrace,
              "Receiving Message from Federate, type NestEventRequest.");

        tm->nextEventRequest(req->getFederationTime(), e);
        break ;

      case Message::NEXT_EVENT_REQUEST_AVAILABLE:
        D.Out(pdTrace,
              "Receiving Message from Federate, type NestEventRequestAvailable.");

        tm->nextEventRequestAvailable(req->getFederationTime(), e);
        break ;

      case Message::FLUSH_QUEUE_REQUEST:
        e = e_UnimplementedService ;
        break ;

      case Message::GET_OBJECT_CLASS_HANDLE:
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetObjectClassHandle.");

        rep.setObjectClass(om->getObjectClassHandle(req->getName()));
        break ;

      case Message::GET_OBJECT_CLASS_NAME:
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetObjectClassName.");

        try {
            rep.setName(om->getObjectClassName(req->getObjectClass()).c_str());
        }
        catch (ObjectClassNotDefined) {
            rep.setException(e_ObjectClassNotDefined);
        }
        catch (RTIinternalError) {
            rep.setException(e_RTIinternalError);
        }
        break ;

      case Message::GET_OBJECT_INSTANCE_HANDLE:
        D.Out(pdTrace,
              "Receiving Message from Federate, type getObjectInstanceHandle.");
        rep.setObject(om->getObjectInstanceHandle(req->getName()));
        break ;

      case Message::GET_OBJECT_INSTANCE_NAME:
        D.Out(pdTrace,
              "Receiving Message from Federate, type getObjectInstanceName.");

        rep.setName(om->getObjectInstanceName(req->getObject()));
        break ;

      case Message::GET_ATTRIBUTE_HANDLE:
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetAttributeHandle.");

        rep.setAttribute(om->getAttributeHandle(req->getName(),
                                                req->getObjectClass()));
        break ;

      case Message::GET_ATTRIBUTE_NAME:
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetAttributeName.");

        rep.setName(om->getAttributeName(req->getAttribute(),
                                         req->getObjectClass()));
        break ;

      case Message::GET_INTERACTION_CLASS_HANDLE:
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetInteractionHandle.");

        rep.setInteractionClass(om->getInteractionClassHandle(req->getName()));
        break ;

      case Message::GET_INTERACTION_CLASS_NAME:
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetInteractionName.");

        rep.setName(om->getInteractionClassName(req->getInteractionClass()));
        break ;

      case Message::GET_PARAMETER_HANDLE:
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetParameterHandle.");

        rep.setParameter(om->getParameterHandle(req->getName(),
                                                req->getInteractionClass()));
        break ;

      case Message::GET_PARAMETER_NAME:
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetParameterName.");

        rep.setName(om->getParameterName(req->getParameter(),
                                         req->getInteractionClass()));
        break ;

      case Message::GET_SPACE_HANDLE:
        D.Out(pdTrace, "Receiving Message from Federate, type GetSpaceHandle.");
        rep.setSpace(ddm->getRoutingSpaceHandle(req->getName()));
        break ;

      case Message::GET_SPACE_NAME:
        D.Out(pdTrace, "Receiving Message from Federate, type GetSpaceName.");
        rep.setName(ddm->getRoutingSpaceName(req->getSpace()).c_str());
        break ;

      case Message::GET_DIMENSION_HANDLE:
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetDimensionHandle");
        rep.setDimension(ddm->getDimensionHandle(string(req->getName()),
                                                 req->getSpace()));
        break ;

      case Message::GET_DIMENSION_NAME:
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetDimensionName");
        rep.setName(ddm->getDimensionName(req->getDimension(),
                                          req->getSpace()).c_str());
        break ;

      case Message::GET_ATTRIBUTE_SPACE_HANDLE:
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetAttributeSpaceHandle");
        rep.setSpace(ddm->getAttributeSpace(req->getAttribute(),
                                            req->getObjectClass()));
        break ;

      case Message::GET_INTERACTION_SPACE_HANDLE:
        D.Out(pdTrace,
              "Receiving Message from Federate: GetInteractionSpaceHandle");
        rep.setSpace(ddm->getInteractionSpace(req->getInteractionClass()));
        break ;

      case Message::GET_OBJECT_CLASS:
        D[pdTrace] << "Message from Federate: getObjectClass" << endl ;
	rep.setObjectClass(om->getObjectClass(req->getObject()));
	break ;	

      case Message::DDM_CREATE_REGION:
        D[pdTrace] << "Receiving Message from Federate: CreateRegion" << endl ;
        rep.setRegion(ddm->createRegion(req->getSpace(), req->getNumber(), e));
	rep.setNumber(rootObject->getRoutingSpace(req->getSpace()).size());
        break ;

      case Message::DDM_MODIFY_REGION:
	D[pdTrace] << "Receiving Message from Federate: Modify Region" << endl ;
	ddm->modifyRegion(req->getRegion(), req->getExtents(), e);
	break ;

      case Message::DDM_DELETE_REGION:
        D[pdTrace] << "Receiving Message from Federate: DeleteRegion" << endl ;
        ddm->deleteRegion(req->getRegion(), e);
        break ;

      case Message::DDM_ASSOCIATE_REGION:
	D[pdTrace] << "Receiving Message from Federate: Associate Region"
		   << endl ;
	ddm->associateRegion(req->getObject(), req->getRegion(),
			     req->handleArray, req->handleArraySize, e);
	break ;

      case Message::DDM_REGISTER_OBJECT:
	D[pdTrace] << "Receiving Message from Federate: Register with Region"
		   << endl ;
	rep.setObject(ddm->registerObject(req->getObjectClass(),
					  req->getName(),
					  req->handleArray,
					  req->handleArraySize,
					  req->getRegions(),
					  e));
	break ;

      case Message::DDM_UNASSOCIATE_REGION:
	D[pdTrace] << "Receiving Message from Federate: Unassociate Region"
		   << endl ;
	ddm->unassociateRegion(req->getObject(), req->getRegion(), e);
	break ;

      case Message::DDM_SUBSCRIBE_ATTRIBUTES:
	D[pdTrace] << "Receiving Message from Federate: Subscribe Attributes"
		   << endl ;
	ddm->subscribe(req->getObjectClass(), req->getRegion(),
		       req->handleArray, req->handleArraySize, e);
	break ;

      case Message::DDM_UNSUBSCRIBE_ATTRIBUTES:
	D[pdTrace] << "Receiving Message from Federate: Unsubscribe class "
		   << req->getObjectClass() << endl ;
	ddm->unsubscribeAttributes(req->getObjectClass(), req->getRegion(), e);
	break ;

      case Message::DDM_SUBSCRIBE_INTERACTION:
	D[pdTrace] << "Receiving Message from Federate: Subscribe Interaction"
		   << endl ;
	ddm->subscribeInteraction(req->getInteractionClass(),
				  req->getRegion(), e);
	break ;

      case Message::DDM_UNSUBSCRIBE_INTERACTION:
	D[pdTrace] << "Receiving Message from Federate: Unsubscribe Interaction"
		   << endl ;
	ddm->unsubscribeInteraction(req->getInteractionClass(),
				    req->getRegion(), e);
	break ;
	
      case Message::ENABLE_TIME_REGULATION:
      case Message::DISABLE_TIME_REGULATION:
        D.Out(pdTrace,
              "Receiving Message from Federate, type SetTimeRegulating.");

        tm->setTimeRegulating(req->getBoolean(), e);
        break ;

      case Message::ENABLE_TIME_CONSTRAINED:
      case Message::DISABLE_TIME_CONSTRAINED:
        D.Out(pdTrace,
              "Receiving Message from Federate, type SetTimeConstrained.");

        tm->setTimeConstrained(req->getBoolean(), e);
        break ;

      case Message::TICK_REQUEST:
        tm->_tick_multiple = req->getBoolean();

	if (req->getMinTickTime() > 0.0)
	{
            tm->_tick_timeout = req->getMinTickTime();
            tm->_tick_stop_time = currentTickTime() + req->getMaxTickTime();

            tm->_blocking_tick = true ;
        }
        else
            tm->_blocking_tick = false ;

        processOngoingTick();
        break ;

      default:
        D.Out(pdExcept,
              "Receiving Message from Federate, Unknown Type %d.", req->type);
        throw RTIinternalError("");
    }
    stat.federateService(req->type);
    G.Out(pdGendoc,"exit  chooseFederateProcessing");
}

// ----------------------------------------------------------------------------
//! RTIA processes the TICK_REQUEST
void
RTIA::processOngoingTick()
{
    TypeException exc = e_NO_EXCEPTION;
    bool pending;

    do {
        // send a single callback to federate (if any)
	pending = tm->tick(exc);

	// processing a callback may have reset tm->_blocking_tick

        if (!tm->_tick_multiple)
    	    break;

	if (currentTickTime() > tm->_tick_stop_time)
	{
            tm->_blocking_tick = false;
            break;
	}
    }
    while (pending);

    if (!tm->_blocking_tick)
    {
        Message msg_ack;
        if ( exc != e_RTIinternalError )
    	    msg_ack.setException(exc);
        // terminate __tick() call in the federate
        msg_ack.type = Message::TICK_REQUEST;
        msg_ack.setBoolean(pending);
        comm->requestFederateService(&msg_ack);
    }
}

// ----------------------------------------------------------------------------
//! RTIA processes a service request coming from federate.
void
RTIA::processFederateRequest(Message *req)
{
    Message rep ;

    G.Out(pdGendoc,"enter RTIA::processFederateRequest");

    rep.type = req->type ;

    try {
        TypeException exc ;
        chooseFederateProcessing(req, rep, exc);
        if ( exc != e_RTIinternalError )
            rep.setException(exc);
    }
    catch (ArrayIndexOutOfBounds &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_ArrayIndexOutOfBounds);
    }
    catch (AttributeAlreadyOwned &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_AttributeAlreadyOwned);
    }
    catch (AttributeAlreadyBeingDivested &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_AttributeAlreadyBeingDivested);
    }
    catch (AttributeAlreadyBeingAcquired &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_AttributeAlreadyBeingAcquired);
    }
    catch (AttributeAcquisitionWasNotRequested &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_AttributeAcquisitionWasNotRequested);
    }
    catch (AttributeDivestitureWasNotRequested &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_AttributeDivestitureWasNotRequested);
    }
    catch (AttributeNotDefined &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_AttributeNotDefined);
    }
    catch (AttributeNotKnown &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_AttributeNotKnown);
    }
    catch (AttributeNotOwned &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_AttributeNotOwned);
    }
    catch (AttributeNotPublished &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_AttributeNotPublished);
    }
    catch (AttributeNotSubscribed &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_AttributeNotSubscribed);
    }
    catch (ConcurrentAccessAttempted &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_ConcurrentAccessAttempted);
    }
    catch (CouldNotDiscover &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_CouldNotDiscover);
    }
    catch (CouldNotOpenRID &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_CouldNotOpenRID);
    }
    catch (CouldNotOpenFED &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_CouldNotOpenFED,e._reason);
    }
    catch (CouldNotRestore &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_CouldNotRestore);
    }
    catch (DeletePrivilegeNotHeld &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_DeletePrivilegeNotHeld);
    }
    catch (ErrorReadingRID &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_ErrorReadingRID,e._reason);
    }
    catch (ErrorReadingFED &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_ErrorReadingFED,e._reason);
    }
    catch (EventNotKnown &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_EventNotKnown);
    }
    catch (FederateAlreadyPaused &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_FederateAlreadyPaused);
    }
    catch (FederateAlreadyExecutionMember &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_FederateAlreadyExecutionMember,e._reason);
    }
    catch (FederateDoesNotExist &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_FederateDoesNotExist);
    }
    catch (FederateInternalError &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_FederateInternalError);
    }
    catch (FederateNameAlreadyInUse &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_FederateNameAlreadyInUse);
    }
    catch (FederateNotExecutionMember &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_FederateNotExecutionMember);
    }
    catch (FederateNotPaused &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_FederateNotPaused);
    }
    catch (FederateNotPublishing &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_FederateNotPublishing);
    }
    catch (FederateNotSubscribing &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_FederateNotSubscribing);
    }
    catch (FederateOwnsAttributes &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_FederateOwnsAttributes);
    }
    catch (FederatesCurrentlyJoined &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_FederatesCurrentlyJoined);
    }
    catch (FederateWasNotAskedToReleaseAttribute &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_FederateWasNotAskedToReleaseAttribute);
    }
    catch (FederationAlreadyPaused &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_FederationAlreadyPaused);
    }
    catch (FederationExecutionAlreadyExists &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_FederationExecutionAlreadyExists,e._reason);
    }
    catch (FederationExecutionDoesNotExist &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_FederationExecutionDoesNotExist);
    }
    catch (FederationNotPaused &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_FederationNotPaused);
    }
    catch (FederationTimeAlreadyPassed &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_FederationTimeAlreadyPassed);
    }
    catch (IDsupplyExhausted &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_IDsupplyExhausted);
    }
    catch (InteractionClassNotDefined &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_InteractionClassNotDefined);
    }
    catch (InteractionClassNotKnown &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_InteractionClassNotKnown);
    }
    catch (InteractionClassNotPublished &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_InteractionClassNotPublished);
    }
    catch (InteractionParameterNotDefined &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_InteractionParameterNotDefined);
    }
    catch (InteractionParameterNotKnown &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_InteractionParameterNotKnown);
    }
    catch (InvalidDivestitureCondition &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_InvalidDivestitureCondition);
    }
    catch (InvalidExtents &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_InvalidExtents);
    }
    catch (InvalidFederationTime &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_InvalidFederationTime);
    }
    catch (InvalidFederationTimeDelta &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_InvalidFederationTimeDelta);
    }
    catch (InvalidObjectHandle &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_InvalidObjectHandle);
    }
    catch (InvalidOrderType &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_InvalidOrderType);
    }
    catch (InvalidResignAction &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_InvalidResignAction);
    }
    catch (InvalidRetractionHandle &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_InvalidRetractionHandle);
    }
    catch (InvalidRoutingSpace &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_InvalidRoutingSpace);
    }
    catch (InvalidTransportType &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_InvalidTransportType);
    }
    catch (MemoryExhausted &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_MemoryExhausted);
    }
    catch (NameNotFound &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_NameNotFound);
    }
    catch (NoPauseRequested &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_NoPauseRequested);
    }
    catch (NoResumeRequested &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_NoResumeRequested);
    }
    catch (ObjectClassNotDefined &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_ObjectClassNotDefined);
    }
    catch (ObjectClassNotKnown &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_ObjectClassNotKnown);
    }
    catch (ObjectClassNotPublished &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_ObjectClassNotPublished);
    }
    catch (ObjectClassNotSubscribed &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_ObjectClassNotSubscribed);
    }
    catch (ObjectNotKnown &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_ObjectNotKnown);
    }
    catch (ObjectAlreadyRegistered &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_ObjectAlreadyRegistered);
    }
    catch (RegionNotKnown &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_RegionNotKnown);
    }
    catch (RestoreInProgress &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_RestoreInProgress);
    }
    catch (RestoreNotRequested &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_RestoreNotRequested);
    }
    catch (RTIinternalError &e) {
        cout << "RTIA sends InternalError to Fed., " ;
        if (e._reason != NULL)
            cout << "reason : " << e._reason << endl ;
        else
            cout << "no reason given." << endl ;
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_RTIinternalError);
    }
    catch (SpaceNotDefined &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_SpaceNotDefined);
    }
    catch (SaveInProgress &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_SaveInProgress);
    }
    catch (SaveNotInitiated &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_SaveNotInitiated);
    }
    catch (SpecifiedSaveLabelDoesNotExist &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_SpecifiedSaveLabelDoesNotExist);
    }
    catch (TimeAdvanceAlreadyInProgress &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_TimeAdvanceAlreadyInProgress);
    }
    catch (TimeAdvanceWasNotInProgress &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_TimeAdvanceWasNotInProgress);
    }
    catch (TooManyIDsRequested &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_TooManyIDsRequested);
    }
    catch (UnableToPerformSave &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_UnableToPerformSave);
    }
    catch (UnimplementedService &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_UnimplementedService);
    }
    catch (UnknownLabel &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_UnknownLabel);
    }
    catch (ValueCountExceeded &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_ValueCountExceeded);
    }
    catch (ValueLengthExceeded &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.setException(e_ValueLengthExceeded);
    }

    // Default Handler
    catch (Exception &e) {
        D.Out(pdExcept, "Unknown Exception : %s.", e._name);
        rep.setException(e_RTIinternalError);
    }

    delete req ;;

    if (rep.type != Message::TICK_REQUEST) {
       // generic federate service acknowledgment
       // the TICK_REQUEST confirmation is generated in processOngoingTick()
       comm->sendUN(&rep);
       D.Out(pdDebug, "Reply send to Unix socket.");
    }

}

}} // namespace certi/rtia

// $Id: RTIA_federate.cc,v 3.72 2008/04/23 12:55:06 erk Exp $
