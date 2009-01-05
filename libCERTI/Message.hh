// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This program is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
// ----------------------------------------------------------------------------

#ifndef CERTI_MESSAGE_HH
#define CERTI_MESSAGE_HH



#include "Exception.hh"
#include "SocketUN.hh"
#include "BasicMessage.hh"
#include "GAV.hh"
#include "FedTimeD.hh"

#include <vector>
#include <string>

namespace certi {

/**
 * The Message class is used to formalize messages that are going to be
 * exchanged between the RTI and the federate.
 * @ingroup libCERTI
 */
class CERTI_EXPORT Message : public BasicMessage
{
public:
    struct MessageTimeStruct {
        FederationTime date ; // Date, Logical Time, Lookahead, etc.
        bool mode ; // IsRegulator or IsConstrained
    };

    struct MessageT_O_Struct {
        ObjectClassHandle handle ;
        UShort size ;
        TransportType transport ;
        OrderType order ;
    };

    struct MessageJ_R_Struct {
        FederateHandle federate ; // Join
        RTI::ResignAction action ; // Resign
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

    // MessageHeaderUnion  deprecated
    //union MessageHeaderUnion {
    //    MessageTimeStruct time ;
    //    MessageT_O_Struct T_O ;
    //    MessageJ_R_Struct J_R ;
    //    MessageO_I_Struct O_I ;
    //    Message_DDM ddm ;
    //};

    enum Type {
        NOT_USED = 0,
        CLOSE_CONNEXION,

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
        TICK_REQUEST_NEXT,
        TICK_REQUEST_STOP,

	LAST // should be the "last" (not used)
    };

    // -- Structure de l'entete --
    struct MessageHeader {
        Type type ;
        TypeException exception ;
        UShort bodySize ;
        FederationTime date ;
        //MessageHeaderUnion VP ; // Variable Part deprecated
    };

public:
    Message();

	/**
	 * Serialize the message into a buffer
	 */
	virtual void serialize(MessageBuffer& msgBuffer);
	/**
	 * DeSerialize the message from a buffer
	 */
	virtual void deserialize(MessageBuffer& msgBuffer);

	void send(SocketUN* socket, MessageBuffer& msgBuffer) throw (NetworkError, NetworkSignal);
	void receive(SocketUN* socket, MessageBuffer& msgBuffer) throw (NetworkError, NetworkSignal);

	 void trace(const char* context);

    // Return a newly allocated ValueArray, exactly of size HandleArraySize.
    // containing the actual Attrib/Param values.
    std::vector <AttributeValue_t> getValueArray();

    std::string getLabel() const { return label ; };
    void setLabel(std::string new_label);

    //const char *getName() const { return name ; };
    const std::string getName() const {return name;}
    void setName(const char *new_name);

    DimensionHandle getDimension() const { return dimension ; };
    void setDimension(DimensionHandle);

    std::string getFederationName() const { return federationName ; };
    void setFederationName(std::string federation_name);

    std::string getFederateName() const { return federateName ; };
    void setFederateName(std::string federate_name);

    std::string getTag() const { return tag ; };
    void setTag(std::string new_tag);

    SpaceHandle getSpace() const { return space ; };
    void setSpace(SpaceHandle);

    Type getType() const { return type ; };
    void setType(Type);

    unsigned long getNumber() const { return number ; };
    void setNumber(unsigned long);

    long getRegion() const { return region ; };
    void setRegion(long);

    RTI::TickTime getMinTickTime() const { return minTickTime ; };
    void setMinTickTime(RTI::TickTime);

    RTI::TickTime getMaxTickTime() const { return maxTickTime ; };
    void setMaxTickTime(RTI::TickTime);

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

    void setResignAction(RTI::ResignAction);
    RTI::ResignAction getResignAction() const { return resignAction ; };

    void setFedTime(const double);
    double getFedTime() const { return fed_time.getTime(); };

    void setLookahead(const double);

    void setFederationTime(FederationTime);
    FederationTime getFederationTime() const { return fed_time.getTime(); };

    void setBoolean(bool);
    bool getBoolean() const { return boolean ; };

