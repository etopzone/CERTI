// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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
// $Id: Message.hh,v 3.10 2003/03/21 15:06:46 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_MESSAGE_HH
#define _CERTI_MESSAGE_HH

#include <stdio.h>
#include <cstring>
#include "baseTypes.hh"
#include "RTItypes.hh"
#include "SocketUN.hh"
#include "MessageBody.hh"
#include "Exception.hh"
#include "PrettyDebug.hh"

namespace certi {

typedef enum {

    // gestion federation
    CREATE_FEDERATION_EXECUTION = 1,
    DESTROY_FEDERATION_EXECUTION,
    JOIN_FEDERATION_EXECUTION,
    RESIGN_FEDERATION_EXECUTION,
    REGISTER_FEDERATION_SYNCHRONIZATION_POINT,
    SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED,
    ANNOUNCE_SYNCHRONIZATION_POINT,
    SYNCHRONIZATION_POINT_ACHIEVED,
    FEDERATION_SYNCHRONIZED,
    REQUEST_FEDERATION_SAVE,
    INITIATE_FEDERATE_SAVE,
    FEDERATE_SAVE_BEGUN,
    FEDERATE_SAVE_ACHIEVED,
    FEDERATE_SAVE_NOT_ACHIEVED,
    REQUEST_RESTORE,
    INITIATE_RESTORE,
    RESTORE_ACHIEVED,
    RESTORE_NOT_ACHIEVED,

    // Declaration
    PUBLISH_OBJECT_CLASS,
    UNPUBLISH_OBJECT_CLASS,
    PUBLISH_INTERACTION_CLASS,
    UNPUBLISH_INTERACTION_CLASS,
    SUBSCRIBE_OBJECT_CLASS_ATTRIBUTE,
    UNSUBSCRIBE_OBJECT_CLASS_ATTRIBUTE,
    SUBSCRIBE_INTERACTION_CLASS,
    UNSUBSCRIBE_INTERACTION_CLASS,
    START_REGISTRATION_FOR_OBJECT_CLASS,
    STOP_REGISTRATION_FOR_OBJECT_CLASS,
    TURN_INTERACTIONS_ON,
    TURN_INTERACTIONS_OFF,

    // Object
    REQUEST_ID,
    REGISTER_OBJECT,
    UPDATE_ATTRIBUTE_VALUES,
    DISCOVER_OBJECT,
    REFLECT_ATTRIBUTE_VALUES,
    SEND_INTERACTION,
    RECEIVE_INTERACTION,
    DELETE_OBJECT,
    REMOVE_OBJECT,
    CHANGE_ATTRIBUTE_TRANSPORT_TYPE,
    CHANGE_ATTRIBUTE_ORDER_TYPE,
    CHANGE_INTERACTION_TRANSPORT_TYPE,
    CHANGE_INTERACTION_ORDER_TYPE,
    REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE,
    REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE,
    PROVIDE_ATTRIBUTE_VALUE_UPDATE,
    RETRACT,
    REFLECT_RETRACTION,

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
    ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE,
    ATTRIBUTE_OWNERSHIP_UNAVAILABLE,
    UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
    ATTRIBUTE_OWNERSHIP_ACQUISITION,
    CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
    ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE,
    CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION,
    CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION,

    // Time
    REQUEST_FEDERATION_TIME,
    REQUEST_LBTS,
    REQUEST_FEDERATE_TIME,
    REQUEST_MIN_NEXT_EVENT_TIME,
    SET_LOOKAHEAD,
    REQUEST_LOOKAHEAD,
    TIME_ADVANCE_REQUEST,
    NEXT_EVENT_REQUEST,
    FLUSH_QUEUE_REQUEST,
    TIME_ADVANCE_GRANT,

    // Data Distribution
    CREATE_UPDATE_REGION,
    CREATE_SUBSCRIPTION_REGION,
    ASSOCIATE_UPDATE_REGION,
    DISASSOCIATE_UPDATE_REGION,
    CHANGE_THRESHOLDS,
    MODIFY_REGION,
    DELETE_REGION,

    // Support Services
    GET_OBJECT_CLASS_HANDLE,
    GET_OBJECT_CLASS_NAME,
    GET_ATTRIBUTE_HANDLE,
    GET_ATTRIBUTE_NAME,
    GET_INTERACTION_CLASS_HANDLE,
    GET_INTERACTION_CLASS_NAME,
    GET_PARAMETER_HANDLE,
    GET_PARAMETER_NAME,
    GET_SPACE_HANDLE,
    GET_SPACE_NAME,
    GET_DIMENSION_HANDLE,
    GET_DIMENSION_NAME,
    GET_FEDERATE_HANDLE,
    GET_FEDERATE_NAME,
    SET_TIME_REGULATING,
    SET_TIME_CONSTRAINED,
    TICK_REQUEST

} TypeService ;

typedef struct {
    FederationTime date ; // Date, Logical Time, Lookahead, etc.
    Boolean mode ; // IsRegulator or IsConstrained
} MessageTimeStruct ;

typedef struct {
    ObjectHandlecount count ;
    ObjectHandle first ;
    ObjectHandle last ;
} MessageReqIDStruct ;

typedef struct {
    ObjectClassHandle handle ;
    UShort size ;
    TransportType transport ;
    OrderType order ;
} MessageT_O_Struct ;

typedef struct {
    FederateHandle federate ; // Join
    ResignAction action ; // Resign
} MessageJ_R_Struct ;

typedef struct {
    ObjectClassHandle handle ;
    UShort size ;
    FederationTime date ;
} MessageO_I_Struct ;

typedef union {
    MessageTimeStruct time ;
    MessageReqIDStruct ReqID ;
    MessageT_O_Struct T_O ;
    MessageJ_R_Struct J_R ;
    MessageO_I_Struct O_I ;
} MessageHeaderUnion ;

// -- Structure de l'entete --
typedef struct {
    TypeService type ;
    TypeException exception ;
    UShort bodySize ;
    MessageHeaderUnion VP ; // Variable Part
} MessageHeader ;

/*! The Message class is used to formalize messages that are going to be
    exchanged between the RTI and the federate.
*/
class Message
{
    // Construction, Destruction
public:
    // --------------------
    // -- Public Methods --
    // --------------------

