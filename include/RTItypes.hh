// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002  ONERA
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
// $Id: RTItypes.hh,v 3.5 2003/01/17 18:20:03 breholee Exp $
// ---------------------------------------------------------------------------

// Types utilises par le RTI : Constantes, exceptions, handles...

#ifndef _CERTI_RTI_TYPES_HH
#define _CERTI_RTI_TYPES_HH

#include "baseTypes.hh"
#include "local_config.hh"

#define RTI_VERSION PACKAGE"-"VERSION

namespace certi {
  class CAttributeHandleValuePairSet;
// Each time a new exception is added, please handle this new case in
// exception_type.hh and in RTIambassador.cc.
RTI_EXCEPT(ArrayIndexOutOfBounds)
  RTI_EXCEPT(AttributeAlreadyOwned)
  RTI_EXCEPT(AttributeNotDefined)
  RTI_EXCEPT(AttributeNotKnown)
  RTI_EXCEPT(AttributeNotOwned)
  RTI_EXCEPT(AttributeNotPublished)
  RTI_EXCEPT(AttributeNotSubscribed)
  RTI_EXCEPT(ConcurrentAccessAttempted)
  RTI_EXCEPT(CouldNotDiscover)
  RTI_EXCEPT(CouldNotOpenRID)
  RTI_EXCEPT(CouldNotRestore)
  RTI_EXCEPT(DeletePrivilegeNotHeld)
  RTI_EXCEPT(ErrorReadingRID)
  RTI_EXCEPT(EventNotKnown)
  RTI_EXCEPT(FederateAlreadyPaused)
  RTI_EXCEPT(FederateAlreadyExecutionMember)
  RTI_EXCEPT(FederateDoesNotExist)
  RTI_EXCEPT(FederateInternalError)
  RTI_EXCEPT(FederateNameAlreadyInUse)
  RTI_EXCEPT(FederateNotExecutionMember)
  RTI_EXCEPT(FederateNotPaused)
  RTI_EXCEPT(FederateNotPublishing)
  RTI_EXCEPT(FederateNotSubscribing)
  RTI_EXCEPT(FederateOwnsAttributes)
  RTI_EXCEPT(FederatesCurrentlyJoined)
  RTI_EXCEPT(FederationAlreadyPaused)
  RTI_EXCEPT(FederationExecutionAlreadyExists)
  RTI_EXCEPT(FederationExecutionDoesNotExist)
  RTI_EXCEPT(FederationNotPaused)
  RTI_EXCEPT(FederationTimeAlreadyPassed)
  RTI_EXCEPT(IDsupplyExhausted)
  RTI_EXCEPT(InteractionClassNotDefined)
  RTI_EXCEPT(InteractionClassNotKnown)
  RTI_EXCEPT(InteractionClassNotPublished)
  RTI_EXCEPT(InteractionParameterNotDefined)
  RTI_EXCEPT(InteractionParameterNotKnown)
  RTI_EXCEPT(InvalidDivestitureCondition)
  RTI_EXCEPT(InvalidExtents)
  RTI_EXCEPT(InvalidFederationTime)
  RTI_EXCEPT(InvalidFederationTimeDelta)
  RTI_EXCEPT(InvalidObjectHandle)
  RTI_EXCEPT(InvalidOrderType)
  RTI_EXCEPT(InvalidResignAction)
  RTI_EXCEPT(InvalidRetractionHandle)
  RTI_EXCEPT(InvalidRoutingSpace)
  RTI_EXCEPT(InvalidTransportType)
  RTI_EXCEPT(MemoryExhausted)
  RTI_EXCEPT(NameNotFound)
  RTI_EXCEPT(NoPauseRequested)
  RTI_EXCEPT(NoResumeRequested)
  RTI_EXCEPT(ObjectClassNotDefined)
  RTI_EXCEPT(ObjectClassNotKnown)
  RTI_EXCEPT(ObjectClassNotPublished)
  RTI_EXCEPT(ObjectClassNotSubscribed)
  RTI_EXCEPT(ObjectNotKnown)
  RTI_EXCEPT(ObjectAlreadyRegistered)
  RTI_EXCEPT(RegionNotKnown)
  RTI_EXCEPT(RestoreInProgress)
  RTI_EXCEPT(RestoreNotRequested)
  RTI_EXCEPT(RTIinternalError)
  RTI_EXCEPT(SaveInProgress)
  RTI_EXCEPT(SaveNotInitiated)
  RTI_EXCEPT(SecurityError)
  RTI_EXCEPT(SpaceNotDefined)
  RTI_EXCEPT(SpecifiedSaveLabelDoesNotExist)
  RTI_EXCEPT(TimeAdvanceAlreadyInProgress)
  RTI_EXCEPT(TimeAdvanceWasNotInProgress)
  RTI_EXCEPT(TooManyIDsRequested)
  RTI_EXCEPT(UnableToPerformSave)
  RTI_EXCEPT(UnimplementedService)
  RTI_EXCEPT(UnknownLabel)
  RTI_EXCEPT(ValueCountExceeded)
  RTI_EXCEPT(ValueLengthExceeded)
 
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

typedef ULong Handle;
typedef Handle TransportationHandle;
//typedef TransportationHandle TransportType ;
 
enum ResignAction {
  RELEASE_ATTRIBUTES = 1,
  DELETE_OBJECTS,
  DELETE_OBJECTS_AND_RELEASE_ATTRIBUTES,
  NO_ACTION
};
 
class FederateAmbassador;
 
typedef FederateAmbassador *FederateAmbassadorPtr;
typedef ULong FederateHandle;

#define MAX_FEDERATEHANDLE USHRT_MAX

typedef UShort FederationHandle;
typedef ULong ObjectClassHandle;
typedef ULong InteractionClassHandle;
typedef Handle AttributeHandle;
typedef Handle ParameterHandle;
 
#define MAX_OBJECTID ULONG_MAX

typedef ULong FederateID ;
typedef ULong UniqueID;
typedef UShort ObjectHandlecount;
typedef Double FederationTimeDelta; // Must be the same as FederationTime
typedef Double FederationTime;
typedef Double TickTime;

// BUG: Le Transport des EventRetractHandle n'est pas assure par les 
// Message. Voir Message::WriteRAction dans Message_RW.cc .
struct EventRetractionHandle_s {
  FederationTime theEventTime;
  UniqueID theSerialNumber;
  FederateHandle sendingFederate;
};
typedef struct EventRetractionHandle_s EventRetractionHandle;
// All char * declarations should use null terminated strings

typedef char * FederationExecutionName; // See MAX_FEDERATION_NAME_LENGTH
typedef char * FederateName; // See MAX_FEDERATE_NAME_LENGTH
typedef char * ObjectClassName; // See MAX_USER_TAG_LENGTH
typedef char * ObjectName; // See MAX_USER_TAG_LENGTH
typedef char * InteractionClassName; // See MAX_USER_TAG_LENGTH

// Parameters and Attributes MUST share the same types.
typedef char * AttributeName; // See MAX_USER_TAG_LENGTH
typedef char AttributeValue[MAX_BYTES_PER_VALUE + 1];
typedef char * ParameterName; // See MAX_USER_TAG_LENGTH
typedef AttributeValue ParameterValue;

typedef AttributeValue ValueName;
typedef char ValueType[MAX_BYTES_PER_VALUETYPE + 1];

struct Value {
  ValueName name;
  ValueName value;
  ValueType type;
  ULong length;
};
 
struct range_struct_s {
  ULong first;
  ULong last;
};
typedef struct range_struct_s Range;
 
struct Extents_s {
  ULong _length;
  Range* _buffer;
};
typedef struct Extents_s Extents;
 
struct ExtentSet_s {
  ULong _length;
  Extents* _buffer;
};
typedef struct ExtentSet_s ExtentSet;
 
typedef ULong HLA_Region;
 
struct Threshold_s {
  ULong value;
};
typedef Threshold_s Threshold;
 
struct ThresholdSet_s {
  ULong _length;
  Threshold* _buffer;
};
typedef struct ThresholdSet_s ThresholdSet;
 
typedef Long SpaceHandle;
 
typedef char * SpaceName;

//-----------------------------------------------------------------
// RTItypes for RTI 1.3 compatibility

RTI_EXCEPT(AsynchronousDeliveryAlreadyDisabled)
RTI_EXCEPT(AsynchronousDeliveryAlreadyEnabled)
RTI_EXCEPT(AttributeAcquisitionWasNotRequested)
RTI_EXCEPT(AttributeAcquisitionWasNotCanceled)
RTI_EXCEPT(AttributeAlreadyBeingAcquired)
RTI_EXCEPT(AttributeAlreadyBeingDivested)
RTI_EXCEPT(AttributeDivestitureWasNotRequested)
RTI_EXCEPT(CouldNotOpenFED)
RTI_EXCEPT(DimensionNotDefined)
RTI_EXCEPT(EnableTimeConstrainedPending)
RTI_EXCEPT(EnableTimeConstrainedWasNotPending)
RTI_EXCEPT(EnableTimeRegulationPending)
RTI_EXCEPT(EnableTimeRegulationWasNotPending)
RTI_EXCEPT(ErrorReadingFED)
RTI_EXCEPT(FederateLoggingServiceCalls)
RTI_EXCEPT(FederateWasNotAskedToReleaseAttribute)
RTI_EXCEPT(HandleValuePairMaximumExceeded)
RTI_EXCEPT(InteractionClassNotSubscribed)
RTI_EXCEPT(InvalidHandleValuePairSetContext)
RTI_EXCEPT(InvalidLookahead)
RTI_EXCEPT(InvalidOrderingHandle)
RTI_EXCEPT(InvalidRegionContext)
RTI_EXCEPT(InvalidTransportationHandle)
RTI_EXCEPT(OwnershipAcquisitionPending)
RTI_EXCEPT(RegionInUse)
RTI_EXCEPT(RTICannotRestore)
RTI_EXCEPT(SynchronizationPointLabelWasNotAnnounced)
RTI_EXCEPT(TimeConstrainedAlreadyEnabled)
RTI_EXCEPT(TimeConstrainedWasNotEnabled)
RTI_EXCEPT(TimeRegulationAlreadyEnabled)
RTI_EXCEPT(TimeRegulationWasNotEnabled)

typedef ULong ExtentIndex;
typedef Handle ObjectHandle;
typedef Handle DimensionHandle;

typedef Handle OrderingHandle;
typedef ULong RegionToken;
//typedef OrderingHandle OrderType ;

class Region ;

class FederateHandleSet {
public:
  virtual ULong size() const = 0;

