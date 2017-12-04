// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003-2005  ONERA
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

#include "Statistics.hh"
#include <config.h>

#include <cstdlib>
#include <iostream>
#include <numeric>
#include <string>

using std::cout;
using std::endl;
using std::string;
using std::ostream;
using std::endl;
using std::vector;

namespace certi {
namespace rtia {

Statistics::Statistics()
{
    my_federate_service_count.fill(0);
    my_rti_service_count.fill(0);

    if (getenv("CERTI_NO_STATISTICS")) {
        my_display = false;
    }
}

void Statistics::rtiService(NetworkMessage::Type service)
{
    ++my_rti_service_count[static_cast<size_t>(service)];
}

void Statistics::federateService(Message::Type service)
{
    ++my_federate_service_count[service];
}

bool Statistics::display() const
{
    return my_display;
}

bool Statistics::displayZero() const
{
    return my_display_zero;
}

void Statistics::show(std::ostream& s) const
{
    cout << endl << "RTIA: Statistics (processed messages)" << endl;

    s << "List of federate initiated services " << endl << "--------------------------------------------------" << endl;

    auto i = 0u;
    for (const auto count : my_federate_service_count) {
        if (count > 0 || displayZero()) {
            s.width(8);
            s << count << ' ' << Message::to_string(static_cast<Message::Type>(i)) << " (MSG#" << i << ")" << endl;
        }
        ++i;
    }

    s << endl
      << "List of RTI initiated services " << endl
      << "--------------------------------------------------" << endl;

    i = 0u;
    for (const auto count : my_rti_service_count) {
        if (count > 0 || displayZero()) {
            s.width(8);
            s << count << ' ' << NetworkMessage::to_string(static_cast<NetworkMessage::Type>(i)) << " (MSG#" << i << ")" << endl;
        }
        ++i;
    }

    s << endl
      << " Number of Federate messages : "
      << std::accumulate(begin(my_federate_service_count), end(my_federate_service_count), 0) << endl
      << " Number of RTIG messages : " << std::accumulate(begin(my_rti_service_count), end(my_rti_service_count), 0)
      << endl;
}

std::ostream& operator<<(std::ostream& s, const Statistics& stats)
{
    stats.show(s);
    return s;
}
}
} // namespace certi::rtia

#undef FED_MSG_NAME
#undef RTI_MSG_NAME
