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
// $Id: Trace.cc,v 3.6 2003/07/07 23:05:26 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "Trace.hh"

#include <time.h>

namespace certi {
namespace rtig {

Trace::Trace()
{
    f_trace = fopen(NOM_FICHIER_TRACE, "w");
    if (f_trace == NULL)
        printf("GTr: impossible ouvrir fichier trace %s\n", NOM_FICHIER_TRACE);
}

Trace::~Trace()
{
    if (f_trace) fclose(f_trace);
}

void Trace::write(const char *s)
{
    if (f_trace) fprintf(f_trace, "%ld %s\n", time(0), s);
}

}}

// $Id: Trace.cc,v 3.6 2003/07/07 23:05:26 breholee Exp $
