// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003, 2004  ONERA
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
// $Id: certi.hh,v 3.4 2004/05/18 13:15:06 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef CERTI_CERTI_HH
#define CERTI_CERTI_HH

#include <config.h>

#include <iostream>

// ========== CERTI specific parameters =======================================

// TODO: Most parameters should move in appropriate .hh file (if constant
// is really required) or in a configuration file. Max values shouldn't
// be considered (actual max is usually system-dependant).

// Types utilises par le RTI : Constantes, exceptions, handles...

#include <limits.h>

// -------------------------
// -- Do your tuning here --
// -------------------------

// Define it if you want the complete object and interaction tree to
// be displayed as the FED file is read.
// #define CREAD_DISPLAYS_ROOTOBJ


// If you want statistics to be printed at the end of the execution,
// uncomment the next line.
#define RTI_PRINTS_STATISTICS

// You can comment the next line out if you don't want to use Multicast.
// #define FEDERATION_USES_MULTICAST


// If this macro is defined, some messages will be sent on UDP sockets
// by the RTIG to the RTIAs. Some messages types are NULL messages, and
// all Reflect Attributes Values messages, and Receive Interaction messages,
// even those with data asking for a "RELIABLE" service level.

// #define HLA_USES_UDP

// The next macro must contain the path name of the Audit File. It should
// be an absolute path, but it may be a relative path for testing reasons.
#define RTIG_AUDIT_FILENAME "RTIG.log"

// Define the lower audit level you need, from AUDIT_MIN_LEVEL(0, all)
// to AUDIT_MAX_LEVEL(10, min audit logging).
// Level 0 : NULL messages(time synchronization)
// Level 1 : Update Attribute Values messages
// Level 2 : Send Interaction messages
// Level 6 : Register / Delete Objects messages
// Level 7 : Publish / Subscribe messages
// Level 8 : Pause, Resume, Set Time regulating/constrained messages
// Level 9 : Create/Join/Resign/Destroy Federation msgs, Connections.
// Level 10: RTIG and Audit start/stop.

#define AUDIT_CURRENT_LEVEL 5

// ------------------------------
// -- GSSAPI Security Services --
// ------------------------------

// Comment out HLA_USES_GSSAPI to choose NOT to use GSSAPI services.

//#define HLA_USES_GSSAPI


// Server Principal Name
//(must be like "<rtip principal name>@<network machine name>")

#define HLA_SERVER_PRINCIPAL_NAME "RTIG@brive"


// Expiration delay for GSSAPI sessions(must be greater then the longer
// lifetime of a federate).[In seconds]

#define HLA_GSS_SESSION_DURATION 3600


// You can use GSSAPI at least to authenticate federates(needed to use
// security levels), to sign all messages(source authentication), or
// to sign AND encrypt message.[last option]
// Just choose one of the three options below and affect it to the
// RTI_GSSAPI_USAGE macro.

#define RTI_GSS_NOTHING 0
#define RTI_GSS_SIGN_ONLY 1
#define RTI_GSS_ENCRYPT 2

#define RTI_GSSAPI_USAGE RTI_GSS_NOTHING

// Type of Security services needed : Can be any combination(OR) of the
// following flags(see your GSSAPI documentation for further details) :
//
// GSS_C_DELEG_FLAG Credential delegation(not supported by HLA anyway)
// GSS_C_MUTUAL_FLAG Mutual authentication
// GSS_C_REPLAY_FLAG Replay detection
// GSS_C_SEQUENCE_FLAG Out of sequence message detection.
//
// Note: Currently, no option is suppported by our default GSSAPI mechanism.

#define HLA_GSS_FLAGS 0

// ---------------
// -- Constants --
// ---------------

#define MAX_FEDERATION 20
#define MAX_FEDERATE 20
#define MAX_OBJECT_CLASSES 100
#define MAX_INTERACTION_CLASSES 100
#define MAX_ATTRIBUTES_PER_CLASS 20
#define MAX_PARAMETERS_PER_CLASS 20 // Must be the same as Attributes
#define MAX_BYTES_PER_VALUE 80
#define MAX_BYTES_PER_VALUETYPE 20
#define PRIVILEGE_TO_DELETE_HANDLE 0
#define ROOT_OBJECT_CLASS_HANDLE 0
#define ROOT_INTERACTION_CLASS_HANDLE 0
#define PRIVILEGE_TO_DELETE_NAME "privilegeToDelete"

