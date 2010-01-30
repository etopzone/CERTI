
// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2008  ONERA
//
// This program is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) Any later version.
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
// CERTI Messages description
// ----------------------------------------------------------------------------
// The messages related classes will be placed in the certi package
package certi

// Message is the base class
// for message exchanged between RTIA and Federate (libRTI)
// AKA CERTI Message
native Message {
    language CXX    [#include "Message.hh"]
    language Python [import certi.Message]
    language Java   [import certi.Message]
}

message M_Create_Federation_Execution : merge Message {

}

message M_Destroy_Federation_Execution : merge Message {

}

message M_Join_Federation_Execution : merge Message {

}

message M_Resign_Federation_Execution : merge Message {

}



message M_Enable_Time_Regulation {
    required double fedTime
    required double lookahead
}

message M_Disable_Time_Regulation { }

message M_Enable_Time_Constrained { }

message M_Disable_Time_Constrained { }

native StreamType {
    language CXX [typedef Socket* StreamType;]
}

native M_Type {
    language CXX [typedef Message::Type M_Type;]
}

factory M_Factory {

	factoryCreator  NetworkMessage create(M_Type)
	factoryReceiver NetworkMessage receive(StreamType)
}