// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2006  ONERA
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// ----------------------------------------------------------------------------

#include "Message.hh"
#include <cassert>
#include <iostream>

namespace certi {

static PrettyDebug D("RTIA_MSG","Message::");
static PrettyDebug G("GENDOC",__FILE__);

std::ostream& Message::show(std::ostream& out) {
	out << "[Message -Begin]" << std::endl;
	out << "     type = " << type << std::endl;
	out << "     exception = " << exception << std::endl;
	if (exception != e_NO_EXCEPTION) {
		out << "     exceptionReason = " << exceptionReason << std::endl;
	} else {
		BasicMessage::show(out);
	}
	out << "[Message -End]" << std::endl;
	return out;
} /* end of show */

void Message::serialize(MessageBuffer& msgBuffer) {
	G.Out(pdGendoc,"enter Message::serialize");
	if ((type==NOT_USED) || (type==LAST)) {
		throw RTIinternalError("Invalid network type (not a valid type);");
	}
	D.Out(pdDebug, "Serialize <%s>", getMessageName());
	//show(std::cerr);
	/* type of message */
	msgBuffer.write_int32(type);
	msgBuffer.write_int32(exception);
	if (exception != e_NO_EXCEPTION) {
		msgBuffer.write_string(exceptionReason);
	} else {
		BasicMessage::serialize(msgBuffer);
	}
	G.Out(pdGendoc,"exit Message::serialize");
} /* end of serialize */

void Message::deserialize(MessageBuffer& msgBuffer) {
	G.Out(pdGendoc,"enter Message::deserialize");
	/* We serialize the common Message part
	 * ALL Messages will contain the following
	 */
	Debug(D, pdDebug) << "Deserialize <" << getMessageName()<<">"<<std::endl;
	/* deserialize common part */
	type        = static_cast<Message::Type>(msgBuffer.read_int32());
	exception   = static_cast<TypeException>(msgBuffer.read_int32());
	if (exception != e_NO_EXCEPTION) {
		msgBuffer.read_string(exceptionReason);
	} else {
		BasicMessage::deserialize(msgBuffer);
	}
	G.Out(pdGendoc,"exit Message::deserialize");
} /* end of deserialize */

void
Message::send(SocketUN *socket, MessageBuffer &msgBuffer) throw (NetworkError, NetworkSignal) {
	G.Out(pdGendoc,"enter Message::send");
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
	//msgBuf.show(msgBuffer(0),5);
	/* 3- effectively send the raw message to socket */
	socket->send(static_cast<unsigned char*>(msgBuffer(0)), msgBuffer.size());
	G.Out(pdGendoc,"exit  Message::send");
} /* end of send */

void
Message::receive(SocketUN* socket, MessageBuffer &msgBuffer) throw (NetworkError, NetworkSignal) {
	G.Out(pdGendoc,"enter Message::receive");
	/* 0- Reset receive buffer */
	/* FIXME this reset may not be necessary since we do
	 * raw-receive + assume-size
	 */
	msgBuffer.reset();
	/* 1- Read 'reserved bytes' header from socket */
	D.Out(pdDebug,"(recv) Reading %d 'reserved' bytes",msgBuffer.reservedBytes);
	socket->receive(static_cast<const unsigned char *>(msgBuffer(0)), msgBuffer.reservedBytes);
	//msgBuffer.show(msgBuffer(0),5);fflush(stdout);
	/* 2- update (assume) complete message size from reserved bytes */
	msgBuffer.assumeSizeFromReservedBytes();
	D.Out(pdDebug,"Got a MsgBuffer of size %d bytes (including %d reserved)",msgBuffer.size(),msgBuffer.reservedBytes);
	/* 3- receive the rest of the message */
	socket->receive(static_cast<const unsigned char *>(msgBuffer(msgBuffer.reservedBytes)),msgBuffer.size()-msgBuffer.reservedBytes);
	/* 4- deserialize the message
	 * This is a polymorphic call
	 * which may specialized in a daughter class
	 */
	deserialize(msgBuffer);
	G.Out(pdGendoc,"exit  Message::receive");
} /* end of receive */

/*
void
Message::readHandleArray(MessageBuffer &msgBuffer)
{
    handleArraySize = msgBuffer.read_uint16() ;
    handleArray.resize(handleArraySize) ;
    for ( short i=0 ; i<handleArraySize ; i++)
      {
      handleArray[i] = msgBuffer.read_uint64() ;
      }
}

void
Message::writeHandleArray(MessageBuffer &msgBuffer)
{
    msgBuffer.write_uint16(handleArraySize) ;
    for ( short i=0 ; i<handleArraySize ; i++)
      {
      msgBuffer.write_uint64(handleArray[i]) ;
      }
}
 */

} // namespace certi