  virtual FederateHandle getHandle(ULong i) const
    throw(ArrayIndexOutOfBounds) = 0;
 
  virtual void add(FederateHandle h)
    throw(ValueCountExceeded) = 0;
 
  virtual void remove(FederateHandle h)
    throw(ArrayIndexOutOfBounds) = 0;
 
  virtual void empty() = 0; // Empty the set without deallocating space.

  virtual Boolean isMember(FederateHandle h) const = 0;
};

class FederateHandleSetFactory {
public:
  static FederateHandleSet* create(ULong count)
    throw(MemoryExhausted,
	  ValueCountExceeded);
};

class Region {
public:

  virtual ~Region() { }

  virtual ULong 
  getRangeLowerBound(ExtentIndex theExtent,
		     DimensionHandle theDimension) const
    throw(ArrayIndexOutOfBounds) = 0;

  virtual ULong 
  getRangeUpperBound(ExtentIndex theExtent,
		     DimensionHandle theDimension) const
    throw(ArrayIndexOutOfBounds) = 0;

  virtual void 
  setRangeLowerBound(ExtentIndex theExtent,
		     DimensionHandle theDimension,
		     ULong theLowerBound)
    throw(ArrayIndexOutOfBounds) = 0;

  virtual void 
  setRangeUpperBound(ExtentIndex theExtent,
		     DimensionHandle theDimension,
		     ULong theUpperBound)
    throw(ArrayIndexOutOfBounds) = 0;

