// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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
// $Id: Message.hh,v 3.27 2005/04/05 12:28:56 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_MESSAGE_HH
#define _CERTI_MESSAGE_HH

#include "fedtime.hh"
#include "Exception.hh"
#include "SocketUN.hh"
#include "MessageBody.hh"
#include "BasicMessage.hh"

namespace certi {

/*! The Message class is used to formalize messages that are going to be
  exchanged between the RTI and the federate.
*/
class Message : public BasicMessage
{
public:
    struct MessageTimeStruct {
        FederationTime date ; // Date, Logical Time, Lookahead, etc.
        Boolean mode ; // IsRegulator or IsConstrained
    };

    struct MessageT_O_Struct {
        ObjectClassHandle handle ;
        UShort size ;
        TransportType transport ;
        OrderType order ;
    };

    struct MessageJ_R_Struct {
        FederateHandle federate ; // Join
        ResignAction action ; // Resign
    };

    struct MessageO_I_Struct {
        ObjectClassHandle handle ;
        UShort size ;
        FederationTime date ;
    };

    struct Message_DDM {
        SpaceHandle space ;
        DimensionHandle dimension ;
        RegionHandle region ;
    };

    union MessageHeaderUnion {
        MessageTimeStruct time ;
        MessageT_O_Struct T_O ;
        MessageJ_R_Struct J_R ;
        MessageO_I_Struct O_I ;
        Message_DDM ddm ;
    };

    enum Type {
        NOT_USED = 0,

        // gestion federation
        CREATE_FEDERATION_EXECUTION,
        DESTROY_FEDERATION_EXECUTION,
        JOIN_FEDERATION_EXECUTION,
        RESIGN_FEDERATION_EXECUTION,
        REGISTER_FEDERATION_SYNCHRONIZATION_POINT,
        SYNCHRONIZATION_POINT_REGISTRATION_FAILED,
        SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED,
        ANNOUNCE_SYNCHRONIZATION_POINT,
        SYNCHRONIZATION_POINT_ACHIEVED,
        FEDERATION_SYNCHRONIZED,
        REQUEST_FEDERATION_SAVE,
        INITIATE_FEDERATE_SAVE,
        FEDERATE_SAVE_BEGUN,
        FEDERATE_SAVE_COMPLETE,
        FEDERATE_SAVE_NOT_COMPLETE,
        FEDERATION_SAVED,
        FEDERATION_NOT_SAVED,
        REQUEST_FEDERATION_RESTORE,
        REQUEST_FEDERATION_RESTORE_FAILED,
        REQUEST_FEDERATION_RESTORE_SUCCEEDED,
        INITIATE_FEDERATE_RESTORE,
        FEDERATE_RESTORE_COMPLETE,
        FEDERATE_RESTORE_NOT_COMPLETE,
        FEDERATION_RESTORED,
        FEDERATION_NOT_RESTORED,
        FEDERATION_RESTORE_BEGUN,

        // Declaration
        PUBLISH_OBJECT_CLASS,
        UNPUBLISH_OBJECT_CLASS,
        PUBLISH_INTERACTION_CLASS,
        UNPUBLISH_INTERACTION_CLASS,
        SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES,
        UNSUBSCRIBE_OBJECT_CLASS,
        SUBSCRIBE_INTERACTION_CLASS,
        UNSUBSCRIBE_INTERACTION_CLASS,
        START_REGISTRATION_FOR_OBJECT_CLASS,
        STOP_REGISTRATION_FOR_OBJECT_CLASS,
        TURN_INTERACTIONS_ON,
        TURN_INTERACTIONS_OFF,

        // Object
        REGISTER_OBJECT_INSTANCE,
        UPDATE_ATTRIBUTE_VALUES,
        DISCOVER_OBJECT_INSTANCE,
        REFLECT_ATTRIBUTE_VALUES,
        SEND_INTERACTION,
        RECEIVE_INTERACTION,
        DELETE_OBJECT_INSTANCE,
        LOCAL_DELETE_OBJECT_INSTANCE,
        REMOVE_OBJECT_INSTANCE,
        CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE,
        CHANGE_INTERACTION_TRANSPORTATION_TYPE,
        REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE,
        REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE,
        PROVIDE_ATTRIBUTE_VALUE_UPDATE,
        ATTRIBUTES_IN_SCOPE,
        ATTRIBUTES_OUT_OF_SCOPE,
        TURN_UPDATES_ON_FOR_OBJECT_INSTANCE,
        TURN_UPDATES_OFF_FOR_OBJECT_INSTANCE,

