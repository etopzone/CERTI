// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003, 2004  ONERA
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

#include "AuditFile.hh"

#include <cstdarg>
#include <iostream>
#include <sstream>

namespace {
static const auto AuditMinLevel = certi::AuditLine::Level{0};
static const auto AuditMaxLevel = certi::AuditLine::Level{10};

static const auto StartAudit = certi::AuditLine::Type{128};
static const auto StopAudit = certi::AuditLine::Type{129};

static const auto NormalStatus = certi::AuditLine::Status(certi::Exception::Type::NO_EXCEPTION);
}

namespace certi {

AuditFile::AuditFile(const std::string& log_file_name) : my_audit_file{log_file_name, std::ios::app}
{
    if (!my_audit_file.is_open()) {
        std::cerr << "Could not open Audit file: " << log_file_name << std::endl;
        throw RTIinternalError("Could not open Audit file.");
    }

    // Put legend
    my_audit_file << "date\t"
                  << "fed-o\t"
                  << "fed\t"
                  << "type\t"
                  << "level\t"
                  << "status\t"
                  << "comment" << std::endl;

    // Put a Start delimiter in the Audit File
    putLine(StartAudit, AuditMaxLevel, NormalStatus, "");
}

AuditFile::~AuditFile()
{
    endLine(NormalStatus, "");
    putLine(StopAudit, AuditMaxLevel, NormalStatus, "");
    my_audit_file.close();
}

void AuditFile::startLine(const Handle federation_handle,
                          const FederateHandle federate_handle,
                          const AuditLine::Type type)
{
    // Check already valid opened line
    if (my_current_line.started()) {
        std::cerr << "Audit Error : Current line already valid !" << std::endl;
        return;
    }

    my_current_line = AuditLine(type, AuditMinLevel, NormalStatus, "");
    my_current_line.setFederation(federation_handle);
    my_current_line.setFederate(federate_handle);
}

void AuditFile::setLevel(const AuditLine::Level level)
{
    my_current_line.setLevel(level);
}

void AuditFile::endLine(const AuditLine::Status status, const std::string& reason)
{
    if (my_current_line.started()) {
        my_current_line.end(status, reason);
    }

    // Log depending on level and non-zero status.
    if (my_current_line.getLevel().get() >= AUDIT_CURRENT_LEVEL
        || my_current_line.getStatus().get() != Exception::Type::NO_EXCEPTION) {
        my_current_line.write(my_audit_file);
    }

    my_current_line = AuditLine();
}

void AuditFile::putLine(const AuditLine::Type type,
                        const AuditLine::Level level,
                        const AuditLine::Status status,
                        const std::string& reason)
{
    if (level.get() >= AUDIT_CURRENT_LEVEL) {
        AuditLine line(type, level, status, reason);
        line.write(my_audit_file);
    }
}

AuditFile& AuditFile::operator<<(const char* s)
{
    if (s) {
        my_current_line.addComment(s);
    }
    return *this;
}

AuditFile& AuditFile::operator<<(const std::string& s)
{
    my_current_line.addComment(s);
    return *this;
}

AuditFile& AuditFile::operator<<(const int n)
{
    return (*this << std::to_string(n));
}

AuditFile& AuditFile::operator<<(const long n)
{
    return (*this << std::to_string(n));
}

AuditFile& AuditFile::operator<<(const unsigned int n)
{
    return (*this << std::to_string(n));
}

AuditFile& AuditFile::operator<<(const unsigned long n)
{
    return (*this << std::to_string(n));
}

AuditFile& AuditFile::operator<<(const double n)
{
    return (*this << std::to_string(n));
}
}
