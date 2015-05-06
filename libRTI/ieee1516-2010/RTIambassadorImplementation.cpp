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
#include "RTIHandleFactory.h"
#include "RTI1516fedTime.h"

#include <algorithm>

namespace {

static PrettyDebug D("LIBRTI", __FILE__);
static PrettyDebug G("GENDOC",__FILE__) ;

}

namespace certi
{
/* Deletor Object */
template <class T>
struct Deletor {
    void operator() (T* e) {delete e;};
};

/* Helper functions */
template<typename T>
void
RTI1516ambassador::assignPHVMAndExecuteService(const rti1516e::ParameterHandleValueMap &PHVM, T &req, T &rep) {

    req.setParametersSize(PHVM.size());
    req.setValuesSize(PHVM.size());
    uint32_t i = 0;
    for ( rti1516e::ParameterHandleValueMap::const_iterator it = PHVM.begin(); it != PHVM.end(); it++, ++i)
    {
        req.setParameters(rti1516e::ParameterHandleFriend::toCertiHandle(it->first),i);
        certi::ParameterValue_t paramValue;
        paramValue.resize(it->second.size());
        memcpy(&(paramValue[0]), it->second.data(), it->second.size());
        req.setValues(paramValue, i);
    }
    privateRefs->executeService(&req, &rep);
}

template<typename T>
void
RTI1516ambassador::assignAHVMAndExecuteService(const rti1516e::AttributeHandleValueMap &AHVM, T &req, T &rep) {

    req.setAttributesSize(AHVM.size());
    req.setValuesSize(AHVM.size());
    uint32_t i = 0;
    for ( rti1516e::AttributeHandleValueMap::const_iterator it = AHVM.begin(); it != AHVM.end(); it++, ++i)
    {
        req.setAttributes(rti1516e::AttributeHandleFriend::toCertiHandle(it->first),i);
        certi::AttributeValue_t attrValue;
        attrValue.resize(it->second.size());
        memcpy(&(attrValue[0]), it->second.data(), it->second.size());
        req.setValues(attrValue, i);
    }
    privateRefs->executeService(&req, &rep);
}

template<typename T>
void
RTI1516ambassador::assignAHSAndExecuteService(const rti1516e::AttributeHandleSet &AHS, T &req, T &rep) {
    req.setAttributesSize(AHS.size());
    uint32_t i = 0;
    for ( rti1516e::AttributeHandleSet::const_iterator it = AHS.begin(); it != AHS.end(); it++, ++i)
    {
        certi::AttributeHandle certiHandle = rti1516e::AttributeHandleFriend::toCertiHandle(*it);
        req.setAttributes(certiHandle,i);
    }
    privateRefs->executeService(&req, &rep);
}

std::string varLengthDataAsString(rti1516e::VariableLengthData varLengthData) {
    std::string retVal( (char*)varLengthData.data(), varLengthData.size() );
    return retVal;
}

certi::TransportType toCertiTransportationType(rti1516e::TransportationType theType) {
    return (theType == rti1516e::RELIABLE) ? certi::RELIABLE : certi::BEST_EFFORT;
}
rti1516e::TransportationType toRTI1516TransportationType(certi::TransportType theType) {
    return (theType == certi::RELIABLE) ? rti1516e::RELIABLE : rti1516e::BEST_EFFORT;
}
certi::OrderType toCertiOrderType(rti1516e::OrderType theType) {
    return (theType == rti1516e::RECEIVE) ? certi::RECEIVE : certi::TIMESTAMP;
}
rti1516e::OrderType toRTI1516OrderType(certi::OrderType theType) {
    return (theType == certi::RECEIVE) ? rti1516e::RECEIVE : rti1516e::TIMESTAMP;
}
/* end of Helper functions */


RTI1516ambassador::RTI1516ambassador() throw()
		        : privateRefs(0)
{
}

RTI1516ambassador::~RTI1516ambassador()
{
    certi::M_Close_Connexion req, rep ;

    G.Out(pdGendoc,"        ====>executeService CLOSE_CONNEXION");
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
bool RTI1516ambassador::__tick_kernel(bool multiple, TickTime minimum, TickTime maximum)
throw (rti1516e::SpecifiedSaveLabelDoesNotExist,
       rti1516e::NotConnected,
       rti1516e::RTIinternalError)
        {
    M_Tick_Request vers_RTI;
    std::auto_ptr<Message> vers_Fed(NULL);

    // Request callback(s) from the local RTIA
    vers_RTI.setMultiple(multiple);
    vers_RTI.setMinTickTime(minimum);
    vers_RTI.setMaxTickTime(maximum);

    try {
        vers_RTI.send(privateRefs->socketUn,privateRefs->msgBufSend);
    }
    catch (NetworkError &e) {
        throw rti1516e::RTIinternalError(L"NetworkError in tick() while sending TICK_REQUEST: " + e.wreason());
    }

    // Read response(s) from the local RTIA until Message::TICK_REQUEST is received.
    while (1) {
        try {
            vers_Fed.reset(M_Factory::receive(privateRefs->socketUn));
        }
        catch (NetworkError &e) {
            throw rti1516e::RTIinternalError(L"NetworkError in tick() while receiving response: " + e.wreason());
        }

        // If the type is TICK_REQUEST, the __tick_kernel() has terminated.
        if (vers_Fed->getMessageType() == Message::TICK_REQUEST) {
            if (vers_Fed->getExceptionType() != e_NO_EXCEPTION) {
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
        catch (NetworkError &e) {
            throw rti1516e::RTIinternalError(L"NetworkError in tick() while sending TICK_REQUEST_NEXT: " + e.wreason());
        }
    } // while(1)
    // should never be reached, this is there for calming down static code analysis
    return false;
}

void RTI1516ambassador::connect (
        rti1516e::FederateAmbassador & federateAmbassador,
        rti1516e::CallbackModel theCallbackModel,
        std::wstring const & localSettingsDesignator)
throw (
        rti1516e::ConnectionFailed,
        rti1516e::InvalidLocalSettingsDesignator,
        rti1516e::UnsupportedCallbackModel,
        rti1516e::AlreadyConnected,
        rti1516e::CallNotAllowedFromWithinCallback,
        rti1516e::RTIinternalError)
        {

    switch (theCallbackModel) {
    case rti1516e::HLA_EVOKED:
        // before rti1516-2010 this this done in CFE
        privateRefs->fed_amb = &federateAmbassador ;
        break;
    case rti1516e::HLA_IMMEDIATE:
        throw rti1516e::UnsupportedCallbackModel(L"CONNECT callback model HLA_IMMEDIATE not implemented [yet].");
    default:

        throw rti1516e::UnsupportedCallbackModel(wstringize() << L"CONNECT unsupported callback model " << theCallbackModel);
    } /* end of switch(theCallbackModel) */
}

// 4.3
void RTI1516ambassador::disconnect ()
throw (
        rti1516e::FederateIsExecutionMember,
        rti1516e::CallNotAllowedFromWithinCallback,
        rti1516e::RTIinternalError) {
    privateRefs->fed_amb = NULL;

}

// 4.5
void RTI1516ambassador::createFederationExecution
(std::wstring const & federationExecutionName,
        std::wstring const & fullPathNameToTheFDDfile,
        std::wstring const & LogicalTimeImplementationName)
throw (rti1516e::FederationExecutionAlreadyExists,
        rti1516e::CouldNotOpenFDD,
        rti1516e::ErrorReadingFDD,
        rti1516e::CouldNotCreateLogicalTimeFactory,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {

    certi::M_Create_Federation_Execution req, rep ;

    G.Out(pdGendoc,"enter RTI1516ambassador::createFederationExecution");
    std::string federationExecutionNameAsString(federationExecutionName.begin(), federationExecutionName.end());
    req.setFederationName(federationExecutionNameAsString);

    std::string fullPathNameToTheFDDfileAsString(fullPathNameToTheFDDfile.begin(), fullPathNameToTheFDDfile.end());
    req.setFEDid(fullPathNameToTheFDDfileAsString);

    G.Out(pdGendoc,"             ====>executeService CREATE_FEDERATION_EXECUTION");

    privateRefs->executeService(&req, &rep);

    G.Out(pdGendoc,"exit RTI1516ambassador::createFederationExecution");

    // TODO What to do with the 'logicalTimeImplementationName'? Can't find it's use in SISO-STD-004.1-2004
    // Only exists in C++ interface.
    // Ignored for now.
        }

void RTI1516ambassador::createFederationExecution (
   std::wstring const & federationExecutionName,
   std::vector<std::wstring> const & fomModules,
   std::wstring const & logicalTimeImplementationName)
   throw (
           rti1516e::CouldNotCreateLogicalTimeFactory,
           rti1516e::InconsistentFDD,
           rti1516e::ErrorReadingFDD,
           rti1516e::CouldNotOpenFDD,
           rti1516e::FederationExecutionAlreadyExists,
           rti1516e::NotConnected,
           rti1516e::RTIinternalError) {
    throw rti1516e::RTIinternalError(L"Create with FOM Module Not Implemented");
}

void RTI1516ambassador::createFederationExecutionWithMIM (
   std::wstring const & federationExecutionName,
   std::vector<std::wstring> const & fomModules,
   std::wstring const & mimModule,
   std::wstring const & logicalTimeImplementationName)
   throw (
           rti1516e::CouldNotCreateLogicalTimeFactory,
           rti1516e::InconsistentFDD,
           rti1516e::ErrorReadingFDD,
           rti1516e::CouldNotOpenFDD,
           rti1516e::DesignatorIsHLAstandardMIM,
           rti1516e::ErrorReadingMIM,
           rti1516e::CouldNotOpenMIM,
           rti1516e::FederationExecutionAlreadyExists,
           rti1516e::NotConnected,
           rti1516e::RTIinternalError) {
    throw rti1516e::RTIinternalError(L"Create with FOM Module and MIM Module tNot Implemented");
}

// 4.3
void RTI1516ambassador::destroyFederationExecution
(std::wstring const & federationExecutionName)
throw (rti1516e::FederatesCurrentlyJoined,
        rti1516e::FederationExecutionDoesNotExist,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Destroy_Federation_Execution req, rep ;

    G.Out(pdGendoc,"enter RTI1516ambassador::destroyFederationExecution");

    std::string federationExecutionNameAsString(federationExecutionName.begin(), federationExecutionName.end());
    req.setFederationName(federationExecutionNameAsString);

    G.Out(pdGendoc,"        ====>executeService DESTROY_FEDERATION_EXECUTION");

    privateRefs->executeService(&req, &rep);

    G.Out(pdGendoc,"exit RTI1516ambassador::destroyFederationExecution");
        }

void RTI1516ambassador::listFederationExecutions ()
             throw (
                     rti1516e::NotConnected,
                     rti1516e::RTIinternalError)
                {
throw rti1516e::RTIinternalError(L"listFederationExecutions Not Implemented");
                }

// 4.9
rti1516e::FederateHandle RTI1516ambassador::joinFederationExecution
(
        std::wstring const & federateType,
        std::wstring const & federationExecutionName,
        std::vector<std::wstring> const & additionalFomModules)
throw (rti1516e::CouldNotCreateLogicalTimeFactory,
        rti1516e::FederationExecutionDoesNotExist,
        rti1516e::InconsistentFDD,
        rti1516e::ErrorReadingFDD,
        rti1516e::CouldNotOpenFDD,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::FederateAlreadyExecutionMember,
        rti1516e::NotConnected,
        rti1516e::CallNotAllowedFromWithinCallback,
        rti1516e::RTIinternalError) {

    // RTI shall provide a default federate name if none is provided
    return this->joinFederationExecution(
            L"defaultName",
            federateType,
            federationExecutionName,
            additionalFomModules);
}

rti1516e::FederateHandle RTI1516ambassador::joinFederationExecution
(       std::wstring const & federateName,
        std::wstring const & federateType,
        std::wstring const & federationExecutionName,
        std::vector<std::wstring> const & additionalFomModules)
throw (rti1516e::CouldNotCreateLogicalTimeFactory,
        rti1516e::FederationExecutionDoesNotExist,
        rti1516e::InconsistentFDD,
        rti1516e::ErrorReadingFDD,
        rti1516e::CouldNotOpenFDD,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::FederateAlreadyExecutionMember,
        rti1516e::NotConnected,
        rti1516e::CallNotAllowedFromWithinCallback,
        rti1516e::RTIinternalError)
        {
    M_Join_Federation_Execution req, rep ;

    G.Out(pdGendoc,"enter RTI1516ambassador::joinFederationExecution");

    if ( &federateType == NULL || federateType.length() <= 0 ) {
        throw rti1516e::RTIinternalError(L"Incorrect or empty federate type");
    }
    std::string federateTypeAsString(federateType.begin(), federateType.end());

    if ( &federationExecutionName == NULL || federationExecutionName.length() <= 0 )
        throw rti1516e::RTIinternalError(L"Incorrect or empty federation name");
    std::string federationExecutionNameAsString(federationExecutionName.begin(), federationExecutionName.end());

    req.setFederateName(federateTypeAsString);
    req.setFederationName(federationExecutionNameAsString);
    G.Out(pdGendoc,"        ====>executeService JOIN_FEDERATION_EXECUTION");
    privateRefs->executeService(&req, &rep);
    G.Out(pdGendoc,"exit  RTI1516ambassador::joinFederationExecution");
    PrettyDebug::setFederateName( "LibRTI::"+std::string(federateTypeAsString));

    certi::FederateHandle certiFederateHandle = rep.getFederate();
    rti1516e::FederateHandle rti1516FederateHandle = rti1516e::FederateHandleFriend::createRTI1516Handle(certiFederateHandle);

    return rti1516FederateHandle;
        }

// 4.5
void RTI1516ambassador::resignFederationExecution
(rti1516e::ResignAction resignAction)
throw (rti1516e::InvalidResignAction,
        rti1516e::OwnershipAcquisitionPending,
        rti1516e::FederateOwnsAttributes,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::CallNotAllowedFromWithinCallback,
        rti1516e::RTIinternalError)
        {
    M_Resign_Federation_Execution req, rep ;

    G.Out(pdGendoc,"enter RTI1516ambassador::resignFederationExecution");
    //req.setResignAction(static_cast<certi::ResignAction>(resignAction));
    req.setResignAction(certi::DELETE_OBJECTS_AND_RELEASE_ATTRIBUTES);
    G.Out(pdGendoc,"        ====>executeService RESIGN_FEDERATION_EXECUTION");
    privateRefs->executeService(&req, &rep);
    G.Out(pdGendoc,"exit RTI1516ambassador::resignFederationExecution");
        }

// 4.6
void RTI1516ambassador::registerFederationSynchronizationPoint
(std::wstring const & label,
        rti1516e::VariableLengthData const & theUserSuppliedTag)
throw (rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Register_Federation_Synchronization_Point req, rep ;

    G.Out(pdGendoc,"enter RTI1516ambassador::registerFederationSynchronizationPoint for all federates");
    std::string labelString(label.begin(), label.end());
    req.setLabel(labelString);
    // no federate set
    req.setFederateSetSize(0);
    if ( &theUserSuppliedTag == NULL || theUserSuppliedTag.data() == NULL )
    {
        throw rti1516e::RTIinternalError (L"Calling registerFederationSynchronizationPoint with Tag NULL");
    }
    req.setTag(varLengthDataAsString(theUserSuppliedTag));
    G.Out(pdGendoc,"        ====>executeService REGISTER_FEDERATION_SYNCHRONIZATION_POINT");
    privateRefs->executeService(&req, &rep);

    G.Out(pdGendoc,"exit RTI1516ambassador::registerFederationSynchronizationPoint for all federates");
        }

void RTI1516ambassador::registerFederationSynchronizationPoint
(std::wstring const & label,
        rti1516e::VariableLengthData const & theUserSuppliedTag,
        rti1516e::FederateHandleSet const & syncSet)
throw (rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Register_Federation_Synchronization_Point req, rep ;

    G.Out(pdGendoc,"enter RTI1516ambassador::registerFederationSynchronizationPoint for some federates");

    std::string labelString(label.begin(), label.end());
    req.setLabel(labelString);
    if ( &theUserSuppliedTag == NULL || theUserSuppliedTag.data() == NULL )
    {
        throw rti1516e::RTIinternalError (L"Calling registerFederationSynchronizationPoint with Tag NULL");
    }
    req.setTag(varLengthDataAsString(theUserSuppliedTag));
    // Federate set exists but if size=0 (set empty)
    // We have to send the size even if federate set size is 0
    // (HLA 1.3 compliance to inform ALL federates)

    req.setFederateSetSize(syncSet.size());

    uint32_t i = 0;
    for ( rti1516e::FederateHandleSet::const_iterator it = syncSet.begin(); it != syncSet.end(); it++, ++i)
    {
        req.setFederateSet(rti1516e::FederateHandleFriend::toCertiHandle(*it),i);
    }

    G.Out(pdGendoc,"        ====>executeService REGISTER_FEDERATION_SYNCHRONIZATION_POINT");
    privateRefs->executeService(&req, &rep);

    G.Out(pdGendoc,"exit RTI1516ambassador::registerFederationSynchronizationPoint for some federates");
        }

// 4.14
void RTI1516ambassador::synchronizationPointAchieved
(std::wstring const & label, bool successfully)
throw (rti1516e::SynchronizationPointLabelNotAnnounced,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Synchronization_Point_Achieved req, rep ;

    G.Out(pdGendoc,"enter RTI1516ambassador::synchronizationPointAchieved");

    std::string labelString(label.begin(), label.end());
    req.setLabel(labelString);

    G.Out(pdGendoc,"        ====>executeService SYNCHRONIZATION_POINT_ACHIEVED");
    privateRefs->executeService(&req, &rep);

    G.Out(pdGendoc,"exit  RTI1516ambassador::synchronizationPointAchieved");
        }

// 4.16
void RTI1516ambassador::requestFederationSave
(std::wstring const & label)
throw (rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Request_Federation_Save req, rep ;

    G.Out(pdGendoc,"enter RTI1516ambassador::requestFederationSave without time");

    std::string labelString(label.begin(), label.end());
    req.setLabel(labelString);
    G.Out(pdGendoc,"      ====>executeService REQUEST_FEDERATION_SAVE");

    privateRefs->executeService(&req, &rep);
    G.Out(pdGendoc,"exit  RTI1516ambassador::requestFederationSave without time");
        }

void RTI1516ambassador::requestFederationSave
(std::wstring const & label,
        rti1516e::LogicalTime const & theTime)
throw (rti1516e::LogicalTimeAlreadyPassed,
        rti1516e::InvalidLogicalTime,
        rti1516e::FederateUnableToUseTime,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Request_Federation_Save req, rep ;

    G.Out(pdGendoc,"enter RTI1516ambassador::requestFederationSave with time");

    certi::FederationTime certiFedTime(certi_cast<RTI1516fedTime>()(theTime).getFedTime());
    req.setDate(certiFedTime);

    std::string labelString(label.begin(), label.end());
    req.setLabel(labelString);

    G.Out(pdGendoc,"        ====>executeService REQUEST_FEDERATION_SAVE");
    privateRefs->executeService(&req, &rep);

    G.Out(pdGendoc,"exit RTI1516ambassador::requestFederationSave with time");
        }

// 4.13
void RTI1516ambassador::federateSaveBegun ()
throw (rti1516e::SaveNotInitiated,
        rti1516e::FederateNotExecutionMember,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Federate_Save_Begun req, rep ;

    G.Out(pdGendoc,"enter RTI1516ambassador::federateSaveBegun");

    G.Out(pdGendoc,"      ====>executeService FEDERATE_SAVE_BEGUN");
    privateRefs->executeService(&req, &rep);

    G.Out(pdGendoc,"exit  RTI1516ambassador::federateSaveBegun");
        }

// 4.14
void RTI1516ambassador::federateSaveComplete ()
throw (rti1516e::FederateHasNotBegunSave,
        rti1516e::FederateNotExecutionMember,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Federate_Save_Complete req, rep ;

    G.Out(pdGendoc,"enter RTI1516ambassador::federateSaveComplete");
    G.Out(pdGendoc,"      ====>executeService FEDERATE_SAVE_COMPLETE");
    privateRefs->executeService(&req, &rep);
    G.Out(pdGendoc,"exit  RTI1516ambassador::federateSaveComplete");
        }

void RTI1516ambassador::federateSaveNotComplete()
throw (rti1516e::FederateHasNotBegunSave,
        rti1516e::FederateNotExecutionMember,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Federate_Save_Not_Complete req, rep ;

    G.Out(pdGendoc,"enter RTI1516ambassador::federateSaveNotComplete");
    G.Out(pdGendoc,"      ====>executeService FEDERATE_SAVE_NOT_COMPLETE");
    privateRefs->executeService(&req, &rep);

    G.Out(pdGendoc,"exit  RTI1516ambassador::federateSaveNotComplete");
        }

void RTI1516ambassador::abortFederationSave ()
        throw (rti1516e::FederateNotExecutionMember,
                rti1516e::SaveNotInProgress,
                rti1516e::NotConnected,
                rti1516e::RTIinternalError) {
    throw rti1516e::RTIinternalError(L"abortFederationSave Not Implemented");
}

// 4.22
void RTI1516ambassador::queryFederationSaveStatus ()
throw (rti1516e::FederateNotExecutionMember,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"queryFederationSaveStatus Not implemented");
        }

// 4.24
void RTI1516ambassador::requestFederationRestore
(std::wstring const & label)
throw (rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Request_Federation_Restore req, rep ;

    G.Out(pdGendoc,"enter RTI1516ambassador::requestFederationRestore");
    std::string labelString(label.begin(), label.end());
    req.setLabel(labelString);
    G.Out(pdGendoc,"      ====>executeService REQUEST_FEDERATION_RESTORE");
    privateRefs->executeService(&req, &rep);
    G.Out(pdGendoc,"exit  RTI1516ambassador::requestFederationRestore");
        }

// 4.28
void RTI1516ambassador::federateRestoreComplete ()
throw (rti1516e::RestoreNotRequested,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Federate_Restore_Complete req, rep ;

    G.Out(pdGendoc,"enter RTI1516ambassador::federateRestoreComplete");

    G.Out(pdGendoc,"      ====>executeService FEDERATE_RESTORE_COMPLETE");
    privateRefs->executeService(&req, &rep);
    G.Out(pdGendoc,"exit  RTI1516ambassador::federateRestoreComplete");
        }

void RTI1516ambassador::federateRestoreNotComplete ()
throw (rti1516e::RestoreNotRequested,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Federate_Restore_Not_Complete req, rep ;

    G.Out(pdGendoc,"enter RTI1516ambassador::federateRestoreNotComplete");
    G.Out(pdGendoc,"      ====>executeService FEDERATE_RESTORE_NOT_COMPLETE");

    privateRefs->executeService(&req, &rep);
    G.Out(pdGendoc,"exit  RTI1516ambassador::federateRestoreNotComplete");
        }

// 4.30
void RTI1516ambassador::abortFederationRestore ()
    throw (
            rti1516e::RestoreNotInProgress,
            rti1516e::FederateNotExecutionMember,
            rti1516e::NotConnected,
            rti1516e::RTIinternalError) {
    throw rti1516e::RTIinternalError(L"abortFederationRestore Not Implemented");
}

// 4.24
void RTI1516ambassador::queryFederationRestoreStatus ()
throw (rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"queryFederationRestoreStatus Not implemented");
        }

/////////////////////////////////////
// Declaration Management Services //
/////////////////////////////////////

// 5.2
void RTI1516ambassador::publishObjectClassAttributes
(rti1516e::ObjectClassHandle theClass,
        rti1516e::AttributeHandleSet const & attributeList)
throw (rti1516e::ObjectClassNotDefined,
        rti1516e::AttributeNotDefined,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Publish_Object_Class req, rep ;
    G.Out(pdGendoc,"enter RTI1516ambassador::publishObjectClass");

    const certi::ObjectClassHandle objectClassHandle =  rti1516e::ObjectClassHandleFriend::toCertiHandle(theClass);
    req.setObjectClass(objectClassHandle);

    req.setAttributesSize(attributeList.size());
    uint32_t i = 0;
    for ( rti1516e::AttributeHandleSet::const_iterator it = attributeList.begin(); it != attributeList.end(); it++, ++i)
    {
        req.setAttributes(rti1516e::AttributeHandleFriend::toCertiHandle(*it),i);
    }
    G.Out(pdGendoc,"      ====>executeService PUBLISH_OBJECT_CLASS");
    privateRefs->executeService(&req, &rep);
    G.Out(pdGendoc,"exit  RTI1516ambassador::publishObjectClass");
        }

// 5.3
void RTI1516ambassador::unpublishObjectClass
(rti1516e::ObjectClassHandle theClass)
throw (rti1516e::ObjectClassNotDefined,
        rti1516e::OwnershipAcquisitionPending,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Unpublish_Object_Class req, rep ;
    G.Out(pdGendoc,"enter RTI1516ambassador::unpublishObjectClass");

    const certi::ObjectClassHandle objectClassHandle = rti1516e::ObjectClassHandleFriend::toCertiHandle(theClass);
    req.setObjectClass(objectClassHandle);
    G.Out(pdGendoc,"      ====>executeService UNPUBLISH_OBJECT_CLASS");
    privateRefs->executeService(&req, &rep);
    G.Out(pdGendoc,"exit  RTI1516ambassador::unpublishObjectClass");
        }

void RTI1516ambassador::unpublishObjectClassAttributes
(rti1516e::ObjectClassHandle theClass,
        rti1516e::AttributeHandleSet const & attributeList)
throw (rti1516e::ObjectClassNotDefined,
        rti1516e::AttributeNotDefined,
        rti1516e::OwnershipAcquisitionPending,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"Not yet implemented");
        }

// 5.4
void RTI1516ambassador::publishInteractionClass
(rti1516e::InteractionClassHandle theInteraction)
throw (rti1516e::InteractionClassNotDefined,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Publish_Interaction_Class req, rep ;
    const certi::InteractionClassHandle classHandle = rti1516e::InteractionClassHandleFriend::toCertiHandle(theInteraction);
    req.setInteractionClass(classHandle);
    G.Out(pdGendoc,"      ====>executeService PUBLISH_INTERACTION_CLASS");
    privateRefs->executeService(&req, &rep);
        }

// 5.5
void RTI1516ambassador::unpublishInteractionClass
(rti1516e::InteractionClassHandle theInteraction)
throw (rti1516e::InteractionClassNotDefined,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Unpublish_Interaction_Class req, rep ;
    const certi::InteractionClassHandle classHandle = rti1516e::InteractionClassHandleFriend::toCertiHandle(theInteraction);
    req.setInteractionClass(classHandle);

    privateRefs->executeService(&req, &rep);
        }

// 5.6
void RTI1516ambassador::subscribeObjectClassAttributes
(rti1516e::ObjectClassHandle theClass,
        rti1516e::AttributeHandleSet const & attributeList,
        bool active, std::wstring const & updateRateDesignator)
throw (rti1516e::ObjectClassNotDefined,
        rti1516e::AttributeNotDefined,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Subscribe_Object_Class_Attributes req, rep ;
    G.Out(pdGendoc,"enter RTI1516ambassador::subscribeObjectClassAttributes");

    const certi::ObjectClassHandle objectClassHandle = rti1516e::ObjectClassHandleFriend::toCertiHandle(theClass);
    req.setObjectClass(objectClassHandle);

    req.setAttributesSize(attributeList.size());
    uint32_t i = 0;
    for ( rti1516e::AttributeHandleSet::const_iterator it = attributeList.begin(); it != attributeList.end(); it++, ++i)
    {
        req.setAttributes(rti1516e::AttributeHandleFriend::toCertiHandle(*it),i);
    }
    req.setActive(active);

    privateRefs->executeService(&req, &rep);
    G.Out(pdGendoc,"exit  RTI1516ambassador::subscribeObjectClassAttributes");
        }

// 5.7
void RTI1516ambassador::unsubscribeObjectClass
(rti1516e::ObjectClassHandle theClass)
throw (rti1516e::ObjectClassNotDefined,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Unsubscribe_Object_Class req, rep ;

    const certi::ObjectClassHandle objectClassHandle = rti1516e::ObjectClassHandleFriend::toCertiHandle(theClass);
    req.setObjectClass(objectClassHandle);

    privateRefs->executeService(&req, &rep);
        }

void RTI1516ambassador::unsubscribeObjectClassAttributes
(rti1516e::ObjectClassHandle theClass,
        rti1516e::AttributeHandleSet const & attributeList)
throw (rti1516e::ObjectClassNotDefined,
        rti1516e::AttributeNotDefined,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"Not yet implemented");
        }

// 5.8
void RTI1516ambassador::subscribeInteractionClass
(rti1516e::InteractionClassHandle theClass,
        bool active)
throw (rti1516e::InteractionClassNotDefined,
        rti1516e::FederateServiceInvocationsAreBeingReportedViaMOM,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Subscribe_Interaction_Class req, rep ;
    const certi::InteractionClassHandle classHandle = rti1516e::InteractionClassHandleFriend::toCertiHandle(theClass);
    req.setInteractionClass(classHandle);

    privateRefs->executeService(&req, &rep);
        }

// 5.9
void RTI1516ambassador::unsubscribeInteractionClass
(rti1516e::InteractionClassHandle theClass)
throw (rti1516e::InteractionClassNotDefined,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Unsubscribe_Interaction_Class req, rep ;

    const certi::InteractionClassHandle classHandle = rti1516e::InteractionClassHandleFriend::toCertiHandle(theClass);
    req.setInteractionClass(classHandle);

    privateRefs->executeService(&req, &rep);
        }

////////////////////////////////
// Object Management Services //
////////////////////////////////

// 6.2
void RTI1516ambassador::reserveObjectInstanceName
(std::wstring const & theObjectInstanceName)
throw (rti1516e::IllegalName,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Reserve_Object_Instance_Name req, rep;

    std::string objInstanceName(theObjectInstanceName.begin(), theObjectInstanceName.end());
    req.setObjectName(objInstanceName);
    privateRefs->executeService(&req, &rep);
        }

// 6.4
void RTI1516ambassador::releaseObjectInstanceName (
        std::wstring const & theObjectInstanceName)
throw (
        rti1516e::ObjectInstanceNameNotReserved,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError) {
    throw rti1516e::RTIinternalError(L"releaseObjectInstanceName Not Implemented");
}

// 6.5
void RTI1516ambassador::reserveMultipleObjectInstanceName (
        std::set<std::wstring> const & theObjectInstanceNames)
throw (
        rti1516e::IllegalName,
        rti1516e::NameSetWasEmpty,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError) {
    throw rti1516e::RTIinternalError(L"reserveMultipleObjectInstanceName Not Implemented");
}

// 6.7
void RTI1516ambassador::releaseMultipleObjectInstanceName (
        std::set<std::wstring> const & theObjectInstanceNames)
throw (
        rti1516e::ObjectInstanceNameNotReserved,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError) {
    throw rti1516e::RTIinternalError(L"releaseMultipleObjectInstanceName Not Implemented");
}

// 6.8
rti1516e::ObjectInstanceHandle RTI1516ambassador::registerObjectInstance
(rti1516e::ObjectClassHandle theClass)
throw (rti1516e::ObjectClassNotDefined,
        rti1516e::ObjectClassNotPublished,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Register_Object_Instance req, rep ;

    req.setObjectClass(rti1516e::ObjectClassHandleFriend::toCertiHandle(theClass));
    privateRefs->executeService(&req, &rep);
    return rti1516e::ObjectInstanceHandleFriend::createRTI1516Handle(rep.getObject());
        }

rti1516e::ObjectInstanceHandle RTI1516ambassador::registerObjectInstance
(rti1516e::ObjectClassHandle theClass,
        std::wstring const & theObjectInstanceName)
throw (rti1516e::ObjectClassNotDefined,
        rti1516e::ObjectClassNotPublished,
        rti1516e::ObjectInstanceNameNotReserved,
        rti1516e::ObjectInstanceNameInUse,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Register_Object_Instance req, rep ;

    std::string nameString(theObjectInstanceName.begin(), theObjectInstanceName.end());
    req.setObjectName(nameString);
    req.setObjectClass(rti1516e::ObjectClassHandleFriend::toCertiHandle(theClass));
    privateRefs->executeService(&req, &rep);

    return rti1516e::ObjectInstanceHandleFriend::createRTI1516Handle(rep.getObject());
        }

// 6.10
void RTI1516ambassador::updateAttributeValues
(rti1516e::ObjectInstanceHandle theObject,
        rti1516e::AttributeHandleValueMap const & theAttributeValues,
        rti1516e::VariableLengthData const & theUserSuppliedTag)
throw (rti1516e::ObjectInstanceNotKnown,
        rti1516e::AttributeNotDefined,
        rti1516e::AttributeNotOwned,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    G.Out(pdGendoc,"enter RTI1516ambassador::updateAttributeValues without time");
    M_Update_Attribute_Values req, rep ;

    req.setObject(rti1516e::ObjectInstanceHandleFriend::toCertiHandle(theObject));
    if ( &theUserSuppliedTag == NULL || theUserSuppliedTag.data() == NULL)
    {
        throw rti1516e::RTIinternalError(L"Calling updateAttributeValues with Tag NULL");
    }

    req.setTag(varLengthDataAsString(theUserSuppliedTag));

    assignAHVMAndExecuteService(theAttributeValues, req, rep);

    G.Out(pdGendoc,"exit  RTI1516ambassador::updateAttributeValues without time");
        }

rti1516e::MessageRetractionHandle RTI1516ambassador::updateAttributeValues
(rti1516e::ObjectInstanceHandle theObject,
        rti1516e::AttributeHandleValueMap const & theAttributeValues,
        rti1516e::VariableLengthData const & theUserSuppliedTag,
        rti1516e::LogicalTime const & theTime)
throw (rti1516e::ObjectInstanceNotKnown,
        rti1516e::AttributeNotDefined,
        rti1516e::AttributeNotOwned,
        rti1516e::InvalidLogicalTime,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    G.Out(pdGendoc,"enter RTI1516ambassador::updateAttributeValues with time");
    M_Update_Attribute_Values req, rep ;

    req.setObject(rti1516e::ObjectInstanceHandleFriend::toCertiHandle(theObject));

    certi::FederationTime certiFedTime(certi_cast<RTI1516fedTime>()(theTime).getFedTime());
    req.setDate(certiFedTime);

    if ( &theUserSuppliedTag == NULL || theUserSuppliedTag.data() == NULL)
    {
        throw rti1516e::RTIinternalError(L"Calling updateAttributeValues with Tag NULL");
    }

    req.setTag(varLengthDataAsString(theUserSuppliedTag));

    assignAHVMAndExecuteService(theAttributeValues, req, rep);

    G.Out(pdGendoc,"return  RTI1516ambassador::updateAttributeValues with time");
    certi::FederateHandle certiHandle = rep.getEventRetraction().getSendingFederate();
    uint64_t serialNum = rep.getEventRetraction().getSN();
    return rti1516e::MessageRetractionHandleFriend::createRTI1516Handle(certiHandle, serialNum);
        }

// 6.12
void RTI1516ambassador::sendInteraction
(rti1516e::InteractionClassHandle theInteraction,
        rti1516e::ParameterHandleValueMap const & theParameterValues,
        rti1516e::VariableLengthData const & theUserSuppliedTag)
throw (rti1516e::InteractionClassNotPublished,
        rti1516e::InteractionClassNotDefined,
        rti1516e::InteractionParameterNotDefined,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Send_Interaction req, rep ;

    const certi::InteractionClassHandle classHandle = rti1516e::InteractionClassHandleFriend::toCertiHandle(theInteraction);
    req.setInteractionClass(classHandle);

    if (&theUserSuppliedTag == NULL || theUserSuppliedTag.data() == NULL )
    {
        throw rti1516e::RTIinternalError (L"Calling sendIntercation with Tag NULL") ;
    }

    req.setTag(varLengthDataAsString(theUserSuppliedTag));
    req.setRegion(0);

    assignPHVMAndExecuteService(theParameterValues, req, rep);
        }

rti1516e::MessageRetractionHandle RTI1516ambassador::sendInteraction
(rti1516e::InteractionClassHandle theInteraction,
        rti1516e::ParameterHandleValueMap const & theParameterValues,
        rti1516e::VariableLengthData const & theUserSuppliedTag,
        rti1516e::LogicalTime const & theTime)
throw (rti1516e::InteractionClassNotPublished,
        rti1516e::InteractionClassNotDefined,
        rti1516e::InteractionParameterNotDefined,
        rti1516e::InvalidLogicalTime,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Send_Interaction req, rep ;

    const certi::InteractionClassHandle classHandle = rti1516e::InteractionClassHandleFriend::toCertiHandle(theInteraction);
    req.setInteractionClass(classHandle);

    certi::FederationTime certiFedTime(certi_cast<RTI1516fedTime>()(theTime).getFedTime());
    req.setDate(certiFedTime);

    if (&theUserSuppliedTag == NULL || theUserSuppliedTag.data() == NULL ) {
        throw rti1516e::RTIinternalError(L"Calling sendInteraction with Tag NULL") ;
    }

    req.setTag(varLengthDataAsString(theUserSuppliedTag));
    req.setRegion(0);

    assignPHVMAndExecuteService(theParameterValues, req, rep);

    certi::FederateHandle certiHandle = rep.getEventRetraction().getSendingFederate();
    uint64_t serialNr = rep.getEventRetraction().getSN();
    rti1516e::MessageRetractionHandle rti1516handle = rti1516e::MessageRetractionHandleFriend::createRTI1516Handle(certiHandle, serialNr);

    return rti1516handle;
        }

// 6.14
void RTI1516ambassador::deleteObjectInstance
(rti1516e::ObjectInstanceHandle theObject,
        rti1516e::VariableLengthData const & theUserSuppliedTag)
throw (rti1516e::DeletePrivilegeNotHeld,
        rti1516e::ObjectInstanceNotKnown,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Delete_Object_Instance req, rep ;

    req.setObject(rti1516e::ObjectInstanceHandleFriend::toCertiHandle(theObject));
    if ( &theUserSuppliedTag == NULL || theUserSuppliedTag.data() == NULL)
    {
        throw rti1516e::RTIinternalError(L"Calling deleteObjectInstance with Tag NULL") ;
    }

    req.setTag(varLengthDataAsString(theUserSuppliedTag));

    privateRefs->executeService(&req, &rep);
        }

rti1516e::MessageRetractionHandle RTI1516ambassador::deleteObjectInstance
(rti1516e::ObjectInstanceHandle theObject,
        rti1516e::VariableLengthData const & theUserSuppliedTag,
        rti1516e::LogicalTime  const & theTime)
throw (rti1516e::DeletePrivilegeNotHeld,
        rti1516e::ObjectInstanceNotKnown,
        rti1516e::InvalidLogicalTime,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Delete_Object_Instance req, rep ;

    req.setObject(rti1516e::ObjectInstanceHandleFriend::toCertiHandle(theObject));

    certi::FederationTime certiFedTime(certi_cast<RTI1516fedTime>()(theTime).getFedTime());
    req.setDate(certiFedTime);

    if ( &theUserSuppliedTag == NULL || theUserSuppliedTag.data() == NULL)
    {
        throw rti1516e::RTIinternalError(L"Calling deleteObjectInstance with Tag NULL") ;
    }

    req.setTag(varLengthDataAsString(theUserSuppliedTag));

    privateRefs->executeService(&req, &rep);

    certi::FederateHandle certiHandle = rep.getEventRetraction().getSendingFederate();
    uint64_t serialNum = rep.getEventRetraction().getSN();
    return rti1516e::MessageRetractionHandleFriend::createRTI1516Handle(certiHandle, serialNum);
        }

// 6.12
void RTI1516ambassador::localDeleteObjectInstance
(rti1516e::ObjectInstanceHandle theObject)
throw (rti1516e::ObjectInstanceNotKnown,
        rti1516e::FederateOwnsAttributes,
        rti1516e::OwnershipAcquisitionPending,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    throw rti1516e::RTIinternalError(L"unimplemented service localDeleteObjectInstance");
    M_Local_Delete_Object_Instance req, rep ;

    req.setObject(rti1516e::ObjectInstanceHandleFriend::toCertiHandle(theObject));
    privateRefs->executeService(&req, &rep);
        }

// 6.19
void RTI1516ambassador::requestAttributeValueUpdate
(rti1516e::ObjectInstanceHandle theObject,
        rti1516e::AttributeHandleSet const & theAttributes,
        rti1516e::VariableLengthData const & theUserSuppliedTag)
throw (rti1516e::ObjectInstanceNotKnown,
        rti1516e::AttributeNotDefined,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Request_Object_Attribute_Value_Update req, rep ;

    G.Out(pdGendoc,"enter RTI1516ambassador::requestObjectAttributeValueUpdate");
    req.setObject(rti1516e::ObjectInstanceHandleFriend::toCertiHandle(theObject));

    size_t attr_num = theAttributes.size();
    req.setAttributesSize( attr_num );
    uint32_t i = 0;
    for ( rti1516e::AttributeHandleSet::const_iterator it = theAttributes.begin(); i < attr_num; ++it, ++i)
    {
        req.setAttributes(rti1516e::AttributeHandleFriend::toCertiHandle(*it),i);
    }
    req.setTag(varLengthDataAsString(theUserSuppliedTag));

    privateRefs->executeService(&req, &rep);
    G.Out(pdGendoc,"exit  RTI1516ambassador::requestObjectAttributeValueUpdate");
        }

void RTI1516ambassador::requestAttributeValueUpdate
(rti1516e::ObjectClassHandle theClass,
        rti1516e::AttributeHandleSet const & theAttributes,
        rti1516e::VariableLengthData const & theUserSuppliedTag)
throw (rti1516e::ObjectClassNotDefined,
        rti1516e::AttributeNotDefined,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Request_Class_Attribute_Value_Update req, rep ;
    G.Out(pdGendoc,"enter RTI1516ambassador::requestClassAttributeValueUpdate");
    req.setObjectClass(rti1516e::ObjectClassHandleFriend::toCertiHandle(theClass));

    assignAHSAndExecuteService(theAttributes, req, rep);

    G.Out(pdGendoc,"exit RTI1516ambassador::requestClassAttributeValueUpdate");
        }

// 6.23
void RTI1516ambassador::requestAttributeTransportationTypeChange
(rti1516e::ObjectInstanceHandle theObject,
        rti1516e::AttributeHandleSet const & theAttributes,
        rti1516e::TransportationType theType)
throw (rti1516e::AttributeAlreadyBeingChanged,
        rti1516e::ObjectInstanceNotKnown,
        rti1516e::AttributeNotDefined,
        rti1516e::AttributeNotOwned,
        rti1516e::FederateNotExecutionMember,
        rti1516e::InvalidTransportationType,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Change_Attribute_Transportation_Type req, rep ;
    req.setObject(rti1516e::ObjectInstanceHandleFriend::toCertiHandle(theObject));
    req.setTransportationType(toCertiTransportationType(theType));

    req.setAttributesSize(theAttributes.size());
    uint32_t i = 0;
    for ( rti1516e::AttributeHandleSet::const_iterator it = theAttributes.begin(); it != theAttributes.end(); it++, ++i)
    {
        req.setAttributes(rti1516e::AttributeHandleFriend::toCertiHandle(*it),i);
    }

    privateRefs->executeService(&req, &rep);
        }
// 6.25
void RTI1516ambassador::queryAttributeTransportationType (
        rti1516e::ObjectInstanceHandle theObject,
        rti1516e::AttributeHandle theAttribute)
throw (
        rti1516e::AttributeNotDefined,
        rti1516e::ObjectInstanceNotKnown,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError) {
    throw rti1516e::RTIinternalError(L"queryAttributeTransportationType Not Implemented");
}

// 6.27
void RTI1516ambassador::requestInteractionTransportationTypeChange
(rti1516e::InteractionClassHandle theClass,
        rti1516e::TransportationType theType)
throw (rti1516e::InteractionClassAlreadyBeingChanged,
        rti1516e::InteractionClassNotPublished,
        rti1516e::InteractionClassNotDefined,
        rti1516e::InvalidTransportationType,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Change_Interaction_Transportation_Type req, rep ;

    req.setInteractionClass(rti1516e::InteractionClassHandleFriend::toCertiHandle(theClass));
    req.setTransportationType(toCertiTransportationType(theType));

    privateRefs->executeService(&req, &rep);
        }

// 6.29
void RTI1516ambassador::queryInteractionTransportationType (
            rti1516e::FederateHandle theFederate,
            rti1516e::InteractionClassHandle theInteraction)
    throw (
            rti1516e::InteractionClassNotDefined,
            rti1516e::SaveInProgress,
            rti1516e::RestoreInProgress,
            rti1516e::FederateNotExecutionMember,
            rti1516e::NotConnected,
            rti1516e::RTIinternalError) {
    throw rti1516e::RTIinternalError(L"queryInteractionTransportationType Not Implemented");
}

///////////////////////////////////
// Ownership Management Services //
///////////////////////////////////
// 7.2
void RTI1516ambassador::unconditionalAttributeOwnershipDivestiture
(rti1516e::ObjectInstanceHandle theObject,
        rti1516e::AttributeHandleSet const & theAttributes)
throw (rti1516e::ObjectInstanceNotKnown,
        rti1516e::AttributeNotDefined,
        rti1516e::AttributeNotOwned,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Unconditional_Attribute_Ownership_Divestiture req, rep ;
    req.setObject(rti1516e::ObjectInstanceHandleFriend::toCertiHandle(theObject));

    req.setAttributesSize(theAttributes.size());
    uint32_t i=0;
    for ( rti1516e::AttributeHandleSet::const_iterator it = theAttributes.begin(); it != theAttributes.end(); it++, ++i)
    {
        req.setAttributes(rti1516e::AttributeHandleFriend::toCertiHandle(*it),i);
    }

    privateRefs->executeService(&req, &rep);
        }

// 7.3
void RTI1516ambassador::negotiatedAttributeOwnershipDivestiture
(rti1516e::ObjectInstanceHandle theObject,
        rti1516e::AttributeHandleSet const & theAttributes,
        rti1516e::VariableLengthData const & theUserSuppliedTag)
throw (rti1516e::ObjectInstanceNotKnown,
        rti1516e::AttributeNotDefined,
        rti1516e::AttributeNotOwned,
        rti1516e::AttributeAlreadyBeingDivested,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Negotiated_Attribute_Ownership_Divestiture req, rep ;
    req.setObject(rti1516e::ObjectInstanceHandleFriend::toCertiHandle(theObject));
    if (&theUserSuppliedTag == NULL || theUserSuppliedTag.data() == NULL) {
        throw rti1516e::RTIinternalError (L"Calling negotiatedAttributeOwnershipDivestiture with Tag NULL") ;
    }
    req.setTag(certi::varLengthDataAsString(theUserSuppliedTag));

    req.setAttributesSize(theAttributes.size());
    uint32_t i=0;
    for ( rti1516e::AttributeHandleSet::const_iterator it = theAttributes.begin(); it != theAttributes.end(); it++, ++i)
    {
        req.setAttributes(rti1516e::AttributeHandleFriend::toCertiHandle(*it),i);
    }

    privateRefs->executeService(&req, &rep);
        }

// 7.6
void RTI1516ambassador::confirmDivestiture
(rti1516e::ObjectInstanceHandle theObject,
        rti1516e::AttributeHandleSet const & confirmedAttributes,
        rti1516e::VariableLengthData const & theUserSuppliedTag)
throw (rti1516e::ObjectInstanceNotKnown,
        rti1516e::AttributeNotDefined,
        rti1516e::AttributeNotOwned,
        rti1516e::AttributeDivestitureWasNotRequested,
        rti1516e::NoAcquisitionPending,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"Not yet implemented");
        }