#define MAX_FEDERATE_NAME_LENGTH 64
#define MAX_FEDERATION_NAME_LENGTH 64
#define MAX_USER_TAG_LENGTH 32

#define EPSILON (1.0e-9)
#define POSITIVE_INFINITY HUGE_VAL

// ========== HLA base types, RTI types, and time-related types ===============

// This part should be HLA 1.3 compliant. Somme HLA 1.1 deprecated types or
// values may still be there though.

namespace certi {

typedef unsigned short UShort ;
typedef short Short ;
typedef unsigned long ULong ;
typedef long Long ;
typedef double Double ;
typedef float Float ;

enum Boolean { RTI_FALSE = 0, RTI_TRUE };

class Exception {
public:
    ULong _serial ;
    char *_reason ;
    const char *_name ;
    Exception();
    Exception(const char *reason);
    Exception(ULong serial, const char *reason = 0);
    Exception(const Exception &toCopy);
    virtual ~Exception();
    Exception & operator = (const Exception &);
    // friend ostream& operator <<(ostream &, Exception *);
    virtual Exception *cloneSelf() const throw() = 0;
    virtual void throwSelf() const = 0;
};

#define RTI_EXCEPT(A) \
class A : public Exception { \
public: \
 A() { _name = ""#A ; } \
 A(const char *reason) : Exception(reason) { _name = "RTI::"#A ; } \
 A(ULong serial, const char *reason = 0) \
 : Exception(serial, reason) { _name = "RTI::"#A ; } \
 Exception *cloneSelf() const throw() { return (new A(_reason)); } \
 void throwSelf() const { throw *this ; } \
};

#define RTI_VERSION "CERTI-3"

#define MAX_FEDERATEHANDLE USHRT_MAX
#define MAX_OBJECTID ULONG_MAX

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
typedef ULong FederateHandle;
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

typedef char *ObjectName ; // See MAX_USER_TAG_LENGTH
typedef char *SpaceName ;

// Parameters and Attributes MUST share the same types.
//typedef char *AttributeName ; // See MAX_USER_TAG_LENGTH
typedef char AttributeValue[MAX_BYTES_PER_VALUE + 1] ;
typedef char *ParameterName ; // See MAX_USER_TAG_LENGTH
typedef AttributeValue ParameterValue ;

typedef AttributeValue ValueName ;
typedef char ValueType[MAX_BYTES_PER_VALUETYPE + 1] ;

struct Value {
    ValueName name ;
    ValueName value ;
    ValueType type ;
    ULong length ;
};

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
    virtual ~FederateHandleSet(void) { ; }

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
    virtual ~Region(void) { ; }

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
    virtual ~AttributeHandleSet(void) { ; }

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
    virtual ~AttributeHandleValuePairSet(void) { ; }

    virtual ULong size(void) const = 0 ;

    virtual Handle getHandle(ULong i) const
        throw (ArrayIndexOutOfBounds) = 0 ;

    virtual ULong getValueLength(ULong i) const
        throw (ArrayIndexOutOfBounds) = 0 ;

    virtual void getValue(ULong i, char *buff, ULong& valueLength) const
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

    virtual void add(Handle h, const char *buff, ULong valueLength)
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
    virtual ~ParameterHandleValuePairSet(void) { ; }

    virtual ULong size(void) const = 0 ;

    virtual Handle getHandle(ULong i) const throw (ArrayIndexOutOfBounds) = 0 ;

    virtual ULong getValueLength(ULong i) const
        throw (ArrayIndexOutOfBounds) = 0 ;

    virtual void getValue(ULong i, char *buff, ULong& valueLength) const
        throw (ArrayIndexOutOfBounds) = 0 ;

    virtual char *getValuePointer(ULong i, ULong& valueLength) const
        throw (ArrayIndexOutOfBounds) = 0 ;

    virtual TransportType getTransportType(void) const
        throw (InvalidHandleValuePairSetContext) = 0 ;

    virtual OrderType getOrderType(void) const
        throw (InvalidHandleValuePairSetContext) = 0 ;

    virtual Region *getRegion(void) const
        throw (InvalidHandleValuePairSetContext, UnimplementedService) = 0 ;

    virtual void add(Handle h, const char *buff, ULong valueLength)
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

// ---------- Time types ------------------------------------------------------

class FedTime {
public:
    virtual ~FedTime();
    virtual void setZero() = 0 ;
    virtual Boolean isZero() = 0 ;
    virtual void setEpsilon() = 0 ;
    virtual void setPositiveInfinity() = 0 ;
    virtual Boolean isPositiveInfinity() = 0 ;

    virtual FedTime& operator+= (const FedTime&)
        throw (InvalidFederationTime) = 0 ;

    virtual FedTime& operator-= (const FedTime&)
        throw (InvalidFederationTime) = 0 ;

    virtual Boolean operator<= (const FedTime&) const
        throw (InvalidFederationTime) = 0 ;

    virtual Boolean operator<(const FedTime&) const
        throw (InvalidFederationTime) = 0 ;

    virtual Boolean operator>= (const FedTime&) const
        throw (InvalidFederationTime) = 0 ;

    virtual Boolean operator>(const FedTime&) const
        throw (InvalidFederationTime) = 0 ;

    virtual Boolean operator== (const FedTime&) const
        throw (InvalidFederationTime) = 0 ;

    virtual FedTime& operator= (const FedTime&)
        throw (InvalidFederationTime) = 0 ;

    virtual int encodedLength() const = 0 ;
    virtual void encode(char *buff) const = 0 ;
    virtual int getPrintableLength() const = 0 ;
    virtual void getPrintableString(char *) = 0 ;
};

class FedTimeFactory {
public:
    static FedTime* makeZero() throw (MemoryExhausted);
    static FedTime* decode(const char *buf) throw (MemoryExhausted);
};

class RTIfedTime : public FedTime 
{
public:
    // Constructors and Destructors
    RTIfedTime();
    RTIfedTime(const Double&);
    RTIfedTime(const FedTime&);
    RTIfedTime(const RTIfedTime&);
    virtual ~RTIfedTime();

    // Overloaded functions from FedTime
    void setZero();
    Boolean isZero();
    void setEpsilon();
    void setPositiveInfinity();
    Boolean isPositiveInfinity();
    int encodedLength() const ;
    void encode(char *buff) const ;
    int getPrintableLength() const ;
    void getPrintableString(char *);

    // Overloaded operators from FedTime
    FedTime& operator+= (const FedTime&) throw (InvalidFederationTime);
    FedTime& operator-= (const FedTime&) throw (InvalidFederationTime);
    Boolean operator<= (const FedTime&) const throw (InvalidFederationTime);
    Boolean operator<(const FedTime&) const throw (InvalidFederationTime);
    Boolean operator>= (const FedTime&) const throw (InvalidFederationTime);
    Boolean operator>(const FedTime&) const throw (InvalidFederationTime);
    Boolean operator== (const FedTime&) const throw (InvalidFederationTime);
    FedTime& operator= (const FedTime&) throw (InvalidFederationTime);

    // Implementation functions
    Double getTime() const ;

    // Implementation operators
    Boolean operator!= (const FedTime&) const throw (InvalidFederationTime);
    FedTime& operator= (const RTIfedTime&) throw (InvalidFederationTime);
    FedTime& operator*= (const FedTime&) throw (InvalidFederationTime);
    FedTime& operator/= (const FedTime&) throw (InvalidFederationTime);
    RTIfedTime operator+(const FedTime&) throw (InvalidFederationTime);
    RTIfedTime operator-(const FedTime&) throw (InvalidFederationTime);
    RTIfedTime operator*(const FedTime&) throw (InvalidFederationTime);
    RTIfedTime operator/(const FedTime&) throw (InvalidFederationTime);
    // Boolean operator== (const Double&) const throw (InvalidFederationTime);

    // Implementation friends
    friend std::ostream& operator<<(std::ostream&, const FedTime&);

    // Implementation member variables
    Double _fedTime ;
    Double _zero ;
    Double _epsilon ;
    Double _positiveInfinity ;

    // private inline function
    inline void initZero() {
        _zero = _fedTime == 0?0:1 ;
    }
};

// Global operators
RTIfedTime operator+(const Double&, const FedTime&)
    throw (InvalidFederationTime);
RTIfedTime operator-(const Double&, const FedTime&)
    throw (InvalidFederationTime);
RTIfedTime operator*(const Double&, const FedTime&)
    throw (InvalidFederationTime);
RTIfedTime operator/(const Double&, const FedTime&)
    throw (InvalidFederationTime);

} // certi namespace

#endif // CERTI_CERTI_HH

// $Id: certi.hh,v 3.4 2004/05/18 13:15:06 breholee Exp $
