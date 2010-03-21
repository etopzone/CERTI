// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
// Copyright (C) 2009 Mathias Froehlich
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: GettimeofdayClock.cc,v 1.1 2010/03/21 17:10:10 erk Exp $
// ----------------------------------------------------------------------------

#include "GettimeofdayClock.hh"

#include <sys/time.h>
#include <iostream>

namespace libhla {
namespace clock {

GettimeofdayClock::GettimeofdayClock() : Clock("GettimeofdayClock")
{
}   

double 
GettimeofdayClock::getResolution() {
    return 1000;
}

uint64_t GettimeofdayClock::getCurrentTicksValue() {
    struct timeval tv;
    if (-1 == gettimeofday(&tv, 0))
        std::cerr << "Cannot get current timestamp!" << std::endl;
    /* convert struct timespec to number of micro-seconds */ 
    return tv.tv_sec*1000000 + tv.tv_usec;
}

double   
GettimeofdayClock::tick2NanoSecond(const uint64_t ticks) {
    return 1e3*ticks;
}

GettimeofdayClock::~GettimeofdayClock()
{
}

} /* end of clock namespace */
} /* and of libhla namespace */
