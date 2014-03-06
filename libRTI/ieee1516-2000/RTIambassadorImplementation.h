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
// This interface is used to access the services of the RTI. 

#ifndef RTIambassador_h
#define RTIambassador_h

#include <RTI/RTIambassador.h>
#include <RTI/RTIambassadorFactory.h>
#include "RTIambassadorImplementation.h"
#include "RTIambPrivateRefs.h"

namespace certi
{
class RTI_EXPORT RTI1516ambassador : rti1516::RTIambassador
{
    friend std::auto_ptr< rti1516::RTIambassador >
    rti1516::RTIambassadorFactory::createRTIambassador(std::vector< std::wstring > & args)
    throw (rti1516::BadInitializationParameter, rti1516::RTIinternalError);

private:
    RTI1516ambPrivateRefs* privateRefs ;

    // Helper functions
    template<typename T> void
    assignAHSAndExecuteService(const rti1516::AttributeHandleSet &AHS, T &req, T &rep);
    template<typename T> void
    assignPHVMAndExecuteService(const rti1516::ParameterHandleValueMap &PHVM, T &req, T &rep);
    template<typename T> void
    assignAHVMAndExecuteService(const rti1516::AttributeHandleValueMap &AHVM, T &req, T &rep);
    // Helper function for CallBacks
    bool __tick_kernel(bool, TickTime, TickTime)
    throw (rti1516::SpecifiedSaveLabelDoesNotExist, rti1516::RTIinternalError);

protected:
    RTI1516ambassador()
throw ();

public:
    virtual
    ~RTI1516ambassador();
    // throw ()

    // 4.2
    virtual void createFederationExecution
    (std::wstring const & federationExecutionName,
            std::wstring const & fullPathNameToTheFDDfile,
            std::wstring const & logicalTimeImplementationName = L"")
    throw (rti1516::FederationExecutionAlreadyExists,
           rti1516::CouldNotOpenFDD,
           rti1516::ErrorReadingFDD,
           rti1516::CouldNotCreateLogicalTimeFactory,
           rti1516::RTIinternalError);

    // 4.3
    virtual void destroyFederationExecution
    (std::wstring const & federationExecutionName)
    throw (rti1516::FederatesCurrentlyJoined,
           rti1516::FederationExecutionDoesNotExist,
           rti1516::RTIinternalError);

    // 4.4
    virtual rti1516::FederateHandle joinFederationExecution
    (std::wstring const & federateType,
           std::wstring const & federationExecutionName,
           rti1516::FederateAmbassador & federateAmbassador)
    throw (rti1516::FederateAlreadyExecutionMember,
           rti1516::FederationExecutionDoesNotExist,
           rti1516::SaveInProgress,
           rti1516::RestoreInProgress,
           rti1516::CouldNotCreateLogicalTimeFactory,
           rti1516::RTIinternalError);

    // 4.5
    virtual void resignFederationExecution
    (rti1516::ResignAction resignAction)
    throw (rti1516::OwnershipAcquisitionPending,
            rti1516::FederateOwnsAttributes,
            rti1516::FederateNotExecutionMember,
            rti1516::RTIinternalError);