// 7.8
void RTI1516ambassador::attributeOwnershipAcquisition
(rti1516e::ObjectInstanceHandle theObject,
        rti1516e::AttributeHandleSet const & desiredAttributes,
        rti1516e::VariableLengthData const & theUserSuppliedTag)
throw (rti1516e::ObjectInstanceNotKnown,
        rti1516e::ObjectClassNotPublished,
        rti1516e::AttributeNotDefined,
        rti1516e::AttributeNotPublished,
        rti1516e::FederateOwnsAttributes,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Attribute_Ownership_Acquisition req, rep ;

    req.setObject(rti1516e::ObjectInstanceHandleFriend::toCertiHandle(theObject));
    if (&theUserSuppliedTag == NULL || theUserSuppliedTag.data() == NULL )
    {
        throw rti1516e::RTIinternalError (L"Calling attributeOwnershipAcquisition with Tag NULL") ;
    }
    req.setTag(certi::varLengthDataAsString(theUserSuppliedTag));

    req.setAttributesSize(desiredAttributes.size());
    uint32_t i = 0;
    for ( rti1516e::AttributeHandleSet::const_iterator it = desiredAttributes.begin(); it != desiredAttributes.end(); it++, ++i)
    {
        req.setAttributes(rti1516e::AttributeHandleFriend::toCertiHandle(*it),i);
    }

    privateRefs->executeService(&req, &rep);
        }

