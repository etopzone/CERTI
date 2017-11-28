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

#ifndef CERTI_STATISTICS
#define CERTI_STATISTICS

#include <array>

#include <libCERTI/Message.hh>
#include <libCERTI/NetworkMessage.hh>

namespace certi {
namespace rtia {

/** Display statistics on sent/received messages by RTIA. This class
  collects information type exchanged by federate/RTIA and
  RTIA/RTIG. Statistics are displayed on exit only if
  CERTI_NO_STATISTICS environment variable has not been set.
*/
class Statistics {
public:
    /// Initialize the two sets used for collecting number of messages exchanged.
    Statistics();

    /// Increment counter for RTIG message type received.
    void rtiService(NetworkMessage::Type);
    
    /// Increment counter for Federate message type received.
    void federateService(Message::Type);
    
    bool display() const;
    
    bool displayZero() const;
    
    void show(std::ostream& stream) const;

protected:
    /// Collects number of messages exchanged between federate and RTIA.
    std::array<int, Message::LAST> my_federate_service_count;
    /// Collects number of messages exchanged between RTIG and RTIA.
    std::array<int, NetworkMessage::the_message_type_count> my_rti_service_count;
    
    /// Display number of sent messages, for each message type
    bool my_display {true};
    /// Display messages information including messages not sent
    bool my_display_zero {false};
};

/// Display collected data.
std::ostream& operator<<(std::ostream& s, const Statistics& stats);

}
} // namespaces

#endif // CERTI_STATISTICS
