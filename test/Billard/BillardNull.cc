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
// $Id: BillardNull.cc,v 3.1 2003/10/27 10:51:39 breholee Exp $
// ----------------------------------------------------------------------------

#include "BillardNull.hh"

// ----------------------------------------------------------------------------
void
BillardNull::discoverObjectInstance(ObjectHandle,
				    ObjectClassHandle,
				    const char *)
    throw (CouldNotDiscover,
           ObjectClassNotKnown,
           InvalidFederationTime,
           FederateInternalError)
{
}

// ----------------------------------------------------------------------------
void
BillardNull::announceSynchronizationPoint(const char *, const char *)
    throw (FederateInternalError)
{
}

// ----------------------------------------------------------------------------
void
BillardNull::synchronizationPointRegistrationSucceeded(const char *)
    throw (FederateInternalError)
{
}

// ----------------------------------------------------------------------------
void
BillardNull::synchronizationPointRegistrationFailed(const char *)
    throw (FederateInternalError)
{
}

// ----------------------------------------------------------------------------
void
BillardNull::federationSynchronized(const char *)
    throw (FederateInternalError)
{
}

// ----------------------------------------------------------------------------
void
BillardNull::receiveInteraction(InteractionClassHandle,
				const ParameterHandleValuePairSet &,
				const FedTime &,
				const char *,
				EventRetractionHandle)
    throw (InteractionClassNotKnown,
           InteractionParameterNotKnown,
           InvalidFederationTime,
           FederateInternalError)
{
}

// ----------------------------------------------------------------------------
void
BillardNull::reflectAttributeValues(ObjectHandle,
				    const AttributeHandleValuePairSet &,
				    const FedTime &,
				    const char *,
				    EventRetractionHandle)
    throw (ObjectNotKnown,
           AttributeNotKnown,
           InvalidFederationTime,
           FederateInternalError)
{
}

// ----------------------------------------------------------------------------
void
BillardNull::removeObjectInstance(ObjectHandle theObject,
				  const FedTime &,
				  const char *,
				  EventRetractionHandle)
    throw (ObjectNotKnown, InvalidFederationTime, FederateInternalError)
{
}

// ----------------------------------------------------------------------------
void
BillardNull::turnInteractionsOn(InteractionClassHandle)
    throw (InteractionClassNotPublished, FederateInternalError)
{
}

// ----------------------------------------------------------------------------
void
BillardNull::turnInteractionsOff(InteractionClassHandle)
    throw (InteractionClassNotPublished, FederateInternalError)
{
}

// ----------------------------------------------------------------------------
void
BillardNull::startRegistrationForObjectClass(ObjectClassHandle)
    throw (ObjectClassNotPublished, FederateInternalError)
{
}

// ----------------------------------------------------------------------------
void
BillardNull::stopRegistrationForObjectClass(ObjectClassHandle)
    throw (ObjectClassNotPublished, FederateInternalError)
{
}

// ----------------------------------------------------------------------------
void
BillardNull::timeAdvanceGrant(const FedTime &)
    throw (InvalidFederationTime,
	   TimeAdvanceWasNotInProgress,
           FederationTimeAlreadyPassed,
	   FederateInternalError)
{
}

// ----------------------------------------------------------------------------
void
BillardNull::timeRegulationEnabled(const FedTime &)
    throw (InvalidFederationTime,
	   EnableTimeRegulationWasNotPending,
           FederateInternalError)
{
}

// ----------------------------------------------------------------------------
void
BillardNull::requestAttributeOwnershipRelease(ObjectHandle,
					      const AttributeHandleSet &,
					      const char *)
    throw (ObjectNotKnown,
	   AttributeNotKnown,
	   AttributeNotOwned,
           FederateInternalError)
{
}

// ----------------------------------------------------------------------------
void
BillardNull::informAttributeOwnership(ObjectHandle theObject,
				      AttributeHandle theAttribute,
				      FederateHandle theOwner)
    throw (ObjectNotKnown, AttributeNotKnown, FederateInternalError)
{
}

// ----------------------------------------------------------------------------
void
BillardNull::attributeIsNotOwned(ObjectHandle theObject,
				 AttributeHandle theAttribute)
    throw (ObjectNotKnown, AttributeNotKnown, FederateInternalError)
{
}

// ----------------------------------------------------------------------------
void
BillardNull::attributeOwnershipUnavailable(ObjectHandle theObject,
					   const AttributeHandleSet &)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeAlreadyOwned,
           AttributeAcquisitionWasNotRequested,
           FederateInternalError)
{
}

// ----------------------------------------------------------------------------
void
BillardNull::attributeOwnershipAcquisitionNotification(
    ObjectHandle,
    const AttributeHandleSet&)
    throw (ObjectNotKnown,
           AttributeNotKnown,
           AttributeAcquisitionWasNotRequested,
           AttributeAlreadyOwned,
           AttributeNotPublished,
           FederateInternalError)
{
}

// ----------------------------------------------------------------------------
void
BillardNull::attributeOwnershipDivestitureNotification(
    ObjectHandle,
    const AttributeHandleSet& attrs)
    throw (ObjectNotKnown,
           AttributeNotKnown,
           AttributeNotOwned,
           AttributeDivestitureWasNotRequested,
           FederateInternalError)
{
}

// ----------------------------------------------------------------------------
void
BillardNull::requestAttributeOwnershipAssumption(ObjectHandle,
						 const AttributeHandleSet &,
						 const char *)
    throw (ObjectNotKnown,
           AttributeNotKnown,
           AttributeAlreadyOwned,
           AttributeNotPublished,
           FederateInternalError)
{
}

// ----------------------------------------------------------------------------
void
BillardNull::confirmAttributeOwnershipAcquisitionCancellation(
    ObjectHandle,
    const AttributeHandleSet&
    theAttributes)
    throw (ObjectNotKnown,
           AttributeNotKnown,
           AttributeAlreadyOwned,
           AttributeAcquisitionWasNotCanceled,
           FederateInternalError)
{
}

// $Id: BillardNull.cc,v 3.1 2003/10/27 10:51:39 breholee Exp $