        // Ownership
        REQUEST_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
        REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION,
        NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
        ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION,
        ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION,
        REQUEST_ATTRIBUTE_OWNERSHIP_ACQUISITION,
        REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE,
        QUERY_ATTRIBUTE_OWNERSHIP,
        INFORM_ATTRIBUTE_OWNERSHIP,
        IS_ATTRIBUTE_OWNED_BY_FEDERATE,
        ATTRIBUTE_IS_NOT_OWNED,
        ATTRIBUTE_OWNED_BY_RTI,
        ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE,
        ATTRIBUTE_OWNERSHIP_UNAVAILABLE,
        UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
        ATTRIBUTE_OWNERSHIP_ACQUISITION,
        CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
        ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE,
        CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION,
        CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION,

        // Time
        CHANGE_ATTRIBUTE_ORDER_TYPE,
        CHANGE_INTERACTION_ORDER_TYPE,
        ENABLE_TIME_REGULATION,
        DISABLE_TIME_REGULATION,
        ENABLE_TIME_CONSTRAINED,
        DISABLE_TIME_CONSTRAINED,
        QUERY_LBTS,
        QUERY_FEDERATE_TIME,
        QUERY_MIN_NEXT_EVENT_TIME,
        MODIFY_LOOKAHEAD,
        QUERY_LOOKAHEAD,
        RETRACT,
        REQUEST_RETRACTION,
        TIME_ADVANCE_REQUEST,
        TIME_ADVANCE_REQUEST_AVAILABLE,
        NEXT_EVENT_REQUEST,
        NEXT_EVENT_REQUEST_AVAILABLE,
        FLUSH_QUEUE_REQUEST,
        TIME_ADVANCE_GRANT,
        ENABLE_ASYNCHRONOUS_DELIVERY,
        DISABLE_ASYNCHRONOUS_DELIVERY,
        TIME_REGULATION_ENABLED,
        TIME_CONSTRAINED_ENABLED,

        // Data Distribution Management
        DDM_CREATE_REGION,
        DDM_MODIFY_REGION,
        DDM_DELETE_REGION,
        DDM_REGISTER_OBJECT,
        DDM_ASSOCIATE_REGION,
        DDM_UNASSOCIATE_REGION,
        DDM_SUBSCRIBE_ATTRIBUTES,
        DDM_UNSUBSCRIBE_ATTRIBUTES,
        DDM_SUBSCRIBE_INTERACTION,
        DDM_UNSUBSCRIBE_INTERACTION,
        DDM_REQUEST_UPDATE,

        // Support Services
        GET_OBJECT_CLASS_HANDLE,
        GET_OBJECT_CLASS_NAME,
        GET_ATTRIBUTE_HANDLE,
        GET_ATTRIBUTE_NAME,
        GET_INTERACTION_CLASS_HANDLE,
        GET_INTERACTION_CLASS_NAME,
        GET_PARAMETER_HANDLE,
        GET_PARAMETER_NAME,
        GET_OBJECT_INSTANCE_HANDLE,
        GET_OBJECT_INSTANCE_NAME,
        GET_SPACE_HANDLE,
        GET_SPACE_NAME,
        GET_DIMENSION_HANDLE,
        GET_DIMENSION_NAME,
        GET_ATTRIBUTE_SPACE_HANDLE,
        GET_OBJECT_CLASS,
        GET_INTERACTION_SPACE_HANDLE,
        GET_TRANSPORTATION_HANDLE,
        GET_TRANSPORTATION_NAME,
        GET_ORDERING_HANDLE,
        GET_ORDERING_NAME,
        ENABLE_CLASS_RELEVANCE_ADVISORY_SWITCH,
        DISABLE_CLASS_RELEVANCE_ADVISORY_SWITCH,
        ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH,
        DISABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH,
        ENABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH,
        DISABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH,
        ENABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH,
        DISABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH,
        TICK_REQUEST,

