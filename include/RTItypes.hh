// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libRTI
//
// CERTI-libRTI is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libRTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: RTItypes.hh,v 3.9 2003/02/17 09:17:03 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_RTI_TYPES_HH
#define _CERTI_RTI_TYPES_HH

#include "baseTypes.hh"
#include "local_config.hh"

#define RTI_VERSION "CERTI-3"

#define MAX_FEDERATEHANDLE USHRT_MAX
#define MAX_OBJECTID ULONG_MAX

namespace certi {
RTI_EXCEPT(ArrayIndexOutOfBounds)
RTI_EXCEPT(AsynchronousDeliveryAlreadyDisabled)
RTI_EXCEPT(AsynchronousDeliveryAlreadyEnabled)
RTI_EXCEPT(AttributeAcquisitionWasNotCanceled)
RTI_EXCEPT(AttributeAcquisitionWasNotRequested)
RTI_EXCEPT(AttributeAlreadyBeingAcquired)
RTI_EXCEPT(AttributeAlreadyBeingDivested)
RTI_EXCEPT(AttributeAlreadyOwned)
RTI_EXCEPT(AttributeDivestitureWasNotRequested)
RTI_EXCEPT(AttributeNotDefined)
RTI_EXCEPT(AttributeNotKnown)
RTI_EXCEPT(AttributeNotOwned)
RTI_EXCEPT(AttributeNotPublished)
RTI_EXCEPT(AttributeNotSubscribed)//deprecated
RTI_EXCEPT(ConcurrentAccessAttempted)
RTI_EXCEPT(CouldNotDiscover)
RTI_EXCEPT(CouldNotOpenFED)
RTI_EXCEPT(CouldNotOpenRID)//deprecated
RTI_EXCEPT(CouldNotRestore)
RTI_EXCEPT(DeletePrivilegeNotHeld)
RTI_EXCEPT(DimensionNotDefined)
RTI_EXCEPT(EnableTimeConstrainedPending)
RTI_EXCEPT(EnableTimeConstrainedWasNotPending)
RTI_EXCEPT(EnableTimeRegulationPending)
RTI_EXCEPT(EnableTimeRegulationWasNotPending)
RTI_EXCEPT(ErrorReadingFED)
RTI_EXCEPT(ErrorReadingRID)//deprecated
RTI_EXCEPT(EventNotKnown)
RTI_EXCEPT(FederateAlreadyExecutionMember)
RTI_EXCEPT(FederateAlreadyPaused)//deprecated
RTI_EXCEPT(FederateDoesNotExist)//deprecated
RTI_EXCEPT(FederateInternalError)
RTI_EXCEPT(FederateLoggingServiceCalls)
RTI_EXCEPT(FederateNameAlreadyInUse)//deprecated
RTI_EXCEPT(FederateNotExecutionMember)
RTI_EXCEPT(FederateNotPaused)//deprecated
RTI_EXCEPT(FederateNotPublishing) // deprecated
RTI_EXCEPT(FederateNotSubscribing)//deprecated
RTI_EXCEPT(FederateOwnsAttributes)
RTI_EXCEPT(FederatesCurrentlyJoined)
RTI_EXCEPT(FederateWasNotAskedToReleaseAttribute)
RTI_EXCEPT(FederationAlreadyPaused)
RTI_EXCEPT(FederationExecutionAlreadyExists)
RTI_EXCEPT(FederationExecutionDoesNotExist)
RTI_EXCEPT(FederationNotPaused)//deprecated
RTI_EXCEPT(FederationTimeAlreadyPassed)
RTI_EXCEPT(HandleValuePairMaximumExceeded)
RTI_EXCEPT(IDsupplyExhausted)//deprecated
RTI_EXCEPT(InteractionClassNotDefined)
RTI_EXCEPT(InteractionClassNotKnown)
RTI_EXCEPT(InteractionClassNotPublished)
RTI_EXCEPT(InteractionClassNotSubscribed)
RTI_EXCEPT(InteractionParameterNotDefined)
RTI_EXCEPT(InteractionParameterNotKnown)
RTI_EXCEPT(InvalidDivestitureCondition)//deprecated
RTI_EXCEPT(InvalidExtents)
RTI_EXCEPT(InvalidFederationTime)
RTI_EXCEPT(InvalidFederationTimeDelta)//deprecated
RTI_EXCEPT(InvalidHandleValuePairSetContext)
RTI_EXCEPT(InvalidLookahead)
RTI_EXCEPT(InvalidObjectHandle)
RTI_EXCEPT(InvalidOrderingHandle)
RTI_EXCEPT(InvalidOrderType)//deprecated
RTI_EXCEPT(InvalidRegionContext)
RTI_EXCEPT(InvalidResignAction)
RTI_EXCEPT(InvalidRetractionHandle)
RTI_EXCEPT(InvalidRoutingSpace)//deprecated
RTI_EXCEPT(InvalidTransportationHandle)
RTI_EXCEPT(InvalidTransportType)//deprecated
RTI_EXCEPT(MemoryExhausted)
RTI_EXCEPT(NameNotFound)
RTI_EXCEPT(NoPauseRequested)//deprecated
RTI_EXCEPT(NoResumeRequested)//deprecated
RTI_EXCEPT(ObjectAlreadyRegistered)
RTI_EXCEPT(ObjectClassNotDefined)
RTI_EXCEPT(ObjectClassNotKnown)
RTI_EXCEPT(ObjectClassNotPublished)
RTI_EXCEPT(ObjectClassNotSubscribed)
RTI_EXCEPT(ObjectNotKnown)
RTI_EXCEPT(OwnershipAcquisitionPending)
RTI_EXCEPT(RegionInUse)
RTI_EXCEPT(RegionNotKnown)
RTI_EXCEPT(RestoreInProgress)
RTI_EXCEPT(RestoreNotRequested)
RTI_EXCEPT(RTICannotRestore)//deprecated
RTI_EXCEPT(RTIinternalError)
RTI_EXCEPT(SaveInProgress)
RTI_EXCEPT(SaveNotInitiated)
RTI_EXCEPT(SecurityError)//deprecated
RTI_EXCEPT(SpaceNotDefined)
RTI_EXCEPT(SpecifiedSaveLabelDoesNotExist)
RTI_EXCEPT(SynchronizationPointLabelWasNotAnnounced)
RTI_EXCEPT(TimeAdvanceAlreadyInProgress)
RTI_EXCEPT(TimeAdvanceWasNotInProgress)
RTI_EXCEPT(TimeConstrainedAlreadyEnabled)
RTI_EXCEPT(TimeConstrainedWasNotEnabled)
RTI_EXCEPT(TimeRegulationAlreadyEnabled)
RTI_EXCEPT(TimeRegulationWasNotEnabled)
RTI_EXCEPT(TooManyIDsRequested)//deprecated
RTI_EXCEPT(UnableToPerformSave)
RTI_EXCEPT(UnimplementedService)//CERTI
RTI_EXCEPT(UnknownLabel)
RTI_EXCEPT(ValueCountExceeded)
RTI_EXCEPT(ValueLengthExceeded)

class CAttributeHandleValuePairSet ;
class FederateAmbassador ;
class Region ;

enum OrderType {
    RECEIVE = 1,
    TIMESTAMP
};
 
enum OwnershipDivestitureCondition {
    NEGOTIATED = 1,
    UNCONDITIONAL
};
 
enum ObjectRemovalReason {
    OUT_OF_REGION = 1,
    OBJECT_DELETED,
    NO_LONGER_SUBSCRIBED
};
 
enum TransportType {
    RELIABLE = 1,
    BEST_EFFORT
};

enum ResignAction {
    RELEASE_ATTRIBUTES = 1,
    DELETE_OBJECTS,
    DELETE_OBJECTS_AND_RELEASE_ATTRIBUTES,
    NO_ACTION
};

typedef ULong ExtentIndex ;
typedef FederateAmbassador *FederateAmbassadorPtr ;
typedef ULong FederateHandle ;
typedef UShort FederationHandle ;
typedef Double FederationTime ;
typedef Double FederationTimeDelta ;
typedef ULong FederateID ;
typedef ULong Handle ;
typedef ULong InteractionClassHandle ;
typedef ULong ObjectClassHandle ;
typedef UShort ObjectHandlecount ;
typedef ULong RegionToken ;
typedef Double TickTime ;
typedef ULong UniqueID ;
typedef Long SpaceHandle ;

typedef Handle AttributeHandle ;
typedef Handle DimensionHandle ;
typedef Handle ObjectHandle ;
typedef Handle OrderingHandle ;
typedef Handle ParameterHandle ;
typedef Handle TransportationHandle ;

// BUG: Le Transport des EventRetractHandle n'est pas assure par les 
// Message. Voir Message::WriteRAction dans Message_RW.cc .
struct EventRetractionHandle_s {
    FederationTime theEventTime ;
    UniqueID theSerialNumber ;
    FederateHandle sendingFederate ;
};

typedef struct EventRetractionHandle_s EventRetractionHandle ;

typedef char * ObjectName ; // See MAX_USER_TAG_LENGTH
typedef char * SpaceName ;

// Parameters and Attributes MUST share the same types.
typedef char * AttributeName ; // See MAX_USER_TAG_LENGTH
typedef char AttributeValue[MAX_BYTES_PER_VALUE + 1] ;
typedef char * ParameterName ; // See MAX_USER_TAG_LENGTH
typedef AttributeValue ParameterValue ;

typedef AttributeValue ValueName ;
typedef char ValueType[MAX_BYTES_PER_VALUETYPE + 1] ;

struct Value {
    ValueName name ;
    ValueName value ;
    ValueType type ;
    ULong length ;
};
 
// struct range_struct_s {
//     ULong first ;
//     ULong last ;
// };
// typedef struct range_struct_s Range ;
 
// struct Extents_s {
//     ULong _length ;
//     Range* _buffer ;
// };
// typedef struct Extents_s Extents ;
 
// struct ExtentSet_s {
//     ULong _length ;
//     Extents* _buffer ;
// };
// typedef struct ExtentSet_s ExtentSet ;
 
typedef ULong HLA_Region ;
 
struct Threshold_s {
    ULong value ;
};
typedef Threshold_s Threshold ;
 
struct ThresholdSet_s {
    ULong _length ;
    Threshold* _buffer ;
};
typedef struct ThresholdSet_s ThresholdSet ;
 
class FederateHandleSet
{
public:
    virtual ~FederateHandleSet(void) {;}