    // Constructor & Destructor
    Message();
    ~Message();

    // ----------------------------
    // -- Network Public Methods --(cf. Message_RW.cc)
    // ----------------------------

    // Read and Write NetworkMessage Objects to and from Socket objects.
    void write(SocketUN *Socket) throw (NetworkError, NetworkSignal);
    void read(SocketUN *Socket) throw (NetworkError, NetworkSignal);

    // ------------------------------
    // -- Attribute Access Methods --
    // ------------------------------

    // Value Array Management
    void setValue(int Rank, const char *Value)
        throw (RTIinternalError); // Bad Rank, Bad Value

    // If Value == NULL return a newly allocated copy of Value, else copy it
    // in Value.
    char *getValue(int Rank, char *Value = NULL)
        throw (RTIinternalError); // Bad Rank

    // Return a newly allocated ValueArray, exactly of size HandleArraySize.
    // containing the actual Attrib/Param values. You must FREE this structure.
    AttributeValue *getValueArray();

    // Strings Access

    // All the Set* methods check the length of the string, and throw
    // ValueLengthExceeded if the new string is too long.

    // All the Get* methods return a pointer on the actual stored value.
    // This pointer is READ-ONLY, and should not be de-allocated.

    const char *getLabel(void) const { return label ; };
    void setLabel(const char *new_label);

    const char *getName(void) const { return name ; };
    void setName(const char *new_name);

    DimensionHandle getDimension(void) const { return dimension ; };
    void setDimension(DimensionHandle);

    const char *getFederationName(void) const { return federationName ; };
    void setFederationName(const char *federation_name);

    const char *getFederateName(void) const { return federateName ; };
    void setFederateName(const char *federate_name);

    const char *getTag(void) const { return tag ; };
    void setTag(const char *new_tag);

    SpaceHandle getSpace(void) const { return space ; };
    void setSpace(SpaceHandle);

    // -----------------------
    // -- Public Attributes --
    // -----------------------

    TypeService type ;
    FederationTime date ;
    Boolean boolean ;
    FederationTimeDelta lookahead ;
    TypeException exception ;
    char exceptionReason[MAX_EXCEPTION_REASON_LENGTH + 1] ;
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
    TransportationHandle transportation ;
    OrderingHandle ordering ;

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
    Boolean readHeader(SocketUN *Socket);

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

// $Id: Message.hh,v 3.10 2003/03/21 15:06:46 breholee Exp $