	LAST // should be the "last" (not used)
    };

    // -- Structure de l'entete --
    struct MessageHeader {
        Type type ;
        TypeException exception ;
        UShort bodySize ;
        MessageHeaderUnion VP ; // Variable Part
    };

public:
    // Constructor & Destructor
    Message();
    ~Message();

    // Read and Write NetworkMessage Objects to and from Socket objects.
    void write(SocketUN *Socket) throw (NetworkError, NetworkSignal);
    void read(SocketUN *Socket) throw (NetworkError, NetworkSignal);

    // -- Attribute Access Methods
    // Value Array Management
    void setValue(int Rank, const char *Value)
        throw (RTIinternalError); // Bad Rank, Bad Value

    // If Value == NULL return a newly allocated copy of Value, else copy it
    // in Value.
    char *getValue(int Rank, char *Value = 0) const
        throw (RTIinternalError); // Bad Rank

    // Return a newly allocated ValueArray, exactly of size HandleArraySize.
    // containing the actual Attrib/Param values. You must FREE this structure.
    AttributeValue *getValueArray();

    // Strings Access

    // All the Set* methods check the length of the string, and throw
    // ValueLengthExceeded if the new string is too long.

    // All the Get* methods return a pointer on the actual stored value.
    // This pointer is READ-ONLY, and should not be de-allocated.

    const char *getLabel() const { return label ; };
    void setLabel(const char *new_label);

    const char *getName() const { return name ; };
    void setName(const char *new_name);

    DimensionHandle getDimension() const { return dimension ; };
    void setDimension(DimensionHandle);

    const char *getFederationName() const { return federationName ; };
    void setFederationName(const char *federation_name);

    const char *getFederateName() const { return federateName ; };
    void setFederateName(const char *federate_name);

    const char *getTag() const { return tag ; };
    void setTag(const char *new_tag);

    SpaceHandle getSpace() const { return space ; };
    void setSpace(SpaceHandle);

    Type getType() const { return type ; };
    void setType(Type);

    unsigned long getNumber() const { return number ; };
    void setNumber(unsigned long);

    long getRegion() const { return region ; };
    void setRegion(long);

    void setFederationTimeDelta(FederationTimeDelta);
    FederationTimeDelta getFederationTimeDelta() const
    { return lookahead ; };

    void setAttribute(AttributeHandle);
    AttributeHandle getAttribute() const { return attribute ; };

    void setInteractionClass(InteractionClassHandle);
    InteractionClassHandle getInteractionClass() const {
        return interactionClass ;
    };

    void setObjectClass(ObjectClassHandle);
    ObjectClassHandle getObjectClass() const { return objectClass ; };

    void setResignAction(ResignAction);
    ResignAction getResignAction() const { return resignAction ; };

    void setFedTime(const FedTime&);
    const FedTime& getFedTime() const { return fed_time; };

    void setLookahead(const FedTime&);

    void setFederationTime(FederationTime);
    FederationTime getFederationTime() const { return fed_time._fedTime ; };
    
    void setBoolean(Boolean);
    Boolean getBoolean() const { return boolean ; };

    void setObject(ObjectHandle);
    ObjectHandle getObject() const { return object ; };

    void setTransportation(TransportationHandle);
    TransportationHandle getTransportation() const
    { return ((transport == RELIABLE) ? 1 : 0); };

    TransportType getTransportType() const
    { return transport ; };

    void setOrdering(OrderingHandle);
    OrderingHandle getOrdering() const
    { return ((order == RECEIVE) ? 1 : 0); };

    OrderType getOrderType() const { return order ; };

    void setEventRetraction(EventRetractionHandle);
    EventRetractionHandle getEventRetraction() const

    { return eventRetraction ; };

    void setParameter(ParameterHandle);
    ParameterHandle getParameter() const { return parameter ; };

