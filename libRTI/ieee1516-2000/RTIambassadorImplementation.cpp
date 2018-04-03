// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2014  ONERA
//
// This file is part of CERTI-libRTI
//
// CERTI-libRTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libRTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// ----------------------------------------------------------------------------

#include "RTIambassadorImplementation.h"
#include <RTI/RangeBounds.h>

#ifndef _WIN32
#include <cstdlib>
#include <cstring>
#endif

#include "PrettyDebug.hh"

#include "M_Classes.hh"
#include "RTI1516fedTime.h"
#include "RTIHandleFactory.h"

#include <algorithm>

namespace {

static PrettyDebug D("LIBRTI", __FILE__);
static PrettyDebug G("GENDOC", __FILE__);
}

namespace certi {
/* Deletor Object */
template <class T>
struct Deletor {
    void operator()(T* e)
    {
        delete e;
    };
};

/* Helper functions */
template <typename T>
void RTI1516ambassador::assignPHVMAndExecuteService(const rti1516::ParameterHandleValueMap& PHVM, T& req, T& rep)
{
    req.setParametersSize(PHVM.size());
    req.setValuesSize(PHVM.size());
    uint32_t i = 0;
    for (rti1516::ParameterHandleValueMap::const_iterator it = PHVM.begin(); it != PHVM.end(); ++it, ++i) {
        req.setParameters(rti1516::ParameterHandleFriend::toCertiHandle(it->first), i);
        certi::ParameterValue_t paramValue;
        paramValue.resize(it->second.size());
        memcpy(&(paramValue[0]), it->second.data(), it->second.size());
        req.setValues(paramValue, i);
    }
    privateRefs->executeService(&req, &rep);
}

template <typename T>
void RTI1516ambassador::assignAHVMAndExecuteService(const rti1516::AttributeHandleValueMap& AHVM, T& req, T& rep)
{
    req.setAttributesSize(AHVM.size());
    req.setValuesSize(AHVM.size());
    uint32_t i = 0;
    for (rti1516::AttributeHandleValueMap::const_iterator it = AHVM.begin(); it != AHVM.end(); ++it, ++i) {
        req.setAttributes(rti1516::AttributeHandleFriend::toCertiHandle(it->first), i);
        certi::AttributeValue_t attrValue;
        attrValue.resize(it->second.size());
        memcpy(&(attrValue[0]), it->second.data(), it->second.size());
        req.setValues(attrValue, i);
    }
    privateRefs->executeService(&req, &rep);
}

template <typename T>
void RTI1516ambassador::assignAHSAndExecuteService(const rti1516::AttributeHandleSet& AHS, T& req, T& rep)
{
    req.setAttributesSize(AHS.size());
    uint32_t i = 0;
    for (rti1516::AttributeHandleSet::const_iterator it = AHS.begin(); it != AHS.end(); ++it, ++i) {
        certi::AttributeHandle certiHandle = rti1516::AttributeHandleFriend::toCertiHandle(*it);
        req.setAttributes(certiHandle, i);
    }
    privateRefs->executeService(&req, &rep);
}

std::string varLengthDataAsString(rti1516::VariableLengthData varLengthData)
{
    std::string retVal((char*) varLengthData.data(), varLengthData.size());
    return retVal;
}

certi::TransportType toCertiTransportationType(rti1516::TransportationType theType)
{
    return (theType == rti1516::RELIABLE) ? certi::RELIABLE : certi::BEST_EFFORT;
}
rti1516::TransportationType toRTI1516TransportationType(certi::TransportType theType)
{
    return (theType == certi::RELIABLE) ? rti1516::RELIABLE : rti1516::BEST_EFFORT;
}
certi::OrderType toCertiOrderType(rti1516::OrderType theType)
{
    return (theType == rti1516::RECEIVE) ? certi::RECEIVE : certi::TIMESTAMP;
}
rti1516::OrderType toRTI1516OrderType(certi::OrderType theType)
{
    return (theType == certi::RECEIVE) ? rti1516::RECEIVE : rti1516::TIMESTAMP;
}
/* end of Helper functions */

RTI1516ambassador::RTI1516ambassador() throw() : privateRefs(0)
{
}

RTI1516ambassador::~RTI1516ambassador()
{
    certi::M_Close_Connexion req, rep;

    Debug(G, pdGendoc) << "        ====>executeService CLOSE_CONNEXION" << std::endl;
    privateRefs->executeService(&req, &rep);
    // after the response is received, the privateRefs->socketUn must not be used

    delete privateRefs;
}

// ----------------------------------------------------------------------------
//! Generic callback evocation (CERTI extension).
/*! Blocks up to "minimum" seconds until a callback delivery and then evokes a
 *  single callback.
 *  @return true if additional callbacks pending, false otherwise
 */
bool RTI1516ambassador::__tick_kernel(bool multiple,
                                      TickTime minimum,
                                      TickTime maximum) throw(rti1516::SpecifiedSaveLabelDoesNotExist,
                                                              rti1516::RTIinternalError)
{
    M_Tick_Request vers_RTI;
    std::unique_ptr<Message> vers_Fed;

    // Request callback(s) from the local RTIA
    vers_RTI.setMultiple(multiple);
    vers_RTI.setMinTickTime(minimum);
    vers_RTI.setMaxTickTime(maximum);

    try {
        vers_RTI.send(privateRefs->socketUn, privateRefs->msgBufSend);
    }
    catch (NetworkError& e) {
        std::stringstream msg;
        msg << "NetworkError in tick() while sending TICK_REQUEST: " << e.reason();
        std::wstring message(msg.str().begin(), msg.str().end());
        throw rti1516::RTIinternalError(message);
    }

    // Read response(s) from the local RTIA until Message::TICK_REQUEST is received.
    while (1) {
        try {
            vers_Fed.reset(M_Factory::receive(privateRefs->socketUn));
        }
        catch (NetworkError& e) {
            std::stringstream msg;
            msg << "NetworkError in tick() while receiving response: " << e.reason();
            std::wstring message(msg.str().begin(), msg.str().end());
            throw rti1516::RTIinternalError(message);
        }

        // If the type is TICK_REQUEST, the __tick_kernel() has terminated.
        if (vers_Fed->getMessageType() == Message::TICK_REQUEST) {
            if (vers_Fed->getExceptionType() != Exception::Type::NO_EXCEPTION) {
                // tick() may only throw exceptions defined in the HLA standard
                // the RTIA is responsible for sending 'allowed' exceptions only
                privateRefs->processException(vers_Fed.get());
            }
            return static_cast<M_Tick_Request*>(vers_Fed.get())->getMultiple();
        }

        try {
            // Otherwise, the RTI calls a FederateAmbassador service.
            privateRefs->callFederateAmbassador(vers_Fed.get());
        }
        catch (RTIinternalError&) {
            // RTIA awaits TICK_REQUEST_NEXT, terminate the tick() processing
            privateRefs->sendTickRequestStop();
            // ignore the response and re-throw the original exception
            throw;
        }

        try {
            // Request next callback from the RTIA
            M_Tick_Request_Next tick_next;
            tick_next.send(privateRefs->socketUn, privateRefs->msgBufSend);
        }
        catch (NetworkError& e) {
            std::stringstream msg;
            msg << "NetworkError in tick() while sending TICK_REQUEST_NEXT: " << e.reason();

            std::wstring message(msg.str().begin(), msg.str().end());
            throw rti1516::RTIinternalError(message);
        }
    } // while(1)
    // should never be reached, this is there for calming down static code analysis
    return false;
}

// 4.2
void RTI1516ambassador::createFederationExecution(
    std::wstring const& federationExecutionName,
    std::wstring const& fullPathNameToTheFDDfile,
    std::wstring const& /*LogicalTimeImplementationName*/) throw(rti1516::FederationExecutionAlreadyExists,
                                                                 rti1516::CouldNotOpenFDD,
                                                                 rti1516::ErrorReadingFDD,
                                                                 rti1516::CouldNotCreateLogicalTimeFactory,
                                                                 rti1516::RTIinternalError)
{
    certi::M_Create_Federation_Execution_V4 req, rep;

    Debug(G, pdGendoc) << "enter RTI1516ambassador::createFederationExecution" << std::endl;
    req.setFederationExecutionName({federationExecutionName.begin(), federationExecutionName.end()});

    req.setFomModuleDesignatorsSize(1);
    req.setFomModuleDesignators({fullPathNameToTheFDDfile.begin(), fullPathNameToTheFDDfile.end()}, 0);
    
    req.setRtiVersion(IEEE_1516_2000);

    Debug(G, pdGendoc) << "             ====>executeService CREATE_FEDERATION_EXECUTION" << std::endl;

    privateRefs->executeService(&req, &rep);

    Debug(G, pdGendoc) << "exit RTI1516ambassador::createFederationExecution" << std::endl;
}

// 4.3
void RTI1516ambassador::destroyFederationExecution(std::wstring const& federationExecutionName) throw(
    rti1516::FederatesCurrentlyJoined, rti1516::FederationExecutionDoesNotExist, rti1516::RTIinternalError)
{
    M_Destroy_Federation_Execution req, rep;

    Debug(G, pdGendoc) << "enter RTI1516ambassador::destroyFederationExecution" << std::endl;

    std::string federationExecutionNameAsString(federationExecutionName.begin(), federationExecutionName.end());
    req.setFederationName(federationExecutionNameAsString);

    Debug(G, pdGendoc) << "        ====>executeService DESTROY_FEDERATION_EXECUTION" << std::endl;

    privateRefs->executeService(&req, &rep);

    Debug(G, pdGendoc) << "exit RTI1516ambassador::destroyFederationExecution" << std::endl;
}

// 4.4
rti1516::FederateHandle RTI1516ambassador::joinFederationExecution(
    std::wstring const& federateType,
    std::wstring const& federationExecutionName,
    rti1516::FederateAmbassador& federateAmbassador) throw(rti1516::FederateAlreadyExecutionMember,
                                                           rti1516::FederationExecutionDoesNotExist,
                                                           rti1516::SaveInProgress,
                                                           rti1516::RestoreInProgress,
                                                           rti1516::CouldNotCreateLogicalTimeFactory,
                                                           rti1516::RTIinternalError)
{
    M_Join_Federation_Execution_V4 req, rep;

    Debug(G, pdGendoc) << "enter RTI1516ambassador::joinFederationExecution" << std::endl;

    if (federateType.length() <= 0) {
        throw rti1516::RTIinternalError(L"Incorrect or empty federate name");
    }

    if (federationExecutionName.length() <= 0) {
        throw rti1516::RTIinternalError(L"Incorrect or empty federation name");
    }
    
    std::string federateTypeAsString(federateType.begin(), federateType.end());
    
    std::string federationExecutionNameAsString(federationExecutionName.begin(), federationExecutionName.end());

    privateRefs->fed_amb = &federateAmbassador;

    req.setFederateName(federateTypeAsString);
    req.setFederationExecutionName(federationExecutionNameAsString);
    
    req.setRtiVersion(IEEE_1516_2000);
    
    Debug(G, pdGendoc) << "        ====>executeService JOIN_FEDERATION_EXECUTION" << std::endl;
    privateRefs->executeService(&req, &rep);
    
    Debug(G, pdGendoc) << "exit  RTI1516ambassador::joinFederationExecution" << std::endl;
    PrettyDebug::setFederateName("LibRTI::" + std::string(federateTypeAsString));

    certi::FederateHandle certiFederateHandle = rep.getFederate();
    rti1516::FederateHandle rti1516FederateHandle
        = rti1516::FederateHandleFriend::createRTI1516Handle(certiFederateHandle);

    return rti1516FederateHandle;
}

// 4.5
void RTI1516ambassador::resignFederationExecution(rti1516::ResignAction /*resignAction*/) throw(
    rti1516::OwnershipAcquisitionPending,
    rti1516::FederateOwnsAttributes,
    rti1516::FederateNotExecutionMember,
    rti1516::RTIinternalError)
{
    M_Resign_Federation_Execution req, rep;

    Debug(G, pdGendoc) << "enter RTI1516ambassador::resignFederationExecution" << std::endl;
    //req.setResignAction(static_cast<certi::ResignAction>(resignAction));
    req.setResignAction(certi::DELETE_OBJECTS_AND_RELEASE_ATTRIBUTES);
    Debug(G, pdGendoc) << "        ====>executeService RESIGN_FEDERATION_EXECUTION" << std::endl;
    privateRefs->executeService(&req, &rep);
    Debug(G, pdGendoc) << "exit RTI1516ambassador::resignFederationExecution" << std::endl;
}

// 4.6
void RTI1516ambassador::registerFederationSynchronizationPoint(
    std::wstring const& label,
    rti1516::VariableLengthData const& theUserSuppliedTag) throw(rti1516::FederateNotExecutionMember,
                                                                 rti1516::SaveInProgress,
                                                                 rti1516::RestoreInProgress,
                                                                 rti1516::RTIinternalError)
{
    M_Register_Federation_Synchronization_Point req, rep;

    Debug(G, pdGendoc) << "enter RTI1516ambassador::registerFederationSynchronizationPoint for all federates" << std::endl;
    std::string labelString(label.begin(), label.end());
    req.setLabel(labelString);
    // no federate set
    req.setFederateSetSize(0);
    if (theUserSuppliedTag.data() == NULL) {
        throw rti1516::RTIinternalError(L"Calling registerFederationSynchronizationPoint with Tag NULL");
    }
    req.setTag(varLengthDataAsString(theUserSuppliedTag));
    Debug(G, pdGendoc) << "        ====>executeService REGISTER_FEDERATION_SYNCHRONIZATION_POINT" << std::endl;
    privateRefs->executeService(&req, &rep);

    Debug(G, pdGendoc) << "exit RTI1516ambassador::registerFederationSynchronizationPoint for all federates" << std::endl;
}

void RTI1516ambassador::registerFederationSynchronizationPoint(
    std::wstring const& label,
    rti1516::VariableLengthData const& theUserSuppliedTag,
    rti1516::FederateHandleSet const& syncSet) throw(rti1516::FederateNotExecutionMember,
                                                     rti1516::SaveInProgress,
                                                     rti1516::RestoreInProgress,
                                                     rti1516::RTIinternalError)
{
    M_Register_Federation_Synchronization_Point req, rep;

    Debug(G, pdGendoc) << "enter RTI1516ambassador::registerFederationSynchronizationPoint for some federates" << std::endl;

    std::string labelString(label.begin(), label.end());
    req.setLabel(labelString);
    if (theUserSuppliedTag.data() == NULL) {
        throw rti1516::RTIinternalError(L"Calling registerFederationSynchronizationPoint with Tag NULL");
    }
    req.setTag(varLengthDataAsString(theUserSuppliedTag));
    // Federate set exists but if size=0 (set empty)
    // We have to send the size even if federate set size is 0
    // (HLA 1.3 compliance to inform ALL federates)

    req.setFederateSetSize(syncSet.size());

    uint32_t i = 0;
    for (rti1516::FederateHandleSet::const_iterator it = syncSet.begin(); it != syncSet.end(); ++it, ++i) {
        req.setFederateSet(rti1516::FederateHandleFriend::toCertiHandle(*it), i);
    }

    Debug(G, pdGendoc) << "        ====>executeService REGISTER_FEDERATION_SYNCHRONIZATION_POINT" << std::endl;
    privateRefs->executeService(&req, &rep);

    Debug(G, pdGendoc) << "exit RTI1516ambassador::registerFederationSynchronizationPoint for some federates" << std::endl;
}

// 4.9
void RTI1516ambassador::synchronizationPointAchieved(std::wstring const& label) throw(
    rti1516::SynchronizationPointLabelNotAnnounced,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    M_Synchronization_Point_Achieved req, rep;

    Debug(G, pdGendoc) << "enter RTI1516ambassador::synchronizationPointAchieved" << std::endl;

    std::string labelString(label.begin(), label.end());
    req.setLabel(labelString);

    Debug(G, pdGendoc) << "        ====>executeService SYNCHRONIZATION_POINT_ACHIEVED" << std::endl;
    privateRefs->executeService(&req, &rep);

    Debug(G, pdGendoc) << "exit  RTI1516ambassador::synchronizationPointAchieved" << std::endl;
}

// 4.11
void RTI1516ambassador::requestFederationSave(std::wstring const& label) throw(rti1516::FederateNotExecutionMember,
                                                                               rti1516::SaveInProgress,
                                                                               rti1516::RestoreInProgress,
                                                                               rti1516::RTIinternalError)
{
    M_Request_Federation_Save req, rep;

    Debug(G, pdGendoc) << "enter RTI1516ambassador::requestFederationSave without time" << std::endl;

    std::string labelString(label.begin(), label.end());
    req.setLabel(labelString);
    Debug(G, pdGendoc) << "      ====>executeService REQUEST_FEDERATION_SAVE" << std::endl;

    privateRefs->executeService(&req, &rep);
    Debug(G, pdGendoc) << "exit  RTI1516ambassador::requestFederationSave without time" << std::endl;
}

void RTI1516ambassador::requestFederationSave(std::wstring const& label, rti1516::LogicalTime const& theTime) throw(
    rti1516::LogicalTimeAlreadyPassed,
    rti1516::InvalidLogicalTime,
    rti1516::FederateUnableToUseTime,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    M_Request_Federation_Save req, rep;

    Debug(G, pdGendoc) << "enter RTI1516ambassador::requestFederationSave with time" << std::endl;

    certi::FederationTime certiFedTime(certi_cast<RTI1516fedTime>()(theTime).getFedTime());
    req.setDate(certiFedTime);

    std::string labelString(label.begin(), label.end());
    req.setLabel(labelString);

    Debug(G, pdGendoc) << "        ====>executeService REQUEST_FEDERATION_SAVE" << std::endl;
    privateRefs->executeService(&req, &rep);

    Debug(G, pdGendoc) << "exit RTI1516ambassador::requestFederationSave with time" << std::endl;
}

// 4.13
void RTI1516ambassador::federateSaveBegun() throw(rti1516::SaveNotInitiated,
                                                  rti1516::FederateNotExecutionMember,
                                                  rti1516::RestoreInProgress,
                                                  rti1516::RTIinternalError)
{
    M_Federate_Save_Begun req, rep;

    Debug(G, pdGendoc) << "enter RTI1516ambassador::federateSaveBegun" << std::endl;

    Debug(G, pdGendoc) << "      ====>executeService FEDERATE_SAVE_BEGUN" << std::endl;
    privateRefs->executeService(&req, &rep);

    Debug(G, pdGendoc) << "exit  RTI1516ambassador::federateSaveBegun" << std::endl;
}

// 4.14
void RTI1516ambassador::federateSaveComplete() throw(rti1516::FederateHasNotBegunSave,
                                                     rti1516::FederateNotExecutionMember,
                                                     rti1516::RestoreInProgress,
                                                     rti1516::RTIinternalError)
{
    M_Federate_Save_Complete req, rep;

    Debug(G, pdGendoc) << "enter RTI1516ambassador::federateSaveComplete" << std::endl;
    Debug(G, pdGendoc) << "      ====>executeService FEDERATE_SAVE_COMPLETE" << std::endl;
    privateRefs->executeService(&req, &rep);
    Debug(G, pdGendoc) << "exit  RTI1516ambassador::federateSaveComplete" << std::endl;
}

void RTI1516ambassador::federateSaveNotComplete() throw(rti1516::FederateHasNotBegunSave,
                                                        rti1516::FederateNotExecutionMember,
                                                        rti1516::RestoreInProgress,
                                                        rti1516::RTIinternalError)
{
    M_Federate_Save_Not_Complete req, rep;

    Debug(G, pdGendoc) << "enter RTI1516ambassador::federateSaveNotComplete" << std::endl;
    Debug(G, pdGendoc) << "      ====>executeService FEDERATE_SAVE_NOT_COMPLETE" << std::endl;
    privateRefs->executeService(&req, &rep);

    Debug(G, pdGendoc) << "exit  RTI1516ambassador::federateSaveNotComplete" << std::endl;
}

// 4.16
void RTI1516ambassador::queryFederationSaveStatus() throw(rti1516::FederateNotExecutionMember,
                                                          rti1516::RestoreInProgress,
                                                          rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

// 4.18
void RTI1516ambassador::requestFederationRestore(std::wstring const& label) throw(rti1516::FederateNotExecutionMember,
                                                                                  rti1516::SaveInProgress,
                                                                                  rti1516::RestoreInProgress,
                                                                                  rti1516::RTIinternalError)
{
    M_Request_Federation_Restore req, rep;

    Debug(G, pdGendoc) << "enter RTI1516ambassador::requestFederationRestore" << std::endl;
    std::string labelString(label.begin(), label.end());
    req.setLabel(labelString);
    Debug(G, pdGendoc) << "      ====>executeService REQUEST_FEDERATION_RESTORE" << std::endl;
    privateRefs->executeService(&req, &rep);
    Debug(G, pdGendoc) << "exit  RTI1516ambassador::requestFederationRestore" << std::endl;
}

// 4.22
void RTI1516ambassador::federateRestoreComplete() throw(rti1516::RestoreNotRequested,
                                                        rti1516::FederateNotExecutionMember,
                                                        rti1516::SaveInProgress,
                                                        rti1516::RTIinternalError)
{
    M_Federate_Restore_Complete req, rep;

    Debug(G, pdGendoc) << "enter RTI1516ambassador::federateRestoreComplete" << std::endl;

    Debug(G, pdGendoc) << "      ====>executeService FEDERATE_RESTORE_COMPLETE" << std::endl;
    privateRefs->executeService(&req, &rep);
    Debug(G, pdGendoc) << "exit  RTI1516ambassador::federateRestoreComplete" << std::endl;
}

void RTI1516ambassador::federateRestoreNotComplete() throw(rti1516::RestoreNotRequested,
                                                           rti1516::FederateNotExecutionMember,
                                                           rti1516::SaveInProgress,
                                                           rti1516::RTIinternalError)
{
    M_Federate_Restore_Not_Complete req, rep;

    Debug(G, pdGendoc) << "enter RTI1516ambassador::federateRestoreNotComplete" << std::endl;
    Debug(G, pdGendoc) << "      ====>executeService FEDERATE_RESTORE_NOT_COMPLETE" << std::endl;

    privateRefs->executeService(&req, &rep);
    Debug(G, pdGendoc) << "exit  RTI1516ambassador::federateRestoreNotComplete" << std::endl;
}

// 4.24
void RTI1516ambassador::queryFederationRestoreStatus() throw(rti1516::FederateNotExecutionMember,
                                                             rti1516::SaveInProgress,
                                                             rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

/////////////////////////////////////
// Declaration Management Services //
/////////////////////////////////////

// 5.2
void RTI1516ambassador::publishObjectClassAttributes(
    rti1516::ObjectClassHandle theClass,
    rti1516::AttributeHandleSet const& attributeList) throw(rti1516::ObjectClassNotDefined,
                                                            rti1516::AttributeNotDefined,
                                                            rti1516::FederateNotExecutionMember,
                                                            rti1516::SaveInProgress,
                                                            rti1516::RestoreInProgress,
                                                            rti1516::RTIinternalError)
{
    M_Publish_Object_Class req, rep;
    Debug(G, pdGendoc) << "enter RTI1516ambassador::publishObjectClass" << std::endl;

    const certi::ObjectClassHandle objectClassHandle = rti1516::ObjectClassHandleFriend::toCertiHandle(theClass);
    req.setObjectClass(objectClassHandle);

    req.setAttributesSize(attributeList.size());
    uint32_t i = 0;
    for (rti1516::AttributeHandleSet::const_iterator it = attributeList.begin(); it != attributeList.end(); ++it, ++i) {
        req.setAttributes(rti1516::AttributeHandleFriend::toCertiHandle(*it), i);
    }
    Debug(G, pdGendoc) << "      ====>executeService PUBLISH_OBJECT_CLASS" << std::endl;
    privateRefs->executeService(&req, &rep);
    Debug(G, pdGendoc) << "exit  RTI1516ambassador::publishObjectClass" << std::endl;
}

// 5.3
void RTI1516ambassador::unpublishObjectClass(rti1516::ObjectClassHandle theClass) throw(
    rti1516::ObjectClassNotDefined,
    rti1516::OwnershipAcquisitionPending,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    M_Unpublish_Object_Class req, rep;
    Debug(G, pdGendoc) << "enter RTI1516ambassador::unpublishObjectClass" << std::endl;

    const certi::ObjectClassHandle objectClassHandle = rti1516::ObjectClassHandleFriend::toCertiHandle(theClass);
    req.setObjectClass(objectClassHandle);
    Debug(G, pdGendoc) << "      ====>executeService UNPUBLISH_OBJECT_CLASS" << std::endl;
    privateRefs->executeService(&req, &rep);
    Debug(G, pdGendoc) << "exit  RTI1516ambassador::unpublishObjectClass" << std::endl;
}

void RTI1516ambassador::unpublishObjectClassAttributes(
    rti1516::ObjectClassHandle /*theClass*/,
    rti1516::AttributeHandleSet const& /*attributeList*/) throw(rti1516::ObjectClassNotDefined,
                                                            rti1516::AttributeNotDefined,
                                                            rti1516::OwnershipAcquisitionPending,
                                                            rti1516::FederateNotExecutionMember,
                                                            rti1516::SaveInProgress,
                                                            rti1516::RestoreInProgress,
                                                            rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

// 5.4
void RTI1516ambassador::publishInteractionClass(rti1516::InteractionClassHandle theInteraction) throw(
    rti1516::InteractionClassNotDefined,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    M_Publish_Interaction_Class req, rep;
    const certi::InteractionClassHandle classHandle
        = rti1516::InteractionClassHandleFriend::toCertiHandle(theInteraction);
    req.setInteractionClass(classHandle);
    Debug(G, pdGendoc) << "      ====>executeService PUBLISH_INTERACTION_CLASS" << std::endl;
    privateRefs->executeService(&req, &rep);
}

// 5.5
void RTI1516ambassador::unpublishInteractionClass(rti1516::InteractionClassHandle theInteraction) throw(
    rti1516::InteractionClassNotDefined,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    M_Unpublish_Interaction_Class req, rep;
    const certi::InteractionClassHandle classHandle
        = rti1516::InteractionClassHandleFriend::toCertiHandle(theInteraction);
    req.setInteractionClass(classHandle);

    privateRefs->executeService(&req, &rep);
}

// 5.6
void RTI1516ambassador::subscribeObjectClassAttributes(rti1516::ObjectClassHandle theClass,
                                                       rti1516::AttributeHandleSet const& attributeList,
                                                       bool active) throw(rti1516::ObjectClassNotDefined,
                                                                          rti1516::AttributeNotDefined,
                                                                          rti1516::FederateNotExecutionMember,
                                                                          rti1516::SaveInProgress,
                                                                          rti1516::RestoreInProgress,
                                                                          rti1516::RTIinternalError)
{
    M_Subscribe_Object_Class_Attributes req, rep;
    Debug(G, pdGendoc) << "enter RTI1516ambassador::subscribeObjectClassAttributes" << std::endl;

    const certi::ObjectClassHandle objectClassHandle = rti1516::ObjectClassHandleFriend::toCertiHandle(theClass);
    req.setObjectClass(objectClassHandle);

    req.setAttributesSize(attributeList.size());
    uint32_t i = 0;
    for (rti1516::AttributeHandleSet::const_iterator it = attributeList.begin(); it != attributeList.end(); ++it, ++i) {
        req.setAttributes(rti1516::AttributeHandleFriend::toCertiHandle(*it), i);
    }
    req.setActive(active);

    privateRefs->executeService(&req, &rep);
    Debug(G, pdGendoc) << "exit  RTI1516ambassador::subscribeObjectClassAttributes" << std::endl;
}

// 5.7
void RTI1516ambassador::unsubscribeObjectClass(rti1516::ObjectClassHandle theClass) throw(
    rti1516::ObjectClassNotDefined,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    M_Unsubscribe_Object_Class req, rep;

    const certi::ObjectClassHandle objectClassHandle = rti1516::ObjectClassHandleFriend::toCertiHandle(theClass);
    req.setObjectClass(objectClassHandle);

    privateRefs->executeService(&req, &rep);
}

void RTI1516ambassador::unsubscribeObjectClassAttributes(
    rti1516::ObjectClassHandle /*theClass*/,
    rti1516::AttributeHandleSet const& /*attributeList*/) throw(rti1516::ObjectClassNotDefined,
                                                            rti1516::AttributeNotDefined,
                                                            rti1516::FederateNotExecutionMember,
                                                            rti1516::SaveInProgress,
                                                            rti1516::RestoreInProgress,
                                                            rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

// 5.8
void RTI1516ambassador::subscribeInteractionClass(rti1516::InteractionClassHandle theClass, bool /*active*/) throw(
    rti1516::InteractionClassNotDefined,
    rti1516::FederateServiceInvocationsAreBeingReportedViaMOM,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    M_Subscribe_Interaction_Class req, rep;
    const certi::InteractionClassHandle classHandle = rti1516::InteractionClassHandleFriend::toCertiHandle(theClass);
    req.setInteractionClass(classHandle);

    privateRefs->executeService(&req, &rep);
}

// 5.9
void RTI1516ambassador::unsubscribeInteractionClass(rti1516::InteractionClassHandle theClass) throw(
    rti1516::InteractionClassNotDefined,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    M_Unsubscribe_Interaction_Class req, rep;

    const certi::InteractionClassHandle classHandle = rti1516::InteractionClassHandleFriend::toCertiHandle(theClass);
    req.setInteractionClass(classHandle);

    privateRefs->executeService(&req, &rep);
}

////////////////////////////////
// Object Management Services //
////////////////////////////////

// 6.2
void RTI1516ambassador::reserveObjectInstanceName(std::wstring const& theObjectInstanceName) throw(
    rti1516::IllegalName,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    M_Reserve_Object_Instance_Name req, rep;

    std::string objInstanceName(theObjectInstanceName.begin(), theObjectInstanceName.end());
    req.setObjectName(objInstanceName);
    privateRefs->executeService(&req, &rep);
}

// 6.4
rti1516::ObjectInstanceHandle RTI1516ambassador::registerObjectInstance(rti1516::ObjectClassHandle theClass) throw(
    rti1516::ObjectClassNotDefined,
    rti1516::ObjectClassNotPublished,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    M_Register_Object_Instance req, rep;

    req.setObjectClass(rti1516::ObjectClassHandleFriend::toCertiHandle(theClass));
    privateRefs->executeService(&req, &rep);
    return rti1516::ObjectInstanceHandleFriend::createRTI1516Handle(rep.getObject());
}

rti1516::ObjectInstanceHandle RTI1516ambassador::registerObjectInstance(
    rti1516::ObjectClassHandle theClass,
    std::wstring const& theObjectInstanceName) throw(rti1516::ObjectClassNotDefined,
                                                     rti1516::ObjectClassNotPublished,
                                                     rti1516::ObjectInstanceNameNotReserved,
                                                     rti1516::ObjectInstanceNameInUse,
                                                     rti1516::FederateNotExecutionMember,
                                                     rti1516::SaveInProgress,
                                                     rti1516::RestoreInProgress,
                                                     rti1516::RTIinternalError)
{
    M_Register_Object_Instance req, rep;

    std::string nameString(theObjectInstanceName.begin(), theObjectInstanceName.end());
    req.setObjectName(nameString);
    req.setObjectClass(rti1516::ObjectClassHandleFriend::toCertiHandle(theClass));
    privateRefs->executeService(&req, &rep);

    return rti1516::ObjectInstanceHandleFriend::createRTI1516Handle(rep.getObject());
}

// 6.6
void RTI1516ambassador::updateAttributeValues(
    rti1516::ObjectInstanceHandle theObject,
    rti1516::AttributeHandleValueMap const& theAttributeValues,
    rti1516::VariableLengthData const& theUserSuppliedTag) throw(rti1516::ObjectInstanceNotKnown,
                                                                 rti1516::AttributeNotDefined,
                                                                 rti1516::AttributeNotOwned,
                                                                 rti1516::FederateNotExecutionMember,
                                                                 rti1516::SaveInProgress,
                                                                 rti1516::RestoreInProgress,
                                                                 rti1516::RTIinternalError)
{
    Debug(G, pdGendoc) << "enter RTI1516ambassador::updateAttributeValues without time" << std::endl;
    M_Update_Attribute_Values req, rep;

    req.setObject(rti1516::ObjectInstanceHandleFriend::toCertiHandle(theObject));
    if (theUserSuppliedTag.data() == NULL) {
        throw rti1516::RTIinternalError(L"Calling updateAttributeValues with Tag NULL");
    }

    req.setTag(varLengthDataAsString(theUserSuppliedTag));

    assignAHVMAndExecuteService(theAttributeValues, req, rep);

    Debug(G, pdGendoc) << "exit  RTI1516ambassador::updateAttributeValues without time" << std::endl;
}

rti1516::MessageRetractionHandle
RTI1516ambassador::updateAttributeValues(rti1516::ObjectInstanceHandle theObject,
                                         rti1516::AttributeHandleValueMap const& theAttributeValues,
                                         rti1516::VariableLengthData const& theUserSuppliedTag,
                                         rti1516::LogicalTime const& theTime) throw(rti1516::ObjectInstanceNotKnown,
                                                                                    rti1516::AttributeNotDefined,
                                                                                    rti1516::AttributeNotOwned,
                                                                                    rti1516::InvalidLogicalTime,
                                                                                    rti1516::FederateNotExecutionMember,
                                                                                    rti1516::SaveInProgress,
                                                                                    rti1516::RestoreInProgress,
                                                                                    rti1516::RTIinternalError)
{
    Debug(G, pdGendoc) << "enter RTI1516ambassador::updateAttributeValues with time" << std::endl;
    M_Update_Attribute_Values req, rep;

    req.setObject(rti1516::ObjectInstanceHandleFriend::toCertiHandle(theObject));

    certi::FederationTime certiFedTime(certi_cast<RTI1516fedTime>()(theTime).getFedTime());
    req.setDate(certiFedTime);

    if (theUserSuppliedTag.data() == NULL) {
        throw rti1516::RTIinternalError(L"Calling updateAttributeValues with Tag NULL");
    }

    req.setTag(varLengthDataAsString(theUserSuppliedTag));

    assignAHVMAndExecuteService(theAttributeValues, req, rep);

    Debug(G, pdGendoc) << "return  RTI1516ambassador::updateAttributeValues with time" << std::endl;
    certi::FederateHandle certiHandle = rep.getEventRetraction().getSendingFederate();
    uint64_t serialNum = rep.getEventRetraction().getSN();
    return rti1516::MessageRetractionHandleFriend::createRTI1516Handle(certiHandle, serialNum);
}

// 6.8
void RTI1516ambassador::sendInteraction(
    rti1516::InteractionClassHandle theInteraction,
    rti1516::ParameterHandleValueMap const& theParameterValues,
    rti1516::VariableLengthData const& theUserSuppliedTag) throw(rti1516::InteractionClassNotPublished,
                                                                 rti1516::InteractionClassNotDefined,
                                                                 rti1516::InteractionParameterNotDefined,
                                                                 rti1516::FederateNotExecutionMember,
                                                                 rti1516::SaveInProgress,
                                                                 rti1516::RestoreInProgress,
                                                                 rti1516::RTIinternalError)
{
    M_Send_Interaction req, rep;

    const certi::InteractionClassHandle classHandle
        = rti1516::InteractionClassHandleFriend::toCertiHandle(theInteraction);
    req.setInteractionClass(classHandle);

    if (theUserSuppliedTag.data() == NULL) {
        throw rti1516::RTIinternalError(L"Calling sendIntercation with Tag NULL");
    }

    req.setTag(varLengthDataAsString(theUserSuppliedTag));
    req.setRegion(0);

    assignPHVMAndExecuteService(theParameterValues, req, rep);
}

rti1516::MessageRetractionHandle
RTI1516ambassador::sendInteraction(rti1516::InteractionClassHandle theInteraction,
                                   rti1516::ParameterHandleValueMap const& theParameterValues,
                                   rti1516::VariableLengthData const& theUserSuppliedTag,
                                   rti1516::LogicalTime const& theTime) throw(rti1516::InteractionClassNotPublished,
                                                                              rti1516::InteractionClassNotDefined,
                                                                              rti1516::InteractionParameterNotDefined,
                                                                              rti1516::InvalidLogicalTime,
                                                                              rti1516::FederateNotExecutionMember,
                                                                              rti1516::SaveInProgress,
                                                                              rti1516::RestoreInProgress,
                                                                              rti1516::RTIinternalError)
{
    M_Send_Interaction req, rep;

    const certi::InteractionClassHandle classHandle
        = rti1516::InteractionClassHandleFriend::toCertiHandle(theInteraction);
    req.setInteractionClass(classHandle);

    certi::FederationTime certiFedTime(certi_cast<RTI1516fedTime>()(theTime).getFedTime());
    req.setDate(certiFedTime);

    if (theUserSuppliedTag.data() == NULL) {
        throw rti1516::RTIinternalError(L"Calling sendInteraction with Tag NULL");
    }

    req.setTag(varLengthDataAsString(theUserSuppliedTag));
    req.setRegion(0);

    assignPHVMAndExecuteService(theParameterValues, req, rep);

    certi::FederateHandle certiHandle = rep.getEventRetraction().getSendingFederate();
    uint64_t serialNr = rep.getEventRetraction().getSN();
    rti1516::MessageRetractionHandle rti1516handle
        = rti1516::MessageRetractionHandleFriend::createRTI1516Handle(certiHandle, serialNr);

    return rti1516handle;
}

// 6.10
void RTI1516ambassador::deleteObjectInstance(
    rti1516::ObjectInstanceHandle theObject,
    rti1516::VariableLengthData const& theUserSuppliedTag) throw(rti1516::DeletePrivilegeNotHeld,
                                                                 rti1516::ObjectInstanceNotKnown,
                                                                 rti1516::FederateNotExecutionMember,
                                                                 rti1516::SaveInProgress,
                                                                 rti1516::RestoreInProgress,
                                                                 rti1516::RTIinternalError)
{
    M_Delete_Object_Instance req, rep;

    req.setObject(rti1516::ObjectInstanceHandleFriend::toCertiHandle(theObject));
    if (theUserSuppliedTag.data() == NULL) {
        throw rti1516::RTIinternalError(L"Calling deleteObjectInstance with Tag NULL");
    }

    req.setTag(varLengthDataAsString(theUserSuppliedTag));

    privateRefs->executeService(&req, &rep);
}

rti1516::MessageRetractionHandle
RTI1516ambassador::deleteObjectInstance(rti1516::ObjectInstanceHandle theObject,
                                        rti1516::VariableLengthData const& theUserSuppliedTag,
                                        rti1516::LogicalTime const& theTime) throw(rti1516::DeletePrivilegeNotHeld,
                                                                                   rti1516::ObjectInstanceNotKnown,
                                                                                   rti1516::InvalidLogicalTime,
                                                                                   rti1516::FederateNotExecutionMember,
                                                                                   rti1516::SaveInProgress,
                                                                                   rti1516::RestoreInProgress,
                                                                                   rti1516::RTIinternalError)
{
    M_Delete_Object_Instance req, rep;

    req.setObject(rti1516::ObjectInstanceHandleFriend::toCertiHandle(theObject));

    certi::FederationTime certiFedTime(certi_cast<RTI1516fedTime>()(theTime).getFedTime());
    req.setDate(certiFedTime);

    if (theUserSuppliedTag.data() == NULL) {
        throw rti1516::RTIinternalError(L"Calling deleteObjectInstance with Tag NULL");
    }

    req.setTag(varLengthDataAsString(theUserSuppliedTag));

    privateRefs->executeService(&req, &rep);

    certi::FederateHandle certiHandle = rep.getEventRetraction().getSendingFederate();
    uint64_t serialNum = rep.getEventRetraction().getSN();
    return rti1516::MessageRetractionHandleFriend::createRTI1516Handle(certiHandle, serialNum);
}

// 6.12
void RTI1516ambassador::localDeleteObjectInstance(rti1516::ObjectInstanceHandle theObject) throw(
    rti1516::ObjectInstanceNotKnown,
    rti1516::FederateOwnsAttributes,
    rti1516::OwnershipAcquisitionPending,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    throw rti1516::RTIinternalError(L"unimplemented service localDeleteObjectInstance");
    M_Local_Delete_Object_Instance req, rep;

    req.setObject(rti1516::ObjectInstanceHandleFriend::toCertiHandle(theObject));
    privateRefs->executeService(&req, &rep);
}

// 6.13
void RTI1516ambassador::changeAttributeTransportationType(
    rti1516::ObjectInstanceHandle theObject,
    rti1516::AttributeHandleSet const& theAttributes,
    rti1516::TransportationType theType) throw(rti1516::ObjectInstanceNotKnown,
                                               rti1516::AttributeNotDefined,
                                               rti1516::AttributeNotOwned,
                                               rti1516::FederateNotExecutionMember,
                                               rti1516::SaveInProgress,
                                               rti1516::RestoreInProgress,
                                               rti1516::RTIinternalError)
{
    M_Change_Attribute_Transportation_Type req, rep;
    req.setObject(rti1516::ObjectInstanceHandleFriend::toCertiHandle(theObject));
    req.setTransportationType(toCertiTransportationType(theType));

    req.setAttributesSize(theAttributes.size());
    uint32_t i = 0;
    for (rti1516::AttributeHandleSet::const_iterator it = theAttributes.begin(); it != theAttributes.end(); ++it, ++i) {
        req.setAttributes(rti1516::AttributeHandleFriend::toCertiHandle(*it), i);
    }

    privateRefs->executeService(&req, &rep);
}

// 6.14
void RTI1516ambassador::changeInteractionTransportationType(
    rti1516::InteractionClassHandle theClass,
    rti1516::TransportationType theType) throw(rti1516::InteractionClassNotDefined,
                                               rti1516::InteractionClassNotPublished,
                                               rti1516::FederateNotExecutionMember,
                                               rti1516::SaveInProgress,
                                               rti1516::RestoreInProgress,
                                               rti1516::RTIinternalError)
{
    M_Change_Interaction_Transportation_Type req, rep;

    req.setInteractionClass(rti1516::InteractionClassHandleFriend::toCertiHandle(theClass));
    req.setTransportationType(toCertiTransportationType(theType));

    privateRefs->executeService(&req, &rep);
}

// 6.17
void RTI1516ambassador::requestAttributeValueUpdate(
    rti1516::ObjectInstanceHandle theObject,
    rti1516::AttributeHandleSet const& theAttributes,
    rti1516::VariableLengthData const& theUserSuppliedTag) throw(rti1516::ObjectInstanceNotKnown,
                                                                 rti1516::AttributeNotDefined,
                                                                 rti1516::FederateNotExecutionMember,
                                                                 rti1516::SaveInProgress,
                                                                 rti1516::RestoreInProgress,
                                                                 rti1516::RTIinternalError)
{
    M_Request_Object_Attribute_Value_Update req, rep;

    Debug(G, pdGendoc) << "enter RTI1516ambassador::requestObjectAttributeValueUpdate" << std::endl;
    req.setObject(rti1516::ObjectInstanceHandleFriend::toCertiHandle(theObject));

    size_t attr_num = theAttributes.size();
    req.setAttributesSize(attr_num);
    uint32_t i = 0;
    for (rti1516::AttributeHandleSet::const_iterator it = theAttributes.begin(); i < attr_num; ++it, ++i) {
        req.setAttributes(rti1516::AttributeHandleFriend::toCertiHandle(*it), i);
    }
    req.setTag(varLengthDataAsString(theUserSuppliedTag));

    privateRefs->executeService(&req, &rep);
    Debug(G, pdGendoc) << "exit  RTI1516ambassador::requestObjectAttributeValueUpdate" << std::endl;
}

void RTI1516ambassador::requestAttributeValueUpdate(
    rti1516::ObjectClassHandle theClass,
    rti1516::AttributeHandleSet const& theAttributes,
    rti1516::VariableLengthData const& /*theUserSuppliedTag*/) throw(rti1516::ObjectClassNotDefined,
                                                                 rti1516::AttributeNotDefined,
                                                                 rti1516::FederateNotExecutionMember,
                                                                 rti1516::SaveInProgress,
                                                                 rti1516::RestoreInProgress,
                                                                 rti1516::RTIinternalError)
{
    M_Request_Class_Attribute_Value_Update req, rep;
    Debug(G, pdGendoc) << "enter RTI1516ambassador::requestClassAttributeValueUpdate" << std::endl;
    req.setObjectClass(rti1516::ObjectClassHandleFriend::toCertiHandle(theClass));

    assignAHSAndExecuteService(theAttributes, req, rep);

    Debug(G, pdGendoc) << "exit RTI1516ambassador::requestClassAttributeValueUpdate" << std::endl;
}

///////////////////////////////////
// Ownership Management Services //
///////////////////////////////////
// 7.2
void RTI1516ambassador::unconditionalAttributeOwnershipDivestiture(
    rti1516::ObjectInstanceHandle theObject,
    rti1516::AttributeHandleSet const& theAttributes) throw(rti1516::ObjectInstanceNotKnown,
                                                            rti1516::AttributeNotDefined,
                                                            rti1516::AttributeNotOwned,
                                                            rti1516::FederateNotExecutionMember,
                                                            rti1516::SaveInProgress,
                                                            rti1516::RestoreInProgress,
                                                            rti1516::RTIinternalError)
{
    M_Unconditional_Attribute_Ownership_Divestiture req, rep;
    req.setObject(rti1516::ObjectInstanceHandleFriend::toCertiHandle(theObject));

    req.setAttributesSize(theAttributes.size());
    uint32_t i = 0;
    for (rti1516::AttributeHandleSet::const_iterator it = theAttributes.begin(); it != theAttributes.end(); ++it, ++i) {
        req.setAttributes(rti1516::AttributeHandleFriend::toCertiHandle(*it), i);
    }

    privateRefs->executeService(&req, &rep);
}

// 7.3
void RTI1516ambassador::negotiatedAttributeOwnershipDivestiture(
    rti1516::ObjectInstanceHandle theObject,
    rti1516::AttributeHandleSet const& theAttributes,
    rti1516::VariableLengthData const& theUserSuppliedTag) throw(rti1516::ObjectInstanceNotKnown,
                                                                 rti1516::AttributeNotDefined,
                                                                 rti1516::AttributeNotOwned,
                                                                 rti1516::AttributeAlreadyBeingDivested,
                                                                 rti1516::FederateNotExecutionMember,
                                                                 rti1516::SaveInProgress,
                                                                 rti1516::RestoreInProgress,
                                                                 rti1516::RTIinternalError)
{
    M_Negotiated_Attribute_Ownership_Divestiture req, rep;
    req.setObject(rti1516::ObjectInstanceHandleFriend::toCertiHandle(theObject));
    if (theUserSuppliedTag.data() == NULL) {
        throw rti1516::RTIinternalError(L"Calling negotiatedAttributeOwnershipDivestiture with Tag NULL");
    }
    req.setTag(certi::varLengthDataAsString(theUserSuppliedTag));

    req.setAttributesSize(theAttributes.size());
    uint32_t i = 0;
    for (rti1516::AttributeHandleSet::const_iterator it = theAttributes.begin(); it != theAttributes.end(); ++it, ++i) {
        req.setAttributes(rti1516::AttributeHandleFriend::toCertiHandle(*it), i);
    }

    privateRefs->executeService(&req, &rep);
}

// 7.6
void RTI1516ambassador::confirmDivestiture(
    rti1516::ObjectInstanceHandle /*theObject*/,
    rti1516::AttributeHandleSet const& /*confirmedAttributes*/,
    rti1516::VariableLengthData const& /*theUserSuppliedTag*/) throw(rti1516::ObjectInstanceNotKnown,
                                                                 rti1516::AttributeNotDefined,
                                                                 rti1516::AttributeNotOwned,
                                                                 rti1516::AttributeDivestitureWasNotRequested,
                                                                 rti1516::NoAcquisitionPending,
                                                                 rti1516::FederateNotExecutionMember,
                                                                 rti1516::SaveInProgress,
                                                                 rti1516::RestoreInProgress,
                                                                 rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

// 7.8
void RTI1516ambassador::attributeOwnershipAcquisition(
    rti1516::ObjectInstanceHandle theObject,
    rti1516::AttributeHandleSet const& desiredAttributes,
    rti1516::VariableLengthData const& theUserSuppliedTag) throw(rti1516::ObjectInstanceNotKnown,
                                                                 rti1516::ObjectClassNotPublished,
                                                                 rti1516::AttributeNotDefined,
                                                                 rti1516::AttributeNotPublished,
                                                                 rti1516::FederateOwnsAttributes,
                                                                 rti1516::FederateNotExecutionMember,
                                                                 rti1516::SaveInProgress,
                                                                 rti1516::RestoreInProgress,
                                                                 rti1516::RTIinternalError)
{
    M_Attribute_Ownership_Acquisition req, rep;

    req.setObject(rti1516::ObjectInstanceHandleFriend::toCertiHandle(theObject));
    if (theUserSuppliedTag.data() == NULL) {
        throw rti1516::RTIinternalError(L"Calling attributeOwnershipAcquisition with Tag NULL");
    }
    req.setTag(certi::varLengthDataAsString(theUserSuppliedTag));

    req.setAttributesSize(desiredAttributes.size());
    uint32_t i = 0;
    for (rti1516::AttributeHandleSet::const_iterator it = desiredAttributes.begin(); it != desiredAttributes.end();
         ++it, ++i) {
        req.setAttributes(rti1516::AttributeHandleFriend::toCertiHandle(*it), i);
    }

    privateRefs->executeService(&req, &rep);
}

// 7.9
void RTI1516ambassador::attributeOwnershipAcquisitionIfAvailable(
    rti1516::ObjectInstanceHandle theObject,
    rti1516::AttributeHandleSet const& desiredAttributes) throw(rti1516::ObjectInstanceNotKnown,
                                                                rti1516::ObjectClassNotPublished,
                                                                rti1516::AttributeNotDefined,
                                                                rti1516::AttributeNotPublished,
                                                                rti1516::FederateOwnsAttributes,
                                                                rti1516::AttributeAlreadyBeingAcquired,
                                                                rti1516::FederateNotExecutionMember,
                                                                rti1516::SaveInProgress,
                                                                rti1516::RestoreInProgress,
                                                                rti1516::RTIinternalError)
{
    M_Attribute_Ownership_Acquisition_If_Available req, rep;

    req.setObject(rti1516::ObjectInstanceHandleFriend::toCertiHandle(theObject));

    req.setAttributesSize(desiredAttributes.size());
    uint32_t i = 0;
    for (rti1516::AttributeHandleSet::const_iterator it = desiredAttributes.begin(); it != desiredAttributes.end();
         ++it, ++i) {
        req.setAttributes(rti1516::AttributeHandleFriend::toCertiHandle(*it), i);
    }

    privateRefs->executeService(&req, &rep);
}

// 7.12 (in RTI1.3 this function is called: AttributeOwnershipReleaseResponse)
void RTI1516ambassador::attributeOwnershipDivestitureIfWanted(
    rti1516::ObjectInstanceHandle theObject,
    rti1516::AttributeHandleSet const& theAttributes,
    rti1516::AttributeHandleSet& theDivestedAttributes) // filled by RTI
    throw(rti1516::ObjectInstanceNotKnown,
          rti1516::AttributeNotDefined,
          rti1516::AttributeNotOwned,
          rti1516::FederateNotExecutionMember,
          rti1516::SaveInProgress,
          rti1516::RestoreInProgress,
          rti1516::RTIinternalError)
{
    M_Attribute_Ownership_Release_Response req, rep;

    req.setObject(rti1516::ObjectInstanceHandleFriend::toCertiHandle(theObject));

    assignAHSAndExecuteService(theAttributes, req, rep);

    if (rep.getExceptionType() == Exception::Type::NO_EXCEPTION) {
        theDivestedAttributes.clear();
        for (uint32_t i = 0; i < rep.getAttributesSize(); ++i) {
            theDivestedAttributes.insert(rti1516::AttributeHandleFriend::createRTI1516Handle(rep.getAttributes()[i]));
        }
    }
}

// 7.13
void RTI1516ambassador::cancelNegotiatedAttributeOwnershipDivestiture(
    rti1516::ObjectInstanceHandle theObject,
    rti1516::AttributeHandleSet const& theAttributes) throw(rti1516::ObjectInstanceNotKnown,
                                                            rti1516::AttributeNotDefined,
                                                            rti1516::AttributeNotOwned,
                                                            rti1516::AttributeDivestitureWasNotRequested,
                                                            rti1516::FederateNotExecutionMember,
                                                            rti1516::SaveInProgress,
                                                            rti1516::RestoreInProgress,
                                                            rti1516::RTIinternalError)
{
    M_Cancel_Negotiated_Attribute_Ownership_Divestiture req, rep;

    req.setObject(rti1516::ObjectInstanceHandleFriend::toCertiHandle(theObject));

    req.setAttributesSize(theAttributes.size());
    uint32_t i = 0;
    for (rti1516::AttributeHandleSet::const_iterator it = theAttributes.begin(); it != theAttributes.end(); ++it, ++i) {
        req.setAttributes(rti1516::AttributeHandleFriend::toCertiHandle(*it), i);
    }

    privateRefs->executeService(&req, &rep);
}

// 7.14
void RTI1516ambassador::cancelAttributeOwnershipAcquisition(
    rti1516::ObjectInstanceHandle theObject,
    rti1516::AttributeHandleSet const& theAttributes) throw(rti1516::ObjectInstanceNotKnown,
                                                            rti1516::AttributeNotDefined,
                                                            rti1516::AttributeAlreadyOwned,
                                                            rti1516::AttributeAcquisitionWasNotRequested,
                                                            rti1516::FederateNotExecutionMember,
                                                            rti1516::SaveInProgress,
                                                            rti1516::RestoreInProgress,
                                                            rti1516::RTIinternalError)
{
    M_Cancel_Attribute_Ownership_Acquisition req, rep;

    req.setObject(rti1516::ObjectInstanceHandleFriend::toCertiHandle(theObject));

    req.setAttributesSize(theAttributes.size());
    uint32_t i = 0;
    for (rti1516::AttributeHandleSet::const_iterator it = theAttributes.begin(); it != theAttributes.end(); ++it, ++i) {
        req.setAttributes(rti1516::AttributeHandleFriend::toCertiHandle(*it), i);
    }

    privateRefs->executeService(&req, &rep);
}

// 7.16
void RTI1516ambassador::queryAttributeOwnership(
    rti1516::ObjectInstanceHandle theObject,
    rti1516::AttributeHandle theAttribute) throw(rti1516::ObjectInstanceNotKnown,
                                                 rti1516::AttributeNotDefined,
                                                 rti1516::FederateNotExecutionMember,
                                                 rti1516::SaveInProgress,
                                                 rti1516::RestoreInProgress,
                                                 rti1516::RTIinternalError)
{
    M_Query_Attribute_Ownership req, rep;

    req.setObject(rti1516::ObjectInstanceHandleFriend::toCertiHandle(theObject));
    req.setAttribute(rti1516::AttributeHandleFriend::toCertiHandle(theAttribute));

    privateRefs->executeService(&req, &rep);
}

// 7.18
bool RTI1516ambassador::isAttributeOwnedByFederate(
    rti1516::ObjectInstanceHandle theObject,
    rti1516::AttributeHandle theAttribute) throw(rti1516::ObjectInstanceNotKnown,
                                                 rti1516::AttributeNotDefined,
                                                 rti1516::FederateNotExecutionMember,
                                                 rti1516::SaveInProgress,
                                                 rti1516::RestoreInProgress,
                                                 rti1516::RTIinternalError)
{
    M_Is_Attribute_Owned_By_Federate req, rep;

    req.setObject(rti1516::ObjectInstanceHandleFriend::toCertiHandle(theObject));
    req.setAttribute(rti1516::AttributeHandleFriend::toCertiHandle(theAttribute));

    privateRefs->executeService(&req, &rep);

    return (rep.getTag() == "RTI_TRUE") ? true : false;
}

//////////////////////////////
// Time Management Services //
//////////////////////////////

// 8.2
void RTI1516ambassador::enableTimeRegulation(rti1516::LogicalTimeInterval const& theLookahead) throw(
    rti1516::TimeRegulationAlreadyEnabled,
    rti1516::InvalidLookahead,
    rti1516::InTimeAdvancingState,
    rti1516::RequestForTimeRegulationPending,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    M_Enable_Time_Regulation req, rep;

    //req.setDate(certi_cast<RTIfedTime>()(theFederateTime).getTime());  //JRE: DATE IS NOT USED!

    //JRE: is dit wel goed?
    //JvY: TODO Controleren of dit blijft werken met andere tijdsimplementaties
    union ud {
        double dv;
        uint64_t uv;
    } value;
#ifdef HOST_IS_BIG_ENDIAN
    memcpy(&(value.uv), theLookahead.encode().data(), sizeof(double));
#else
    value.uv = CERTI_DECODE_DOUBLE_FROM_UINT64BE(theLookahead.encode().data());
#endif
    double lookAheadTime = value.dv;
    req.setLookahead(lookAheadTime);
    privateRefs->executeService(&req, &rep);
}

// 8.4
void RTI1516ambassador::disableTimeRegulation() throw(rti1516::TimeRegulationIsNotEnabled,
                                                      rti1516::FederateNotExecutionMember,
                                                      rti1516::SaveInProgress,
                                                      rti1516::RestoreInProgress,
                                                      rti1516::RTIinternalError)
{
    M_Disable_Time_Regulation req, rep;

    privateRefs->executeService(&req, &rep);
}

// 8.5
void RTI1516ambassador::enableTimeConstrained() throw(rti1516::TimeConstrainedAlreadyEnabled,
                                                      rti1516::InTimeAdvancingState,
                                                      rti1516::RequestForTimeConstrainedPending,
                                                      rti1516::FederateNotExecutionMember,
                                                      rti1516::SaveInProgress,
                                                      rti1516::RestoreInProgress,
                                                      rti1516::RTIinternalError)
{
    M_Enable_Time_Constrained req, rep;
    privateRefs->executeService(&req, &rep);
}

// 8.7
void RTI1516ambassador::disableTimeConstrained() throw(rti1516::TimeConstrainedIsNotEnabled,
                                                       rti1516::FederateNotExecutionMember,
                                                       rti1516::SaveInProgress,
                                                       rti1516::RestoreInProgress,
                                                       rti1516::RTIinternalError)
{
    M_Disable_Time_Constrained req, rep;
    privateRefs->executeService(&req, &rep);
}

// 8.8
void RTI1516ambassador::timeAdvanceRequest(rti1516::LogicalTime const& theTime) throw(
    rti1516::InvalidLogicalTime,
    rti1516::LogicalTimeAlreadyPassed,
    rti1516::InTimeAdvancingState,
    rti1516::RequestForTimeRegulationPending,
    rti1516::RequestForTimeConstrainedPending,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    M_Time_Advance_Request req, rep;

    certi::FederationTime certiFedTime(certi_cast<RTI1516fedTime>()(theTime).getFedTime());
    req.setDate(certiFedTime);
    privateRefs->executeService(&req, &rep);
}

// 8.9
void RTI1516ambassador::timeAdvanceRequestAvailable(rti1516::LogicalTime const& theTime) throw(
    rti1516::InvalidLogicalTime,
    rti1516::LogicalTimeAlreadyPassed,
    rti1516::InTimeAdvancingState,
    rti1516::RequestForTimeRegulationPending,
    rti1516::RequestForTimeConstrainedPending,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    M_Time_Advance_Request_Available req, rep;

    certi::FederationTime certiFedTime(certi_cast<RTI1516fedTime>()(theTime).getFedTime());
    req.setDate(certiFedTime);

    privateRefs->executeService(&req, &rep);
}

// 8.10
void RTI1516ambassador::nextMessageRequest(rti1516::LogicalTime const& theTime) throw(
    rti1516::InvalidLogicalTime,
    rti1516::LogicalTimeAlreadyPassed,
    rti1516::InTimeAdvancingState,
    rti1516::RequestForTimeRegulationPending,
    rti1516::RequestForTimeConstrainedPending,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    M_Next_Event_Request req, rep;

    certi::FederationTime certiFedTime(certi_cast<RTI1516fedTime>()(theTime).getFedTime());
    req.setDate(certiFedTime);

    privateRefs->executeService(&req, &rep);
}

// 8.11
void RTI1516ambassador::nextMessageRequestAvailable(rti1516::LogicalTime const& theTime) throw(
    rti1516::InvalidLogicalTime,
    rti1516::LogicalTimeAlreadyPassed,
    rti1516::InTimeAdvancingState,
    rti1516::RequestForTimeRegulationPending,
    rti1516::RequestForTimeConstrainedPending,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    M_Next_Event_Request_Available req, rep;

    certi::FederationTime certiFedTime(certi_cast<RTI1516fedTime>()(theTime).getFedTime());
    req.setDate(certiFedTime);

    privateRefs->executeService(&req, &rep);
}

// 8.12
void RTI1516ambassador::flushQueueRequest(rti1516::LogicalTime const& theTime) throw(
    rti1516::InvalidLogicalTime,
    rti1516::LogicalTimeAlreadyPassed,
    rti1516::InTimeAdvancingState,
    rti1516::RequestForTimeRegulationPending,
    rti1516::RequestForTimeConstrainedPending,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    // JvY: Implementation copied from previous CERTI implementation, including immediate throw.
    throw rti1516::RTIinternalError(L"Unimplemented Service flushQueueRequest");
    M_Flush_Queue_Request req, rep;

    certi::FederationTime certiFedTime(certi_cast<RTI1516fedTime>()(theTime).getFedTime());
    req.setDate(certiFedTime);

    privateRefs->executeService(&req, &rep);
}

// 8.14
void RTI1516ambassador::enableAsynchronousDelivery() throw(rti1516::AsynchronousDeliveryAlreadyEnabled,
                                                           rti1516::FederateNotExecutionMember,
                                                           rti1516::SaveInProgress,
                                                           rti1516::RestoreInProgress,
                                                           rti1516::RTIinternalError)
{
    // throw AsynchronousDeliveryAlreadyEnabled("Default value (non HLA)");

    M_Enable_Asynchronous_Delivery req, rep;

    privateRefs->executeService(&req, &rep);
}

// 8.15
void RTI1516ambassador::disableAsynchronousDelivery() throw(rti1516::AsynchronousDeliveryAlreadyDisabled,
                                                            rti1516::FederateNotExecutionMember,
                                                            rti1516::SaveInProgress,
                                                            rti1516::RestoreInProgress,
                                                            rti1516::RTIinternalError)
{
    M_Disable_Asynchronous_Delivery req, rep;

    privateRefs->executeService(&req, &rep);
}

// 8.16
bool RTI1516ambassador::queryGALT(rti1516::LogicalTime& theTime) throw(rti1516::FederateNotExecutionMember,
                                                                       rti1516::SaveInProgress,
                                                                       rti1516::RestoreInProgress,
                                                                       rti1516::RTIinternalError)
{
    //TODO JRE: goed testen! Is GALT wel precies het zelfde als LBTS?
    M_Query_Lbts req, rep;

    privateRefs->executeService(&req, &rep);

    //TODO JRE: goed testen of deze return value wel klopt!
    certi::FederationTime fedTime = rep.getDate();
    if (fedTime.getTime() == 0.0) {
        return false;
    }

    // JvY: TODO Controleren of dit blijft werken met andere tijdsimplementaties
    certi_cast<RTI1516fedTime>()(theTime) = rep.getDate().getTime();

    return true;
}

// 8.17
void RTI1516ambassador::queryLogicalTime(rti1516::LogicalTime& theTime) throw(rti1516::FederateNotExecutionMember,
                                                                              rti1516::SaveInProgress,
                                                                              rti1516::RestoreInProgress,
                                                                              rti1516::RTIinternalError)
{
    M_Query_Federate_Time req, rep;

    privateRefs->executeService(&req, &rep);

    // JvY: TODO Controleren of dit blijft werken met andere tijdsimplementaties
    certi_cast<RTI1516fedTime>()(theTime) = rep.getDate().getTime();
}

// 8.18
bool RTI1516ambassador::queryLITS(rti1516::LogicalTime& theTime) throw(rti1516::FederateNotExecutionMember,
                                                                       rti1516::SaveInProgress,
                                                                       rti1516::RestoreInProgress,
                                                                       rti1516::RTIinternalError)
{
    //TODO JRE: goed testen! Is LITS wel precies het zelfde als QueryMinNextEventTime?
    M_Query_Min_Next_Event_Time req, rep;

    privateRefs->executeService(&req, &rep);

    //TODO JRE: goed testen of deze return value wel klopt!
    certi::FederationTime fedTime = rep.getDate();
    if (fedTime.getTime() == 0.0) {
        return false;
    }
    // JvY: TODO Controleren of dit blijft werken met andere tijdsimplementaties
    certi_cast<RTI1516fedTime>()(theTime) = rep.getDate().getTime();

    return true;
}

// 8.19
void RTI1516ambassador::modifyLookahead(rti1516::LogicalTimeInterval const& /*theLookahead*/) throw(
    rti1516::TimeRegulationIsNotEnabled,
    rti1516::InvalidLookahead,
    rti1516::InTimeAdvancingState,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

// 8.20
void RTI1516ambassador::queryLookahead(rti1516::LogicalTimeInterval& /*interval*/) throw(
    rti1516::TimeRegulationIsNotEnabled,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

// 8.21
void RTI1516ambassador::retract(rti1516::MessageRetractionHandle theHandle) throw(
    rti1516::InvalidRetractionHandle,
    rti1516::TimeRegulationIsNotEnabled,
    rti1516::MessageCanNoLongerBeRetracted,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    throw rti1516::RTIinternalError(L"Unimplemented Service retract");
    M_Retract req, rep;

    certi::EventRetraction event = rti1516::MessageRetractionHandleFriend::createEventRetraction(theHandle);
    req.setEventRetraction(event);

    privateRefs->executeService(&req, &rep);
}

// 8.23
void RTI1516ambassador::changeAttributeOrderType(rti1516::ObjectInstanceHandle theObject,
                                                 rti1516::AttributeHandleSet const& theAttributes,
                                                 rti1516::OrderType theType) throw(rti1516::ObjectInstanceNotKnown,
                                                                                   rti1516::AttributeNotDefined,
                                                                                   rti1516::AttributeNotOwned,
                                                                                   rti1516::FederateNotExecutionMember,
                                                                                   rti1516::SaveInProgress,
                                                                                   rti1516::RestoreInProgress,
                                                                                   rti1516::RTIinternalError)
{
    M_Change_Attribute_Order_Type req, rep;

    req.setObject(rti1516::ObjectInstanceHandleFriend::toCertiHandle(theObject));
    req.setOrder(certi::toCertiOrderType(theType));

    assignAHSAndExecuteService(theAttributes, req, rep);
}

// 8.24
void RTI1516ambassador::changeInteractionOrderType(
    rti1516::InteractionClassHandle theClass, rti1516::OrderType theType) throw(rti1516::InteractionClassNotDefined,
                                                                                rti1516::InteractionClassNotPublished,
                                                                                rti1516::FederateNotExecutionMember,
                                                                                rti1516::SaveInProgress,
                                                                                rti1516::RestoreInProgress,
                                                                                rti1516::RTIinternalError)
{
    M_Change_Interaction_Order_Type req, rep;

    req.setInteractionClass(rti1516::InteractionClassHandleFriend::toCertiHandle(theClass));
    req.setOrder(certi::toCertiOrderType(theType));

    privateRefs->executeService(&req, &rep);
}

//////////////////////////////////
// Data Distribution Management //
//////////////////////////////////

// 9.2
rti1516::RegionHandle RTI1516ambassador::createRegion(rti1516::DimensionHandleSet const& /*theDimensions*/) throw(
    rti1516::InvalidDimensionHandle,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

// 9.3
void RTI1516ambassador::commitRegionModifications(rti1516::RegionHandleSet const& /*theRegionHandleSet*/) throw(
    rti1516::InvalidRegion,
    rti1516::RegionNotCreatedByThisFederate,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

// 9.4
void RTI1516ambassador::deleteRegion(rti1516::RegionHandle /*theRegion*/) throw(rti1516::InvalidRegion,
                                                                            rti1516::RegionNotCreatedByThisFederate,
                                                                            rti1516::RegionInUseForUpdateOrSubscription,
                                                                            rti1516::FederateNotExecutionMember,
                                                                            rti1516::SaveInProgress,
                                                                            rti1516::RestoreInProgress,
                                                                            rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

// 9.5
rti1516::ObjectInstanceHandle RTI1516ambassador::registerObjectInstanceWithRegions(
    rti1516::ObjectClassHandle /*theClass*/,
    rti1516::AttributeHandleSetRegionHandleSetPairVector const&
        /*theAttributeHandleSetRegionHandleSetPairVector*/) throw(rti1516::ObjectClassNotDefined,
                                                              rti1516::ObjectClassNotPublished,
                                                              rti1516::AttributeNotDefined,
                                                              rti1516::AttributeNotPublished,
                                                              rti1516::InvalidRegion,
                                                              rti1516::RegionNotCreatedByThisFederate,
                                                              rti1516::InvalidRegionContext,
                                                              rti1516::FederateNotExecutionMember,
                                                              rti1516::SaveInProgress,
                                                              rti1516::RestoreInProgress,
                                                              rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

rti1516::ObjectInstanceHandle RTI1516ambassador::registerObjectInstanceWithRegions(
    rti1516::ObjectClassHandle /*theClass*/,
    rti1516::AttributeHandleSetRegionHandleSetPairVector const& /*theAttributeHandleSetRegionHandleSetPairVector*/,
    std::wstring const& /*theObjectInstanceName*/) throw(rti1516::ObjectClassNotDefined,
                                                     rti1516::ObjectClassNotPublished,
                                                     rti1516::AttributeNotDefined,
                                                     rti1516::AttributeNotPublished,
                                                     rti1516::InvalidRegion,
                                                     rti1516::RegionNotCreatedByThisFederate,
                                                     rti1516::InvalidRegionContext,
                                                     rti1516::ObjectInstanceNameNotReserved,
                                                     rti1516::ObjectInstanceNameInUse,
                                                     rti1516::FederateNotExecutionMember,
                                                     rti1516::SaveInProgress,
                                                     rti1516::RestoreInProgress,
                                                     rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

// 9.6
void RTI1516ambassador::associateRegionsForUpdates(
    rti1516::ObjectInstanceHandle /*theObject*/,
    rti1516::AttributeHandleSetRegionHandleSetPairVector const&
        /*theAttributeHandleSetRegionHandleSetPairVector*/) throw(rti1516::ObjectInstanceNotKnown,
                                                              rti1516::AttributeNotDefined,
                                                              rti1516::InvalidRegion,
                                                              rti1516::RegionNotCreatedByThisFederate,
                                                              rti1516::InvalidRegionContext,
                                                              rti1516::FederateNotExecutionMember,
                                                              rti1516::SaveInProgress,
                                                              rti1516::RestoreInProgress,
                                                              rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

// 9.7
void RTI1516ambassador::unassociateRegionsForUpdates(
    rti1516::ObjectInstanceHandle /*theObject*/,
    rti1516::AttributeHandleSetRegionHandleSetPairVector const&
        /*theAttributeHandleSetRegionHandleSetPairVector*/) throw(rti1516::ObjectInstanceNotKnown,
                                                              rti1516::AttributeNotDefined,
                                                              rti1516::InvalidRegion,
                                                              rti1516::RegionNotCreatedByThisFederate,
                                                              rti1516::FederateNotExecutionMember,
                                                              rti1516::SaveInProgress,
                                                              rti1516::RestoreInProgress,
                                                              rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

// 9.8
void RTI1516ambassador::subscribeObjectClassAttributesWithRegions(
    rti1516::ObjectClassHandle /*theClass*/,
    rti1516::AttributeHandleSetRegionHandleSetPairVector const& /*theAttributeHandleSetRegionHandleSetPairVector*/,
    bool /*active*/) throw(rti1516::ObjectClassNotDefined,
                       rti1516::AttributeNotDefined,
                       rti1516::InvalidRegion,
                       rti1516::RegionNotCreatedByThisFederate,
                       rti1516::InvalidRegionContext,
                       rti1516::FederateNotExecutionMember,
                       rti1516::SaveInProgress,
                       rti1516::RestoreInProgress,
                       rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

// 9.9
void RTI1516ambassador::unsubscribeObjectClassAttributesWithRegions(
    rti1516::ObjectClassHandle /*theClass*/,
    rti1516::AttributeHandleSetRegionHandleSetPairVector const&
        /*theAttributeHandleSetRegionHandleSetPairVector*/) throw(rti1516::ObjectClassNotDefined,
                                                              rti1516::AttributeNotDefined,
                                                              rti1516::InvalidRegion,
                                                              rti1516::RegionNotCreatedByThisFederate,
                                                              rti1516::FederateNotExecutionMember,
                                                              rti1516::SaveInProgress,
                                                              rti1516::RestoreInProgress,
                                                              rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

// 9.10
void RTI1516ambassador::subscribeInteractionClassWithRegions(
    rti1516::InteractionClassHandle /*theClass*/,
    rti1516::RegionHandleSet const& /*theRegionHandleSet*/,
    bool /*active*/) throw(rti1516::InteractionClassNotDefined,
                       rti1516::InvalidRegion,
                       rti1516::RegionNotCreatedByThisFederate,
                       rti1516::InvalidRegionContext,
                       rti1516::FederateServiceInvocationsAreBeingReportedViaMOM,
                       rti1516::FederateNotExecutionMember,
                       rti1516::SaveInProgress,
                       rti1516::RestoreInProgress,
                       rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

// 9.11
void RTI1516ambassador::unsubscribeInteractionClassWithRegions(
    rti1516::InteractionClassHandle /*theClass*/,
    rti1516::RegionHandleSet const& /*theRegionHandleSet*/) throw(rti1516::InteractionClassNotDefined,
                                                              rti1516::InvalidRegion,
                                                              rti1516::RegionNotCreatedByThisFederate,
                                                              rti1516::FederateNotExecutionMember,
                                                              rti1516::SaveInProgress,
                                                              rti1516::RestoreInProgress,
                                                              rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

// 9.12
void RTI1516ambassador::sendInteractionWithRegions(
    rti1516::InteractionClassHandle /*theInteraction*/,
    rti1516::ParameterHandleValueMap const& /*theParameterValues*/,
    rti1516::RegionHandleSet const& /*theRegionHandleSet*/,
    rti1516::VariableLengthData const& /*theUserSuppliedTag*/) throw(rti1516::InteractionClassNotDefined,
                                                                 rti1516::InteractionClassNotPublished,
                                                                 rti1516::InteractionParameterNotDefined,
                                                                 rti1516::InvalidRegion,
                                                                 rti1516::RegionNotCreatedByThisFederate,
                                                                 rti1516::InvalidRegionContext,
                                                                 rti1516::FederateNotExecutionMember,
                                                                 rti1516::SaveInProgress,
                                                                 rti1516::RestoreInProgress,
                                                                 rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

rti1516::MessageRetractionHandle RTI1516ambassador::sendInteractionWithRegions(
    rti1516::InteractionClassHandle /*theInteraction*/,
    rti1516::ParameterHandleValueMap const& /*theParameterValues*/,
    rti1516::RegionHandleSet const& /*theRegionHandleSet*/,
    rti1516::VariableLengthData const& /*theUserSuppliedTag*/,
    rti1516::LogicalTime const& /*theTime*/) throw(rti1516::InteractionClassNotDefined,
                                               rti1516::InteractionClassNotPublished,
                                               rti1516::InteractionParameterNotDefined,
                                               rti1516::InvalidRegion,
                                               rti1516::RegionNotCreatedByThisFederate,
                                               rti1516::InvalidRegionContext,
                                               rti1516::InvalidLogicalTime,
                                               rti1516::FederateNotExecutionMember,
                                               rti1516::SaveInProgress,
                                               rti1516::RestoreInProgress,
                                               rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

// 9.13
void RTI1516ambassador::requestAttributeValueUpdateWithRegions(
    rti1516::ObjectClassHandle /*theClass*/,
    rti1516::AttributeHandleSetRegionHandleSetPairVector const& /*theSet*/,
    rti1516::VariableLengthData const& /*theUserSuppliedTag*/) throw(rti1516::ObjectClassNotDefined,
                                                                 rti1516::AttributeNotDefined,
                                                                 rti1516::InvalidRegion,
                                                                 rti1516::RegionNotCreatedByThisFederate,
                                                                 rti1516::InvalidRegionContext,
                                                                 rti1516::FederateNotExecutionMember,
                                                                 rti1516::SaveInProgress,
                                                                 rti1516::RestoreInProgress,
                                                                 rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

//////////////////////////
// RTI Support Services //
//////////////////////////

// 10.2
rti1516::ObjectClassHandle RTI1516ambassador::getObjectClassHandle(std::wstring const& theName) throw(
    rti1516::NameNotFound, rti1516::FederateNotExecutionMember, rti1516::RTIinternalError)
{
    M_Get_Object_Class_Handle req, rep;

    Debug(G, pdGendoc) << "enter RTI1516ambassador::getObjectClassHandle" << std::endl;

    std::string nameAsString(theName.begin(), theName.end());
    req.setClassName(nameAsString);
    privateRefs->executeService(&req, &rep);

    Debug(G, pdGendoc) << "exit RTI1516ambassador::getObjectClassHandle" << std::endl;
    rti1516::ObjectClassHandle rti1516Handle
        = rti1516::ObjectClassHandleFriend::createRTI1516Handle(rep.getObjectClass());

    return rti1516Handle;
}

// 10.3
std::wstring RTI1516ambassador::getObjectClassName(rti1516::ObjectClassHandle theHandle) throw(
    rti1516::InvalidObjectClassHandle, rti1516::FederateNotExecutionMember, rti1516::RTIinternalError)
{
    M_Get_Object_Class_Name req, rep;

    certi::ObjectClassHandle certiHandle = rti1516::ObjectClassHandleFriend::toCertiHandle(theHandle);
    req.setObjectClass(certiHandle);
    try {
        privateRefs->executeService(&req, &rep);
    }
    catch (rti1516::ObjectClassNotDefined& e) {
        throw rti1516::InvalidObjectClassHandle(e.what());
    }

    std::string nameString = rep.getClassName();
    std::wstring nameWString(nameString.begin(), nameString.end());

    //return hla_strdup(rep.getClassName());
    return nameWString;
}

// 10.4
rti1516::AttributeHandle
RTI1516ambassador::getAttributeHandle(rti1516::ObjectClassHandle whichClass,
                                      std::wstring const& theAttributeName) throw(rti1516::InvalidObjectClassHandle,
                                                                                  rti1516::NameNotFound,
                                                                                  rti1516::FederateNotExecutionMember,
                                                                                  rti1516::RTIinternalError)
{
    Debug(G, pdGendoc) << "enter RTI::RTI1516ambassador::getAttributeHandle" << std::endl;
    M_Get_Attribute_Handle req, rep;

    std::string nameAsString(theAttributeName.begin(), theAttributeName.end());
    req.setAttributeName(nameAsString);
    req.setObjectClass(rti1516::ObjectClassHandleFriend::toCertiHandle(whichClass));

    try {
        privateRefs->executeService(&req, &rep);
    }
    catch (rti1516::ObjectClassNotDefined& e) {
        if (!whichClass.isValid()) {
            throw rti1516::InvalidObjectClassHandle(e.what());
        }
        else {
            throw rti1516::NameNotFound(e.what());
        }
    }

    Debug(G, pdGendoc) << "exit  RTI::RTI1516ambassador::getAttributeHandle" << std::endl;
    return rti1516::AttributeHandleFriend::createRTI1516Handle(rep.getAttribute());
}

// 10.5
std::wstring
RTI1516ambassador::getAttributeName(rti1516::ObjectClassHandle whichClass,
                                    rti1516::AttributeHandle theHandle) throw(rti1516::InvalidObjectClassHandle,
                                                                              rti1516::InvalidAttributeHandle,
                                                                              rti1516::AttributeNotDefined,
                                                                              rti1516::FederateNotExecutionMember,
                                                                              rti1516::RTIinternalError)
{
    M_Get_Attribute_Name req, rep;

    req.setAttribute(rti1516::AttributeHandleFriend::toCertiHandle(theHandle));
    req.setObjectClass(rti1516::ObjectClassHandleFriend::toCertiHandle(whichClass));
    try {
        privateRefs->executeService(&req, &rep);
    }
    catch (rti1516::ObjectClassNotDefined& e) {
        if (!whichClass.isValid()) {
            throw rti1516::InvalidObjectClassHandle(e.what());
        }
        else {
            throw;
        }
    }
    catch (rti1516::AttributeNotDefined& e) {
        if (!theHandle.isValid()) {
            throw rti1516::InvalidAttributeHandle(e.what());
        }
        else {
            throw;
        }
    }

    //return hla_strdup(rep.getAttributeName());

    std::string nameString = rep.getAttributeName();
    std::wstring nameWString(nameString.begin(), nameString.end());

    return nameWString;
}

// 10.6
rti1516::InteractionClassHandle RTI1516ambassador::getInteractionClassHandle(std::wstring const& theName) throw(
    rti1516::NameNotFound, rti1516::FederateNotExecutionMember, rti1516::RTIinternalError)
{
    M_Get_Interaction_Class_Handle req, rep;
    std::string nameString(theName.begin(), theName.end());
    req.setClassName(nameString);

    privateRefs->executeService(&req, &rep);

    return rti1516::InteractionClassHandleFriend::createRTI1516Handle(rep.getInteractionClass());
}

// 10.7
std::wstring RTI1516ambassador::getInteractionClassName(rti1516::InteractionClassHandle theHandle) throw(
    rti1516::InvalidInteractionClassHandle, rti1516::FederateNotExecutionMember, rti1516::RTIinternalError)
{
    M_Get_Interaction_Class_Name req, rep;
    req.setInteractionClass(rti1516::InteractionClassHandleFriend::toCertiHandle(theHandle));
    try {
        privateRefs->executeService(&req, &rep);
    }
    catch (rti1516::InteractionClassNotDefined& e) {
        if (!theHandle.isValid()) {
            throw rti1516::InvalidInteractionClassHandle(e.what());
        }
        else {
            throw;
        }
    }

    //return hla_strdup(rep.getClassName());
    std::string nameString = rep.getClassName();
    std::wstring nameWString(nameString.begin(), nameString.end());

    return nameWString;
}

// 10.8
rti1516::ParameterHandle
RTI1516ambassador::getParameterHandle(rti1516::InteractionClassHandle whichClass,
                                      std::wstring const& theName) throw(rti1516::InvalidInteractionClassHandle,
                                                                         rti1516::NameNotFound,
                                                                         rti1516::FederateNotExecutionMember,
                                                                         rti1516::RTIinternalError)
{
    M_Get_Parameter_Handle req, rep;
    std::string nameString(theName.begin(), theName.end());
    req.setParameterName(nameString);
    req.setInteractionClass(rti1516::InteractionClassHandleFriend::toCertiHandle(whichClass));

    try {
        privateRefs->executeService(&req, &rep);
    }
    catch (rti1516::InteractionClassNotDefined& e) {
        if (!whichClass.isValid()) {
            throw rti1516::InvalidInteractionClassHandle(e.what());
        }
        else {
            throw;
        }
    }

    return rti1516::ParameterHandleFriend::createRTI1516Handle(rep.getParameter());
}

// 10.9
std::wstring
RTI1516ambassador::getParameterName(rti1516::InteractionClassHandle whichClass,
                                    rti1516::ParameterHandle theHandle) throw(rti1516::InvalidInteractionClassHandle,
                                                                              rti1516::InvalidParameterHandle,
                                                                              rti1516::InteractionParameterNotDefined,
                                                                              rti1516::FederateNotExecutionMember,
                                                                              rti1516::RTIinternalError)
{
    M_Get_Parameter_Name req, rep;

    req.setParameter(rti1516::ParameterHandleFriend::toCertiHandle(theHandle));
    req.setInteractionClass(rti1516::InteractionClassHandleFriend::toCertiHandle(whichClass));

    try {
        privateRefs->executeService(&req, &rep);
    }
    catch (rti1516::InteractionClassNotDefined& e) {
        if (!whichClass.isValid()) {
            throw rti1516::InvalidInteractionClassHandle(e.what());
        }
        else {
            throw;
        }
    }

    //return hla_strdup(rep.getParameterName());
    std::string nameString = rep.getParameterName();
    std::wstring nameWString(nameString.begin(), nameString.end());

    return nameWString;
}

// 10.10
rti1516::ObjectInstanceHandle RTI1516ambassador::getObjectInstanceHandle(std::wstring const& theName) throw(
    rti1516::ObjectInstanceNotKnown, rti1516::FederateNotExecutionMember, rti1516::RTIinternalError)
{
    M_Get_Object_Instance_Handle req, rep;
    std::string nameString(theName.begin(), theName.end());
    req.setObjectInstanceName(nameString);

    privateRefs->executeService(&req, &rep);
    return rti1516::ObjectInstanceHandleFriend::createRTI1516Handle(rep.getObject());
}

// 10.11
std::wstring RTI1516ambassador::getObjectInstanceName(rti1516::ObjectInstanceHandle theHandle) throw(
    rti1516::ObjectInstanceNotKnown, rti1516::FederateNotExecutionMember, rti1516::RTIinternalError)
{
    M_Get_Object_Instance_Name req, rep;
    req.setObject(rti1516::ObjectInstanceHandleFriend::toCertiHandle(theHandle));
    privateRefs->executeService(&req, &rep);

    //return hla_strdup(rep.getObjectInstanceName());
    std::string nameString = rep.getObjectInstanceName();
    std::wstring nameWString(nameString.begin(), nameString.end());

    return nameWString;
}

// 10.12
rti1516::DimensionHandle RTI1516ambassador::getDimensionHandle(std::wstring const& theName) throw(
    rti1516::NameNotFound, rti1516::FederateNotExecutionMember, rti1516::RTIinternalError)
{
    M_Get_Dimension_Handle req, rep;

    std::string nameString(theName.begin(), theName.end());
    req.setDimensionName(nameString);
    //req.setSpace(space);    //SPACE NIET NODIG IN 1516 STANDAARD???
    privateRefs->executeService(&req, &rep);
    return rti1516::DimensionHandleFriend::createRTI1516Handle(rep.getDimension());
}

// 10.13
std::wstring RTI1516ambassador::getDimensionName(rti1516::DimensionHandle theHandle) throw(
    rti1516::InvalidDimensionHandle, rti1516::FederateNotExecutionMember, rti1516::RTIinternalError)
{
    M_Get_Dimension_Name req, rep;

    req.setDimension(rti1516::DimensionHandleFriend::toCertiHandle(theHandle));
    //req.setSpace(space);
    privateRefs->executeService(&req, &rep);
    //return hla_strdup(rep.getDimensionName());
    std::string nameString = rep.getDimensionName();
    std::wstring nameWString(nameString.begin(), nameString.end());

    return nameWString;
}

// 10.14
unsigned long RTI1516ambassador::getDimensionUpperBound(rti1516::DimensionHandle /*theHandle*/) throw(
    rti1516::InvalidDimensionHandle, rti1516::FederateNotExecutionMember, rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

// 10.15
rti1516::DimensionHandleSet RTI1516ambassador::getAvailableDimensionsForClassAttribute(
    rti1516::ObjectClassHandle theClass, rti1516::AttributeHandle theHandle) throw(rti1516::InvalidObjectClassHandle,
                                                                                   rti1516::InvalidAttributeHandle,
                                                                                   rti1516::AttributeNotDefined,
                                                                                   rti1516::FederateNotExecutionMember,
                                                                                   rti1516::RTIinternalError)
{
    M_Get_Attribute_Space_Handle req, rep;

    req.setAttribute(rti1516::AttributeHandleFriend::toCertiHandle(theHandle));
    req.setObjectClass(rti1516::ObjectClassHandleFriend::toCertiHandle(theClass));
    privateRefs->executeService(&req, &rep);

    //JRE TODO: Use space handle to to get the DimensionHandleSet?@!
    //return rep.getSpace();
    rti1516::DimensionHandleSet invalidSet;
    return invalidSet;
}

// 10.16
rti1516::ObjectClassHandle RTI1516ambassador::getKnownObjectClassHandle(rti1516::ObjectInstanceHandle theObject) throw(
    rti1516::ObjectInstanceNotKnown, rti1516::FederateNotExecutionMember, rti1516::RTIinternalError)
{
    M_Get_Object_Class req, rep;

    req.setObject(rti1516::ObjectInstanceHandleFriend::toCertiHandle(theObject));
    privateRefs->executeService(&req, &rep);
    return rti1516::ObjectClassHandleFriend::createRTI1516Handle(rep.getObjectClass());
}

// 10.17
rti1516::DimensionHandleSet
RTI1516ambassador::getAvailableDimensionsForInteractionClass(rti1516::InteractionClassHandle theClass) throw(
    rti1516::InvalidInteractionClassHandle, rti1516::FederateNotExecutionMember, rti1516::RTIinternalError)
{
    M_Get_Interaction_Space_Handle req, rep;

    req.setInteractionClass(rti1516::InteractionClassHandleFriend::toCertiHandle(theClass));
    this->privateRefs->executeService(&req, &rep);

    //JRE TODO: Use space handle to to get the DimensionHandleSet?@!
    //return rep.getSpace();
    rti1516::DimensionHandleSet invalidSet;
    return invalidSet;
}

// 10.18
rti1516::TransportationType RTI1516ambassador::getTransportationType(std::wstring const& transportationName) throw(
    rti1516::InvalidTransportationName, rti1516::FederateNotExecutionMember, rti1516::RTIinternalError)
{
    M_Get_Transportation_Handle req, rep;
    std::string nameString(transportationName.begin(), transportationName.end());
    req.setTransportationName(nameString);
    privateRefs->executeService(&req, &rep);

    return certi::toRTI1516TransportationType(rep.getTransportation());
}

// 10.19
std::wstring RTI1516ambassador::getTransportationName(rti1516::TransportationType transportationType) throw(
    rti1516::InvalidTransportationType, rti1516::FederateNotExecutionMember, rti1516::RTIinternalError)
{
    M_Get_Transportation_Name req, rep;

    req.setTransportation(certi::toCertiTransportationType(transportationType));
    privateRefs->executeService(&req, &rep);

    //return hla_strdup(rep.getTransportationName());
    std::string nameString = rep.getTransportationName();
    std::wstring nameWString(nameString.begin(), nameString.end());

    return nameWString;
}

// 10.20
rti1516::OrderType RTI1516ambassador::getOrderType(std::wstring const& orderName) throw(
    rti1516::InvalidOrderName, rti1516::FederateNotExecutionMember, rti1516::RTIinternalError)
{
    M_Get_Ordering_Handle req, rep;

    std::string nameAsString(orderName.begin(), orderName.end());
    req.setOrderingName(nameAsString);
    privateRefs->executeService(&req, &rep);

    return certi::toRTI1516OrderType(rep.getOrdering());
}

// 10.21
std::wstring RTI1516ambassador::getOrderName(rti1516::OrderType orderType) throw(rti1516::InvalidOrderType,
                                                                                 rti1516::FederateNotExecutionMember,
                                                                                 rti1516::RTIinternalError)
{
    M_Get_Ordering_Name req, rep;

    req.setOrdering(certi::toCertiOrderType(orderType));
    privateRefs->executeService(&req, &rep);

    //return hla_strdup(rep.getOrderingName());
    std::string nameString = rep.getOrderingName();
    std::wstring nameWString(nameString.begin(), nameString.end());

    return nameWString;
}

// 10.22
/**
 * Sets the ClassRelevanceAdvisory (CRA) switch to true. The switch
 * state is hold on the RTIG side. That's why the message
 * ENABLE_CLASS_RELEVANCE_ADVISORY_SWITCH
 * is transmitted to RTIA. RTIA transmits the message towards RTIG.
 *
 * By default, the CRA switch is true. This causes a delivery of the
 * federate service startRegistrationForObjectClass to a publisher
 * if there are any new subscribers for the federates published object
 * classes. If there are no more subscribers a publisher gets the
 * federate service stopRegistrationForObjectClass.
 *
 * By disabling the CRA switch the federate is no longer informed by
 * subscriptions to its published object classes, i.e. the federate
 * services startRegistrationForObjectClass and
 * stopRegistrationForObjectClass respectively are not invoked.
 * @see disableClassRelevanceAdvisorySwitch()
 */
void RTI1516ambassador::enableObjectClassRelevanceAdvisorySwitch() throw(
    rti1516::ObjectClassRelevanceAdvisorySwitchIsOn,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    M_Enable_Class_Relevance_Advisory_Switch req, rep;
    privateRefs->executeService(&req, &rep);
}

// 10.23
/**
 * Sets the ClassRelevanceAdvisory (CRA) switch to false. The switch
 * state is hold on the RTIG side. That's why the message
 * DISABLE_CLASS_RELEVANCE_ADVISORY_SWITCH
 * is transmitted to RTIA. RTIA transmits the message towards RTIG.
 *
 * By default, the CRA switch is true. This causes a delivery of the
 * federate service startRegistrationForObjectClass to a publisher
 * if there are any new subscribers for the federates published object
 * classes. If there are no more subscribers a publisher gets the
 * federate service stopRegistrationForObjectClass.
 * @see enableClassRelevanceAdvisorySwitch()
 *
 * By disabling the CRA switch the federate is no longer informed by
 * subscriptions to its published object classes, i.e. the federate
 * services startRegistrationForObjectClass and
 * stopRegistrationForObjectClass respectively are not invoked.
 */
void RTI1516ambassador::disableObjectClassRelevanceAdvisorySwitch() throw(
    rti1516::ObjectClassRelevanceAdvisorySwitchIsOff,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    M_Disable_Class_Relevance_Advisory_Switch req, rep;
    privateRefs->executeService(&req, &rep);
}

// 10.24
/**
 * Sets the AttributeRelevanceAdvisory (ARA) switch to true. The switch
 * state is hold on the RTIG side. That's why the message
 * ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH
 * is transmitted to RTIA. RTIA transmits the message towards RTIG.
 *
 * By default, the ARA switch is false. When enabling the ARA switch
 * the federate is informed by the federate service
 * turnUpdatesOnForObjectInstance of new object instances within remote
 * federates actively subscribed to its published attributes. If there
 * are no active subscribers for a set of instance-attributes the federate
 * receives the federate service turnUpdatesOffForObjectInstance.
 *
 * By disabling the ARA switch the federate is no longer informed by
 * subscriptions to its published attributes, i.e. the federate
 * services turnUpdatesOnForObjectInstance and
 * turnUpdatesOffForObjectInstance respectively are not invoked.
 * @see disableAttributeRelevanceAdvisorySwitch()
 */
void RTI1516ambassador::enableAttributeRelevanceAdvisorySwitch() throw(rti1516::AttributeRelevanceAdvisorySwitchIsOn,
                                                                       rti1516::FederateNotExecutionMember,
                                                                       rti1516::SaveInProgress,
                                                                       rti1516::RestoreInProgress,
                                                                       rti1516::RTIinternalError)
{
    M_Enable_Attribute_Relevance_Advisory_Switch req, rep;
    privateRefs->executeService(&req, &rep);
}

// 10.25
/**
 * Sets the AttributeRelevanceAdvisory (ARA) switch to false. The switch
 * state is hold on the RTIG side. That's why the message
 * DISABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH
 * is transmitted to RTIA. RTIA transmits the message towards RTIG.
 *
 * By default, the ARA switch is false. When enabling the ARA switch
 * the federate is informed by the federate service
 * turnUpdatesOnForObjectInstance of new object instances within remote
 * federates actively subscribed to its published attributes. If there
 * are no active subscribers for a set of instance-attributes the federate
 * receives the federate service turnUpdatesOffForObjectInstance.
 * @see enableAttributeRelevanceAdvisorySwitch()
 *
 * By disabling the ARA switch the federate is no longer informed by
 * subscriptions to its published attributes, i.e. the federate
 * services turnUpdatesOnForObjectInstance and
 * turnUpdatesOffForObjectInstance respectively are not invoked.
 */
void RTI1516ambassador::disableAttributeRelevanceAdvisorySwitch() throw(rti1516::AttributeRelevanceAdvisorySwitchIsOff,
                                                                        rti1516::FederateNotExecutionMember,
                                                                        rti1516::SaveInProgress,
                                                                        rti1516::RestoreInProgress,
                                                                        rti1516::RTIinternalError)
{
    M_Disable_Attribute_Relevance_Advisory_Switch req, rep;
    privateRefs->executeService(&req, &rep);
}

// 10.26
/**
 * Sets the AttributeScopeAdvisory (ASA) switch to true. The switch state
 * is hold on the RTIG side. That's why the message
 * ENABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH
 * is transmitted to RTIA. RTIA transmits the message towards RTIG.
 *
 * By default, the ASA switch is false. When enabling the ASA switch
 * the federate is informed by the federate services
 * attributesInScope and attributesOutScope respectively of discovered
 * or registrated but not owned attribute-instances intersecting or
 * leaving its subscription regions.
 *
 * By disabling the ASA switch the federate is no longer informed of
 * changes in attribute-instance scope, i.e. the federate
 * services attributesInScope and attributesOutScope respectively are
 * not invoked.
 * @see disableAttributeScopeAdvisorySwitch()
 */
void RTI1516ambassador::enableAttributeScopeAdvisorySwitch() throw(rti1516::AttributeScopeAdvisorySwitchIsOn,
                                                                   rti1516::FederateNotExecutionMember,
                                                                   rti1516::SaveInProgress,
                                                                   rti1516::RestoreInProgress,
                                                                   rti1516::RTIinternalError)
{
    M_Enable_Attribute_Scope_Advisory_Switch req, rep;
    privateRefs->executeService(&req, &rep);
}

// 10.27
/**
 * Sets the AttributeScopeAdvisory (ASA) switch to false. The switch state
 * is hold on the RTIG side. That's why the message
 * DISABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH
 * is transmitted to RTIA. RTIA transmits the message towards RTIG.
 *
 * By default, the ASA switch is false. When enabling the ASA switch
 * the federate is informed by the federate services
 * attributesInScope and attributesOutScope respectively of discovered
 * or registrated but not owned attribute-instances intersecting or
 * leaving its subscription regions.
 * @see enableAttributeScopeAdvisorySwitch()
 *
 * By disabling the ASA switch the federate is no longer informed of
 * changes in attribute-instance scope, i.e. the federate
 * services attributesInScope and attributesOutScope respectively are
 * not invoked.
 */
void RTI1516ambassador::disableAttributeScopeAdvisorySwitch() throw(rti1516::AttributeScopeAdvisorySwitchIsOff,
                                                                    rti1516::FederateNotExecutionMember,
                                                                    rti1516::SaveInProgress,
                                                                    rti1516::RestoreInProgress,
                                                                    rti1516::RTIinternalError)
{
    M_Disable_Attribute_Scope_Advisory_Switch req, rep;
    privateRefs->executeService(&req, &rep);
}

// 10.28
/**
 * Sets the InteractionRelevanceAdvisory (IRA) switch to true. The switch
 * state is hold on the RTIG side. That's why the message
 * ENABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH
 * is transmitted to RTIA. RTIA transmits the message towards RTIG.
 *
 * By default, the IRA switch is true. This causes a delivery of the
 * federate service turnInteractionsOn to a publisher if there are
 * any new subscribers for the federates published interaction
 * classes. If there are no more subscribers a publisher gets the
 * federate service turnInteractionsOff().
 *
 * By disabling the IRA switch the federate is no longer informed by
 * subscriptions to its published interaction classes, i.e. the federate
 * services turnInteractionsOn and turnInteractionsOff respectively are
 * not invoked.
 * @see disableInteractionRelevanceAdvisorySwitch()
 */
void RTI1516ambassador::enableInteractionRelevanceAdvisorySwitch() throw(
    rti1516::InteractionRelevanceAdvisorySwitchIsOn,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    M_Enable_Interaction_Relevance_Advisory_Switch req, rep;
    privateRefs->executeService(&req, &rep);
}

// 10.29
/**
 * Sets the InteractionRelevanceAdvisory (IRA) switch to false. The switch
 * state is hold on the RTIG side. That's why the message
 * DISABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH
 * is transmitted to RTIA. RTIA transmits the message towards RTIG.
 *
 * By default, the IRA switch is true. This causes a delivery of the
 * federate service turnInteractionsOn to a publisher if there are
 * any new subscribers for the federates published interaction
 * classes. If there are no more subscribers a publisher gets the
 * federate service turnInteractionsOff().
 * @see enableInteractionRelevanceAdvisorySwitch()
 *
 * By disabling the IRA switch the federate is no longer informed by
 * subscriptions to its published interaction classes, i.e. the federate
 * services turnInteractionsOn and turnInteractionsOff respectively are
 * not invoked.
 */
void RTI1516ambassador::disableInteractionRelevanceAdvisorySwitch() throw(
    rti1516::InteractionRelevanceAdvisorySwitchIsOff,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    M_Disable_Interaction_Relevance_Advisory_Switch req, rep;
    privateRefs->executeService(&req, &rep);
}

// 10.30

rti1516::DimensionHandleSet RTI1516ambassador::getDimensionHandleSet(rti1516::RegionHandle /*theRegionHandle*/) throw(
    rti1516::InvalidRegion,
    rti1516::FederateNotExecutionMember,
    rti1516::SaveInProgress,
    rti1516::RestoreInProgress,
    rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

// 10.31

rti1516::RangeBounds RTI1516ambassador::getRangeBounds(
    rti1516::RegionHandle /*theRegionHandle*/,
    rti1516::DimensionHandle /*theDimensionHandle*/) throw(rti1516::InvalidRegion,
                                                       rti1516::RegionDoesNotContainSpecifiedDimension,
                                                       rti1516::FederateNotExecutionMember,
                                                       rti1516::SaveInProgress,
                                                       rti1516::RestoreInProgress,
                                                       rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

// 10.32
void RTI1516ambassador::setRangeBounds(
    rti1516::RegionHandle /*theRegionHandle*/,
    rti1516::DimensionHandle /*theDimensionHandle*/,
    rti1516::RangeBounds const& /*theRangeBounds*/) throw(rti1516::InvalidRegion,
                                                      rti1516::RegionNotCreatedByThisFederate,
                                                      rti1516::RegionDoesNotContainSpecifiedDimension,
                                                      rti1516::InvalidRangeBound,
                                                      rti1516::FederateNotExecutionMember,
                                                      rti1516::SaveInProgress,
                                                      rti1516::RestoreInProgress,
                                                      rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

// 10.33
unsigned long RTI1516ambassador::normalizeFederateHandle(rti1516::FederateHandle /*theFederateHandle*/) throw(
    rti1516::FederateNotExecutionMember, rti1516::InvalidFederateHandle, rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

// 10.34
unsigned long RTI1516ambassador::normalizeServiceGroup(rti1516::ServiceGroupIndicator /*theServiceGroup*/) throw(
    rti1516::FederateNotExecutionMember, rti1516::InvalidServiceGroup, rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

// 10.37
bool RTI1516ambassador::evokeCallback(double approximateMinimumTimeInSeconds) throw(rti1516::FederateNotExecutionMember,
                                                                                    rti1516::RTIinternalError)
{
    return __tick_kernel(false, approximateMinimumTimeInSeconds, approximateMinimumTimeInSeconds);
}

// 10.38
bool RTI1516ambassador::evokeMultipleCallbacks(
    double approximateMinimumTimeInSeconds,
    double approximateMaximumTimeInSeconds) throw(rti1516::FederateNotExecutionMember, rti1516::RTIinternalError)
{
    return __tick_kernel(true, approximateMinimumTimeInSeconds, approximateMaximumTimeInSeconds);
}

// 10.39
void RTI1516ambassador::enableCallbacks() throw(rti1516::FederateNotExecutionMember,
                                                rti1516::SaveInProgress,
                                                rti1516::RestoreInProgress,
                                                rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

// 10.40
void RTI1516ambassador::disableCallbacks() throw(rti1516::FederateNotExecutionMember,
                                                 rti1516::SaveInProgress,
                                                 rti1516::RestoreInProgress,
                                                 rti1516::RTIinternalError)
{
    /* TODO */
    throw rti1516::RTIinternalError(L"Not yet implemented");
}

rti1516::FederateHandle RTI1516ambassador::decodeFederateHandle(rti1516::VariableLengthData const& encodedValue) const
{
    return rti1516::FederateHandleFriend::createRTI1516Handle(encodedValue);
}

rti1516::ObjectClassHandle
RTI1516ambassador::decodeObjectClassHandle(rti1516::VariableLengthData const& encodedValue) const
{
    return rti1516::ObjectClassHandleFriend::createRTI1516Handle(encodedValue);
}

rti1516::InteractionClassHandle
RTI1516ambassador::decodeInteractionClassHandle(rti1516::VariableLengthData const& encodedValue) const
{
    return rti1516::InteractionClassHandleFriend::createRTI1516Handle(encodedValue);
}

rti1516::ObjectInstanceHandle
RTI1516ambassador::decodeObjectInstanceHandle(rti1516::VariableLengthData const& encodedValue) const
{
    return rti1516::ObjectInstanceHandleFriend::createRTI1516Handle(encodedValue);
}

rti1516::AttributeHandle RTI1516ambassador::decodeAttributeHandle(rti1516::VariableLengthData const& encodedValue) const
{
    return rti1516::AttributeHandleFriend::createRTI1516Handle(encodedValue);
}

rti1516::ParameterHandle RTI1516ambassador::decodeParameterHandle(rti1516::VariableLengthData const& encodedValue) const
{
    return rti1516::ParameterHandleFriend::createRTI1516Handle(encodedValue);
}

rti1516::DimensionHandle RTI1516ambassador::decodeDimensionHandle(rti1516::VariableLengthData const& encodedValue) const
{
    return rti1516::DimensionHandleFriend::createRTI1516Handle(encodedValue);
}

rti1516::MessageRetractionHandle
RTI1516ambassador::decodeMessageRetractionHandle(rti1516::VariableLengthData const& encodedValue) const
{
    return rti1516::MessageRetractionHandleFriend::createRTI1516Handle(encodedValue);
}

rti1516::RegionHandle RTI1516ambassador::decodeRegionHandle(rti1516::VariableLengthData const& encodedValue) const
{
    return rti1516::RegionHandleFriend::createRTI1516Handle(encodedValue);
}

} // end namespace certi
