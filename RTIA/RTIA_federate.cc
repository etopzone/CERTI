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
// $Id: RTIA_federate.cc,v 3.8 2003/03/21 15:06:46 breholee Exp $
// ----------------------------------------------------------------------------

#include "RTIA.hh"

namespace certi {
namespace rtia {

static pdCDebug D("RTIA", "(RTIA fed) ");

// ----------------------------------------------------------------------------
//! Choose federate processing.
void
RTIA::chooseFederateProcessing(Message *req, Message &rep, TypeException &e)
{
    e = e_NO_EXCEPTION ;

    switch(req->type) {

        // 2.1
    case CREATE_FEDERATION_EXECUTION:
        nb_requetes[0]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type CreateFederation.");

        fm->createFederationExecution(req->getFederationName(), e);
        D.Out(pdTrace, "Receiving Message from Federate, "
              "type CreateFederation done.");

        break ;

        // 2.2
    case DESTROY_FEDERATION_EXECUTION:
        nb_requetes[1]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type DestroyFederation.");

        fm->destroyFederationExecution(req->getFederationName(), e);
        break ;

        // 2.3
    case JOIN_FEDERATION_EXECUTION: {
        nb_requetes[2]++ ;

        D.Out(pdTrace, "Receiving Message from Federate, type JoinFederation.");

        rep.federate =
            fm->joinFederationExecution(req->getFederateName(),
                                        req->getFederationName(), e);

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
    case RESIGN_FEDERATION_EXECUTION:
        nb_requetes[3]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type ResignFederation.");

        fm->resignFederationExecution(req->resignAction, e);
        break ;

        // 2.5
    case REGISTER_FEDERATION_SYNCHRONIZATION_POINT:
        nb_requetes[4]++ ;
        D.Out(pdTrace, "Receiving Message from Federate, type RequestPause.");

        fm->registerSynchronization(req->getLabel(), req->getTag(), e);
        break ;

        // 2.7
    case SYNCHRONIZATION_POINT_ACHIEVED:
        nb_requetes[5]++ ;
        D.Out(pdTrace, "Receiving Message from Federate, type PauseAchieved.");

        fm->unregisterSynchronization(req->getLabel(), e);
        break ;

        // 2.11
    case REQUEST_FEDERATION_SAVE:
        // unimplemented
        break ;

        // 2.13
    case FEDERATE_SAVE_BEGUN:
        // unimplemented
        break ;

        // 2.14(1)
    case FEDERATE_SAVE_ACHIEVED:
        // unimplemented
        break ;

        // 2.14(2)
    case FEDERATE_SAVE_NOT_ACHIEVED:
        // unimplemented
        break ;

        // 2.15
    case REQUEST_RESTORE:
        // unimplemented
        break ;

        // 2.17(1)
    case RESTORE_ACHIEVED:
        // unimplemented
        break ;

        // 2.17(2)
    case RESTORE_NOT_ACHIEVED:
        // unimplemented
        break ;

        // 3.1(1)
    case PUBLISH_OBJECT_CLASS:
        nb_requetes[8]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type PublishObjectClass.");

        dm->publishObjectClass(req->objectClass,
                               req->handleArray,
                               req->handleArraySize,
                               e);
        break ;

        // 3.1(2)
    case UNPUBLISH_OBJECT_CLASS:
        nb_requetes[9]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type UnpublishObjectClass.");

        dm->unpublishObjectClass(req->objectClass, e);
        break ;

        // 3.2(1)
    case PUBLISH_INTERACTION_CLASS:
        nb_requetes[10]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type PublishInteractionClass.");

        dm->publishInteractionClass(req->interactionClass, e);
        break ;

        // 3.2(2)
    case UNPUBLISH_INTERACTION_CLASS:
        nb_requetes[11]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, "
              "type UnpublishInteractionClass");

        dm->unpublishInteractionClass(req->interactionClass, e);
        break ;

        // 3.3.(1)
    case SUBSCRIBE_OBJECT_CLASS_ATTRIBUTE:
        nb_requetes[12]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type SubscribeObjectClass.");
        dm->subscribeObjectClassAttribute(req->objectClass,
                                          req->handleArray,
                                          req->handleArraySize,
                                          e);
        break ;

        // 3.3(2)
    case UNSUBSCRIBE_OBJECT_CLASS_ATTRIBUTE:
        nb_requetes[13]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type UnsubscribeObjectClass.");

        // BUG: Why attributes are not transmitted ?
        dm->unsubscribeObjectClassAttribute(req->objectClass, e);
        break ;

        // 3.4(1)
    case SUBSCRIBE_INTERACTION_CLASS:
        nb_requetes[14]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type SubscribeInteraction.");

        dm->subscribeInteractionClass(req->interactionClass, e);
        break ;

        // 3.4(2)
    case UNSUBSCRIBE_INTERACTION_CLASS:
        nb_requetes[15]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type UnsubscribeInteraction.");

        dm->unsubscribeInteractionClass(req->interactionClass, e);
        break ;

        // 4.1
    case REQUEST_ID:
        nb_requetes[16]++ ;

        D.Out(pdTrace, "Receiving Message from Federate, type RequestIDs.");

        ObjectHandle first ;
        ObjectHandle last ;
        om->requestID(req->idCount, first, last, e);

        rep.firstId = first ;
        rep.lastId = last ;
        break ;

        // 4.2
    case REGISTER_OBJECT: {
        nb_requetes[17]++ ;

        FederationTime date = tm->requestFederateTime();
        FederationTime heure = date + tm->requestLookahead();

        D.Out(pdTrace, "Receiving Message from Federate, type RegisterObject.");

        rep.object = om->registerObject(req->objectClass,
                                        req->getName(),
                                        date, heure, e);
        break ; /*FAYET 25.07.01*/
    }

        // 4.3
    case UPDATE_ATTRIBUTE_VALUES: {
        nb_requetes[18]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type UpdateAttribValues.");

        AttributeValue *ValueArray = req->getValueArray();

        try {
            rep.eventRetraction = om->updateAttributeValues(req->object,
                                                            req->handleArray,
                                                            ValueArray,
                                                            req->handleArraySize,
                                                            req->date,
                                                            req->getTag(),
                                                            e);
            free(ValueArray);
        } catch (Exception *e) {
            free(ValueArray);
            throw e ;
        }
    }
        break ;

        // 4.6
    case SEND_INTERACTION: {
        nb_requetes[19]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type SendInteraction.");

        ParameterValue *ValueArray = (ParameterValue *) req->getValueArray();

        try {
            rep.eventRetraction = om->sendInteraction(req->interactionClass,
                                                      req->handleArray,
                                                      ValueArray,
                                                      req->handleArraySize,
                                                      req->date,
                                                      req->getTag(),
                                                      e);
            free(ValueArray);
        } catch (Exception *e) {
            free(ValueArray);
            throw e ;
        }
    }
        break ;

        // 4.8
    case DELETE_OBJECT:
        nb_requetes[20]++ ;
        D.Out(pdTrace, "Receiving Message from Federate, type DeleteObject.");
        rep.eventRetraction = om->deleteObject(req->object, req->getTag(), e);
        break ;

        // 4.10
    case CHANGE_ATTRIBUTE_TRANSPORT_TYPE:
        nb_requetes[21]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type ChangeAttribTransport.");

        om->changeAttributeTransportType(req->object,
                                         req->handleArray,
                                         req->handleArraySize,
                                         req->transport,
                                         e);
        break ;

        // 4.11
    case CHANGE_ATTRIBUTE_ORDER_TYPE:
        nb_requetes[22]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type ChangeAttribOrder.");

        om->changeAttributeOrderType(req->object,
                                     req->handleArray,
                                     req->handleArraySize,
                                     req->order,
                                     e);
        break ;

        // 4.12
    case CHANGE_INTERACTION_TRANSPORT_TYPE:
        nb_requetes[23]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type ChangeInterTransport.");

        om->changeInteractionTransportType(req->interactionClass,
                                           req->transport,
                                           e);
        break ;

        // 4.13
    case CHANGE_INTERACTION_ORDER_TYPE:
        nb_requetes[24]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type ChangeInterOrder.");

        om->changeInteractionOrderType(req->interactionClass, req->order, e);
        break ;

        // 4.14(1)
    case REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE:
        // unimplemented
        break ;

        // 4.14(2)
    case REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE:
        // unimplemented
        break ;

        // 4.16
    case RETRACT:
        // unimplemented
        break ;

        // 5.1
    case UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
        nb_requetes[47]++ ;
        D.Out(pdTrace, "Receiving Message from Federate, "
              "type unconditionalAttributeOwnershipDivestiture.");
        D.Out(pdTrace, "Object %u number of attributes %u ",
              req->object, req->handleArraySize);
        owm->unconditionalAttributeOwnershipDivestiture(req->object,
                                                        req->handleArray,
                                                        req->handleArraySize,
                                                        e);
        break ;


        // 5.2
    case NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
        nb_requetes[45]++ ;
        D.Out(pdTrace, "Receiving Message from Federate, "
              "type negotiatedAttributeOwnershipDivestiture.");
        D.Out(pdTrace, "Object %u ; %u nb Attribute ", req->object,
              req->handleArraySize);
        owm->negotiatedAttributeOwnershipDivestiture(req->object,
                                                     req->handleArray,
                                                     req->handleArraySize,
                                                     req->getTag(),
                                                     e);
        break ;


        // 5.6
    case ATTRIBUTE_OWNERSHIP_ACQUISITION:
        nb_requetes[48]++ ;
        D.Out(pdTrace, "Receiving Message from Federate, "
              "type attributeOwnershipAcquisition.");
        D.Out(pdTrace, "Object %u ; Nb d'Attributs ", req->object,
              req->handleArraySize);
        owm->attributeOwnershipAcquisition(req->object,
                                           req->handleArray,
                                           req->handleArraySize,
                                           req->getTag(), e);
        break ;

        // 5.8
    case ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE:
        {
            nb_requetes[50]++ ;
            D.Out(pdTrace, "Receiving Message from Federate, "
                  "type attributeOwnershipRealeaseResponse.");
            D.Out(pdTrace, "Object %u nb Attribute %u ",
                  req->object, req->handleArraySize);

            AttributeHandleSet* theAttributes=
                owm->attributeOwnershipRealeaseResponse(req->object,
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
    case CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE:
        nb_requetes[49]++ ;
        D.Out(pdTrace, "Receiving Message from Federate, "
              "type cancelNegociatedAttributeOwnershipDivestiture.");
        D.Out(pdTrace, "Object %u nb Attribute %u ",
              req->object, req->handleArraySize);
        owm->cancelnegotiatedAttributeOwnershipDivestiture(req->object,
                                                           req->handleArray,
                                                           req->handleArraySize,
                                                           e);
        break ;


        // 5.10
    case CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION:
        nb_requetes[51]++ ;
        D.Out(pdTrace, "Receiving Message from Federate, "
              "type cancelAttributeOwnershipAcquisition.");
        D.Out(pdTrace, "Object %u nb Attribute %u ",
              req->object, req->handleArraySize);
        owm->cancelattributeOwnershipAcquisition(req->object,
                                                 req->handleArray,
                                                 req->handleArraySize,
                                                 e);
        break ;
        // 5.12
    case ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE:
        nb_requetes[46]++ ;
        D.Out(pdTrace, "Receiving Message from Federate, "
              "type attributeOwnershipAcquisitionIfAvailable.");
        D.Out(pdTrace, "Object %u nb Attribute %u ",
              req->object, req->handleArraySize);
        owm->attributeOwnershipAcquisitionIfAvailable(req->object,
                                                      req->handleArray,
                                                      req->handleArraySize,
                                                      e);
        break ;

        // 5.14
    case QUERY_ATTRIBUTE_OWNERSHIP:
        nb_requetes[44]++ ;
        D.Out(pdTrace, "Receiving Message from Federate, "
              "type queryAttributeOwnership.");
        D.Out(pdTrace, "Object %u Attribute %u ",
              req->object, req->attribute);
        owm->queryAttributeOwnership(req->object, req->attribute, e);

        break ;

        // 5.16
    case IS_ATTRIBUTE_OWNED_BY_FEDERATE:
        nb_requetes[43]++ ;
        D.Out(pdTrace, "Receiving Message from Federate, "
              "type isAttributeOwnedByFederate.");
        D.Out(pdTrace, "Object %u Attribute %u ",
              req->object, req->attribute);
        rep.setTag(owm->attributeOwnedByFederate(req->object,
                                                 req->attribute, e));
        break ;

        // 6.1
    case REQUEST_FEDERATION_TIME:
        nb_requetes[25]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type RequestFederationTime.");

        rep.date = tm->requestFederationTime();
        break ;

        // 6.2
    case REQUEST_LBTS:
        nb_requetes[26]++ ;
        D.Out(pdTrace, "Receiving Message from Federate, type RequestLBTS.");

        rep.date = tm->requestLBTS();
        break ;

        // 6.3
    case REQUEST_FEDERATE_TIME:
        nb_requetes[27]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type RequestFederateTime.");

        rep.date = tm->requestFederateTime();
        break ;

        // 6.4
    case REQUEST_MIN_NEXT_EVENT_TIME:
        // unimplemented
        break ;

        // 6.5
    case SET_LOOKAHEAD:
        nb_requetes[28]++ ;
        D.Out(pdTrace, "Receiving Message from Federate, type SetLookAhead.");

        tm->setLookahead(req->lookahead, e);
        break ;

        // 6.6
    case REQUEST_LOOKAHEAD:
        nb_requetes[29]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type RequestLookAhead.");

        rep.lookahead = tm->requestLookahead();
        break ;

        // 6.7
    case TIME_ADVANCE_REQUEST:
        nb_requetes[30]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type TimeAdvanceRequest.");

        tm->timeAdvanceRequest(req->date, e);
        break ;

        // 6.8
    case NEXT_EVENT_REQUEST:
        nb_requetes[31]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type NestEventRequest.");

        tm->nextEventRequest(req->date, e);
        break ;

        // 6.9
    case FLUSH_QUEUE_REQUEST:
        // unimplemented
        break ;

        // 7.1
    case CREATE_UPDATE_REGION:
        // unimplemented
        break ;

        // 7.2
    case CREATE_SUBSCRIPTION_REGION:
        // unimplemented
        break ;

        // 7.3(1)
    case ASSOCIATE_UPDATE_REGION:
        // unimplemented
        break ;

        // 7.3(2)
    case DISASSOCIATE_UPDATE_REGION:
        // unimplemented
        break ;

        // 7.5
    case MODIFY_REGION:
        // unimplemented
        break ;

        // 7.6
    case DELETE_REGION:
        // unimplemented
        break ;

        // 8.1
    case GET_OBJECT_CLASS_HANDLE:
        nb_requetes[32]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetObjectClassHandle.");

        rep.objectClass = om->getObjectClassHandle(req->getName());
        break ;

        // 8.2
    case GET_OBJECT_CLASS_NAME:
        nb_requetes[33]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetObjectClassName.");

        rep.setName(om->getObjectClassName(req->objectClass));
        break ;

        // 8.3
    case GET_ATTRIBUTE_HANDLE:
        nb_requetes[34]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetAttributeHandle.");

        rep.attribute = om->getAttributeHandle(req->getName(),
                                               req->objectClass);
        break ;

        // 8.4
    case GET_ATTRIBUTE_NAME:
        nb_requetes[35]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetAttributeName.");

        rep.setName(om->getAttributeName(req->attribute,
                                         req->objectClass));
        break ;

        // 8.5
    case GET_INTERACTION_CLASS_HANDLE:
        nb_requetes[36]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetInteractionHandle.");

        rep.interactionClass = om->getInteractionClassHandle(req->getName());
        break ;

        // 8.6
    case GET_INTERACTION_CLASS_NAME:
        nb_requetes[37]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetInteractionName.");

        rep.setName(om->getInteractionClassName(req->interactionClass));
        break ;

        // 8.7
    case GET_PARAMETER_HANDLE:
        nb_requetes[38]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetParameterHandle.");

        rep.parameter = om->getParameterHandle(req->getName(),
                                               req->interactionClass);
        break ;

        // 8.8
    case GET_PARAMETER_NAME:
        nb_requetes[39]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetParameterName.");

        rep.setName(om->getParameterName(req->parameter,
                                         req->interactionClass));
        break ;

        // 8.9
    case GET_SPACE_HANDLE:
        // nb_requetes...
        D.Out(pdTrace, "Receiving Message from Federate, type GetSpaceHandle.");
        rep.space = ddm->getRoutingSpaceHandle(req->getName());
        break ;

        // 8.10
    case GET_SPACE_NAME:
        // nb_requetes...
        D.Out(pdTrace, "Receiving Message from Federate, type GetSpaceName.");
        rep.setName(ddm->getRoutingSpaceName(req->getSpace()).c_str());
        break ;

    case GET_DIMENSION_HANDLE:
        // nb_requetes...
        D.Out(pdTrace, 
              "Receiving Message from Federate, type GetDimensionHandle");
        rep.setDimension(ddm->getDimensionHandle(string(req->getName()),
                                                 req->getSpace()));
        break ;

    case GET_DIMENSION_NAME:
        // nb_requetes...
        D.Out(pdTrace,
              "Receiving Message from Federate, type GetDimensionName");
        rep.setName(ddm->getDimensionName(req->getDimension(),
                                          req->getSpace()).c_str());
        break ;

        // 8.11
    case GET_FEDERATE_HANDLE:
        // unimplemented
        break ;

        // 8.12
    case GET_FEDERATE_NAME:
        // unimplemented
        break ;

        // 8.13
    case SET_TIME_REGULATING:
        nb_requetes[40]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type SetTimeRegulating.");

        tm->setTimeRegulating(req->boolean, e);
        break ;

        // 8.14
    case SET_TIME_CONSTRAINED:
        nb_requetes[41]++ ;
        D.Out(pdTrace,
              "Receiving Message from Federate, type SetTimeConstrained.");

        tm->setTimeConstrained(req->boolean, e);
        break ;

        // 8.15
    case TICK_REQUEST:
        nb_requetes[42]++ ;
        D.Out(pdDebug, "Receiving Message from Federate, type TickRequest.");

        rep.boolean = tm->tick(e);
        break ;

    default:
        D.Out(pdExcept,
              "Receiving Message from Federate, Unknown Type %d.", req->type);
        throw RTIinternalError();
    }
}

// ----------------------------------------------------------------------------
//! RTIA processes a service request coming from federate.
void
RTIA::processFederateRequest(Message *req)
{
    Message rep ;

    rep.exception = e_NO_EXCEPTION ;
    rep.type = req->type ;

    try {
        chooseFederateProcessing(req, rep, rep.exception);
    }
    catch (ArrayIndexOutOfBounds &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_ArrayIndexOutOfBounds ;
    }
    catch (AttributeAlreadyOwned &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_AttributeAlreadyOwned ;
    }
    catch (AttributeAlreadyBeingDivested &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_AttributeAlreadyBeingDivested ;
    }
    catch (AttributeAlreadyBeingAcquired &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_AttributeAlreadyBeingAcquired ;
    }
    catch (AttributeAcquisitionWasNotRequested &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_AttributeAcquisitionWasNotRequested ;
    }
    catch (AttributeDivestitureWasNotRequested &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_AttributeDivestitureWasNotRequested ;
    }
    catch (AttributeNotDefined &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_AttributeNotDefined ;
    }
    catch (AttributeNotKnown &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_AttributeNotKnown ;
    }
    catch (AttributeNotOwned &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_AttributeNotOwned ;
    }
    catch (AttributeNotPublished &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_AttributeNotPublished ;
    }
    catch (AttributeNotSubscribed &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_AttributeNotSubscribed ;
    }
    catch (ConcurrentAccessAttempted &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_ConcurrentAccessAttempted ;
    }
    catch (CouldNotDiscover &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_CouldNotDiscover ;
    }
    catch (CouldNotOpenRID &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_CouldNotOpenRID ;
    }
    catch (CouldNotRestore &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_CouldNotRestore ;
    }
    catch (DeletePrivilegeNotHeld &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_DeletePrivilegeNotHeld ;
    }
    catch (ErrorReadingRID &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_ErrorReadingRID ;
    }
    catch (EventNotKnown &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_EventNotKnown ;
    }
    catch (FederateAlreadyPaused &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_FederateAlreadyPaused ;
    }
    catch (FederateAlreadyExecutionMember &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_FederateAlreadyExecutionMember ;
    }
    catch (FederateDoesNotExist &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_FederateDoesNotExist ;
    }
    catch (FederateInternalError &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_FederateInternalError ;
    }
    catch (FederateNameAlreadyInUse &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_FederateNameAlreadyInUse ;
    }
    catch (FederateNotExecutionMember &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_FederateNotExecutionMember ;
    }
    catch (FederateNotPaused &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_FederateNotPaused ;
    }
    catch (FederateNotPublishing &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_FederateNotPublishing ;
    }
    catch (FederateNotSubscribing &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_FederateNotSubscribing ;
    }
    catch (FederateOwnsAttributes &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_FederateOwnsAttributes ;
    }
    catch (FederatesCurrentlyJoined &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_FederatesCurrentlyJoined ;
    }
    catch (FederateWasNotAskedToReleaseAttribute &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_FederateWasNotAskedToReleaseAttribute ;
    }
    catch (FederationAlreadyPaused &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_FederationAlreadyPaused ;
    }
    catch (FederationExecutionAlreadyExists &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_FederationExecutionAlreadyExists ;
    }
    catch (FederationExecutionDoesNotExist &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_FederationExecutionDoesNotExist ;
    }
    catch (FederationNotPaused &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_FederationNotPaused ;
    }
    catch (FederationTimeAlreadyPassed &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_FederationTimeAlreadyPassed ;
    }
    catch (IDsupplyExhausted &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_IDsupplyExhausted ;
    }
    catch (InteractionClassNotDefined &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_InteractionClassNotDefined ;
    }
    catch (InteractionClassNotKnown &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_InteractionClassNotKnown ;
    }
    catch (InteractionClassNotPublished &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_InteractionClassNotPublished ;
    }
    catch (InteractionParameterNotDefined &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_InteractionParameterNotDefined ;
    }
    catch (InteractionParameterNotKnown &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_InteractionParameterNotKnown ;
    }
    catch (InvalidDivestitureCondition &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_InvalidDivestitureCondition ;
    }
    catch (InvalidExtents &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_InvalidExtents ;
    }
    catch (InvalidFederationTime &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_InvalidFederationTime ;
    }
    catch (InvalidFederationTimeDelta &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_InvalidFederationTimeDelta ;
    }
    catch (InvalidObjectHandle &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_InvalidObjectHandle ;
    }
    catch (InvalidOrderType &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_InvalidOrderType ;
    }
    catch (InvalidResignAction &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_InvalidResignAction ;
    }
    catch (InvalidRetractionHandle &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_InvalidRetractionHandle ;
    }
    catch (InvalidRoutingSpace &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_InvalidRoutingSpace ;
    }
    catch (InvalidTransportType &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_InvalidTransportType ;
    }
    catch (MemoryExhausted &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_MemoryExhausted ;
    }
    catch (NameNotFound &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_NameNotFound ;
    }
    catch (NoPauseRequested &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_NoPauseRequested ;
    }
    catch (NoResumeRequested &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_NoResumeRequested ;
    }
    catch (ObjectClassNotDefined &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_ObjectClassNotDefined ;
    }
    catch (ObjectClassNotKnown &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_ObjectClassNotKnown ;
    }
    catch (ObjectClassNotPublished &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_ObjectClassNotPublished ;
    }
    catch (ObjectClassNotSubscribed &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_ObjectClassNotSubscribed ;
    }
    catch (ObjectNotKnown &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_ObjectNotKnown ;
    }
    catch (ObjectAlreadyRegistered &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_ObjectAlreadyRegistered ;
    }
    catch (RegionNotKnown &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_RegionNotKnown ;
    }
    catch (RestoreInProgress &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_RestoreInProgress ;
    }
    catch (RestoreNotRequested &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_RestoreNotRequested ;
    }
    catch (RTIinternalError &e) {
        cout << "RTIA sends InternalError to Fed., " ;
        if (e._reason != NULL)
            cout << "reason : " << e._reason << endl ;
        else
            cout << "no reason given." << endl ;
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_RTIinternalError ;
    }
    catch (SpaceNotDefined &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_SpaceNotDefined ;
    }
    catch (SaveInProgress &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_SaveInProgress ;
    }
    catch (SaveNotInitiated &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_SaveNotInitiated ;
    }
    catch (SpecifiedSaveLabelDoesNotExist &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_SpecifiedSaveLabelDoesNotExist ;
    }
    catch (TimeAdvanceAlreadyInProgress &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_TimeAdvanceAlreadyInProgress ;
    }
    catch (TimeAdvanceWasNotInProgress &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_TimeAdvanceWasNotInProgress ;
    }
    catch (TooManyIDsRequested &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_TooManyIDsRequested ;
    }
    catch (UnableToPerformSave &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_UnableToPerformSave ;
    }
    catch (UnimplementedService &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_UnimplementedService ;
    }
    catch (UnknownLabel &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_UnknownLabel ;
    }
    catch (ValueCountExceeded &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_ValueCountExceeded ;
    }
    catch (ValueLengthExceeded &e) {
        D.Out(pdExcept, "Catched %s Exception.", e._name);
        rep.exception = e_ValueLengthExceeded ;
    }

    // Default Handler
    catch (Exception &e) {
        D.Out(pdExcept, "Unknown Exception : %s.", e._name);
        rep.exception = e_RTIinternalError ;
    }

    delete req ;

    comm->sendUN(&rep);
    D.Out(pdDebug, "Reply send to Unix socket.");
}

}} // namespace certi/rtia

// $Id: RTIA_federate.cc,v 3.8 2003/03/21 15:06:46 breholee Exp $
