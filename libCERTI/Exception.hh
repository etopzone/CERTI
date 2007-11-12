// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: Exception.hh,v 3.2 2007/11/12 14:49:10 siron Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_EXCEPTION_HH
#define _CERTI_EXCEPTION_HH

#define MAX_EXCEPTION_REASON_LENGTH 100

namespace certi {

typedef enum {
    e_NO_EXCEPTION = 0,
    e_ArrayIndexOutOfBounds,
    e_AsynchronousDeliveryAlreadyEnabled,
    e_AsynchronousDeliveryAlreadyDisabled,
    e_AttributeAlreadyOwned,
    e_AttributeAlreadyBeingAcquired,
    e_AttributeAlreadyBeingDivested,
    e_AttributeAcquisitionWasNotRequested,
    e_AttributeDivestitureWasNotRequested,
    e_AttributeNotDefined,
    e_AttributeNotKnown,
    e_AttributeNotOwned,
    e_AttributeNotPublished,
    e_AttributeNotSubscribed,
    e_ConcurrentAccessAttempted,
    e_CouldNotDiscover,
    e_CouldNotOpenRID,
    e_CouldNotOpenFED,
    e_CouldNotRestore,
    e_DeletePrivilegeNotHeld,
    e_ErrorReadingRID,
    e_EventNotKnown,
    e_FederateAlreadyPaused,
    e_FederateAlreadyExecutionMember,
    e_FederateDoesNotExist,
    e_FederateInternalError,
    e_FederateNameAlreadyInUse,
    e_FederateNotExecutionMember,
    e_FederateNotPaused,
    e_FederateNotPublishing,
    e_FederateNotSubscribing,
    e_FederateOwnsAttributes,
    e_FederatesCurrentlyJoined,
    e_FederateWasNotAskedToReleaseAttribute,
    e_FederationAlreadyPaused,
    e_FederationExecutionAlreadyExists,
    e_FederationExecutionDoesNotExist,
    e_FederationNotPaused,
    e_FederationTimeAlreadyPassed,
    e_RegionNotKnown,
    e_IDsupplyExhausted,
    e_InteractionClassNotDefined,
    e_InteractionClassNotKnown,
    e_InteractionClassNotPublished,
    e_InteractionParameterNotDefined,
    e_InteractionParameterNotKnown,
    e_InvalidDivestitureCondition,
    e_InvalidExtents,
    e_InvalidFederationTime,
    e_InvalidFederationTimeDelta,
    e_InvalidObjectHandle,
    e_InvalidOrderType,
    e_InvalidResignAction,
    e_InvalidRetractionHandle,
    e_InvalidRoutingSpace,
    e_InvalidTransportType,
    e_MemoryExhausted,
    e_NameNotFound,
    e_NoPauseRequested,
    e_NoResumeRequested,
    e_ObjectClassNotDefined,
    e_ObjectClassNotKnown,
    e_ObjectClassNotPublished,
    e_ObjectClassNotSubscribed,
    e_ObjectNotKnown,
    e_ObjectAlreadyRegistered,
    e_RestoreInProgress,
    e_RestoreNotRequested,
    e_RTICannotRestore,
    e_RTIinternalError,
    e_SpaceNotDefined,
    e_SaveInProgress,
    e_SaveNotInitiated,
    e_SecurityError,
    e_SpecifiedSaveLabelDoesNotExist,
    e_TimeAdvanceAlreadyInProgress,
    e_TimeAdvanceWasNotInProgress,
    e_TooManyIDsRequested,
    e_UnableToPerformSave,
    e_UnimplementedService,
    e_UnknownLabel,
    e_ValueCountExceeded,
    e_ValueLengthExceeded
} TypeException ;

} // namespace certi

#endif // _CERTI_EXCEPTION_HH

// $Id: Exception.hh,v 3.2 2007/11/12 14:49:10 siron Exp $