    // 4.6
    virtual void registerFederationSynchronizationPoint
    (std::wstring const & label,
            rti1516::VariableLengthData const & theUserSuppliedTag)
    throw (rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    virtual void registerFederationSynchronizationPoint
    (std::wstring const & label,
            rti1516::VariableLengthData const & theUserSuppliedTag,
            rti1516::FederateHandleSet const & syncSet)
    throw (rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 4.9
    virtual void synchronizationPointAchieved
    (std::wstring const & label)
    throw (rti1516::SynchronizationPointLabelNotAnnounced,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 4.11
    virtual void requestFederationSave
    (std::wstring const & label)
    throw (rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    virtual void requestFederationSave
    (std::wstring const & label,
            rti1516::LogicalTime const & theTime)
    throw (rti1516::LogicalTimeAlreadyPassed,
            rti1516::InvalidLogicalTime,
            rti1516::FederateUnableToUseTime,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 4.13
    virtual void federateSaveBegun ()
    throw (rti1516::SaveNotInitiated,
            rti1516::FederateNotExecutionMember,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 4.14
    virtual void federateSaveComplete ()
    throw (rti1516::FederateHasNotBegunSave,
            rti1516::FederateNotExecutionMember,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    virtual void federateSaveNotComplete()
    throw (rti1516::FederateHasNotBegunSave,
            rti1516::FederateNotExecutionMember,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 4.16
    virtual void queryFederationSaveStatus ()
    throw (rti1516::FederateNotExecutionMember,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 4.18
    virtual void requestFederationRestore
    (std::wstring const & label)
    throw (rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 4.22
    virtual void federateRestoreComplete ()
    throw (rti1516::RestoreNotRequested,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RTIinternalError);

    virtual void federateRestoreNotComplete ()
    throw (rti1516::RestoreNotRequested,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RTIinternalError);

    // 4.24
    virtual void queryFederationRestoreStatus ()
    throw (rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RTIinternalError);

    /////////////////////////////////////
    // Declaration Management Services //
    /////////////////////////////////////

    // 5.2
    virtual void publishObjectClassAttributes
    (rti1516::ObjectClassHandle theClass,
            rti1516::AttributeHandleSet const & attributeList)
    throw (rti1516::ObjectClassNotDefined,
            rti1516::AttributeNotDefined,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 5.3
    virtual void unpublishObjectClass
    (rti1516::ObjectClassHandle theClass)
    throw (rti1516::ObjectClassNotDefined,
            rti1516::OwnershipAcquisitionPending,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    virtual void unpublishObjectClassAttributes
    (rti1516::ObjectClassHandle theClass,
            rti1516::AttributeHandleSet const & attributeList)
    throw (rti1516::ObjectClassNotDefined,
            rti1516::AttributeNotDefined,
            rti1516::OwnershipAcquisitionPending,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 5.4
    virtual void publishInteractionClass
    (rti1516::InteractionClassHandle theInteraction)
    throw (rti1516::InteractionClassNotDefined,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 5.5
    virtual void unpublishInteractionClass
    (rti1516::InteractionClassHandle theInteraction)
    throw (rti1516::InteractionClassNotDefined,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 5.6
    virtual void subscribeObjectClassAttributes
    (rti1516::ObjectClassHandle theClass,
            rti1516::AttributeHandleSet const & attributeList,
            bool active = true)
    throw (rti1516::ObjectClassNotDefined,
            rti1516::AttributeNotDefined,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 5.7
    virtual void unsubscribeObjectClass
    (rti1516::ObjectClassHandle theClass)
    throw (rti1516::ObjectClassNotDefined,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    virtual void unsubscribeObjectClassAttributes
    (rti1516::ObjectClassHandle theClass,
            rti1516::AttributeHandleSet const & attributeList)
    throw (rti1516::ObjectClassNotDefined,
            rti1516::AttributeNotDefined,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 5.8
    virtual void subscribeInteractionClass
    (rti1516::InteractionClassHandle theClass,
            bool active = true)
    throw (rti1516::InteractionClassNotDefined,
            rti1516::FederateServiceInvocationsAreBeingReportedViaMOM,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 5.9
    virtual void unsubscribeInteractionClass
    (rti1516::InteractionClassHandle theClass)
    throw (rti1516::InteractionClassNotDefined,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    ////////////////////////////////
    // Object Management Services //
    ////////////////////////////////

    // 6.2
    virtual void reserveObjectInstanceName
    (std::wstring const & theObjectInstanceName)
    throw (rti1516::IllegalName,
            rti1516::FederateNotExecutionMember,
           rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 6.4
    virtual rti1516::ObjectInstanceHandle registerObjectInstance
    (rti1516::ObjectClassHandle theClass)
    throw (rti1516::ObjectClassNotDefined,
            rti1516::ObjectClassNotPublished,
            rti1516::FederateNotExecutionMember,
           rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    virtual rti1516::ObjectInstanceHandle registerObjectInstance
    (rti1516::ObjectClassHandle theClass,
            std::wstring const & theObjectInstanceName)
    throw (rti1516::ObjectClassNotDefined,
            rti1516::ObjectClassNotPublished,
            rti1516::ObjectInstanceNameNotReserved,
            rti1516::ObjectInstanceNameInUse,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 6.6
    virtual void updateAttributeValues
    (rti1516::ObjectInstanceHandle theObject,
            rti1516::AttributeHandleValueMap const & theAttributeValues,
            rti1516::VariableLengthData const & theUserSuppliedTag)
    throw (rti1516::ObjectInstanceNotKnown,
            rti1516::AttributeNotDefined,
            rti1516::AttributeNotOwned,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    virtual rti1516::MessageRetractionHandle updateAttributeValues
    (rti1516::ObjectInstanceHandle theObject,
            rti1516::AttributeHandleValueMap const & theAttributeValues,
            rti1516::VariableLengthData const & theUserSuppliedTag,
            rti1516::LogicalTime const & theTime)
    throw (rti1516::ObjectInstanceNotKnown,
            rti1516::AttributeNotDefined,
            rti1516::AttributeNotOwned,
            rti1516::InvalidLogicalTime,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 6.8
    virtual void sendInteraction
    (rti1516::InteractionClassHandle theInteraction,
            rti1516::ParameterHandleValueMap const & theParameterValues,
            rti1516::VariableLengthData const & theUserSuppliedTag)
    throw (rti1516::InteractionClassNotPublished,
            rti1516::InteractionClassNotDefined,
            rti1516::InteractionParameterNotDefined,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    virtual rti1516::MessageRetractionHandle sendInteraction
    (rti1516::InteractionClassHandle theInteraction,
            rti1516::ParameterHandleValueMap const & theParameterValues,
            rti1516::VariableLengthData const & theUserSuppliedTag,
            rti1516::LogicalTime const & theTime)
    throw (rti1516::InteractionClassNotPublished,
            rti1516::InteractionClassNotDefined,
            rti1516::InteractionParameterNotDefined,
            rti1516::InvalidLogicalTime,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 6.10
    virtual void deleteObjectInstance
    (rti1516::ObjectInstanceHandle theObject,
            rti1516::VariableLengthData const & theUserSuppliedTag)
    throw (rti1516::DeletePrivilegeNotHeld,
            rti1516::ObjectInstanceNotKnown,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    virtual rti1516::MessageRetractionHandle deleteObjectInstance
    (rti1516::ObjectInstanceHandle theObject,
            rti1516::VariableLengthData const & theUserSuppliedTag,
            rti1516::LogicalTime  const & theTime)
    throw (rti1516::DeletePrivilegeNotHeld,
            rti1516::ObjectInstanceNotKnown,
            rti1516::InvalidLogicalTime,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 6.12
    virtual void localDeleteObjectInstance
    (rti1516::ObjectInstanceHandle theObject)
    throw (rti1516::ObjectInstanceNotKnown,
            rti1516::FederateOwnsAttributes,
            rti1516::OwnershipAcquisitionPending,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 6.13
    virtual void changeAttributeTransportationType
    (rti1516::ObjectInstanceHandle theObject,
            rti1516::AttributeHandleSet const & theAttributes,
            rti1516::TransportationType theType)
    throw (rti1516::ObjectInstanceNotKnown,
            rti1516::AttributeNotDefined,
            rti1516::AttributeNotOwned,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 6.14
    virtual void changeInteractionTransportationType
    (rti1516::InteractionClassHandle theClass,
            rti1516::TransportationType theType)
    throw (rti1516::InteractionClassNotDefined,
            rti1516::InteractionClassNotPublished,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 6.17
    virtual void requestAttributeValueUpdate
    (rti1516::ObjectInstanceHandle theObject,
            rti1516::AttributeHandleSet const & theAttributes,
            rti1516::VariableLengthData const & theUserSuppliedTag)
    throw (rti1516::ObjectInstanceNotKnown,
            rti1516::AttributeNotDefined,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    virtual void requestAttributeValueUpdate
    (rti1516::ObjectClassHandle theClass,
            rti1516::AttributeHandleSet const & theAttributes,
            rti1516::VariableLengthData const & theUserSuppliedTag)
    throw (rti1516::ObjectClassNotDefined,
            rti1516::AttributeNotDefined,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    ///////////////////////////////////
    // Ownership Management Services //
    ///////////////////////////////////
    // 7.2
    virtual void unconditionalAttributeOwnershipDivestiture
    (rti1516::ObjectInstanceHandle theObject,
            rti1516::AttributeHandleSet const & theAttributes)
    throw (rti1516::ObjectInstanceNotKnown,
            rti1516::AttributeNotDefined,
            rti1516::AttributeNotOwned,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 7.3
    virtual void negotiatedAttributeOwnershipDivestiture
    (rti1516::ObjectInstanceHandle theObject,
            rti1516::AttributeHandleSet const & theAttributes,
            rti1516::VariableLengthData const & theUserSuppliedTag)
    throw (rti1516::ObjectInstanceNotKnown,
            rti1516::AttributeNotDefined,
            rti1516::AttributeNotOwned,
            rti1516::AttributeAlreadyBeingDivested,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 7.6
    virtual void confirmDivestiture
    (rti1516::ObjectInstanceHandle theObject,
            rti1516::AttributeHandleSet const & confirmedAttributes,
            rti1516::VariableLengthData const & theUserSuppliedTag)
    throw (rti1516::ObjectInstanceNotKnown,
            rti1516::AttributeNotDefined,
            rti1516::AttributeNotOwned,
            rti1516::AttributeDivestitureWasNotRequested,
            rti1516::NoAcquisitionPending,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 7.8
    virtual void attributeOwnershipAcquisition
    (rti1516::ObjectInstanceHandle theObject,
            rti1516::AttributeHandleSet const & desiredAttributes,
            rti1516::VariableLengthData const & theUserSuppliedTag)
    throw (rti1516::ObjectInstanceNotKnown,
            rti1516::ObjectClassNotPublished,
            rti1516::AttributeNotDefined,
            rti1516::AttributeNotPublished,
            rti1516::FederateOwnsAttributes,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 7.9
    virtual void attributeOwnershipAcquisitionIfAvailable
    (rti1516::ObjectInstanceHandle theObject,
            rti1516::AttributeHandleSet const & desiredAttributes)
    throw (rti1516::ObjectInstanceNotKnown,
            rti1516::ObjectClassNotPublished,
            rti1516::AttributeNotDefined,
            rti1516::AttributeNotPublished,
            rti1516::FederateOwnsAttributes,
            rti1516::AttributeAlreadyBeingAcquired,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 7.12
    virtual void attributeOwnershipDivestitureIfWanted
    (rti1516::ObjectInstanceHandle theObject,
            rti1516::AttributeHandleSet const & theAttributes,
            rti1516::AttributeHandleSet & theDivestedAttributes) // filled by RTI
    throw (rti1516::ObjectInstanceNotKnown,
            rti1516::AttributeNotDefined,
            rti1516::AttributeNotOwned,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 7.13
    virtual void cancelNegotiatedAttributeOwnershipDivestiture
    (rti1516::ObjectInstanceHandle theObject,
            rti1516::AttributeHandleSet const & theAttributes)
    throw (rti1516::ObjectInstanceNotKnown,
            rti1516::AttributeNotDefined,
            rti1516::AttributeNotOwned,
            rti1516::AttributeDivestitureWasNotRequested,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 7.14
    virtual void cancelAttributeOwnershipAcquisition
    (rti1516::ObjectInstanceHandle theObject,
            rti1516::AttributeHandleSet const & theAttributes)
    throw (rti1516::ObjectInstanceNotKnown,
            rti1516::AttributeNotDefined,
            rti1516::AttributeAlreadyOwned,
            rti1516::AttributeAcquisitionWasNotRequested,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 7.16
    virtual void queryAttributeOwnership
    (rti1516::ObjectInstanceHandle theObject,
            rti1516::AttributeHandle theAttribute)
    throw (rti1516::ObjectInstanceNotKnown,
            rti1516::AttributeNotDefined,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 7.18
    virtual bool isAttributeOwnedByFederate
    (rti1516::ObjectInstanceHandle theObject,
            rti1516::AttributeHandle theAttribute)
    throw (rti1516::ObjectInstanceNotKnown,
            rti1516::AttributeNotDefined,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    //////////////////////////////
    // Time Management Services //
    //////////////////////////////

    // 8.2
    virtual void enableTimeRegulation
    (rti1516::LogicalTimeInterval const & theLookahead)
    throw (rti1516::TimeRegulationAlreadyEnabled,
            rti1516::InvalidLookahead,
            rti1516::InTimeAdvancingState,
            rti1516::RequestForTimeRegulationPending,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 8.4
    virtual void disableTimeRegulation ()
    throw (rti1516::TimeRegulationIsNotEnabled,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 8.5
    virtual void enableTimeConstrained ()
    throw (rti1516::TimeConstrainedAlreadyEnabled,
            rti1516::InTimeAdvancingState,
            rti1516::RequestForTimeConstrainedPending,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 8.7
    virtual void disableTimeConstrained ()
    throw (rti1516::TimeConstrainedIsNotEnabled,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 8.8
    virtual void timeAdvanceRequest
    (rti1516::LogicalTime const & theTime)
    throw (rti1516::InvalidLogicalTime,
            rti1516::LogicalTimeAlreadyPassed,
            rti1516::InTimeAdvancingState,
            rti1516::RequestForTimeRegulationPending,
            rti1516::RequestForTimeConstrainedPending,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 8.9
    virtual void timeAdvanceRequestAvailable
    (rti1516::LogicalTime const & theTime)
    throw (rti1516::InvalidLogicalTime,
            rti1516::LogicalTimeAlreadyPassed,
            rti1516::InTimeAdvancingState,
            rti1516::RequestForTimeRegulationPending,
            rti1516::RequestForTimeConstrainedPending,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 8.10
    virtual void nextMessageRequest
    (rti1516::LogicalTime const & theTime)
    throw (rti1516::InvalidLogicalTime,
            rti1516::LogicalTimeAlreadyPassed,
            rti1516::InTimeAdvancingState,
            rti1516::RequestForTimeRegulationPending,
            rti1516::RequestForTimeConstrainedPending,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 8.11
    virtual void nextMessageRequestAvailable
    (rti1516::LogicalTime const & theTime)
    throw (rti1516::InvalidLogicalTime,
            rti1516::LogicalTimeAlreadyPassed,
            rti1516::InTimeAdvancingState,
            rti1516::RequestForTimeRegulationPending,
            rti1516::RequestForTimeConstrainedPending,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 8.12
    virtual void flushQueueRequest
    (rti1516::LogicalTime const & theTime)
    throw (rti1516::InvalidLogicalTime,
            rti1516::LogicalTimeAlreadyPassed,
            rti1516::InTimeAdvancingState,
            rti1516::RequestForTimeRegulationPending,
            rti1516::RequestForTimeConstrainedPending,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 8.14
    virtual void enableAsynchronousDelivery ()
    throw (rti1516::AsynchronousDeliveryAlreadyEnabled,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 8.15
    virtual void disableAsynchronousDelivery ()
    throw (rti1516::AsynchronousDeliveryAlreadyDisabled,
            rti1516::FederateNotExecutionMember,
           rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 8.16
    virtual bool queryGALT (rti1516::LogicalTime & theTime)
    throw (rti1516::FederateNotExecutionMember,
           rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 8.17
    virtual void queryLogicalTime (rti1516::LogicalTime & theTime)
    throw (rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 8.18
    virtual bool queryLITS (rti1516::LogicalTime & theTime)
    throw (rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 8.19
    virtual void modifyLookahead
    (rti1516::LogicalTimeInterval const & theLookahead)
    throw (rti1516::TimeRegulationIsNotEnabled,
            rti1516::InvalidLookahead,
            rti1516::InTimeAdvancingState,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 8.20
    virtual void queryLookahead (rti1516::LogicalTimeInterval & interval)
    throw (rti1516::TimeRegulationIsNotEnabled,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 8.21
    virtual void retract
    (rti1516::MessageRetractionHandle theHandle)
    throw (rti1516::InvalidRetractionHandle,
            rti1516::TimeRegulationIsNotEnabled,
            rti1516::MessageCanNoLongerBeRetracted,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 8.23
    virtual void changeAttributeOrderType
    (rti1516::ObjectInstanceHandle theObject,
            rti1516::AttributeHandleSet const & theAttributes,
            rti1516::OrderType theType)
    throw (rti1516::ObjectInstanceNotKnown,
            rti1516::AttributeNotDefined,
            rti1516::AttributeNotOwned,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 8.24
    virtual void changeInteractionOrderType
    (rti1516::InteractionClassHandle theClass,
            rti1516::OrderType theType)
    throw (rti1516::InteractionClassNotDefined,
            rti1516::InteractionClassNotPublished,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    //////////////////////////////////
    // Data Distribution Management //
    //////////////////////////////////

    // 9.2
    virtual rti1516::RegionHandle createRegion
    (rti1516::DimensionHandleSet const & theDimensions)
    throw (rti1516::InvalidDimensionHandle,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 9.3
    virtual void commitRegionModifications
    (rti1516::RegionHandleSet const & theRegionHandleSet)
    throw (rti1516::InvalidRegion,
            rti1516::RegionNotCreatedByThisFederate,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 9.4
    virtual void deleteRegion
    (rti1516::RegionHandle theRegion)
    throw (rti1516::InvalidRegion,
            rti1516::RegionNotCreatedByThisFederate,
            rti1516::RegionInUseForUpdateOrSubscription,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 9.5
    virtual rti1516::ObjectInstanceHandle registerObjectInstanceWithRegions
    (rti1516::ObjectClassHandle theClass,
            rti1516::AttributeHandleSetRegionHandleSetPairVector const &
            theAttributeHandleSetRegionHandleSetPairVector)
    throw (rti1516::ObjectClassNotDefined,
            rti1516::ObjectClassNotPublished,
            rti1516::AttributeNotDefined,
            rti1516::AttributeNotPublished,
            rti1516::InvalidRegion,
            rti1516::RegionNotCreatedByThisFederate,
            rti1516::InvalidRegionContext,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    virtual rti1516::ObjectInstanceHandle registerObjectInstanceWithRegions
    (rti1516::ObjectClassHandle theClass,
            rti1516::AttributeHandleSetRegionHandleSetPairVector const &
            theAttributeHandleSetRegionHandleSetPairVector,
            std::wstring const & theObjectInstanceName)
    throw (rti1516::ObjectClassNotDefined,
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
            rti1516::RTIinternalError);

    // 9.6
    virtual void associateRegionsForUpdates
    (rti1516::ObjectInstanceHandle theObject,
            rti1516::AttributeHandleSetRegionHandleSetPairVector const &
            theAttributeHandleSetRegionHandleSetPairVector)
    throw (rti1516::ObjectInstanceNotKnown,
            rti1516::AttributeNotDefined,
            rti1516::InvalidRegion,
            rti1516::RegionNotCreatedByThisFederate,
            rti1516::InvalidRegionContext,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 9.7
    virtual void unassociateRegionsForUpdates
    (rti1516::ObjectInstanceHandle theObject,
            rti1516::AttributeHandleSetRegionHandleSetPairVector const &
            theAttributeHandleSetRegionHandleSetPairVector)
    throw (rti1516::ObjectInstanceNotKnown,
            rti1516::AttributeNotDefined,
            rti1516:: InvalidRegion,
            rti1516::RegionNotCreatedByThisFederate,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 9.8
    virtual void subscribeObjectClassAttributesWithRegions
    (rti1516::ObjectClassHandle theClass,
            rti1516::AttributeHandleSetRegionHandleSetPairVector const &
            theAttributeHandleSetRegionHandleSetPairVector,
            bool active = true)
    throw (rti1516::ObjectClassNotDefined,
            rti1516::AttributeNotDefined,
            rti1516::InvalidRegion,
            rti1516::RegionNotCreatedByThisFederate,
            rti1516::InvalidRegionContext,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 9.9
    virtual void unsubscribeObjectClassAttributesWithRegions
    (rti1516::ObjectClassHandle theClass,
            rti1516::AttributeHandleSetRegionHandleSetPairVector const &
            theAttributeHandleSetRegionHandleSetPairVector)
    throw (rti1516::ObjectClassNotDefined,
            rti1516::AttributeNotDefined,
            rti1516::InvalidRegion,
            rti1516::RegionNotCreatedByThisFederate,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 9.10
    virtual void subscribeInteractionClassWithRegions
    (rti1516::InteractionClassHandle theClass,
            rti1516::RegionHandleSet const & theRegionHandleSet,
            bool active = true)
    throw (rti1516::InteractionClassNotDefined,
            rti1516::InvalidRegion,
            rti1516::RegionNotCreatedByThisFederate,
            rti1516::InvalidRegionContext,
            rti1516::FederateServiceInvocationsAreBeingReportedViaMOM,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 9.11
    virtual void unsubscribeInteractionClassWithRegions
    (rti1516::InteractionClassHandle theClass,
            rti1516::RegionHandleSet const & theRegionHandleSet)
    throw (rti1516::InteractionClassNotDefined,
            rti1516::InvalidRegion,
            rti1516::RegionNotCreatedByThisFederate,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 9.12
    virtual void sendInteractionWithRegions
    (rti1516::InteractionClassHandle theInteraction,
            rti1516::ParameterHandleValueMap const & theParameterValues,
            rti1516::RegionHandleSet const & theRegionHandleSet,
            rti1516::VariableLengthData const & theUserSuppliedTag)
    throw (rti1516::InteractionClassNotDefined,
            rti1516::InteractionClassNotPublished,
            rti1516::InteractionParameterNotDefined,
            rti1516::InvalidRegion,
            rti1516::RegionNotCreatedByThisFederate,
            rti1516::InvalidRegionContext,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    virtual rti1516::MessageRetractionHandle sendInteractionWithRegions
    (rti1516::InteractionClassHandle theInteraction,
            rti1516::ParameterHandleValueMap const & theParameterValues,
            rti1516::RegionHandleSet const & theRegionHandleSet,
            rti1516::VariableLengthData const & theUserSuppliedTag,
            rti1516::LogicalTime const & theTime)
    throw (rti1516::InteractionClassNotDefined,
            rti1516::InteractionClassNotPublished,
            rti1516::InteractionParameterNotDefined,
            rti1516::InvalidRegion,
            rti1516::RegionNotCreatedByThisFederate,
            rti1516::InvalidRegionContext,
            rti1516::InvalidLogicalTime,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 9.13
    virtual void requestAttributeValueUpdateWithRegions
    (rti1516::ObjectClassHandle theClass,
            rti1516::AttributeHandleSetRegionHandleSetPairVector const & theSet,
            rti1516::VariableLengthData const & theUserSuppliedTag)
    throw (rti1516::ObjectClassNotDefined,
            rti1516::AttributeNotDefined,
            rti1516::InvalidRegion,
            rti1516::RegionNotCreatedByThisFederate,
            rti1516::InvalidRegionContext,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    //////////////////////////
    // RTI Support Services //
    //////////////////////////

    // 10.2
    virtual rti1516::ObjectClassHandle getObjectClassHandle
    (std::wstring const & theName)
    throw (rti1516::NameNotFound,
            rti1516::FederateNotExecutionMember,
            rti1516::RTIinternalError);

    // 10.3
    virtual std::wstring getObjectClassName
    (rti1516::ObjectClassHandle theHandle)
    throw (rti1516::InvalidObjectClassHandle,
            rti1516::FederateNotExecutionMember,
            rti1516::RTIinternalError);

    // 10.4
    virtual rti1516::AttributeHandle getAttributeHandle
    (rti1516::ObjectClassHandle whichClass,
            std::wstring const & theAttributeName)
    throw (rti1516::InvalidObjectClassHandle,
            rti1516::NameNotFound,
            rti1516::FederateNotExecutionMember,
            rti1516::RTIinternalError);

    // 10.5
    virtual std::wstring getAttributeName
    (rti1516::ObjectClassHandle whichClass,
            rti1516::AttributeHandle theHandle)
    throw (rti1516::InvalidObjectClassHandle,
            rti1516::InvalidAttributeHandle,
            rti1516::AttributeNotDefined,
            rti1516::FederateNotExecutionMember,
            rti1516::RTIinternalError);

    // 10.6
    virtual rti1516::InteractionClassHandle getInteractionClassHandle
    (std::wstring const & theName)
    throw (rti1516::NameNotFound,
            rti1516::FederateNotExecutionMember,
            rti1516::RTIinternalError);

    // 10.7
    virtual std::wstring getInteractionClassName
    (rti1516::InteractionClassHandle theHandle)
    throw (rti1516::InvalidInteractionClassHandle,
            rti1516::FederateNotExecutionMember,
            rti1516::RTIinternalError);

    // 10.8
    virtual rti1516::ParameterHandle getParameterHandle
    (rti1516::InteractionClassHandle whichClass,
            std::wstring const & theName)
    throw (rti1516::InvalidInteractionClassHandle,
            rti1516::NameNotFound,
            rti1516::FederateNotExecutionMember,
            rti1516::RTIinternalError);

    // 10.9
    virtual std::wstring getParameterName
    (rti1516::InteractionClassHandle whichClass,
            rti1516::ParameterHandle theHandle)
    throw (rti1516::InvalidInteractionClassHandle,
            rti1516::InvalidParameterHandle,
            rti1516::InteractionParameterNotDefined,
            rti1516::FederateNotExecutionMember,
            rti1516::RTIinternalError);

    // 10.10
    virtual rti1516::ObjectInstanceHandle getObjectInstanceHandle
    (std::wstring const & theName)
    throw (rti1516::ObjectInstanceNotKnown,
            rti1516::FederateNotExecutionMember,
            rti1516::RTIinternalError);

    // 10.11
    virtual std::wstring getObjectInstanceName
    (rti1516::ObjectInstanceHandle theHandle)
    throw (rti1516::ObjectInstanceNotKnown,
            rti1516::FederateNotExecutionMember,
            rti1516::RTIinternalError);

    // 10.12
    virtual rti1516::DimensionHandle getDimensionHandle
    (std::wstring const & theName)
    throw (rti1516::NameNotFound,
            rti1516::FederateNotExecutionMember,
            rti1516::RTIinternalError);

    // 10.13
    virtual std::wstring getDimensionName
    (rti1516::DimensionHandle theHandle)
    throw (rti1516::InvalidDimensionHandle,
            rti1516::FederateNotExecutionMember,
            rti1516::RTIinternalError);

    // 10.14
    virtual unsigned long getDimensionUpperBound
    (rti1516::DimensionHandle theHandle)
    throw (rti1516::InvalidDimensionHandle,
            rti1516::FederateNotExecutionMember,
            rti1516::RTIinternalError);

    // 10.15
    virtual rti1516::DimensionHandleSet getAvailableDimensionsForClassAttribute
    (rti1516::ObjectClassHandle theClass,
            rti1516::AttributeHandle theHandle)
    throw (rti1516::InvalidObjectClassHandle,
            rti1516::InvalidAttributeHandle,
            rti1516::AttributeNotDefined,
            rti1516::FederateNotExecutionMember,
            rti1516::RTIinternalError);

    // 10.16
    virtual rti1516::ObjectClassHandle getKnownObjectClassHandle
    (rti1516::ObjectInstanceHandle theObject)
    throw (rti1516::ObjectInstanceNotKnown,
            rti1516::FederateNotExecutionMember,
            rti1516::RTIinternalError);

    // 10.17
    virtual rti1516::DimensionHandleSet getAvailableDimensionsForInteractionClass
    (rti1516::InteractionClassHandle theClass)
    throw (rti1516::InvalidInteractionClassHandle,
            rti1516::FederateNotExecutionMember,
            rti1516::RTIinternalError);

    // 10.18
    virtual rti1516::TransportationType getTransportationType
    (std::wstring const & transportationName)
    throw (rti1516::InvalidTransportationName,
            rti1516::FederateNotExecutionMember,
            rti1516::RTIinternalError);

    // 10.19
    virtual std::wstring getTransportationName
    (rti1516::TransportationType transportationType)
    throw (rti1516::InvalidTransportationType,
            rti1516::FederateNotExecutionMember,
            rti1516::RTIinternalError);

    // 10.20
    virtual rti1516::OrderType getOrderType
    (std::wstring const & orderName)
    throw (rti1516::InvalidOrderName,
            rti1516::FederateNotExecutionMember,
            rti1516::RTIinternalError);

    // 10.21
    virtual std::wstring getOrderName
    (rti1516::OrderType orderType)
    throw (rti1516::InvalidOrderType,
            rti1516::FederateNotExecutionMember,
            rti1516::RTIinternalError);

    // 10.22
    virtual void enableObjectClassRelevanceAdvisorySwitch ()
    throw (rti1516::ObjectClassRelevanceAdvisorySwitchIsOn,
            rti1516::FederateNotExecutionMember,
           rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 10.23
    virtual void disableObjectClassRelevanceAdvisorySwitch ()
    throw (rti1516::ObjectClassRelevanceAdvisorySwitchIsOff,
            rti1516::FederateNotExecutionMember,
           rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 10.24
    virtual void enableAttributeRelevanceAdvisorySwitch ()
    throw (rti1516::AttributeRelevanceAdvisorySwitchIsOn,
            rti1516::FederateNotExecutionMember,
           rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 10.25
    virtual void disableAttributeRelevanceAdvisorySwitch ()
    throw (rti1516::AttributeRelevanceAdvisorySwitchIsOff,
            rti1516::FederateNotExecutionMember,
           rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 10.26
    virtual void enableAttributeScopeAdvisorySwitch ()
    throw (rti1516::AttributeScopeAdvisorySwitchIsOn,
            rti1516::FederateNotExecutionMember,
           rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 10.27
    virtual void disableAttributeScopeAdvisorySwitch ()
    throw (rti1516::AttributeScopeAdvisorySwitchIsOff,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 10.28
    virtual void enableInteractionRelevanceAdvisorySwitch ()
    throw (rti1516::InteractionRelevanceAdvisorySwitchIsOn,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 10.29
    virtual void disableInteractionRelevanceAdvisorySwitch ()
    throw (rti1516::InteractionRelevanceAdvisorySwitchIsOff,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 10.30
    virtual
    rti1516::DimensionHandleSet getDimensionHandleSet
    (rti1516::RegionHandle theRegionHandle)
    throw (rti1516::InvalidRegion,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 10.31
    virtual
    rti1516::RangeBounds getRangeBounds
    (rti1516::RegionHandle theRegionHandle,
            rti1516::DimensionHandle theDimensionHandle)
    throw (rti1516::InvalidRegion,
            rti1516::RegionDoesNotContainSpecifiedDimension,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 10.32
    virtual void setRangeBounds
    (rti1516::RegionHandle theRegionHandle,
            rti1516::DimensionHandle theDimensionHandle,
            rti1516::RangeBounds const & theRangeBounds)
    throw (rti1516::InvalidRegion,
            rti1516::RegionNotCreatedByThisFederate,
            rti1516::RegionDoesNotContainSpecifiedDimension,
            rti1516::InvalidRangeBound,
            rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 10.33
    virtual unsigned long normalizeFederateHandle
    (rti1516::FederateHandle theFederateHandle)
    throw (rti1516::FederateNotExecutionMember,
            rti1516::InvalidFederateHandle,
            rti1516::RTIinternalError);

    // 10.34
    virtual unsigned long normalizeServiceGroup
    (rti1516::ServiceGroupIndicator theServiceGroup)
    throw (rti1516::FederateNotExecutionMember,
            rti1516::InvalidServiceGroup,
            rti1516::RTIinternalError);

    // 10.37
    virtual bool evokeCallback(double approximateMinimumTimeInSeconds)
    throw (rti1516::FederateNotExecutionMember,
            rti1516::RTIinternalError);

    // 10.38
    virtual bool evokeMultipleCallbacks(double approximateMinimumTimeInSeconds,
            double approximateMaximumTimeInSeconds)
    throw (rti1516::FederateNotExecutionMember,
            rti1516::RTIinternalError);

    // 10.39
    virtual void enableCallbacks ()
    throw (rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    // 10.40
    virtual void disableCallbacks ()
    throw (rti1516::FederateNotExecutionMember,
            rti1516::SaveInProgress,
            rti1516::RestoreInProgress,
            rti1516::RTIinternalError);

    virtual rti1516::FederateHandle decodeFederateHandle(
            rti1516::VariableLengthData const & encodedValue) const;

    virtual rti1516::ObjectClassHandle decodeObjectClassHandle(
            rti1516::VariableLengthData const & encodedValue) const;

    virtual rti1516::InteractionClassHandle decodeInteractionClassHandle(
            rti1516::VariableLengthData const & encodedValue) const;

    virtual rti1516::ObjectInstanceHandle decodeObjectInstanceHandle(
            rti1516::VariableLengthData const & encodedValue) const;

    virtual rti1516::AttributeHandle decodeAttributeHandle(
            rti1516::VariableLengthData const & encodedValue) const;

    virtual rti1516::ParameterHandle decodeParameterHandle(
            rti1516::VariableLengthData const & encodedValue) const;

    virtual rti1516::DimensionHandle decodeDimensionHandle(
            rti1516::VariableLengthData const & encodedValue) const;

    virtual rti1516::MessageRetractionHandle decodeMessageRetractionHandle(
            rti1516::VariableLengthData const & encodedValue) const;

    virtual rti1516::RegionHandle decodeRegionHandle(
            rti1516::VariableLengthData const & encodedValue) const;

};
}

#endif // RTI_RTI1516ambassador_h
