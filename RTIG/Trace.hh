// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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
// $Id: Trace.hh,v 3.4 2003/06/27 17:26:28 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef CERTI_TRACE_HH_
#define CERTI_TRACE_HH_

#include <stdio.h>
#include <sys/types.h>

#define NOM_FICHIER_TRACE "trace.txt"

namespace certi {
namespace rtig {

class Trace
{
public:
    Trace();
    ~Trace();
    void write(const char *);

private:
    FILE *f_trace ;
};

}}

#endif // CERTI_TRACE_HH_

// $Id: Trace.hh,v 3.4 2003/06/27 17:26:28 breholee Exp $