    void setFederate(FederateHandle);
    FederateHandle getFederate() const { return federate ; };

    AttributeHandleSet* getAHS() const ;
    void setAHS(const AttributeHandleSet &);
    void setAHS(const AttributeHandle *, int);

    AttributeHandleValuePairSet* getAHVPS() const ;
    void setAHVPS(const AttributeHandleValuePairSet &);

    ParameterHandleValuePairSet* getPHVPS() const ;
    void setPHVPS(const ParameterHandleValuePairSet &);

    void setAttributes(AttributeHandle *, ushort);
    void setAttributes(AttributeHandle *, AttributeValue *, ushort);
    void setParameters(ParameterHandle *, ParameterValue *, ushort);

    void setException(TypeException, const char *the_reason = "\0");
    TypeException getExceptionType() const { return exception ; };
    const char *getExceptionReason() const { return exceptionReason ; };

    // Public attributes
    Type type ;

protected:
    TypeException exception ;
    char exceptionReason[MAX_EXCEPTION_REASON_LENGTH + 1] ;

    RTIfedTime fed_time;
    Boolean boolean ;
    FederationTimeDelta lookahead ;
    FederateHandle federate ;
    ResignAction resignAction ;
    UShort idCount ;
    ObjectHandle firstId ;
    ObjectHandle lastId ;
    ObjectClassHandle objectClass ;
    InteractionClassHandle interactionClass ;
    AttributeHandle attribute ;
    ParameterHandle parameter ;
    ObjectHandle object ;
    TransportType transport ;
    OrderType order ;
    EventRetractionHandle eventRetraction ;
    SpaceHandle space ;
    DimensionHandle dimension ;
    // TransportationHandle transportation ;
    // OrderingHandle ordering ;
    unsigned long number ;
    long region ;

public:
    // used for both Attributes and Parameters arrays.
    UShort handleArraySize ;
    AttributeHandle handleArray[MAX_ATTRIBUTES_PER_CLASS] ;

    Message& operator = (const Message&);

    void display(char *);

private:

    // ---------------------
    // -- Private Methods --(cf. Message_RW.cc)
    // ---------------------

    // Read a Message Body from a Socket. Should be called after
    // ReadHeader.
    void readBody(SocketUN *Socket);

    // Read a Header from a socket, and process it to read its
    // content. Return RTI_TRUE if the ReadBody Method has to be
    // called.
    bool readHeader(SocketUN *Socket);

    // The message is written onto the socket by WriteHeader if no
    // body is required, or by WriteBody is a body has been required
    // by WriteHeader.

    // Prepare and write a Body to a socket. Should be called after
    // WriteHeader.
    void writeBody(SocketUN *Socket);

    // Prepare and Write a Header to a Socket, and return RTI_TRUE if
    // the WriteBody method has to be called.
    bool writeHeader(SocketUN *Socket);

    // -- Other Private Read Methods --
    void readHandleArray(MessageBody *Body);
    void readLabel(MessageBody *Body);
    void readName(MessageBody *Body);
    void readFederationName(MessageBody *Body);
    void readFederateName(MessageBody *Body);
    void readResignAction(MessageBody *Body);
    void readTag(MessageBody *Body);
    void readValueArray(MessageBody *Body);

    // -- Other Private Write Methods --
    void writeHandleArray(MessageBody *Body);
    void writeResignAction(MessageBody *Body);
    void writeValueArray(MessageBody *Body);


    // ------------------------
    // -- Private Attributes --
    // ------------------------

    MessageHeader header ;
    char label[MAX_USER_TAG_LENGTH + 1] ;
    char name[MAX_USER_TAG_LENGTH + 1] ;
    char federateName[MAX_FEDERATE_NAME_LENGTH + 1] ;
    char federationName[MAX_FEDERATION_NAME_LENGTH + 1] ;
    char tag[MAX_USER_TAG_LENGTH + 1] ;
    AttributeValue valueArray[MAX_ATTRIBUTES_PER_CLASS] ;
};

} // namespace certi

#endif // _CERTI_MESSAGE_HH

// $Id: Message.hh,v 3.27 2005/04/05 12:28:56 breholee Exp $