    void setObject(ObjectHandle);
    ObjectHandle getObject() const { return object ; };

    void setTransportation(TransportType);
    TransportType getTransportation() const
    { return transport; }

    void setOrdering(OrderType);
    OrderType getOrdering() const
    { return order; }

    void setEventRetraction(EventRetractionHandle);
    EventRetractionHandle getEventRetraction() const
    { return eventRetraction ; };

    void setParameter(ParameterHandle);
    ParameterHandle getParameter() const { return parameter ; };

    void setFederate(FederateHandle);
    FederateHandle getFederate() const { return federate ; };

    const std::vector<AttributeHandle>& getAHS() const ;
    void setAHS(const std::vector<AttributeHandle> &);
    void setAHS(const AttributeHandle *, int);

    std::vector<std::pair<AttributeHandle, AttributeValue_t> > getAHVPS() const ;
    void setAHVPS(const std::vector<std::pair<AttributeHandle, AttributeValue_t> > &);

    std::vector<std::pair<ParameterHandle, ParameterValue_t> > getPHVPS() const ;
    void setPHVPS(const std::vector<std::pair<ParameterHandle, ParameterValue_t> > &);

    void setAttributes(std::vector <AttributeHandle> &, ushort);
    void setAttributes(std::vector <AttributeHandle> &, std::vector <AttributeValue_t> &, ushort);
    void setParameters(std::vector <ParameterHandle> &, std::vector <ParameterValue_t> &, ushort);

    void setException(TypeException, const std::string& the_reason = "");
    TypeException getExceptionType() const { return exception ; };
    const char *getExceptionReason() const { return exceptionReason.c_str() ; };

    std::string getFEDid() const { return FEDid ; };
    void setFEDid(std::string FEDid);

        void displayvalueArray(char *);

    // Public attributes
    Type type ;

protected:
    TypeException exception ;
    std::string exceptionReason;

    FedTime fed_time;
    bool boolean ;
    FederationTimeDelta lookahead ;
    FederateHandle federate ;
    RTI::ResignAction resignAction ;
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
    RTI::TickTime minTickTime;
    RTI::TickTime maxTickTime;

public:

    // used for both Attributes and Parameters arrays.
    UShort handleArraySize ;
    std::vector <AttributeHandle> handleArray ;
    std::vector <AttributeValue_t> valueArray ;

    Message &operator=(const Message &);

    void display(char *);

private:

    // Read a Message Body from a Socket. Should be called after
    // ReadHeader.
    void readBody(MessageBuffer &msgBuffer);

    // Read a Header (buffer beginning)
    void readHeader(MessageBuffer &msgBuffer);

    // The message is written onto the socket by WriteHeader if no
    // body is required, or by WriteBody is a body has been required
    // by WriteHeader.

    // Prepare and write a Body to a socket. Should be called after
    // WriteHeader.
    void writeBody(MessageBuffer &msgBuffer);

    // Write a Header (buffer beginning)
    void writeHeader(MessageBuffer &msgBuffer);

    // -- Other Private Read Methods --
    void readHandleArray(MessageBuffer &msgBuffer);
    void readLabel(MessageBuffer &msgBuffer);
    void readName(MessageBuffer &msgBuffer);
    void readFederationName(MessageBuffer &msgBuffer);
    void readFederateName(MessageBuffer &msgBuffer);
    void readResignAction(MessageBuffer &msgBuffer);
    void readTag(MessageBuffer &msgBuffer);
    void readValueArray(MessageBuffer &msgBuffer);
    void readFEDid(MessageBuffer &msgBuffer);

    // -- Other Private Write Methods --
    void writeHandleArray(MessageBuffer &msgBuffer);
    void writeResignAction(MessageBuffer &msgBuffer);
    void writeValueArray(MessageBuffer &msgBuffer);
    void writeFEDid(MessageBuffer &msgBuffer);
    void writeFederationName(MessageBuffer &msgBuffer);

    MessageHeader header ;
    std::string label ;
    std::string name ;
    std::string  federateName ;
    std::string federationName ;

    std::string tag ;
    std::string FEDid ;
};

} // namespace certi

#endif // CERTI_MESSAGE_HH
