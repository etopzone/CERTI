// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
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
//
// ----------------------------------------------------------------------------

#include "NetworkMessage.hh"
#include "PrettyDebug.hh"

using std::vector;
using std::endl;

namespace certi {

static PrettyDebug D("RTIG_MSG", "NetworkMessage:");
static PrettyDebug G("GENDOC", __FILE__);

// ----------------------------------------------------------------------------
void NetworkMessage::serialize(MessageBuffer& msgBuffer)
{
    Debug(G, pdGendoc) << "enter NetworkMessage::serialize" << std::endl;
    /* We serialize the common Network messages part
	 * ALL Network Message will contain the following
	 */
    if ((type == Type::NOT_USED) || (type == Type::LAST)) {
        throw RTIinternalError("Invalid network type (not a valid type);");
    }
    Debug(D, pdDebug) << "Serialize <" << getMessageName() << ">" << std::endl;
    /* type of message */
    msgBuffer.write_int32(static_cast<std::underlying_type<Type>::type>(type));
    msgBuffer.write_uint32(federate);
    msgBuffer.write_uint32(federation);
    msgBuffer.write_int32(static_cast<int32_t>(exception));
    if (exception != Exception::Type::NO_EXCEPTION) {
        msgBuffer.write_string(exceptionReason);
    }
    else {
        BasicMessage::serialize(msgBuffer);
    }
    Debug(G, pdGendoc) << "exit NetworkMessage::serialize" << std::endl;
} /* end of serialize */

void NetworkMessage::deserialize(MessageBuffer& msgBuffer)
{
    Debug(G, pdGendoc) << "enter NetworkMessage::deserialize" << std::endl;
    /* We serialize the common Network message part
	 * ALL Network Messages will contain the following
	 */
    Debug(D, pdDebug) << "Deserialize <" << getMessageName() << ">" << endl;
    /* deserialize common part */
    type = static_cast<NetworkMessage::Type>(msgBuffer.read_int32());
    federate = msgBuffer.read_uint32();
    federation = msgBuffer.read_uint32();
    exception = static_cast<Exception::Type>(msgBuffer.read_int32());
    if (exception != Exception::Type::NO_EXCEPTION) {
        msgBuffer.read_string(exceptionReason);
    }
    else {
        BasicMessage::deserialize(msgBuffer);
    }
    Debug(G, pdGendoc) << "exit NetworkMessage::deserialize" << std::endl;
} /* end of deserialize */

void NetworkMessage::send(Socket* socket, MessageBuffer& msgBuffer)
{
    Debug(G, pdGendoc) << "enter NetworkMessage::send" << std::endl;
    /* 0- reset send buffer */
    msgBuffer.reset();
    /* 1- serialize the message
     * This is a polymorphic call
     * which may specialized in a daughter class
     */
    serialize(msgBuffer);
    /* 2- update message buffer 'reserved bytes' header */
    msgBuffer.updateReservedBytes();
    Debug(D, pdDebug) << "Sending <" << getMessageName() << "> whose buffer has <" << msgBuffer.size() << "> bytes"
                      << std::endl;
    //msgBuffer.show(msgBuf(0),5);
    /* 3- effectively send the raw message to socket */

    if (NULL != socket) { // send only if socket is unequal to null
        socket->send(static_cast<unsigned char*>(msgBuffer(0)), msgBuffer.size());
    }
    else { // socket pointer was null - not sending
        Debug(D, pdDebug) << "Not sending -- socket is deleted." << std::endl;
    }
    Debug(G, pdGendoc) << "exit  NetworkMessage::send" << std::endl;
} /* end of send */

void NetworkMessage::send(std::vector<Socket*> sockets, MessageBuffer& msgBuffer)
{
    Debug(G, pdGendoc) << "enter NetworkMessage::send" << std::endl;
    /* 0- reset send buffer */
    msgBuffer.reset();
    /* 1- serialize the message
     * This is a polymorphic call
     * which may specialized in a daughter class
     */
    serialize(msgBuffer);
    /* 2- update message buffer 'reserved bytes' header */
    msgBuffer.updateReservedBytes();
    Debug(D, pdDebug) << "Sending <" << getMessageName() << "> whose buffer has <" << msgBuffer.size() << "> bytes"
                      << std::endl;
    //msgBuffer.show(msgBuf(0),5);
    /* 3- effectively send the raw message to socket */

    for (const auto& socket : sockets) {
        if (socket) { // send only if socket is unequal to null
            socket->send(static_cast<unsigned char*>(msgBuffer(0)), msgBuffer.size());
        }
    }
    Debug(G, pdGendoc) << "exit  NetworkMessage::send" << std::endl;
} /* end of send */

void NetworkMessage::receive(Socket* socket, MessageBuffer& msgBuffer)
{
    Debug(G, pdGendoc) << "enter NetworkMessage::receive" << std::endl;
    /* 0- Reset receive buffer */
    /* FIXME this reset may not be necessary since we do
	 * raw-receive + assume-size
	 */
    msgBuffer.reset();
    /* 1- Read 'reserved bytes' header from socket */
    //D.Out(pdDebug,"Reading %d 'reserved' bytes",msgBuffer.reservedBytes);
    socket->receive(msgBuffer(0), msgBuffer.reservedBytes);
    //msgBuffer.show(msgBuf(0),5);fflush(stdout);
    /* 2- update (assume) complete message size from reserved bytes */
    msgBuffer.assumeSizeFromReservedBytes();
    Debug(D, pdDebug) << "Got a MsgBuf of size " << msgBuffer.size() << " bytes (including " << msgBuffer.reservedBytes
                      << " reserved)" << std::endl;
    /* 3- receive the rest of the message */
    socket->receive(msgBuffer(msgBuffer.reservedBytes), msgBuffer.size() - msgBuffer.reservedBytes);
    /* 4- deserialize the message
	 * This is a polymorphic call
	 * which may specialized in a daughter class
	 */
    deserialize(msgBuffer);
    Debug(G, pdGendoc) << "exit  NetworkMessage::receive" << std::endl;
} /* end of receive */

} // namespace certi