// 7.9
void RTI1516ambassador::attributeOwnershipAcquisitionIfAvailable
(rti1516e::ObjectInstanceHandle theObject,
        rti1516e::AttributeHandleSet const & desiredAttributes)
throw (rti1516e::ObjectInstanceNotKnown,
        rti1516e::ObjectClassNotPublished,
        rti1516e::AttributeNotDefined,
        rti1516e::AttributeNotPublished,
        rti1516e::FederateOwnsAttributes,
        rti1516e::AttributeAlreadyBeingAcquired,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Attribute_Ownership_Acquisition_If_Available req, rep ;

    req.setObject(rti1516e::ObjectInstanceHandleFriend::toCertiHandle(theObject));

    req.setAttributesSize(desiredAttributes.size());
    uint32_t i = 0;
    for ( rti1516e::AttributeHandleSet::const_iterator it = desiredAttributes.begin(); it != desiredAttributes.end(); it++, ++i)
    {
        req.setAttributes(rti1516e::AttributeHandleFriend::toCertiHandle(*it),i);
    }

    privateRefs->executeService(&req, &rep);
        }

void RTI1516ambassador::attributeOwnershipReleaseDenied (
            rti1516e::ObjectInstanceHandle theObject,
            rti1516e::AttributeHandleSet const & theAttributes)
    throw (
            rti1516e::AttributeNotOwned,
            rti1516e::AttributeNotDefined,
            rti1516e::ObjectInstanceNotKnown,
            rti1516e::SaveInProgress,
            rti1516e::RestoreInProgress,
            rti1516e::FederateNotExecutionMember,
            rti1516e::NotConnected,
            rti1516e::RTIinternalError) {
    throw rti1516e::RTIinternalError(L"attributeOwnershipReleaseDenied Not Implemented");
}