    virtual ULong size(void) const = 0 ;

    virtual FederateHandle getHandle(ULong i) const
        throw (ArrayIndexOutOfBounds) = 0 ;
 
    virtual void add(FederateHandle h)
        throw (ValueCountExceeded) = 0 ;
 
    virtual void remove(FederateHandle h)
        throw (AttributeNotDefined, ArrayIndexOutOfBounds) = 0 ;
 
    virtual void empty(void) = 0 ;

    virtual Boolean isMember(FederateHandle h) const = 0 ;
};

class FederateHandleSetFactory
{
public:
    static FederateHandleSet* create(ULong count)
        throw (MemoryExhausted,
               ValueCountExceeded);
};

class Region
{
public:
    virtual ~Region(void) {;}

    virtual ULong 
    getRangeLowerBound(ExtentIndex theExtent,
                       DimensionHandle theDimension) const
        throw (ArrayIndexOutOfBounds) = 0 ;

    virtual ULong 
    getRangeUpperBound(ExtentIndex theExtent,
                       DimensionHandle theDimension) const
        throw (ArrayIndexOutOfBounds) = 0 ;

    virtual void 
    setRangeLowerBound(ExtentIndex theExtent,
                       DimensionHandle theDimension,
                       ULong theLowerBound)
        throw (ArrayIndexOutOfBounds) = 0 ;

