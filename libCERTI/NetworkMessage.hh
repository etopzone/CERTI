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
// the License, or (at your option) Any later version.
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
// $Id: NetworkMessage.hh,v 3.2 2003/02/17 09:17:03 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_NETWORK_MESSAGE
#define _CERTI_NETWORK_MESSAGE

#include <stdio.h>
#include <cstring>
#include "baseTypes.hh"
#include "RTItypes.hh"
#include "Exception.hh"
#include "PrettyDebug.hh"
#include "Socket.hh"
#include "MessageBody.hh"

#ifdef FEDERATION_USES_MULTICAST
#define MC_PORT 60123
#define ADRESSE_MULTICAST "224.0.0.251"
#endif // FEDERATION_USES_MULTICAST

#define PORT_TCP_RTIG "60400"
#define PORT_UDP_RTIG "60500"

#define MAX_EXCEPTION_REASON_LENGTH 100

namespace certi {

typedef enum {
    m_NOT_TYPED = 0, // Not used.
    m_CLOSE_CONNEXION,
    m_MESSAGE_NULL,
    m_CREATE_FEDERATION_EXECUTION,
    m_DESTROY_FEDERATION_EXECUTION,
    m_JOIN_FEDERATION_EXECUTION,
    m_RESIGN_FEDERATION_EXECUTION,
    m_SET_TIME_REGULATING,
    m_SET_TIME_CONSTRAINED,
    m_REQUEST_PAUSE,
    m_REQUEST_RESUME,
    m_PUBLISH_OBJECT_CLASS,
    m_UNPUBLISH_OBJECT_CLASS,
    m_PUBLISH_INTERACTION_CLASS,
    m_UNPUBLISH_INTERACTION_CLASS,
    m_SUBSCRIBE_OBJECT_CLASS,
    m_UNSUBSCRIBE_OBJECT_CLASS,
    m_SUBSCRIBE_INTERACTION_CLASS,
    m_UNSUBSCRIBE_INTERACTION_CLASS,
    m_TURN_INTERACTIONS_ON, // Uniquement RTIG->RTIA
    m_TURN_INTERACTIONS_OFF, // Uniquement RTIG->RTIA
    m_REQUEST_ID,
    m_REGISTER_OBJECT,
    m_DISCOVER_OBJECT, // Uniquement RTIG->RTIA
    m_UPDATE_ATTRIBUTE_VALUES,
    m_REFLECT_ATTRIBUTE_VALUES, // Uniquement RTIG->RTIA
    m_SEND_INTERACTION,
    m_RECEIVE_INTERACTION, // Uniquement RTIG->RTIA
    m_DELETE_OBJECT,
    m_REMOVE_OBJECT, // Uniquement RTIG->RTIA
    m_CHANGE_ATTRIBUTE_TRANSPORT_TYPE,
    m_CHANGE_ATTRIBUTE_ORDER_TYPE,
    m_CHANGE_INTERACTION_TRANSPORT_TYPE,
    m_CHANGE_INTERACTION_ORDER_TYPE,
    m_REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE,
    m_REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE,
    m_IS_ATTRIBUTE_OWNED_BY_FEDERATE,
    m_QUERY_ATTRIBUTE_OWNERSHIP,
    m_ATTRIBUTE_IS_NOT_OWNED,
    m_INFORM_ATTRIBUTE_OWNERSHIP,
    m_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
    m_ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION,
    m_ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION,
    m_REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION,
    m_ATTRIBUTE_OWNERSHIP_UNAVAILABLE,
    m_ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE,
    m_UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
    m_ATTRIBUTE_OWNERSHIP_ACQUISITION,
    m_REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE,
    m_CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
    m_ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE,
    m_CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION,
    m_CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION
} TypeNetworkMessage ;

typedef struct {
    FederationTime date ; // Date, Logical Time
    Boolean R_or_C ; // IsRegulator or IsConstrained
} TimeStruct ;

typedef struct {
    ObjectHandlecount count ;
    ObjectHandle first ;
    ObjectHandle last ;
} ReqIDStruct ;

typedef struct {
    ObjectClassHandle handle ;
    UShort handleArraySize ;
    TransportType transport ;
    OrderType order ;
} T_O_Struct ;

typedef struct {
    int NbReg ;
    unsigned long AdrMC ;
    unsigned long Addr ;
    unsigned int peer ;
} JoinStruct ;

typedef struct {
    ObjectClassHandle handle ;
    UShort size ;
    FederationTime date ;
} O_I_Struct ;


typedef union {
    TimeStruct time ; // Les noms des variables n'ont pas d'importance
    ReqIDStruct ReqID ; // mais ils doivent etre presents.
    T_O_Struct T_O ;
    JoinStruct Join ;
    O_I_Struct O_I ;
} HeaderUnion ;

typedef struct {
    TypeNetworkMessage type ;
    TypeException exception ;
    FederationHandle federation       ;
    FederateHandle federate ;
    UShort bodySize ;
    HeaderUnion VP ; // Variable Part
} HeaderStruct ;

class NetworkMessage
{
public :

