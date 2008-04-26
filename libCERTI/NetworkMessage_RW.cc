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
// $Id: NetworkMessage_RW.cc,v 3.47 2008/04/26 14:59:41 erk Exp $
// ----------------------------------------------------------------------------
#include "NetworkMessage.hh"
#include "PrettyDebug.hh"

using std::vector;
using std::endl;
using std::cout;

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
	D.Out(pdDebug, "Serialize <%s>", getName().c_str());
	/* type of message */
	msgBuffer.write_int32(type);
	msgBuffer.write_int32(exception);
	msgBuffer.write_int32(federate);
	msgBuffer.write_int32(federation);
	/*
	 * "builtin" Optional part
	 * The subclass may chose in the constructor the variable part.
	 * isDated may be chosen on Message instance basis
	 * (same message may Dated or Not Dated) 
	 */
	msgBuffer.write_bool(isDated);
	if (isDated) {
		msgBuffer.write_double(date);
	}	
	msgBuffer.write_bool(isLabelled);
	if (isLabelled) {
		msgBuffer.write_string(label);
	}
	msgBuffer.write_bool(isTagged);
	if (isTagged) {
		msgBuffer.write_string(tag);
	}
	G.Out(pdGendoc,"exit NetworkMessage::serialize");
} /* end of serialize */

void NetworkMessage::deserialize(MessageBuffer& msgBuffer) {
	G.Out(pdGendoc,"enter NetworkMessage::deserialize");
	/* We serialize the common Network message part 
	 * ALL Network Messages will contain the following
	 */	
	D[pdDebug] << "Deserialize <" << getName().c_str()<<">"<<endl;	
	/* deserialize common part */
	type        = static_cast<certi::NetworkMessage::Type>(msgBuffer.read_int32());
	exception   = static_cast<certi::TypeException>(msgBuffer.read_int32());
	federate    = msgBuffer.read_int32();
	federation  = msgBuffer.read_int32();
	/*
	 * "builtin" Optional part
	 * The subclass may chose in the constructor the variable part.
	 * isDated may be chosen on Message instance basis
	 * (same message may Dated or Not Dated) 
	 */
	isDated = msgBuffer.read_bool();
	if (isDated) {
		date = msgBuffer.read_double();
	}
	isLabelled = msgBuffer.read_bool();
	if (isLabelled) {
		label = msgBuffer.read_string();
	}
	isTagged = msgBuffer.read_bool();
	if (isTagged) {
		tag = msgBuffer.read_string();
	}
	G.Out(pdGendoc,"exit NetworkMessage::deserialize");
} /* end of deserialize */

void
NetworkMessage::send(Socket *socket) throw (NetworkError, NetworkSignal) {
	G.Out(pdGendoc,"enter NetworkMessage::send");
	/* 0- reset send buffer */
	msgBuf.reset();
	/* 1- serialize the message
	 * This is a polymorphic call 
	 * which may specialized in a daughter class  
	 */
	serialize(msgBuf);
	/* 2- update message buffer 'reserved bytes' header */
	msgBuf.updateReservedBytes();
	D.Out(pdDebug,"Sending <%s> whose buffer has <%u> bytes",getName().c_str(),msgBuf.size());
	//msgBuf.show(msgBuf(0),5);
	/* 3- effectively send the raw message to socket */
	socket->send(static_cast<unsigned char*>(msgBuf(0)), msgBuf.size());
	G.Out(pdGendoc,"exit  NetworkMessage::send");
} /* end of send */

void
NetworkMessage::receive(Socket* socket) throw (NetworkError, NetworkSignal) {
	G.Out(pdGendoc,"enter NetworkMessage::receive");
	/* 0- Reset receive buffer */
	/* FIXME this reset may not be necessary since we do 
	 * raw-receive + assume-size
	 */
	msgBuf.reset();
	/* 1- Read 'reserved bytes' header from socket */
	//D.Out(pdDebug,"Reading %d 'reserved' bytes",msgBuf.reservedBytes);
	socket->receive(msgBuf(0), msgBuf.reservedBytes);	
	//msgBuf.show(msgBuf(0),5);fflush(stdout);
	/* 2- update (assume) complete message size from reserved bytes */
	msgBuf.assumeSizeFromReservedBytes();
	D.Out(pdDebug,"Got a MsgBuf of size %d bytes (including %d reserved)",msgBuf.size(),msgBuf.reservedBytes);
	/* 3- receive the rest of the message */
	socket->receive(msgBuf(msgBuf.reservedBytes),msgBuf.size()-msgBuf.reservedBytes);
	/* 4- deserialize the message 
	 * This is a polymorphic call 
	 * which may specialized in a daughter class  
	 */ 
	deserialize(msgBuf);
	G.Out(pdGendoc,"exit  NetworkMessage::receive");	
} /* end of receive */

} // namespace certi

// $Id: NetworkMessage_RW.cc,v 3.47 2008/04/26 14:59:41 erk Exp $
