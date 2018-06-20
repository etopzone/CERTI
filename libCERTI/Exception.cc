// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
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
// ----------------------------------------------------------------------------

#include "Exception.hh"
#include "PrettyDebug.hh"
#include "certi.hh"

static PrettyDebug PD_Exception("CERTI_EXCEPTION", __FILE__);

const std::string certi::Exception::displayMe() const
{
    std::string msg = "CERTI::Exception [" + ((name().empty()) ? "<noname>" : name())
        + " - reason=" + ((_reason.empty()) ? "<noreason>" : _reason) + "\n";

    Debug(PD_Exception, pdExcept) << msg;
    return msg;
}