    // --------------------
    // -- Public Methods --
    // --------------------

    // Constructor & Destructor
    NetworkMessage();
    ~NetworkMessage();


    // Display method
    void display(const char *);

    // Parameter and Attribute Management

    // Remove the Parameter of rank 'Rank' in the ParamArray and its value in
    // ValueArray. If there are other parameter in the list, they are shifted
    // one rank back.
    // Ex: ParamArraySize = 3,
    // ParamArray =[1, 2, 3], ValueArray =["Val1", "Val2", "Val3"]
    //->removeParameter(1)
    // ParamArraySize = 2,
    // ParamArray =[1, 3], ValueArray =["Val1", "Val3"]
    void removeParameter(UShort Rank);

    // See RemoveParameter for explanations.
    void removeAttribute(UShort Rank);

    // ----------------------------
    // -- Network Public Methods --(cf. NetworkMessage_RW.cc)
    // ----------------------------

    // Read and Write NetworkMessage Objects to and from Socket objects.

    void write(Socket *Socket)
        throw (NetworkError,
               NetworkSignal);

    void read(Socket *Socket)
        throw (NetworkError,
               NetworkSignal);

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

    // Return a newly allocated ValueArray, exactly of size AttribArraySize.
    // containing the actual Attribute values. You must FREE this structure.
    AttributeValue *getAttribValueArray();

    // Return a newly allocated ValueArray, exactly of size ParamArraySize,
    // containing the actual Parameter values. You must FREE this structure.
    ParameterValue *getParamValueArray();

    // -----------------------
    // -- Public Attributes --
    // -----------------------

    TypeNetworkMessage type ;
    TypeException exception ;

    UShort number ;

    FederationHandle federation ;
    FederateHandle federate ;
    char federationName[MAX_FEDERATION_NAME_LENGTH + 1] ;
    char federateName[MAX_FEDERATE_NAME_LENGTH + 1] ;

    int bestEffortPeer ;
    unsigned long bestEffortAddress ;

    Boolean regulator ;
    Boolean constrained ;

    FederationTime date ;

    int numberOfRegulators ;
    unsigned long multicastAddress ;

    char label[MAX_USER_TAG_LENGTH + 1] ;

    ObjectHandlecount idCount ;
    ObjectHandle firstId ;
    ObjectHandle lastId ;

    ObjectHandle object ;
    ObjectClassHandle objectClass ;
    InteractionClassHandle interactionClass ;

    EventRetractionHandle eventRetraction ;

    UShort handleArraySize ;
    AttributeHandle handleArray[MAX_ATTRIBUTES_PER_CLASS] ;

    TransportType transport ;
    OrderType order ;

private:

    // ------------------------
    // -- Private Attributes --
    // ------------------------

    HeaderStruct Header ;
    AttributeValue ValueArray[MAX_ATTRIBUTES_PER_CLASS] ;

    // ---------------------
    // -- Private Methods --(cf. NetworkMessage_RW.cc)
    // ---------------------

    // Read a Message Body from a Socket. Should be called after ReadHeader.
    void readBody(Socket *Socket);

    // Read a Header from a socket, and process it to read its content.
    // Return RTI_TRUE if the ReadBody Method has to be called.
    Boolean readHeader(Socket *Socket);

    // The message is written onto the socket by WriteHeader if no body
    // is required, or by WriteBody is a body has been required by WriteHeader.

    // Prepare and write a Body to a socket. Should be called after
    // WriteHeader.
    void writeBody(Socket *Socket);

    // Prepare and Write a Header to a Socket, and return RTI_TRUE
    // if the WriteBody method has to be called.
    Boolean writeHeader(Socket *Socket);

    // -- Others Private Read Methods --

    void readLabel(MessageBody *Body);
    void readFederationName(MessageBody *Body);
    void readFederateName(MessageBody *Body);

};

#define TAILLE_MSG_RESEAU sizeof(NetworkMessage)

}

#endif // _CERTI_NETWORK_MESSAGE

// $Id: NetworkMessage.hh,v 3.2 2003/02/17 09:17:03 breholee Exp $