// 7.13 (in RTI1.3 this function is called: AttributeOwnershipReleaseResponse)
void RTI1516ambassador::attributeOwnershipDivestitureIfWanted
(rti1516e::ObjectInstanceHandle theObject,
        rti1516e::AttributeHandleSet const & theAttributes,
        rti1516e::AttributeHandleSet & theDivestedAttributes) // filled by RTI
throw (rti1516e::ObjectInstanceNotKnown,
        rti1516e::AttributeNotDefined,
        rti1516e::AttributeNotOwned,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Attribute_Ownership_Release_Response req, rep ;

    req.setObject(rti1516e::ObjectInstanceHandleFriend::toCertiHandle(theObject));

    assignAHSAndExecuteService(theAttributes, req, rep);

    if (rep.getExceptionType() == e_NO_EXCEPTION) {
        theDivestedAttributes.clear();
        for (uint32_t i=0;i<rep.getAttributesSize();++i) {
            theDivestedAttributes.insert(rti1516e::AttributeHandleFriend::createRTI1516Handle(rep.getAttributes()[i]));
        }
    }

        }

// 7.15
void RTI1516ambassador::cancelNegotiatedAttributeOwnershipDivestiture
(rti1516e::ObjectInstanceHandle theObject,
        rti1516e::AttributeHandleSet const & theAttributes)
