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
// $Id: RTIA_federate.cc,v 3.18 2003/06/27 17:26:28 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "RTIA.hh"

#include "FedParser.hh"
#include "XmlParser.hh"

namespace certi {
namespace rtia {

static pdCDebug D("RTIA", "(RTIA fed) ");

// ----------------------------------------------------------------------------
//! Verify that federate is not in saving or restoring state.
void
RTIA::saveAndRestoreStatus(Message::Type type)
    throw (SaveInProgress, RestoreInProgress)
{
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
      default:
        fm->checkFederationSaving();
        fm->checkFederationRestoring();
    }
}

// ----------------------------------------------------------------------------
//! Choose federate processing.
void
RTIA::chooseFederateProcessing(Message *req, Message &rep, TypeException &e)
{
    // Verify not in saving or restoring state.
    saveAndRestoreStatus(req->type);

    e = e_NO_EXCEPTION ;

    switch(req->type) {

        // 2.1
      case Message::CREATE_FEDERATION_EXECUTION:
        D.Out(pdTrace,
              "Receiving Message from Federate, type CreateFederation.");

        fm->createFederationExecution(req->getFederationName(), e);
        D.Out(pdTrace, "Receiving Message from Federate, "
              "type CreateFederation done.");

        break ;

        // 2.2
      case Message::DESTROY_FEDERATION_EXECUTION:
        D.Out(pdTrace,
              "Receiving Message from Federate, type DestroyFederation.");

        fm->destroyFederationExecution(req->getFederationName(), e);
        break ;

        // 2.3
      case Message::JOIN_FEDERATION_EXECUTION: {
          D.Out(pdTrace,
                "Receiving Message from Federate, type JoinFederation.");

          rep.setFederate(fm->joinFederationExecution(req->getFederateName(),
                                                      req->getFederationName(),
                                                      e));

          D.Out(pdTrace, "Trying to use a .fed file");
          string filename = string(req->getFederationName()) + ".fed" ;
          ifstream *fdd = new ifstream(filename.c_str());
          if (fdd->is_open()) {
              fedparser::FedParser *parser = new fedparser::FedParser(rootObject);
              if (parser == 0)
                  throw MemoryExhausted("No memory left to read FED file.");
              parser->readFile(filename.c_str());
              delete parser ;
          }
          else {
              if (XmlParser::exists()) {
                  D.Out(pdTrace, "Trying to use a .xml file");
                  filename = string(req->getFederationName()) + ".xml" ;
                  fdd = new ifstream(filename.c_str());
                  if (fdd->is_open()) {
                      XmlParser *parser = new XmlParser(rootObject);
                      parser->parse(filename);
                      delete fdd ;
                      delete parser ;
                  }
              }
          }
          break ;
      }
        // 2.4
      case Message::RESIGN_FEDERATION_EXECUTION:
        D.Out(pdTrace,
              "Receiving Message from Federate, type ResignFederation.");

        fm->resignFederationExecution(req->getResignAction(), e);
        break ;

        // 2.5
      case Message::REGISTER_FEDERATION_SYNCHRONIZATION_POINT:
        D.Out(pdTrace, "Receiving Message from Federate, type RequestPause.");

        fm->registerSynchronization(req->getLabel(), req->getTag(), e);
        break ;

        // 2.7
      case Message::SYNCHRONIZATION_POINT_ACHIEVED:
        D.Out(pdTrace, "Receiving Message from Federate, type PauseAchieved.");

        fm->unregisterSynchronization(req->getLabel(), e);
        break ;


        // 2.11
      case Message::REQUEST_FEDERATION_SAVE:
        D.Out(pdTrace, "Receiving Message from Federate, type"
              " RequestFederationSave.");

        fm->requestFederationSave(req->getLabel(), req->getFederationTime(), e);
        break ;

        // 2.13
      case Message::FEDERATE_SAVE_BEGUN:
        D.Out(pdTrace, "Receiving Message from Federate, type"
              " FederateSaveBegun.");

        fm->federateSaveBegun(e);
        break ;

        // 2.14
      case Message::FEDERATE_SAVE_COMPLETE:
      case Message::FEDERATE_SAVE_NOT_COMPLETE: {
          D.Out(pdTrace, "Receiving Message from Federate, type"
                " FederateSave(Not)Complete.");

          bool result = req->type == Message::FEDERATE_SAVE_COMPLETE ;
          fm->federateSaveStatus(result, e);
      }
        break ;

        // 2.15
      case Message::REQUEST_FEDERATION_RESTORE:
        D.Out(pdTrace, "Receiving Message from Federate, type"
              " RequestFederationRestore.");

        fm->requestFederationRestore(req->getLabel(), e);
        break ;

        // 2.17
      case Message::FEDERATE_RESTORE_COMPLETE:
      case Message::FEDERATE_RESTORE_NOT_COMPLETE: {
          D.Out(pdTrace, "Receiving Message from Federate, type"
                " FederateRestore(Not)Complete.");

          bool result = req->type == Message::FEDERATE_SAVE_COMPLETE ;
          fm->federateRestoreStatus(result, e);
      }
        break ;

        // 3.1(1)
      case Message::PUBLISH_OBJECT_CLASS:
        D.Out(pdTrace,
              "Receiving Message from Federate, type PublishObjectClass.");

        dm->publishObjectClass(req->getObjectClass(),
                               req->handleArray,
                               req->handleArraySize,
                               e);
        break ;

        // 3.1(2)
      case Message::UNPUBLISH_OBJECT_CLASS:
        D.Out(pdTrace,
              "Receiving Message from Federate, type UnpublishObjectClass.");

        dm->unpublishObjectClass(req->getObjectClass(), e);
        break ;

        // 3.2(1)
      case Message::PUBLISH_INTERACTION_CLASS:
        D.Out(pdTrace,
              "Receiving Message from Federate, type PublishInteractionClass.");

        dm->publishInteractionClass(req->getInteractionClass(), e);
        break ;

        // 3.2(2)
      case Message::UNPUBLISH_INTERACTION_CLASS:
        D.Out(pdTrace,
              "Receiving Message from Federate, "
              "type UnpublishInteractionClass");

        dm->unpublishInteractionClass(req->getInteractionClass(), e);
        break ;

        // 3.3.(1)
      case Message::SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES:
        D.Out(pdTrace,
              "Receiving Message from Federate, type SubscribeObjectClass.");
        dm->subscribeObjectClassAttribute(req->getObjectClass(),
                                          req->handleArray,
                                          req->handleArraySize,
                                          e);
        break ;

        // 3.3(2)
      case Message::UNSUBSCRIBE_OBJECT_CLASS:
        D.Out(pdTrace,
              "Receiving Message from Federate, type UnsubscribeObjectClass.");

        // BUG: Why attributes are not transmitted ?
        dm->unsubscribeObjectClassAttribute(req->getObjectClass(), e);
        break ;

        // 3.4(1)
      case Message::SUBSCRIBE_INTERACTION_CLASS:
        D.Out(pdTrace,
              "Receiving Message from Federate, type SubscribeInteraction.");

        dm->subscribeInteractionClass(req->getInteractionClass(), e);
        break ;

        // 3.4(2)
      case Message::UNSUBSCRIBE_INTERACTION_CLASS:
        D.Out(pdTrace,
              "Receiving Message from Federate, type UnsubscribeInteraction.");

        dm->unsubscribeInteractionClass(req->getInteractionClass(), e);
        break ;

        // 4.1
        // case Message::REQUEST_ID:

        // D.Out(pdTrace, "Receiving Message from Federate, type RequestIDs.");

        // ObjectHandle first ;
        // ObjectHandle last ;
        // om->requestID(req->idCount, first, last, e);

        // rep.firstId = first ;
        // rep.lastId = last ;
        // break ;

        // 4.2
      case Message::REGISTER_OBJECT_INSTANCE: {
          FederationTime date = tm->requestFederateTime();
          FederationTime heure = date + tm->requestLookahead();

          D.Out(pdTrace, "Receiving Message from Federate, type RegisterObject.");

          rep.setObject(om->registerObject(req->getObjectClass(),
                                           req->getName(),
                                           date, heure, e));
          break ;
      }

        // 4.3
      case Message::UPDATE_ATTRIBUTE_VALUES: {
          D.Out(pdTrace,
                "Receiving Message from Federate, type UpdateAttribValues.");

          AttributeValue *ValueArray = req->getValueArray();

          try {
              rep.setEventRetraction(
                  om->updateAttributeValues(req->getObject(),
                                            req->handleArray,
                                            ValueArray,
                                            req->handleArraySize,
                                            req->getFederationTime(),
                                            req->getTag(),
                                            e));
              free(ValueArray);
          } catch (Exception *e) {
              free(ValueArray);
              throw e ;
          }
      }
        break ;

        // 4.6
      case Message::SEND_INTERACTION: {
          D.Out(pdTrace,
                "Receiving Message from Federate, type SendInteraction.");

          ParameterValue *ValueArray = (ParameterValue *) req->getValueArray();

          try {
              rep.setEventRetraction(
                  om->sendInteraction(req->getInteractionClass(),
                                      req->handleArray,
                                      ValueArray,
                                      req->handleArraySize,
                                      req->getFederationTime(),
                                      req->getTag(),
                                      e));
              free(ValueArray);
          } catch (Exception *e) {
              free(ValueArray);
              throw e ;
          }
      }
        break ;

        // 4.8
      case Message::DELETE_OBJECT_INSTANCE:
        D.Out(pdTrace, "Receiving Message from Federate, type DeleteObject.");
        rep.setEventRetraction(om->deleteObject(req->getObject(), req->getTag(),
                                                e));
        break ;

        // 4.10
      case Message::CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE:
        D.Out(pdTrace,
              "Receiving Message from Federate, type ChangeAttribTransport.");

        om->changeAttributeTransportType(req->getObject(),
                                         req->handleArray,
                                         req->handleArraySize,
                                         req->getTransportType(),
                                         e);
        break ;

        // 4.11
      case Message::CHANGE_ATTRIBUTE_ORDER_TYPE:
        D.Out(pdTrace,
              "Receiving Message from Federate, type ChangeAttribOrder.");

        om->changeAttributeOrderType(req->getObject(),
                                     req->handleArray,
                                     req->handleArraySize,
                                     req->getOrderType(),
                                     e);
        break ;

        // 4.12
      case Message::CHANGE_INTERACTION_TRANSPORTATION_TYPE:
        D.Out(pdTrace,
              "Receiving Message from Federate, type ChangeInterTransport.");

        om->changeInteractionTransportType(req->getInteractionClass(),
                                           req->getTransportType(),
                                           e);
        break ;

        // 4.13
      case Message::CHANGE_INTERACTION_ORDER_TYPE:
        D.Out(pdTrace,
              "Receiving Message from Federate, type ChangeInterOrder.");

        om->changeInteractionOrderType(req->getInteractionClass(),
                                       req->getOrderType(), e);
        break ;

        // 4.14(1)
      case Message::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE:
        e = e_UnimplementedService ;
        break ;

        // 4.14(2)
      case Message::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE:
        e = e_UnimplementedService ;
        break ;

        // 4.16
      case Message::RETRACT:
        e = e_UnimplementedService ;
        break ;

        // 5.1
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


        // 5.2
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


        // 5.6
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

        // 5.8
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

      // 5.9
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


        // 5.10
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
        // 5.12
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

        // 5.14
      case Message::QUERY_ATTRIBUTE_OWNERSHIP:
        D.Out(pdTrace, "Receiving Message from Federate, "
              "type queryAttributeOwnership.");
        D.Out(pdTrace, "Object %u Attribute %u ",
              req->getObject(), req->getAttribute());
        owm->queryAttributeOwnership(req->getObject(), req->getAttribute(), e);

        break ;

        // 5.16
      case Message::IS_ATTRIBUTE_OWNED_BY_FEDERATE:
        D.Out(pdTrace, "Receiving Message from Federate, "
              "type isAttributeOwnedByFederate.");
        D.Out(pdTrace, "Object %u Attribute %u ",
              req->getObject(), req->getAttribute());
        rep.setTag(owm->attributeOwnedByFederate(req->getObject(),
                                                 req->getAttribute(), e));
        break ;

        // 6.1
        // case Message::REQUEST_FEDERATION_TIME:
        // D.Out(pdTrace,
        // "Receiving Message from Federate, type RequestFederationTime.");

        // rep.date = tm->requestFederationTime();
        // break ;

        // 6.2
      case Message::QUERY_LBTS:
        D.Out(pdTrace, "Receiving Message from Federate, type RequestLBTS.");

        rep.setFederationTime(tm->requestLBTS());
        break ;

        // 6.3
      case Message::QUERY_FEDERATE_TIME:
        D.Out(pdTrace,
              "Receiving Message from Federate, type RequestFederateTime.");

        rep.setFederationTime(tm->requestFederateTime());
        break ;

        // 6.4
      case Message::QUERY_MIN_NEXT_EVENT_TIME:
        e = e_UnimplementedService ;
        break ;

        // 6.5
      case Message::MODIFY_LOOKAHEAD:
        D.Out(pdTrace, "Receiving Message from Federate, type SetLookAhead.");

        tm->setLookahead(req->getFederationTimeDelta(), e);
        break ;

        // 6.6
      case Message::QUERY_LOOKAHEAD:
        D.Out(pdTrace,
              "Receiving Message from Federate, type RequestLookAhead.");

        rep.setFederationTimeDelta(tm->requestLookahead());
        break ;

        // 6.7
      case Message::TIME_ADVANCE_REQUEST:
        D.Out(pdTrace,
              "Receiving Message from Federate, type TimeAdvanceRequest.");

        tm->timeAdvanceRequest(req->getFederationTime(), e);
        break ;

        // 6.8
      case Message::NEXT_EVENT_REQUEST:
        D.Out(pdTrace,
              "Receiving Message from Federate, type NestEventRequest.");

        tm->nextEventRequest(req->getFederationTime(), e);
        break ;

        // 6.9
      case Message::FLUSH_QUEUE_REQUEST:
        e = e_UnimplementedService ;
        break ;

        // 7.5
      case Message::MODIFY_REGION:
        // unimplemented
        break ;

        // 8.1
      case Message::GET_OBJECT_CLASS_HANDLE:
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetObjectClassHandle.");

        rep.setObjectClass(om->getObjectClassHandle(req->getName()));
        break ;

        // 8.2
      case Message::GET_OBJECT_CLASS_NAME:
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetObjectClassName.");

        try {
            rep.setName(om->getObjectClassName(req->getObjectClass()));
        }
        catch (ObjectClassNotDefined) {
            rep.setException(e_ObjectClassNotDefined);
        }
        catch (RTIinternalError) {
            rep.setException(e_RTIinternalError);
        }
        break ;

      case Message::GET_OBJECT_INSTANCE_NAME:
        D.Out(pdTrace,
              "Receiving Message from Federate, type getObjectInstanceName.");

        rep.setName(om->getObjectInstanceName(req->getObject()));
        break ;

        // 8.3
      case Message::GET_ATTRIBUTE_HANDLE:
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetAttributeHandle.");

        rep.setAttribute(om->getAttributeHandle(req->getName(),
                                                req->getObjectClass()));
        break ;

        // 8.4
      case Message::GET_ATTRIBUTE_NAME:
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetAttributeName.");

        rep.setName(om->getAttributeName(req->getAttribute(),
                                         req->getObjectClass()));
        break ;

        // 8.5
      case Message::GET_INTERACTION_CLASS_HANDLE:
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetInteractionHandle.");

        rep.setInteractionClass(om->getInteractionClassHandle(req->getName()));
        break ;

        // 8.6
      case Message::GET_INTERACTION_CLASS_NAME:
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetInteractionName.");

        rep.setName(om->getInteractionClassName(req->getInteractionClass()));
        break ;

        // 8.7
      case Message::GET_PARAMETER_HANDLE:
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetParameterHandle.");

        rep.setParameter(om->getParameterHandle(req->getName(),
                                                req->getInteractionClass()));
        break ;

        // 8.8
      case Message::GET_PARAMETER_NAME:
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetParameterName.");

        rep.setName(om->getParameterName(req->getParameter(),
                                         req->getInteractionClass()));
        break ;

        // 8.9
      case Message::GET_SPACE_HANDLE:
        D.Out(pdTrace, "Receiving Message from Federate, type GetSpaceHandle.");
        rep.setSpace(ddm->getRoutingSpaceHandle(req->getName()));
        break ;

        // 8.10
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

      case Message::CREATE_REGION:
        D[pdTrace] << "Receiving Message from Federate: CreateRegion" << endl ;
        rep.setRegion(ddm->createRegion(req->getSpace(), req->getNumber(),
                                        e));
        break ;

      case Message::DELETE_REGION:
        D[pdTrace] << "Receiving Message from Federate: DeleteRegion" << endl ;
        ddm->deleteRegion(req->getRegion(), e);
        break ;

        // 8.11
        // case Message::GET_FEDERATE_HANDLE:
        // // unimplemented
        // break ;

        // // 8.12
        // case Message::GET_FEDERATE_NAME:
        // // unimplemented
        // break ;

        // 8.13
      case Message::ENABLE_TIME_REGULATION:
      case Message::DISABLE_TIME_REGULATION:
        D.Out(pdTrace,
              "Receiving Message from Federate, type SetTimeRegulating.");

        tm->setTimeRegulating(req->getBoolean(), e);
        break ;

        // 8.14
      case Message::ENABLE_TIME_CONSTRAINED:
      case Message::DISABLE_TIME_CONSTRAINED:
        D.Out(pdTrace,
              "Receiving Message from Federate, type SetTimeConstrained.");

        tm->setTimeConstrained(req->getBoolean(), e);
        break ;

        // 8.15
      case Message::TICK_REQUEST:
        D.Out(pdDebug, "Receiving Message from Federate, type TickRequest.");

        rep.setBoolean(tm->tick(e));
        break ;

      default:
        D.Out(pdExcept,
              "Receiving Message from Federate, Unknown Type %d.", req->type);
        throw RTIinternalError();
    }

    stat.federateService(req->type);
}

// ----------------------------------------------------------------------------
//! RTIA processes a service request coming from federate.
void
RTIA::processFederateRequest(Message *req)
{
    Message rep ;

    //rep.setException(e_NO_EXCEPTION);
    rep.type = req->type ;

    try {
        TypeException e ;
        chooseFederateProcessing(req, rep, e);
        rep.setException(e);
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
        rep.setException(e_ErrorReadingRID);
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
        rep.setException(e_FederateAlreadyExecutionMember);
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
        rep.setException(e_FederationExecutionAlreadyExists);
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

    delete req ;

    comm->sendUN(&rep);
    D.Out(pdDebug, "Reply send to Unix socket.");
}

}} // namespace certi/rtia

// $Id: RTIA_federate.cc,v 3.18 2003/06/27 17:26:28 breholee Exp $
