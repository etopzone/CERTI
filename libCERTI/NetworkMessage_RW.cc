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
// $Id: NetworkMessage_RW.cc,v 3.45.2.5 2008/04/10 11:35:56 erk Exp $
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
void NetworkMessage::serialize() {
	G.Out(pdGendoc,"enter NetworkMessage::serialize");
	/* We serialize the common Network messages part 
	 * ALL Network Message will contain the following
	 */	
	D.Out(pdDebug, "serialize <%s>", getName().c_str());
	/* type of message */
	msgBuf.write_int32(type);
	msgBuf.write_int32(exception);
	msgBuf.write_int32(federate);
	msgBuf.write_int32(federation);
	/*
	 * "builtin" Optional part
	 * The subclass may chose in the constructor
	 * the variable part.
	 */
	if (isDated) {
		msgBuf.write_double(date);
	}
	if (isLabelled) {
		msgBuf.write_string(label);
	}
	if (isTagged) {
		msgBuf.write_string(tag);
	}
	G.Out(pdGendoc,"exit NetworkMessage::serialize");
} /* end of serialize */

void NetworkMessage::deserialize() {
	G.Out(pdGendoc,"enter NetworkMessage::deserialize");
	/* We serialize the common Network message part 
	 * ALL Network Messages will contain the following
	 */	
	D[pdDebug] << "deserialize <" << getName().c_str()<<">"<<endl;	
	/* deserialize common part */
	type        = static_cast<certi::NetworkMessage::Type>(msgBuf.read_int32());
	exception   = static_cast<certi::TypeException>(msgBuf.read_int32());
	federate    = msgBuf.read_int32();
	federation  = msgBuf.read_int32();
	/*
	 * "builtin" Optional part
	 * The subclass may chose in the constructor
	 * the variable part.
	 */
	if (isDated) {
		date = msgBuf.read_double();
	}
	if (isLabelled) {
		label = msgBuf.read_string();
	}
	if (isTagged) {
		tag = msgBuf.read_string();
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
	serialize();
	/* 2- update message buffer 'reserved bytes' header */
	msgBuf.updateReservedBytes();
	D.Out(pdDebug,"[Header] Sending a buffer of <%u> bytes",msgBuf.size());
	//msgBuf.show(msgBuf(0),5);
	/* 3- effectively send the raw message to socket */
	socket->send(static_cast<unsigned char*>(msgBuf(0)), msgBuf.size());
	G.Out(pdGendoc,"exit  NetworkMessage::send");
} /* end of send */

void
NetworkMessage::receive(Socket* socket) throw (NetworkError, NetworkSignal) {
	G.Out(pdGendoc,"enter NetworkMessage::receive");
	/* 0- Reset receive buffer */
	/* FIXME this reset may not be necessary since we 
	 * do raw-receive + assume-size
	 */
	msgBuf.reset();
	/* 1- Read 'reserved bytes' header from socket */
	D.Out(pdDebug,"reading %d bytes for header",msgBuf.reservedBytes);
	socket->receive(msgBuf(0), msgBuf.reservedBytes);	
	//msgBuf.show(msgBuf(0),5);
	fflush(stdout);
	/* 2- update (assume) complete message size from reserved bytes */
	msgBuf.assumeSizeFromReservedBytes();
	D.Out(pdDebug,"Got a MsgBuf of size %d bytes (including %d reserved)",msgBuf.size(),msgBuf.reservedBytes);
	/* 3- receive the rest of the message */
	socket->receive(msgBuf(msgBuf.reservedBytes),msgBuf.size()-msgBuf.reservedBytes);
	/* 4- deserialize the message 
	 * This is a polymorphic call 
	 * which may specialized in a daughter class  
	 */ 
	deserialize();
	G.Out(pdGendoc,"exit  NetworkMessage::receive");	
} /* end of receive */

} // namespace certi

// $Id: NetworkMessage_RW.cc,v 3.45.2.5 2008/04/10 11:35:56 erk Exp $