throw (rti1516e::ObjectInstanceNotKnown,
        rti1516e::AttributeNotDefined,
        rti1516e::AttributeNotOwned,
        rti1516e::AttributeDivestitureWasNotRequested,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Cancel_Negotiated_Attribute_Ownership_Divestiture req, rep ;

    req.setObject(rti1516e::ObjectInstanceHandleFriend::toCertiHandle(theObject));

    req.setAttributesSize(theAttributes.size());
    uint32_t i = 0;
    for ( rti1516e::AttributeHandleSet::const_iterator it = theAttributes.begin(); it != theAttributes.end(); it++, ++i)
    {
        req.setAttributes(rti1516e::AttributeHandleFriend::toCertiHandle(*it),i);
    }

    privateRefs->executeService(&req, &rep);
        }

// 7.15
void RTI1516ambassador::cancelAttributeOwnershipAcquisition
(rti1516e::ObjectInstanceHandle theObject,
        rti1516e::AttributeHandleSet const & theAttributes)
throw (rti1516e::ObjectInstanceNotKnown,
        rti1516e::AttributeNotDefined,
        rti1516e::AttributeAlreadyOwned,
        rti1516e::AttributeAcquisitionWasNotRequested,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Cancel_Attribute_Ownership_Acquisition req, rep ;

    req.setObject(rti1516e::ObjectInstanceHandleFriend::toCertiHandle(theObject));

    req.setAttributesSize(theAttributes.size());
    uint32_t i = 0;
    for ( rti1516e::AttributeHandleSet::const_iterator it = theAttributes.begin(); it != theAttributes.end(); it++, ++i)
    {
        req.setAttributes(rti1516e::AttributeHandleFriend::toCertiHandle(*it),i);
    }

    privateRefs->executeService(&req, &rep);
        }

// 7.16
void RTI1516ambassador::queryAttributeOwnership
(rti1516e::ObjectInstanceHandle theObject,
        rti1516e::AttributeHandle theAttribute)
throw (rti1516e::ObjectInstanceNotKnown,
        rti1516e::AttributeNotDefined,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Query_Attribute_Ownership req, rep ;

    req.setObject(rti1516e::ObjectInstanceHandleFriend::toCertiHandle(theObject));
    req.setAttribute(rti1516e::AttributeHandleFriend::toCertiHandle(theAttribute));

    privateRefs->executeService(&req, &rep);
        }

// 7.18
bool RTI1516ambassador::isAttributeOwnedByFederate
(rti1516e::ObjectInstanceHandle theObject,
        rti1516e::AttributeHandle theAttribute)
throw (rti1516e::ObjectInstanceNotKnown,
        rti1516e::AttributeNotDefined,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Is_Attribute_Owned_By_Federate req, rep ;

    req.setObject(rti1516e::ObjectInstanceHandleFriend::toCertiHandle(theObject));
    req.setAttribute(rti1516e::AttributeHandleFriend::toCertiHandle(theAttribute));

    privateRefs->executeService(&req, &rep);

    return (rep.getTag() == "RTI_TRUE") ? true : false;
        }

//////////////////////////////
// Time Management Services //
//////////////////////////////