    virtual void 
    setRangeUpperBound(ExtentIndex theExtent,
                       DimensionHandle theDimension,
                       ULong theUpperBound)
        throw (ArrayIndexOutOfBounds) = 0 ;

    virtual SpaceHandle 
    getSpaceHandle(void) const
        throw () = 0 ;

    virtual ULong 
    getNumberOfExtents(void) const
        throw () = 0 ;

    virtual ULong 
    getRangeLowerBoundNotificationLimit(ExtentIndex theExtent,
                                        DimensionHandle theDimension) const
        throw (ArrayIndexOutOfBounds) = 0 ;

    virtual ULong 
    getRangeUpperBoundNotificationLimit(ExtentIndex theExtent,
                                        DimensionHandle theDimension) const
        throw (ArrayIndexOutOfBounds) = 0 ;
};

class AttributeHandleSet
{
public:
    virtual ~AttributeHandleSet(void) {;}

    virtual ULong size(void) const = 0 ;

    virtual AttributeHandle getHandle(ULong i) const
        throw (ArrayIndexOutOfBounds) = 0 ;
 
    virtual void add(AttributeHandle h)
        throw (ArrayIndexOutOfBounds, AttributeNotDefined) = 0 ;
 
    virtual void remove(AttributeHandle h) 
        throw (AttributeNotDefined) = 0 ;
 
    virtual void empty(void) = 0 ;

