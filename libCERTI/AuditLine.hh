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

#ifndef _CERTI_AUDIT_LINE_HH
#define _CERTI_AUDIT_LINE_HH

#include "Exception.hh"
// #include "certi.hh"
#include "Handle.hh"

#include "StrongType.hh"

#include <ctime>
#include <ostream>
#include <string>

namespace certi {

class CERTI_EXPORT AuditLine {
public:
    using Type = NamedType<unsigned short, struct AuditLineTypeParameter>;
    using Level = NamedType<unsigned short, struct AuditLineLevelParameter>;
    using Status = NamedType<Exception::Type, struct AuditLineStatusParameter>;

    AuditLine() = default;
    AuditLine(const Type type, const Level level, const Status status, const std::string& reason);

    /** Write a line to the audit file
     * 
     * Write module writes a line to the AuditFile stream. Line ends by a newline.
     * Formatting is as follows :
     * - date : date of line processing start,
     * - federation : federation involved,
     * - federate : federate involved by message,
     * - type : type of information,
     * - level : level assigned to information,
     * - status : status of processing,
     * - comment : detailed comment.
     */
    void write(std::ostream&);

    /// Add str at the end of comment.
    void addComment(const std::string& str);

    void end(const Status status, const std::string& reason = "");

    Level getLevel() const;
    void setLevel(const Level l);

    Status getStatus() const;

    bool started() const;

    void setFederation(Handle h);
    void setFederate(FederateHandle h);

private:
    Handle my_federation{0};
    FederateHandle my_federate{0};
    Type my_type{0};
    Level my_level{0};
    Status my_status{Exception::Type::NO_EXCEPTION};
    bool my_is_modified{false};

    time_t my_date{std::time(nullptr)}; /// date, automatically set at construction time.
    std::string my_comment{}; /// comment internally managed.
};
}

#endif // _CERTI_AUDIT_LINE_HH
