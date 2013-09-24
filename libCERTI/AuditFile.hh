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

#ifndef _CERTI_AUDIT_FILE_HH
#define _CERTI_AUDIT_FILE_HH

#include "AuditLine.hh"
#include "certi.hh"
#include "Exception.hh"

#include <fstream>
#include <string>

#define AUDIT_MIN_LEVEL 0
#define AUDIT_MAX_LEVEL 10

#define AUDITEVENT_START_AUDIT 128
#define AUDITEVENT_STOP_AUDIT 129
#define AUDITEVENT_START_RTIG 130
#define AUDITEVENT_STOP_RTIG 131

namespace certi {

// ----------------------------------------------------------------------------
//! AuditFile class is used to store information into a file for later usage.
/*! First call the startLine method to give background information about the
  next current audit line. The line is then prepared internally, but not yet
  written to the audit file. You can then set the Level of the line
  separately. If the Level is below the fixed Audit level, nothing will be
  stored or written for this event (except in the case of an exception). The
  default level is the lowest one. Each call to addToLine or to addToLine
  adds the parameter string to the current line. Then a last call to EndLine
  will set the line's status (or Result) and flush the line into the Audit
  file.
*/
class CERTI_EXPORT AuditFile
{
public:
    AuditFile(const std::string&); // Open LogFileName for writing.
    ~AuditFile();

    void startLine(Handle, FederateHandle, unsigned short EventType);
    void setLevel(unsigned short EventLevel);
    //    void addToLine(const std::string);
    //    void addToLinef(const char *Format, ...);
    void endLine(unsigned short, const std::string&);
    void putLine(unsigned short, unsigned short, unsigned short, const std::string&);

    AuditFile &operator<<(const char *);
    AuditFile &operator<<(const std::string& s);
    AuditFile &operator<<(int);
    AuditFile &operator<<(unsigned int);
    AuditFile &operator<<(long);
    AuditFile &operator<<(unsigned long);
    AuditFile &operator<<(double);

protected:
    std::ofstream auditFile ; //!< Stream pointer to output file.
    AuditLine currentLine ; //!< Line currently being processed.
    //char va_Buffer[1024] ; //!< Static buffer for va_printf operations.
};

} // namespace certi

#endif // _CERTI_AUDIT_FILE_HH