    virtual Boolean isEmpty(void) const = 0 ;
    virtual Boolean isMember(AttributeHandle h) const = 0 ;
};

class AttributeHandleSetFactory 
{
public:
    static AttributeHandleSet* create(ULong count)
        throw (MemoryExhausted, ValueCountExceeded);
};

class AttributeHandleValuePairSet
{
public:
    virtual ~AttributeHandleValuePairSet(void) {;}

    virtual ULong size(void) const = 0;

    virtual Handle getHandle(ULong i) const 
        throw (ArrayIndexOutOfBounds) = 0;

    virtual ULong getValueLength(ULong i) const
        throw (ArrayIndexOutOfBounds) = 0 ;
 
    virtual void getValue(ULong i, char* buff, ULong& valueLength) const
        throw (ArrayIndexOutOfBounds) = 0 ;

    virtual char *getValuePointer(ULong i, ULong& valueLength) const
        throw (ArrayIndexOutOfBounds) = 0 ;

    virtual TransportType getTransportType(ULong i) const
        throw (ArrayIndexOutOfBounds, InvalidHandleValuePairSetContext) = 0 ;

    virtual OrderType getOrderType(ULong i) const
        throw (ArrayIndexOutOfBounds, InvalidHandleValuePairSetContext) = 0 ;

    virtual Region *getRegion(ULong i) const
        throw (ArrayIndexOutOfBounds, InvalidHandleValuePairSetContext,
               UnimplementedService) = 0 ;

    virtual void add(Handle h, const char* buff, ULong valueLength)
        throw (ValueLengthExceeded, ValueCountExceeded) = 0 ;

    virtual void remove(Handle h) throw (ArrayIndexOutOfBounds) = 0 ;
 
    virtual void moveFrom(const AttributeHandleValuePairSet& ahvps, ULong& i)
        throw (ValueCountExceeded, ArrayIndexOutOfBounds,
               UnimplementedService) = 0 ;
 
    virtual void empty(void) = 0 ;
    virtual ULong start(void) const = 0 ;
    virtual ULong valid(ULong i) const = 0 ;
    virtual ULong next(ULong i) const = 0 ;
};

class ParameterHandleValuePairSet
{
public:
    virtual ~ParameterHandleValuePairSet(void) {;}

    virtual ULong size(void) const = 0 ;
 
    virtual Handle getHandle(ULong i) const throw (ArrayIndexOutOfBounds) = 0 ;
 
    virtual ULong getValueLength(ULong i) const
        throw (ArrayIndexOutOfBounds) = 0 ;
 
    virtual void getValue(ULong i, char* buff, ULong& valueLength) const
        throw (ArrayIndexOutOfBounds) = 0 ;
 
    virtual char *getValuePointer(ULong i, ULong& valueLength) const
        throw (ArrayIndexOutOfBounds) = 0 ;
 
    virtual TransportType getTransportType(void) const
        throw (InvalidHandleValuePairSetContext) = 0 ;

    virtual OrderType getOrderType(void) const
        throw (InvalidHandleValuePairSetContext) = 0 ;

    virtual Region *getRegion(void) const
        throw (InvalidHandleValuePairSetContext, UnimplementedService) = 0 ;
 
    virtual void add(Handle h, const char* buff, ULong valueLength)
        throw (ValueLengthExceeded, ValueCountExceeded) = 0 ;
 
    virtual void remove(Handle h) throw (ArrayIndexOutOfBounds) = 0 ;

    virtual void moveFrom(const ParameterHandleValuePairSet& phvps, ULong& i)
        throw (ValueCountExceeded,
               ArrayIndexOutOfBounds,
               UnimplementedService) = 0 ;

    virtual void empty(void) = 0 ;
    virtual ULong start(void) const = 0 ;
    virtual ULong valid(ULong i) const = 0 ;
    virtual ULong next(ULong i) const = 0 ;
};

class AttributeSetFactory 
{
public:
    static AttributeHandleValuePairSet* create(ULong count)
        throw (MemoryExhausted, 
               ValueCountExceeded, 
               HandleValuePairMaximumExceeded);
};

class ParameterSetFactory 
{
public:
    static ParameterHandleValuePairSet* create(ULong count)
        throw (MemoryExhausted, ValueCountExceeded, 
               HandleValuePairMaximumExceeded);
};

}

#endif // _CERTI_RTI_TYPES_HH

// $Id: RTItypes.hh,v 3.9 2003/02/17 09:17:03 breholee Exp $