  virtual SpaceHandle 
  getSpaceHandle() const
    throw() = 0;

  virtual ULong 
  getNumberOfExtents() const
    throw() = 0;

  virtual ULong 
  getRangeLowerBoundNotificationLimit(ExtentIndex theExtent,
				      DimensionHandle theDimension) const
    throw(
	  ArrayIndexOutOfBounds) = 0;

  virtual ULong 
  getRangeUpperBoundNotificationLimit(ExtentIndex theExtent,
				      DimensionHandle theDimension) const
    throw(ArrayIndexOutOfBounds) = 0;
};

class HandlePair 
{
public:
  Handle _handle;
  HandlePair *_next;
   
public :
  HandlePair();
  ~HandlePair();
};

class AttributeHandleSet 
{
  // ATTRIBUTES
public:
  ULong _size;
  HandlePair *_head;
    
  // METHODS
public:
  virtual ~AttributeHandleSet();

  virtual ULong size() const;

  virtual AttributeHandle getHandle(ULong i) const
    throw(ArrayIndexOutOfBounds);
 
  virtual void add(AttributeHandle h)
    throw(ArrayIndexOutOfBounds, AttributeNotDefined);
 
  virtual void remove(AttributeHandle h) 
    throw(AttributeNotDefined);
 
  virtual void empty();

