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

#include <iostream>
#include <cstdarg>
#include <sstream>

using std::ofstream ;
using std::ios ;
using std::cerr ;
using std::endl ;

namespace certi {

// ----------------------------------------------------------------------------
//! AuditFile constructor to write to file
/*! Audit file is used to store information about actions taken by the RTIG
 */
AuditFile::AuditFile(const std::string& logfile)
    : auditFile(logfile.c_str(), ios::app)
{
    if (!auditFile.is_open()) {
        cerr << "Could not open Audit file � " << logfile 
	     << " �." << endl ;
        throw RTIinternalError("Could not open Audit file.");
    }

    // Put a Start delimiter in the Audit File
    putLine(AUDITEVENT_START_AUDIT, AUDIT_MAX_LEVEL, e_NO_EXCEPTION, "");
}

// ----------------------------------------------------------------------------
//! delete an AuditFile instance.
/*! if a line is currently being processed, close it. Before closing the file,
  adds a specific end line.
*/
AuditFile::~AuditFile()
{
    endLine(e_NO_EXCEPTION, "");
    putLine(AUDITEVENT_STOP_AUDIT, AUDIT_MAX_LEVEL, e_NO_EXCEPTION, "");
    auditFile.close();
}

// ----------------------------------------------------------------------------
//! Adds last information about current line and writes it to file.
/*! Completes a line previously initialized by a newLine call. Appends the
  current status and a comment. Then write line to file.
*/
void
AuditFile::endLine(unsigned short event_status, const std::string& reason)
{
    if (currentLine.started())
	currentLine.end(event_status, reason);

    // Log depending on level and non-zero status.
    if (currentLine.getLevel() >= AUDIT_CURRENT_LEVEL || 
	currentLine.getStatus())
	currentLine.write(auditFile);
    
    currentLine = AuditLine();
}

// ----------------------------------------------------------------------------
//! addToLine add a comment to the current line.
// void
// AuditFile::addToLine(const string comment)
// {
//     currentLine.addComment(comment);
// }

// ----------------------------------------------------------------------------
//! addToLinef adds a formatted comment to the current line.
// void
// AuditFile::addToLinef(const char *Format, ...)
// {
//     va_list argptr ; // Variable Argument list, see cstdarg

//     if ((currentLine != NULL) && (Format != NULL)) {
//         va_start(argptr, Format);
//         vsprintf(va_Buffer, Format, argptr);
//         va_end(argptr);

//         currentLine->addComment(va_Buffer);
//     }
// }

// ----------------------------------------------------------------------------
//! creates a new line with parameters and writes this line to file.
/*! Sometimes, you may want to directly put a line in the audit without
  calling 3 methods : you can also use the following PutLine method in case
  of an emergency. The line is written immediatly, even before any currently
  builded audit line. The federation and federate numbers are set to(0, 0).
*/
void
AuditFile::putLine(unsigned short event_type,
                   unsigned short event_level,
                   unsigned short event_status,
                   const std::string& reason)
{
    if (event_level >= AUDIT_CURRENT_LEVEL) {
	AuditLine line(event_type, event_level, event_status, reason);
	line.write(auditFile);
    }
}

// ----------------------------------------------------------------------------
//! start a new line and set with parameters.
void
AuditFile::startLine(Handle federation,
                     FederateHandle federate,
                     unsigned short event_type)
{
    // Check already valid opened line
    if (currentLine.started()) {
        cerr << "Audit Error : Current line already valid !" << endl ;
        return ;
    }

    currentLine = AuditLine(event_type, AUDIT_MIN_LEVEL, 0, "");
    currentLine.setFederation(federation);
    currentLine.setFederate(federate);
}

// ----------------------------------------------------------------------------
//! setLevel change the event level.
/*! event level is used to determine if information has to be inserted into
  file. Level is only used by endLine module.
*/
void
AuditFile::setLevel(unsigned short eventLevel)
{
    currentLine.setLevel(eventLevel);
}

// ----------------------------------------------------------------------------
/** operator<<
 */
AuditFile &
AuditFile::operator<<(const char *s)
{
    if (s != 0)
	currentLine.addComment(s);
    return *this ;
}

AuditFile &
AuditFile::operator<<(const std::string& s)
{
    currentLine.addComment(s);
    return *this ;
}

AuditFile &
AuditFile::operator<<(int n)
{
    std::ostringstream s ;
    s << n ;
    currentLine.addComment(s.str());
    return *this ;
}

AuditFile &
AuditFile::operator<<(long n)
{
    std::ostringstream s ;
    s << n ;
    currentLine.addComment(s.str());
    return *this ;
}

AuditFile &
AuditFile::operator<<(unsigned int n)
{
    std::ostringstream s ;
    s << n ;
    currentLine.addComment(s.str());
    return *this ;
}

AuditFile &
AuditFile::operator<<(unsigned long n)
{
    std::ostringstream s ;
    s << n ;
    currentLine.addComment(s.str());
    return *this ;
}

AuditFile &
AuditFile::operator<<(double n)
{
    std::ostringstream s ;
    s << n ;
    currentLine.addComment(s.str());
    return *this ;
}

}
