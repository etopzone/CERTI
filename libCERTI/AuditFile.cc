// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
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
// $Id: AuditFile.cc,v 3.4 2003/02/19 18:07:29 breholee Exp $
// ----------------------------------------------------------------------------

#include "AuditFile.hh"

namespace certi {

// ----------------------------------------------------------------------------
//! AuditFile constructor to write to file
/*! Audit file is used to store information about actions taken by the RTIG
 */
AuditFile::AuditFile(const char *log_file)
{
    // Open Audit File (Append mode)
    auditFile = new ofstream(log_file, ios::app);

    if (!auditFile->is_open()) {
        cerr << "Could not open Audit file " << log_file << '.' << endl ;
        throw RTIinternalError("Could not open Audit file.");
    }

    // Put a Start delimiter in the Audit File
    putLine(AUDITEVENT_START_AUDIT, AUDIT_MAX_LEVEL);

    // Init Current line as empty
    currentLine = 0 ;
}

// ----------------------------------------------------------------------------
//! delete an AuditFile instance.
/*! if a line is currently being processed, close it. Before closing the file,
  adds a specific end line.
*/
AuditFile::~AuditFile(void)
{
    // Flush the current line.
    if (currentLine != NULL)
        endLine();

    // Put a last line in the Audit
    putLine(AUDITEVENT_STOP_AUDIT, AUDIT_MAX_LEVEL);

    // Close Audit File
    auditFile->close();
}

// ----------------------------------------------------------------------------
//! Adds last information about current line and writes it to file.
/*! Completes a line previously initialized by a newLine call. Appends the
  current status and a comment. Then write line to file and delete it.
*/
void
AuditFile::endLine(unsigned short Eventstatus, const char *Reason)
{
    if (currentLine != NULL) {
        currentLine->status = Eventstatus ;

        if (Reason != NULL)
            currentLine->addComment(Reason);

        // Log depending on level and non-zero status.
        if ((currentLine->level >= AUDIT_CURRENT_LEVEL) ||
            (currentLine->status != 0))
            currentLine->write(*auditFile);

        delete currentLine ;
        currentLine = NULL ;
    }
}

// ----------------------------------------------------------------------------
//! addToLine add a comment to the current line.
void
AuditFile::addToLine(const char *Comment)
{
    if (Comment != NULL)
        currentLine->addComment(Comment);
}

// ----------------------------------------------------------------------------
//! addToLinef adds a formatted comment to the current line.
void
AuditFile::addToLinef(const char *Format, ...)
{
    va_list argptr ; // Variable Argument list, see cstdarg

    if ((currentLine != NULL) && (Format != NULL)) {
        va_start(argptr, Format);
        vsprintf(va_Buffer, Format, argptr);
        va_end(argptr);

        currentLine->addComment(va_Buffer);
    }
}

// ----------------------------------------------------------------------------
//! creates a new line with parameters and writes this line to file.
/*! Sometimes, you may want to directly put a line in the audit without
  calling 3 methods : you can also use the following PutLine method in case
  of an emergency. The line is written immediatly, even before any currently
  builded audit line. The federation and federate numbers are set to(0, 0).
*/
void
AuditFile::putLine(unsigned short EventType,
                   unsigned short EventLevel,
                   unsigned short Eventstatus,
                   const char *Reason)
{
    if (EventLevel < AUDIT_CURRENT_LEVEL)
        return ;

    AuditLine *TempLine = new AuditLine();

    TempLine->type = EventType ;
    TempLine->level = EventLevel ;
    TempLine->status = Eventstatus ;
    if (Reason != NULL)
        TempLine->addComment(Reason);

    TempLine->write(*auditFile);

    delete TempLine ;
}

// ----------------------------------------------------------------------------
//! start a new line and set with parameters.
void
AuditFile::startLine(FederationHandle Federation,
                     FederateHandle Federate,
                     unsigned short EventType)
{
    // Check already valid opened line
    if (currentLine != NULL) {
        cerr << "Audit Error : Current line already valid !" << endl ;
        return ;
    }

    currentLine = new AuditLine();

    currentLine->federation = Federation ;
    currentLine->federate = Federate ;
    currentLine->type = EventType ;
    currentLine->level = AUDIT_MIN_LEVEL ;
}

// ----------------------------------------------------------------------------
//! setLevel change the event level.
/*! event level is used to determine if information has to be inserted into
  file. Level is only used by endLine module.
*/
void
AuditFile::setLevel(unsigned short eventLevel)
{
    if (currentLine != NULL) currentLine->level = eventLevel ;
}

}

// $Id: AuditFile.cc,v 3.4 2003/02/19 18:07:29 breholee Exp $
