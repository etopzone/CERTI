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

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>

using std::vector ;

namespace certi {
static PrettyDebug G("GENDOC",__FILE__);

// ----------------------------------------------------------------------------
NetworkMessage::NetworkMessage()
    : type(NOT_USED), exception(e_NO_EXCEPTION)
{
    messageName        = "NetworkMessage (generic)";
    exceptionReason    = "Not Assigned";
    federation         = 0 ;
    federate           = 0 ;

} /* end of NetworkMessage() */

NetworkMessage::~NetworkMessage() {
	
}

std::ostream& NetworkMessage::show(std::ostream& out) {
	out << "[NetworkMessage -Begin]" << std::endl;
	if (isDated()) {
		out << " date       = " << getDate() << std::endl;
	}
	out << " federation = " << federation << std::endl;
	out << " federate   = " << federate << std::endl;
	out << "[NetworkMessage -End]" << std::endl;
	return out;
} /* end of show */

} // namespace certi
