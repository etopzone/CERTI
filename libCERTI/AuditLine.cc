// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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

#include "AuditLine.hh"

#include "certi.hh"

#include <string>

namespace certi {

AuditLine::AuditLine(const AuditLine::Type type,
                     const AuditLine::Level level,
                     const AuditLine::Status status,
                     const std::string& reason)
    : my_type(type), my_level(level), my_status(status), my_comment(reason)
{
}

void AuditLine::addComment(const std::string& str)
{
    my_comment += str;
    my_is_modified = true;
}

void AuditLine::end(const Status status, const std::string& reason)
{
    my_status = status;
    addComment(reason);
}

void AuditLine::write(std::ostream& audit_file)
{
    audit_file << my_date << '\t' << my_federation << '\t' << my_federate << '\t' << my_type.get() << '\t'
               << my_level.get() << '\t' << static_cast<unsigned int>(my_status.get()) << '\t' << my_comment
               << std::endl;

    audit_file.flush();
}

AuditLine::Level AuditLine::getLevel() const
{
    return my_level;
}

void AuditLine::setLevel(const Level l)
{
    my_level = l;
    my_is_modified = true;
}

AuditLine::Status AuditLine::getStatus() const
{
    return my_status;
}

bool AuditLine::started() const
{
    return my_is_modified;
}

void AuditLine::setFederation(Handle h)
{
    my_federation = h;
    my_is_modified = true;
}

void AuditLine::setFederate(FederateHandle h)
{
    my_federate = h;
    my_is_modified = true;
}

} // namespace certi