// 8.2
void RTI1516ambassador::enableTimeRegulation
(rti1516e::LogicalTimeInterval const & theLookahead)
throw (rti1516e::TimeRegulationAlreadyEnabled,
        rti1516e::InvalidLookahead,
        rti1516e::InTimeAdvancingState,
        rti1516e::RequestForTimeRegulationPending,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Enable_Time_Regulation req, rep ;

    //req.setDate(certi_cast<RTIfedTime>()(theFederateTime).getTime());  //JRE: DATE IS NOT USED!

    //JRE: is dit wel goed?
    //JvY: TODO Controleren of dit blijft werken met andere tijdsimplementaties
    union ud {
        double   dv;
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
void RTI1516ambassador::disableTimeRegulation ()
throw (rti1516e::TimeRegulationIsNotEnabled,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Disable_Time_Regulation req, rep ;

    privateRefs->executeService(&req, &rep);
        }

// 8.5
void RTI1516ambassador::enableTimeConstrained ()
throw (rti1516e::TimeConstrainedAlreadyEnabled,
        rti1516e::InTimeAdvancingState,
        rti1516e::RequestForTimeConstrainedPending,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Enable_Time_Constrained req, rep ;
    privateRefs->executeService(&req, &rep);
        }

// 8.7
void RTI1516ambassador::disableTimeConstrained ()
throw (rti1516e::TimeConstrainedIsNotEnabled,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Disable_Time_Constrained req, rep ;
    privateRefs->executeService(&req, &rep);
        }

// 8.8
void RTI1516ambassador::timeAdvanceRequest
(rti1516e::LogicalTime const & theTime)
throw (rti1516e::InvalidLogicalTime,
        rti1516e::LogicalTimeAlreadyPassed,
        rti1516e::InTimeAdvancingState,
        rti1516e::RequestForTimeRegulationPending,
        rti1516e::RequestForTimeConstrainedPending,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Time_Advance_Request req, rep ;

    certi::FederationTime certiFedTime(certi_cast<RTI1516fedTime>()(theTime).getFedTime());
    req.setDate(certiFedTime);
    privateRefs->executeService(&req, &rep);
        }

// 8.9
void RTI1516ambassador::timeAdvanceRequestAvailable
(rti1516e::LogicalTime const & theTime)
throw (rti1516e::InvalidLogicalTime,
        rti1516e::LogicalTimeAlreadyPassed,
        rti1516e::InTimeAdvancingState,
        rti1516e::RequestForTimeRegulationPending,
        rti1516e::RequestForTimeConstrainedPending,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Time_Advance_Request_Available req, rep ;

    certi::FederationTime certiFedTime(certi_cast<RTI1516fedTime>()(theTime).getFedTime());
    req.setDate(certiFedTime);

    privateRefs->executeService(&req, &rep);
        }

// 8.10
void RTI1516ambassador::nextMessageRequest
(rti1516e::LogicalTime const & theTime)
throw (rti1516e::InvalidLogicalTime,
        rti1516e::LogicalTimeAlreadyPassed,
        rti1516e::InTimeAdvancingState,
        rti1516e::RequestForTimeRegulationPending,
        rti1516e::RequestForTimeConstrainedPending,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Next_Event_Request req, rep ;

    certi::FederationTime certiFedTime(certi_cast<RTI1516fedTime>()(theTime).getFedTime());
    req.setDate(certiFedTime);

    privateRefs->executeService(&req, &rep);
        }

// 8.11
void RTI1516ambassador::nextMessageRequestAvailable
(rti1516e::LogicalTime const & theTime)
throw (rti1516e::InvalidLogicalTime,
        rti1516e::LogicalTimeAlreadyPassed,
        rti1516e::InTimeAdvancingState,
        rti1516e::RequestForTimeRegulationPending,
        rti1516e::RequestForTimeConstrainedPending,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Next_Event_Request_Available req, rep ;

    certi::FederationTime certiFedTime(certi_cast<RTI1516fedTime>()(theTime).getFedTime());
    req.setDate(certiFedTime);

    privateRefs->executeService(&req, &rep);
        }

// 8.12
void RTI1516ambassador::flushQueueRequest
(rti1516e::LogicalTime const & theTime)
throw (rti1516e::InvalidLogicalTime,
        rti1516e::LogicalTimeAlreadyPassed,
        rti1516e::InTimeAdvancingState,
        rti1516e::RequestForTimeRegulationPending,
        rti1516e::RequestForTimeConstrainedPending,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    // JvY: Implementation copied from previous CERTI implementation, including immediate throw.
    throw rti1516e::RTIinternalError(L"Unimplemented Service flushQueueRequest");
    M_Flush_Queue_Request req, rep ;

    certi::FederationTime certiFedTime(certi_cast<RTI1516fedTime>()(theTime).getFedTime());
    req.setDate(certiFedTime);

    privateRefs->executeService(&req, &rep);
        }

// 8.14
void RTI1516ambassador::enableAsynchronousDelivery ()
throw (rti1516e::AsynchronousDeliveryAlreadyEnabled,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    // throw AsynchronousDeliveryAlreadyEnabled("Default value (non HLA)");

    M_Enable_Asynchronous_Delivery req, rep ;

    privateRefs->executeService(&req, &rep);
        }

// 8.15
void RTI1516ambassador::disableAsynchronousDelivery ()
throw (rti1516e::AsynchronousDeliveryAlreadyDisabled,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Disable_Asynchronous_Delivery req, rep ;

    privateRefs->executeService(&req, &rep);
        }

// 8.16
bool RTI1516ambassador::queryGALT (rti1516e::LogicalTime & theTime)
throw (rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    //TODO JRE: goed testen! Is GALT wel precies het zelfde als LBTS?
    M_Query_Lbts req, rep ;

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
void RTI1516ambassador::queryLogicalTime (rti1516e::LogicalTime & theTime)
throw (rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Query_Federate_Time req, rep ;

    privateRefs->executeService(&req, &rep);

    // JvY: TODO Controleren of dit blijft werken met andere tijdsimplementaties
    certi_cast<RTI1516fedTime>()(theTime) = rep.getDate().getTime();
        }

// 8.18
bool RTI1516ambassador::queryLITS (rti1516e::LogicalTime & theTime)
throw (rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    //TODO JRE: goed testen! Is LITS wel precies het zelfde als QueryMinNextEventTime?
    M_Query_Min_Next_Event_Time req, rep ;

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
void RTI1516ambassador::modifyLookahead
(rti1516e::LogicalTimeInterval const & theLookahead)
throw (rti1516e::TimeRegulationIsNotEnabled,
        rti1516e::InvalidLookahead,
        rti1516e::InTimeAdvancingState,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"Not yet implemented");
        }

// 8.20
void RTI1516ambassador::queryLookahead (rti1516e::LogicalTimeInterval & interval)
throw (rti1516e::TimeRegulationIsNotEnabled,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"Not yet implemented");
        }

// 8.21
void RTI1516ambassador::retract
(rti1516e::MessageRetractionHandle theHandle)
throw (rti1516e::InvalidMessageRetractionHandle,
        rti1516e::TimeRegulationIsNotEnabled,
        rti1516e::MessageCanNoLongerBeRetracted,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    throw rti1516e::RTIinternalError(L"Unimplemented Service retract");
    M_Retract req, rep ;

    certi::EventRetraction event = rti1516e::MessageRetractionHandleFriend::createEventRetraction(theHandle);
    req.setEventRetraction(event);

    privateRefs->executeService(&req, &rep);
        }

// 8.23
void RTI1516ambassador::changeAttributeOrderType
(rti1516e::ObjectInstanceHandle theObject,
        rti1516e::AttributeHandleSet const & theAttributes,
        rti1516e::OrderType theType)
throw (rti1516e::ObjectInstanceNotKnown,
        rti1516e::AttributeNotDefined,
        rti1516e::AttributeNotOwned,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Change_Attribute_Order_Type req, rep ;

    req.setObject(rti1516e::ObjectInstanceHandleFriend::toCertiHandle(theObject));
    req.setOrder(certi::toCertiOrderType(theType));

    assignAHSAndExecuteService(theAttributes, req, rep);
        }

// 8.24
void RTI1516ambassador::changeInteractionOrderType
(rti1516e::InteractionClassHandle theClass,
        rti1516e::OrderType theType)
throw (rti1516e::InteractionClassNotDefined,
        rti1516e::InteractionClassNotPublished,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Change_Interaction_Order_Type req, rep ;

    req.setInteractionClass(rti1516e::InteractionClassHandleFriend::toCertiHandle(theClass));
    req.setOrder(certi::toCertiOrderType(theType));

    privateRefs->executeService(&req, &rep);
        }

//////////////////////////////////
// Data Distribution Management //
//////////////////////////////////

// 9.2
rti1516e::RegionHandle RTI1516ambassador::createRegion
(rti1516e::DimensionHandleSet const & theDimensions)
throw (rti1516e::InvalidDimensionHandle,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"Not yet implemented");
        }

// 9.3
void RTI1516ambassador::commitRegionModifications
(rti1516e::RegionHandleSet const & theRegionHandleSet)
throw (rti1516e::InvalidRegion,
        rti1516e::RegionNotCreatedByThisFederate,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"Not yet implemented");
        }

// 9.4
void RTI1516ambassador::deleteRegion
(rti1516e::RegionHandle const & theRegion)
throw (rti1516e::RegionInUseForUpdateOrSubscription,
        rti1516e::RegionNotCreatedByThisFederate,
        rti1516e::InvalidRegion,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"Not yet implemented");
        }

// 9.5
rti1516e::ObjectInstanceHandle RTI1516ambassador::registerObjectInstanceWithRegions
(rti1516e::ObjectClassHandle theClass,
        rti1516e::AttributeHandleSetRegionHandleSetPairVector const &
        theAttributeHandleSetRegionHandleSetPairVector)
throw (rti1516e::ObjectClassNotDefined,
        rti1516e::ObjectClassNotPublished,
        rti1516e::AttributeNotDefined,
        rti1516e::AttributeNotPublished,
        rti1516e::InvalidRegion,
        rti1516e::RegionNotCreatedByThisFederate,
        rti1516e::InvalidRegionContext,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"Not yet implemented");
        }

rti1516e::ObjectInstanceHandle RTI1516ambassador::registerObjectInstanceWithRegions
(rti1516e::ObjectClassHandle theClass,
        rti1516e::AttributeHandleSetRegionHandleSetPairVector const &
        theAttributeHandleSetRegionHandleSetPairVector,
        std::wstring const & theObjectInstanceName)
throw (rti1516e::ObjectClassNotDefined,
        rti1516e::ObjectClassNotPublished,
        rti1516e::AttributeNotDefined,
        rti1516e::AttributeNotPublished,
        rti1516e::InvalidRegion,
        rti1516e::RegionNotCreatedByThisFederate,
        rti1516e::InvalidRegionContext,
        rti1516e::ObjectInstanceNameNotReserved,
        rti1516e::ObjectInstanceNameInUse,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"Not yet implemented");
        }

// 9.6
void RTI1516ambassador::associateRegionsForUpdates
(rti1516e::ObjectInstanceHandle theObject,
        rti1516e::AttributeHandleSetRegionHandleSetPairVector const &
        theAttributeHandleSetRegionHandleSetPairVector)
throw (rti1516e::ObjectInstanceNotKnown,
        rti1516e::AttributeNotDefined,
        rti1516e::InvalidRegion,
        rti1516e::RegionNotCreatedByThisFederate,
        rti1516e::InvalidRegionContext,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"Not yet implemented");
        }

// 9.7
void RTI1516ambassador::unassociateRegionsForUpdates
(rti1516e::ObjectInstanceHandle theObject,
        rti1516e::AttributeHandleSetRegionHandleSetPairVector const &
        theAttributeHandleSetRegionHandleSetPairVector)
throw (rti1516e::ObjectInstanceNotKnown,
        rti1516e::AttributeNotDefined,
        rti1516e::InvalidRegion,
        rti1516e::RegionNotCreatedByThisFederate,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"Not yet implemented");
        }

// 9.8
void RTI1516ambassador::subscribeObjectClassAttributesWithRegions
(rti1516e::ObjectClassHandle theClass,
        rti1516e::AttributeHandleSetRegionHandleSetPairVector const &
        theAttributeHandleSetRegionHandleSetPairVector,
        bool active, std::wstring const & updateRateDesignator)
throw (rti1516e::ObjectClassNotDefined,
        rti1516e::AttributeNotDefined,
        rti1516e::InvalidRegion,
        rti1516e::RegionNotCreatedByThisFederate,
        rti1516e::InvalidRegionContext,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"Not yet implemented");
        }

// 9.9
void RTI1516ambassador::unsubscribeObjectClassAttributesWithRegions
(rti1516e::ObjectClassHandle theClass,
        rti1516e::AttributeHandleSetRegionHandleSetPairVector const &
        theAttributeHandleSetRegionHandleSetPairVector)
throw (rti1516e::ObjectClassNotDefined,
        rti1516e::AttributeNotDefined,
        rti1516e::InvalidRegion,
        rti1516e::RegionNotCreatedByThisFederate,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"Not yet implemented");
        }

// 9.10
void RTI1516ambassador::subscribeInteractionClassWithRegions
(rti1516e::InteractionClassHandle theClass,
        rti1516e::RegionHandleSet const & theRegionHandleSet,
        bool active)
throw (rti1516e::InteractionClassNotDefined,
        rti1516e::InvalidRegion,
        rti1516e::RegionNotCreatedByThisFederate,
        rti1516e::InvalidRegionContext,
        rti1516e::FederateServiceInvocationsAreBeingReportedViaMOM,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"Not yet implemented");
        }

// 9.11
void RTI1516ambassador::unsubscribeInteractionClassWithRegions
(rti1516e::InteractionClassHandle theClass,
        rti1516e::RegionHandleSet const & theRegionHandleSet)
throw (rti1516e::InteractionClassNotDefined,
        rti1516e::InvalidRegion,
        rti1516e::RegionNotCreatedByThisFederate,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"Not yet implemented");
        }

// 9.12
void RTI1516ambassador::sendInteractionWithRegions
(rti1516e::InteractionClassHandle theInteraction,
        rti1516e::ParameterHandleValueMap const & theParameterValues,
        rti1516e::RegionHandleSet const & theRegionHandleSet,
        rti1516e::VariableLengthData const & theUserSuppliedTag)
throw (rti1516e::InteractionClassNotDefined,
        rti1516e::InteractionClassNotPublished,
        rti1516e::InteractionParameterNotDefined,
        rti1516e::InvalidRegion,
        rti1516e::RegionNotCreatedByThisFederate,
        rti1516e::InvalidRegionContext,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"Not yet implemented");
        }

rti1516e::MessageRetractionHandle RTI1516ambassador::sendInteractionWithRegions
(rti1516e::InteractionClassHandle theInteraction,
        rti1516e::ParameterHandleValueMap const & theParameterValues,
        rti1516e::RegionHandleSet const & theRegionHandleSet,
        rti1516e::VariableLengthData const & theUserSuppliedTag,
        rti1516e::LogicalTime const & theTime)
throw (rti1516e::InteractionClassNotDefined,
        rti1516e::InteractionClassNotPublished,
        rti1516e::InteractionParameterNotDefined,
        rti1516e::InvalidRegion,
        rti1516e::RegionNotCreatedByThisFederate,
        rti1516e::InvalidRegionContext,
        rti1516e::InvalidLogicalTime,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"Not yet implemented");
        }

// 9.13
void RTI1516ambassador::requestAttributeValueUpdateWithRegions
(rti1516e::ObjectClassHandle theClass,
        rti1516e::AttributeHandleSetRegionHandleSetPairVector const & theSet,
        rti1516e::VariableLengthData const & theUserSuppliedTag)
throw (rti1516e::ObjectClassNotDefined,
        rti1516e::AttributeNotDefined,
        rti1516e::InvalidRegion,
        rti1516e::RegionNotCreatedByThisFederate,
        rti1516e::InvalidRegionContext,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"Not yet implemented");
        }

//////////////////////////
// RTI Support Services //
//////////////////////////

// 10.2
rti1516e::ResignAction RTI1516ambassador::getAutomaticResignDirective ()
throw (
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError) {
    throw rti1516e::RTIinternalError(L"getAutomaticResignDirective Not Implemented");
}

// 10.3
void RTI1516ambassador::setAutomaticResignDirective (
        rti1516e::ResignAction resignAction)
