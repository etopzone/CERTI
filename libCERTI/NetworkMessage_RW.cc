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
//
// ----------------------------------------------------------------------------

#include "NetworkMessage.hh"
#include "PrettyDebug.hh"

using std::vector;
using std::endl;

namespace certi {

static PrettyDebug D("RTIG_MSG", "NetworkMessage:");
static PrettyDebug G("GENDOC",__FILE__ );

// ----------------------------------------------------------------------------
void NetworkMessage::serialize(MessageBuffer& msgBuffer) {
	G.Out(pdGendoc,"enter NetworkMessage::serialize");
	/* We serialize the common Network messages part
	 * ALL Network Message will contain the following
	 */
	if ((type==NOT_USED) || (type==LAST)) {
		throw RTIinternalError("Invalid network type (not a valid type);");
	}
	D.Out(pdDebug, "Serialize <%s>", getMessageName());
	/* type of message */
	msgBuffer.write_int32(type);
	msgBuffer.write_uint32(federate);
	msgBuffer.write_uint32(federation);
	msgBuffer.write_int32(exception);
	if (exception != e_NO_EXCEPTION) {
		msgBuffer.write_string(exceptionReason);
	} else {
		BasicMessage::serialize(msgBuffer);
	}
	G.Out(pdGendoc,"exit NetworkMessage::serialize");
} /* end of serialize */

void NetworkMessage::deserialize(MessageBuffer& msgBuffer) {
	G.Out(pdGendoc,"enter NetworkMessage::deserialize");
	/* We serialize the common Network message part
	 * ALL Network Messages will contain the following
	 */
	Debug(D, pdDebug) << "Deserialize <" << getMessageName() << ">" << endl;
	/* deserialize common part */
	type        = static_cast<NetworkMessage::Type>(msgBuffer.read_int32());
	federate    = msgBuffer.read_uint32();
	federation  = msgBuffer.read_uint32();
	exception   = static_cast<TypeException>(msgBuffer.read_int32());
	if (exception != e_NO_EXCEPTION) {
			msgBuffer.read_string(exceptionReason);
	} else {
			BasicMessage::deserialize(msgBuffer);
	}
	G.Out(pdGendoc,"exit NetworkMessage::deserialize");
} /* end of deserialize */

void
NetworkMessage::send(Socket *socket, MessageBuffer& msgBuffer) throw (NetworkError, NetworkSignal){
	G.Out(pdGendoc,"enter NetworkMessage::send");
	/* 0- reset send buffer */
	msgBuffer.reset();
	/* 1- serialize the message
	 * This is a polymorphic call
	 * which may specialized in a daughter class
	 */
	serialize(msgBuffer);
	/* 2- update message buffer 'reserved bytes' header */
	msgBuffer.updateReservedBytes();
	D.Out(pdDebug,"Sending <%s> whose buffer has <%u> bytes",getMessageName(),msgBuffer.size());
	//msgBuffer.show(msgBuf(0),5);
	/* 3- effectively send the raw message to socket */

	if (NULL != socket) { // send only if socket is unequal to null
		socket->send(static_cast<unsigned char*>(msgBuffer(0)), msgBuffer.size());
	} else { // socket pointer was null - not sending
		D.Out( pdDebug, "Not sending -- socket is deleted." );
	}
	G.Out(pdGendoc,"exit  NetworkMessage::send");
} /* end of send */

void
NetworkMessage::receive(Socket* socket, MessageBuffer& msgBuffer) throw (NetworkError, NetworkSignal) {
	G.Out(pdGendoc,"enter NetworkMessage::receive");
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
	D.Out(pdDebug,"Got a MsgBuf of size %d bytes (including %d reserved)",msgBuffer.size(),msgBuffer.reservedBytes);
	/* 3- receive the rest of the message */
	socket->receive(msgBuffer(msgBuffer.reservedBytes),msgBuffer.size()-msgBuffer.reservedBytes);
	/* 4- deserialize the message
	 * This is a polymorphic call
	 * which may specialized in a daughter class
	 */
	deserialize(msgBuffer);
	G.Out(pdGendoc,"exit  NetworkMessage::receive");
} /* end of receive */

} // namespace certi
