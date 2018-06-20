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
// ----------------------------------------------------------------------------

#include "Clock.hh"
namespace libhla {
namespace clock {

Clock::Clock(const std::string& newName)
{
    name = newName;
}

uint64_t Clock::getDeltaTicks(const uint64_t previousTicks)
{
    return (getCurrentTicksValue() - previousTicks);
}

double Clock::getDeltaNanoSecond(const uint64_t firstTicks, const uint64_t secondTicks)
{
    return tick2NanoSecond(secondTicks - firstTicks);
}

double Clock::getDeltaNanoSecond(const uint64_t previousTicks)
{
    return tick2NanoSecond(getDeltaTicks(previousTicks));
}

Clock::~Clock()
{
}

} /* end namespace clock  */
} /* end namespace libhla */