throw (
        rti1516e::InvalidResignAction,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError){
    throw rti1516e::RTIinternalError(L"setAutomaticResignDirective Not Implemented");
}

// 10.4
rti1516e::FederateHandle RTI1516ambassador::getFederateHandle (
        std::wstring const & theName)
throw (
        rti1516e::NameNotFound,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError){
    throw rti1516e::RTIinternalError(L"getFederateHandle Not Implemented");
}

// 10.5
std::wstring RTI1516ambassador::getFederateName (
        rti1516e::FederateHandle theHandle)
throw (
        rti1516e::InvalidFederateHandle,
        rti1516e::FederateHandleNotKnown,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError) {
    throw rti1516e::RTIinternalError(L"getFederateName Not Implemented");
}

// 10.2
rti1516e::ObjectClassHandle RTI1516ambassador::getObjectClassHandle
(std::wstring const & theName)
throw (rti1516e::NameNotFound,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Get_Object_Class_Handle req, rep ;

    G.Out(pdGendoc,"enter RTI1516ambassador::getObjectClassHandle");

    std::string nameAsString(theName.begin(), theName.end());
    req.setClassName(nameAsString);
    privateRefs->executeService(&req, &rep);

    G.Out(pdGendoc,"exit RTI1516ambassador::getObjectClassHandle");
    rti1516e::ObjectClassHandle rti1516Handle = rti1516e::ObjectClassHandleFriend::createRTI1516Handle(rep.getObjectClass());

    return rti1516Handle;
        }

std::wstring RTI1516ambassador::getObjectClassName
(rti1516e::ObjectClassHandle theHandle)
throw (rti1516e::InvalidObjectClassHandle,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Get_Object_Class_Name req, rep ;

    certi::ObjectClassHandle certiHandle = rti1516e::ObjectClassHandleFriend::toCertiHandle(theHandle);
    req.setObjectClass(certiHandle);
    try {
        privateRefs->executeService(&req, &rep);
    } catch (rti1516e::ObjectClassNotDefined &e)
    {
        throw rti1516e::InvalidObjectClassHandle(e.what());
    }

    std::string nameString = rep.getClassName();
    std::wstring nameWString(nameString.begin(), nameString.end());

    //return hla_strdup(rep.getClassName());
    return nameWString;
        }

rti1516e::ObjectClassHandle RTI1516ambassador::getKnownObjectClassHandle
(rti1516e::ObjectInstanceHandle theObject)
throw (rti1516e::ObjectInstanceNotKnown,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Get_Object_Class req, rep ;

    req.setObject(rti1516e::ObjectInstanceHandleFriend::toCertiHandle(theObject));
    privateRefs->executeService(&req, &rep);
    return rti1516e::ObjectClassHandleFriend::createRTI1516Handle(rep.getObjectClass());
        }

// 10.10
rti1516e::ObjectInstanceHandle RTI1516ambassador::getObjectInstanceHandle
(std::wstring const & theName)
throw (rti1516e::ObjectInstanceNotKnown,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Get_Object_Instance_Handle req, rep ;
    std::string nameString(theName.begin(), theName.end());
    req.setObjectInstanceName(nameString);

    privateRefs->executeService(&req, &rep);
    return rti1516e::ObjectInstanceHandleFriend::createRTI1516Handle(rep.getObject());
        }

// 10.11
std::wstring RTI1516ambassador::getObjectInstanceName
(rti1516e::ObjectInstanceHandle theHandle)
throw (rti1516e::ObjectInstanceNotKnown,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Get_Object_Instance_Name req, rep ;
    req.setObject(rti1516e::ObjectInstanceHandleFriend::toCertiHandle(theHandle));
    privateRefs->executeService(&req, &rep);

    //return hla_strdup(rep.getObjectInstanceName());
    std::string nameString = rep.getObjectInstanceName();
    std::wstring nameWString(nameString.begin(), nameString.end());

    return nameWString;
        }


rti1516e::AttributeHandle RTI1516ambassador::getAttributeHandle
(rti1516e::ObjectClassHandle whichClass,
        std::wstring const & theAttributeName)
throw (rti1516e::InvalidObjectClassHandle,
        rti1516e::NameNotFound,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    G.Out(pdGendoc,"enter RTI::RTI1516ambassador::getAttributeHandle");
    M_Get_Attribute_Handle req, rep ;

    std::string nameAsString(theAttributeName.begin(), theAttributeName.end());
    req.setAttributeName(nameAsString);
    req.setObjectClass(rti1516e::ObjectClassHandleFriend::toCertiHandle(whichClass));

    try {
        privateRefs->executeService(&req, &rep);
    } catch (rti1516e::ObjectClassNotDefined &e)
    {
        if ( ! whichClass.isValid() ) {
            throw rti1516e::InvalidObjectClassHandle(e.what());
        } else {
            throw rti1516e::NameNotFound(e.what());
        }
    }

    G.Out(pdGendoc,"exit  RTI::RTI1516ambassador::getAttributeHandle");
    return rti1516e::AttributeHandleFriend::createRTI1516Handle(rep.getAttribute());
        }

std::wstring RTI1516ambassador::getAttributeName
(rti1516e::ObjectClassHandle whichClass,
        rti1516e::AttributeHandle theHandle)
throw (rti1516e::InvalidObjectClassHandle,
        rti1516e::InvalidAttributeHandle,
        rti1516e::AttributeNotDefined,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Get_Attribute_Name req, rep ;

    req.setAttribute(rti1516e::AttributeHandleFriend::toCertiHandle(theHandle));
    req.setObjectClass(rti1516e::ObjectClassHandleFriend::toCertiHandle(whichClass));
    try {
        privateRefs->executeService(&req, &rep);
    } catch (rti1516e::ObjectClassNotDefined &e)
    {
        if ( !whichClass.isValid() )
        {
            throw rti1516e::InvalidObjectClassHandle(e.what());
        } else
        {
            throw;
        }
    } catch ( rti1516e::AttributeNotDefined &e)
    {
        if (! theHandle.isValid() )
        {
            throw rti1516e::InvalidAttributeHandle(e.what());
        } else
        {
            throw;
        }
    }

    //return hla_strdup(rep.getAttributeName());

    std::string nameString = rep.getAttributeName();
    std::wstring nameWString(nameString.begin(), nameString.end());

    return nameWString;
        }

// 10.13
double RTI1516ambassador::getUpdateRateValue (
        std::wstring const & updateRateDesignator)
throw (
        rti1516e::InvalidUpdateRateDesignator,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError) {
    throw rti1516e::RTIinternalError(L"getUpdateRateValue Not Implemented");
}

// 10.14
double RTI1516ambassador::getUpdateRateValueForAttribute (
        rti1516e::ObjectInstanceHandle theObject,
        rti1516e::AttributeHandle theAttribute)
throw (
        rti1516e::ObjectInstanceNotKnown,
        rti1516e::AttributeNotDefined,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError) {
    throw rti1516e::RTIinternalError(L"getUpdateRateValueForAttributeNot Implemented");
}

// 10.6
rti1516e::InteractionClassHandle RTI1516ambassador::getInteractionClassHandle
(std::wstring const & theName)
throw (rti1516e::NameNotFound,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Get_Interaction_Class_Handle req, rep ;
    std::string nameString(theName.begin(), theName.end());
    req.setClassName(nameString);

    privateRefs->executeService(&req, &rep);

    return rti1516e::InteractionClassHandleFriend::createRTI1516Handle(rep.getInteractionClass());
        }

// 10.7
std::wstring RTI1516ambassador::getInteractionClassName
(rti1516e::InteractionClassHandle theHandle)
throw (rti1516e::InvalidInteractionClassHandle,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Get_Interaction_Class_Name req, rep ;
    req.setInteractionClass(rti1516e::InteractionClassHandleFriend::toCertiHandle(theHandle));
    try {
        privateRefs->executeService(&req, &rep);
    } catch (rti1516e::InteractionClassNotDefined &e)
    {
        if ( !theHandle.isValid() )
        {
            throw rti1516e::InvalidInteractionClassHandle(e.what());
        } else
        {
            throw;
        }
    }

    //return hla_strdup(rep.getClassName());
    std::string nameString = rep.getClassName();
    std::wstring nameWString(nameString.begin(), nameString.end());

    return nameWString;
        }

// 10.8
rti1516e::ParameterHandle RTI1516ambassador::getParameterHandle
(rti1516e::InteractionClassHandle whichClass,
        std::wstring const & theName)
throw (rti1516e::InvalidInteractionClassHandle,
        rti1516e::NameNotFound,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Get_Parameter_Handle req, rep ;
    std::string nameString(theName.begin(), theName.end());
    req.setParameterName(nameString);
    req.setInteractionClass(rti1516e::InteractionClassHandleFriend::toCertiHandle(whichClass));

    try {
        privateRefs->executeService(&req, &rep);
    } catch (rti1516e::InteractionClassNotDefined &e)
    {
        if ( !whichClass.isValid() )
        {
            throw rti1516e::InvalidInteractionClassHandle(e.what());
        } else
        {
            throw;
        }
    }

    return rti1516e::ParameterHandleFriend::createRTI1516Handle(rep.getParameter());
        }

// 10.9
std::wstring RTI1516ambassador::getParameterName
(rti1516e::InteractionClassHandle whichClass,
        rti1516e::ParameterHandle theHandle)
throw ( rti1516e::InvalidInteractionClassHandle,
        rti1516e::InvalidParameterHandle,
        rti1516e::InteractionParameterNotDefined,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Get_Parameter_Name req, rep ;

    req.setParameter(rti1516e::ParameterHandleFriend::toCertiHandle(theHandle));
    req.setInteractionClass(rti1516e::InteractionClassHandleFriend::toCertiHandle(whichClass));

    try {
        privateRefs->executeService(&req, &rep);
    } catch (rti1516e::InteractionClassNotDefined &e)
    {
        if ( !whichClass.isValid() )
        {
            throw rti1516e::InvalidInteractionClassHandle(e.what());
        } else
        {
            throw;
        }
    }

    //return hla_strdup(rep.getParameterName());
    std::string nameString = rep.getParameterName();
    std::wstring nameWString(nameString.begin(), nameString.end());

    return nameWString;
        }

// 10.20
rti1516e::OrderType RTI1516ambassador::getOrderType
(std::wstring const & orderName)
throw (rti1516e::InvalidOrderName,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Get_Ordering_Handle req, rep ;

    std::string nameAsString(orderName.begin(), orderName.end());
    req.setOrderingName(nameAsString);
    privateRefs->executeService(&req, &rep);

    return certi::toRTI1516OrderType(rep.getOrdering());
        }

// 10.21
std::wstring RTI1516ambassador::getOrderName
(rti1516e::OrderType orderType)
throw (rti1516e::InvalidOrderType,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Get_Ordering_Name req, rep ;

    req.setOrdering(certi::toCertiOrderType(orderType));
    privateRefs->executeService(&req, &rep);

    //return hla_strdup(rep.getOrderingName());
    std::string nameString = rep.getOrderingName();
    std::wstring nameWString(nameString.begin(), nameString.end());

    return nameWString;
        }

// 10.18
rti1516e::TransportationType RTI1516ambassador::getTransportationType
(std::wstring const & transportationName)
throw (rti1516e::InvalidTransportationName,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Get_Transportation_Handle req, rep ;
    std::string nameString(transportationName.begin(), transportationName.end());
    req.setTransportationName(nameString);
    privateRefs->executeService(&req, &rep);

    return certi::toRTI1516TransportationType(rep.getTransportation());
        }

// 10.19
std::wstring RTI1516ambassador::getTransportationName
(rti1516e::TransportationType transportationType)
throw (rti1516e::InvalidTransportationType,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Get_Transportation_Name req, rep ;

    req.setTransportation(certi::toCertiTransportationType(transportationType));
    privateRefs->executeService(&req, &rep);

    //return hla_strdup(rep.getTransportationName());
    std::string nameString = rep.getTransportationName();
    std::wstring nameWString(nameString.begin(), nameString.end());

    return nameWString;
        }

