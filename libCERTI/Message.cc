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

#include "Message.hh"
#include "PrettyDebug.hh"
#include "certi.hh"

#include <assert.h>
#include <cstdio>
#include <cstring>
#include <stdlib.h>

using std::vector;

namespace certi {

static PrettyDebug G("GENDOC", __FILE__);

Message::Message()
{
    exception = Exception::Type::NO_EXCEPTION;
    exceptionReason = "";
    messageName = "Message (generic)";
    type = NOT_USED;
    resignAction = certi::NO_ACTION;
    space = 0;
    dimension = 0;
} /* end of Message default constructor */

// ----------------------------------------------------------------------------
/** Store exception into message
    @param the_exception : exception type (enum)
    @param the_reason : exception reason
*/
void Message::setException(const Exception::Type the_exception, const std::string& the_reason)
{
    exception = the_exception;
    exceptionReason = the_reason;
}

} // namespace certi
