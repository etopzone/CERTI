// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003  ONERA
//
// This file is part of CERTI
//
// CERTI is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// ----------------------------------------------------------------------------

#ifndef _CERTI_STATISTICS
#define _CERTI_STATISTICS

// Project
#include "Message.hh"
#include "NetworkMessage.hh"

// Standard libraries
#include <map>

namespace certi {
namespace rtia {

//! Display statistics on sent/received messages by RTIA.
/*! This class collects information type exchanged by federate/RTIA
  and RTIA/RTIG. Statistics are displayed on exit only if
  CERTI_NO_STATISTICS UNIX environment variable has not been set.
*/
class Statistics
{
public:
    Statistics();
    ~Statistics();

    void rtiService(NetworkMessage::Type);
    void federateService(Message::Type);
    bool display() { return myDisplay ; };
    bool displayZero() { return myDisplayZero ; };
    
    friend ostream &operator<<(ostream &, Statistics &);
protected:
    //! Collects number of messages exchanged between federate and RTIA.
    std::map<Message::Type, int> federateServiceSet ;
    //! Collects number of messages exchanged between RTIG and RTIA.
    std::map<NetworkMessage::Type, int> rtiServiceSet ;

    //    static std::string rtiStrMessages[] ;
    //    static std::string federateStrMessages[] ;

    static std::map<Message::Type, std::string> fedMessageName ;
    static std::map<NetworkMessage::Type, std::string> rtiMessageName ;

    bool myDisplay ;
    bool myDisplayZero ;
};

}}
#endif // _CERTI_STATISTICS