// 10.15
rti1516e::DimensionHandleSet RTI1516ambassador::getAvailableDimensionsForClassAttribute
(rti1516e::ObjectClassHandle theClass,
        rti1516e::AttributeHandle theHandle)
throw (rti1516e::InvalidObjectClassHandle,
        rti1516e::InvalidAttributeHandle,
        rti1516e::AttributeNotDefined,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Get_Attribute_Space_Handle req, rep ;

    req.setAttribute(rti1516e::AttributeHandleFriend::toCertiHandle(theHandle));
    req.setObjectClass(rti1516e::ObjectClassHandleFriend::toCertiHandle(theClass));
    privateRefs->executeService(&req, &rep);

    //JRE TODO: Use space handle to to get the DimensionHandleSet?@!
    //return rep.getSpace();
    rti1516e::DimensionHandleSet invalidSet;
    return invalidSet;
        }

// 10.17
rti1516e::DimensionHandleSet RTI1516ambassador::getAvailableDimensionsForInteractionClass
(rti1516e::InteractionClassHandle theClass)
throw (rti1516e::InvalidInteractionClassHandle,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Get_Interaction_Space_Handle req, rep ;

    req.setInteractionClass(rti1516e::InteractionClassHandleFriend::toCertiHandle(theClass));
    this->privateRefs->executeService(&req, &rep);

    //JRE TODO: Use space handle to to get the DimensionHandleSet?@!
    //return rep.getSpace();
    rti1516e::DimensionHandleSet invalidSet;
    return invalidSet;
        }


// 10.12
rti1516e::DimensionHandle RTI1516ambassador::getDimensionHandle
(std::wstring const & theName)
throw (rti1516e::NameNotFound,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Get_Dimension_Handle req, rep ;

    std::string nameString(theName.begin(), theName.end());
    req.setDimensionName(nameString);
    //req.setSpace(space);    //SPACE NIET NODIG IN 1516 STANDAARD???
    privateRefs->executeService(&req, &rep);
    return rti1516e::DimensionHandleFriend::createRTI1516Handle(rep.getDimension());
        }

// 10.13
std::wstring RTI1516ambassador::getDimensionName
(rti1516e::DimensionHandle theHandle)
throw (rti1516e::InvalidDimensionHandle,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Get_Dimension_Name req, rep ;

    req.setDimension(rti1516e::DimensionHandleFriend::toCertiHandle(theHandle));
    //req.setSpace(space);
    privateRefs->executeService(&req, &rep);
    //return hla_strdup(rep.getDimensionName());
    std::string nameString = rep.getDimensionName();
    std::wstring nameWString(nameString.begin(), nameString.end());

    return nameWString;
        }

// 10.14
unsigned long RTI1516ambassador::getDimensionUpperBound
(rti1516e::DimensionHandle theHandle)
throw (rti1516e::InvalidDimensionHandle,
        rti1516e::FederateNotExecutionMember,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"getDimensionUpperBound Not implemented");
        }

// 10.30

rti1516e::DimensionHandleSet RTI1516ambassador::getDimensionHandleSet
(rti1516e::RegionHandle theRegionHandle)
throw (rti1516e::InvalidRegion,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"getDimensionHandleSet Not implemented");
        }

// 10.31

rti1516e::RangeBounds RTI1516ambassador::getRangeBounds
(rti1516e::RegionHandle theRegionHandle,
        rti1516e::DimensionHandle theDimensionHandle)
throw (rti1516e::InvalidRegion,
        rti1516e::RegionDoesNotContainSpecifiedDimension,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"getRangeBounds Not implemented");
        }

// 10.32
void RTI1516ambassador::setRangeBounds
(rti1516e::RegionHandle theRegionHandle,
        rti1516e::DimensionHandle theDimensionHandle,
        rti1516e::RangeBounds const & theRangeBounds)
throw (rti1516e::InvalidRegion,
        rti1516e::RegionNotCreatedByThisFederate,
        rti1516e::RegionDoesNotContainSpecifiedDimension,
        rti1516e::InvalidRangeBound,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"setRangeBounds Not implemented");
        }

// 10.33
unsigned long RTI1516ambassador::normalizeFederateHandle
(rti1516e::FederateHandle theFederateHandle)
throw (rti1516e::FederateNotExecutionMember,
        rti1516e::InvalidFederateHandle,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"normalizeFederateHandle Not implemented");
        }

// 10.34
unsigned long RTI1516ambassador::normalizeServiceGroup
(rti1516e::ServiceGroup theServiceGroup)
throw (rti1516e::FederateNotExecutionMember,
        rti1516e::InvalidServiceGroup,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"normalizeServiceGroup Not implemented");
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
void RTI1516ambassador::enableObjectClassRelevanceAdvisorySwitch ()
throw (rti1516e::ObjectClassRelevanceAdvisorySwitchIsOn,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Enable_Class_Relevance_Advisory_Switch req, rep ;
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
void RTI1516ambassador::disableObjectClassRelevanceAdvisorySwitch ()
throw (rti1516e::ObjectClassRelevanceAdvisorySwitchIsOff,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Disable_Class_Relevance_Advisory_Switch req, rep ;
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
void RTI1516ambassador::enableAttributeRelevanceAdvisorySwitch ()
throw (rti1516e::AttributeRelevanceAdvisorySwitchIsOn,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Enable_Attribute_Relevance_Advisory_Switch req, rep ;
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
void RTI1516ambassador::disableAttributeRelevanceAdvisorySwitch ()
throw (rti1516e::AttributeRelevanceAdvisorySwitchIsOff,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Disable_Attribute_Relevance_Advisory_Switch req, rep ;
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
void RTI1516ambassador::enableAttributeScopeAdvisorySwitch ()
throw (rti1516e::AttributeScopeAdvisorySwitchIsOn,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Enable_Attribute_Scope_Advisory_Switch req, rep ;
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
void RTI1516ambassador::disableAttributeScopeAdvisorySwitch ()
throw (rti1516e::AttributeScopeAdvisorySwitchIsOff,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Disable_Attribute_Scope_Advisory_Switch req, rep ;
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
void RTI1516ambassador::enableInteractionRelevanceAdvisorySwitch ()
throw (rti1516e::InteractionRelevanceAdvisorySwitchIsOn,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Enable_Interaction_Relevance_Advisory_Switch req, rep ;
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
void RTI1516ambassador::disableInteractionRelevanceAdvisorySwitch ()
throw (rti1516e::InteractionRelevanceAdvisorySwitchIsOff,
        rti1516e::FederateNotExecutionMember,
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::NotConnected,
        rti1516e::RTIinternalError)
        {
    M_Disable_Interaction_Relevance_Advisory_Switch req, rep ;
    privateRefs->executeService(&req, &rep);
        }


// 10.37
bool RTI1516ambassador::evokeCallback(double approximateMinimumTimeInSeconds)
throw (rti1516e::CallNotAllowedFromWithinCallback,
        rti1516e::RTIinternalError)
        {
    return __tick_kernel(false, approximateMinimumTimeInSeconds, approximateMinimumTimeInSeconds);
        }

// 10.38
bool RTI1516ambassador::evokeMultipleCallbacks(double approximateMinimumTimeInSeconds,
        double approximateMaximumTimeInSeconds)
throw (rti1516e::CallNotAllowedFromWithinCallback,
        rti1516e::RTIinternalError)
        {
    return __tick_kernel(true, approximateMinimumTimeInSeconds, approximateMaximumTimeInSeconds);
        }

// 10.39
void RTI1516ambassador::enableCallbacks ()
throw (
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"enableCallbacks Not implemented");
        }

// 10.40
void RTI1516ambassador::disableCallbacks ()
throw (
        rti1516e::SaveInProgress,
        rti1516e::RestoreInProgress,
        rti1516e::RTIinternalError)
        {
    /* TODO */
    throw rti1516e::RTIinternalError(L"disableCallbacks Not implemented");
        }

std::auto_ptr<rti1516e::LogicalTimeFactory> RTI1516ambassador::getTimeFactory () const
                     throw (
                             rti1516e::FederateNotExecutionMember,
                             rti1516e::NotConnected,
                             rti1516e::RTIinternalError) {
    /* FIXME this must be fixed ASAP */
    throw rti1516e::RTIinternalError(L"getTimeFactory Not Implemented");
}

rti1516e::FederateHandle RTI1516ambassador::decodeFederateHandle(
        rti1516e::VariableLengthData const & encodedValue) const
throw (
            rti1516e::CouldNotDecode,
            rti1516e::FederateNotExecutionMember,
            rti1516e::NotConnected,
            rti1516e::RTIinternalError)
{
    return rti1516e::FederateHandleFriend::createRTI1516Handle(encodedValue);
}

rti1516e::ObjectClassHandle RTI1516ambassador::decodeObjectClassHandle(
        rti1516e::VariableLengthData const & encodedValue) const
throw (
            rti1516e::CouldNotDecode,
            rti1516e::FederateNotExecutionMember,
            rti1516e::NotConnected,
            rti1516e::RTIinternalError)
{
    return rti1516e::ObjectClassHandleFriend::createRTI1516Handle(encodedValue);
}

rti1516e::InteractionClassHandle RTI1516ambassador::decodeInteractionClassHandle(
        rti1516e::VariableLengthData const & encodedValue) const
throw (
            rti1516e::CouldNotDecode,
            rti1516e::FederateNotExecutionMember,
            rti1516e::NotConnected,
            rti1516e::RTIinternalError)
{
    return rti1516e::InteractionClassHandleFriend::createRTI1516Handle(encodedValue);
}

rti1516e::ObjectInstanceHandle RTI1516ambassador::decodeObjectInstanceHandle(
        rti1516e::VariableLengthData const & encodedValue) const
throw (
            rti1516e::CouldNotDecode,
            rti1516e::FederateNotExecutionMember,
            rti1516e::NotConnected,
            rti1516e::RTIinternalError)
{
    return rti1516e::ObjectInstanceHandleFriend::createRTI1516Handle(encodedValue);
}

rti1516e::AttributeHandle RTI1516ambassador::decodeAttributeHandle(
        rti1516e::VariableLengthData const & encodedValue) const
throw (
            rti1516e::CouldNotDecode,
            rti1516e::FederateNotExecutionMember,
            rti1516e::NotConnected,
            rti1516e::RTIinternalError)
{
    return rti1516e::AttributeHandleFriend::createRTI1516Handle(encodedValue);
}

rti1516e::ParameterHandle RTI1516ambassador::decodeParameterHandle(
        rti1516e::VariableLengthData const & encodedValue) const
throw (
            rti1516e::CouldNotDecode,
            rti1516e::FederateNotExecutionMember,
            rti1516e::NotConnected,
            rti1516e::RTIinternalError)
{
    return rti1516e::ParameterHandleFriend::createRTI1516Handle(encodedValue);
}

rti1516e::DimensionHandle RTI1516ambassador::decodeDimensionHandle(
        rti1516e::VariableLengthData const & encodedValue) const
throw (
            rti1516e::CouldNotDecode,
            rti1516e::FederateNotExecutionMember,
            rti1516e::NotConnected,
            rti1516e::RTIinternalError)
{
    return rti1516e::DimensionHandleFriend::createRTI1516Handle(encodedValue);
}

rti1516e::MessageRetractionHandle RTI1516ambassador::decodeMessageRetractionHandle(
        rti1516e::VariableLengthData const & encodedValue) const
throw (
            rti1516e::CouldNotDecode,
            rti1516e::FederateNotExecutionMember,
            rti1516e::NotConnected,
            rti1516e::RTIinternalError)
{
    return rti1516e::MessageRetractionHandleFriend::createRTI1516Handle(encodedValue);
}

rti1516e::RegionHandle RTI1516ambassador::decodeRegionHandle(
        rti1516e::VariableLengthData const & encodedValue) const
throw (
            rti1516e::CouldNotDecode,
            rti1516e::FederateNotExecutionMember,
            rti1516e::NotConnected,
            rti1516e::RTIinternalError)
{
    return rti1516e::RegionHandleFriend::createRTI1516Handle(encodedValue);
}


} // end namespace certi