  virtual Boolean isEmpty() const;
  virtual Boolean isMember(AttributeHandle h) const;
};

class AttributeHandleSetFactory 
{
  // METHODS
public:
  static AttributeHandleSet* create(ULong count)
    throw(MemoryExhausted, ValueCountExceeded);
};

class AttributeHandleValuePair 
{
  // ATTRIBUTES
public:
  Handle _handle;
  ULong _valuelength;
  char *_value;
  AttributeHandleValuePair *_next;

  // METHODS
public :
  AttributeHandleValuePair();
  ~AttributeHandleValuePair();
};

class AttributeHandleValuePairSet 
{
  // ATTRIBUTES
public:
  OrderType _order;
  TransportType _transport;
  ULong _size;
  AttributeHandleValuePair *_head;

  // METHODS
public:
  virtual ~AttributeHandleValuePairSet();

  virtual ULong size() const ;
  virtual Handle getHandle(ULong i) const throw(ArrayIndexOutOfBounds);
  virtual ULong getValueLength(ULong i) const throw(ArrayIndexOutOfBounds);
 
  virtual void getValue(ULong i, char* buff, ULong& valueLength) const
    throw(ArrayIndexOutOfBounds);

  virtual char *getValuePointer(ULong i, ULong& valueLength) const
    throw(ArrayIndexOutOfBounds);

  virtual TransportType getTransportType(ULong i) const
    throw(ArrayIndexOutOfBounds, InvalidHandleValuePairSetContext);

  virtual OrderType getOrderType(ULong i) const
    throw(ArrayIndexOutOfBounds, InvalidHandleValuePairSetContext);

  virtual Region *getRegion(ULong i) const
    throw(ArrayIndexOutOfBounds, InvalidHandleValuePairSetContext,
	  UnimplementedService);//CERTI

  virtual void add(Handle h, const char* buff, ULong valueLength)
    throw(ValueLengthExceeded, ValueCountExceeded);
  virtual void remove(Handle h) throw(ArrayIndexOutOfBounds);
 
  virtual void moveFrom(const AttributeHandleValuePairSet& ahvps, ULong& i)
    throw(ValueCountExceeded, ArrayIndexOutOfBounds, UnimplementedService);
 
  virtual void empty();
  virtual ULong start() const;
  virtual ULong valid(ULong i) const;
  virtual ULong next(ULong i) const;
};

class CAttributeHandleValuePair
{
public:
  OrderType _order;
  TransportType _transport;
  Boolean _isPublished;
  Boolean _isSubscribed;
  AttributeHandle _attrib;
  Value _value;
  CAttributeHandleValuePair *_next;

public:
  CAttributeHandleValuePair();
  ~CAttributeHandleValuePair();
 
  void AfficheAttribute();  
};

class CAttributeHandleValuePairSet
{
protected:
  CAttributeHandleValuePair *_head;
public:
  UShort _size; 

public:
  CAttributeHandleValuePairSet(void);
  CAttributeHandleValuePairSet(const AttributeHandleValuePairSet & ahvps);
  CAttributeHandleValuePairSet(const AttributeHandleSet & ahs);
  ~CAttributeHandleValuePairSet(void);

  void add(CAttributeHandleValuePair *att);
  CAttributeHandleValuePair *getWithHandle(AttributeHandle) const;
  CAttributeHandleValuePair *getWithName(AttributeName) const;
  CAttributeHandleValuePair *getIeme(UShort) const;
  void del(AttributeHandle);
  void empty(void);

  AttributeHandleValuePairSet* toAHVPS(void) const;
};	

class CParameterHandleValuePair
{
public:
  ParameterHandle _param;
  Value _value;
  CParameterHandleValuePair *_next;

public:
  CParameterHandleValuePair();
  ~CParameterHandleValuePair();
    
  void AfficheParameter();
};

class ParameterHandleValuePairSet;

class CParameterHandleValuePairSet
{ 
protected:
  CParameterHandleValuePair *_head;
public:
  UShort _size;

public:
  CParameterHandleValuePairSet(void);
  CParameterHandleValuePairSet(const ParameterHandleValuePairSet & phvps);
  ~CParameterHandleValuePairSet(void);

  void add(CParameterHandleValuePair *par);
  CParameterHandleValuePair *getWithHandle(ParameterHandle) const;
  CParameterHandleValuePair *getWithName(ParameterName) const;
  CParameterHandleValuePair *getIeme(UShort) const;
  void del(ParameterHandle);
  void empty();

  ParameterHandleValuePairSet* toPHVPS(void) const;
};

class ParameterHandleValuePair 
{
public:
  Handle _handle;
  ULong _valuelength;
  char *_value;
  ParameterHandleValuePair *_next;

public :
  ParameterHandleValuePair();
  ~ParameterHandleValuePair();
};

class ParameterHandleValuePairSet 
{
public:
  OrderType _order;
  TransportType _transport;
  ULong _size;
  ParameterHandleValuePair *_head;

public:
  virtual ~ParameterHandleValuePairSet();

  virtual ULong size() const;
 
  virtual Handle getHandle(ULong i) const throw(ArrayIndexOutOfBounds);
 
  virtual ULong getValueLength(ULong i) const throw(ArrayIndexOutOfBounds);
 
  virtual void getValue(ULong i, char* buff, ULong& valueLength) const
    throw(ArrayIndexOutOfBounds);
 
  virtual char *getValuePointer(ULong i, ULong& valueLength) const
    throw(ArrayIndexOutOfBounds);
 
  virtual TransportType getTransportType(void) const
    throw(InvalidHandleValuePairSetContext);
 
  virtual OrderType getOrderType(void) const
    throw(InvalidHandleValuePairSetContext);
 
  virtual Region *getRegion(void) const
    throw(InvalidHandleValuePairSetContext, UnimplementedService);//CERTI
 
  virtual void add(Handle h, const char* buff, ULong valueLength)
    throw(ValueLengthExceeded, ValueCountExceeded);
 
  virtual void remove(Handle h) throw(ArrayIndexOutOfBounds);

  virtual void moveFrom(const ParameterHandleValuePairSet& phvps, ULong& i)
    throw(ValueCountExceeded, ArrayIndexOutOfBounds, UnimplementedService);
 
  virtual void empty();
  virtual ULong start() const;
  virtual ULong valid(ULong i) const;
  virtual ULong next(ULong i) const;
};

class AttributeSetFactory 
{
public:
  static AttributeHandleValuePairSet* create(ULong count)
    throw(MemoryExhausted, 
	  ValueCountExceeded, 
	  HandleValuePairMaximumExceeded);
};

class ParameterSetFactory 
{
public:
  static ParameterHandleValuePairSet* create(ULong count)
    throw(MemoryExhausted, ValueCountExceeded, 
	  HandleValuePairMaximumExceeded);
};

}

#endif // _CERTI_RTI_TYPES_HH

// $Id: RTItypes.hh,v 3.5 2003/01/17 18:20:03 breholee Exp $
