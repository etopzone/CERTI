// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002  ONERA
//
// This file is part of CERTI-libcerti
//
// CERTI-libcerti is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libcerti is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: AuditFile.cc,v 3.0.2.1 2002/11/22 18:44:01 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "AuditFile.hh"

namespace certi {

// ---------------------------------------------------------------------------
// AuditFile
//
AuditFile::AuditFile(const char *LogFileName)
{
  // Open Audit File(Append mode)
  auditFile = fopen(LogFileName, "a");

  if(auditFile == NULL) {
    fprintf(stderr, "Could not open Audit file %s.\n", LogFileName);
    throw RTIinternalError("Could not open Audit file.");
  }

  // Put a Start delimiter in the Audit File
  putLine(AUDITEVENT_START_AUDIT, AUDIT_MAX_LEVEL);

  // Init Current line as empty
  currentLine = NULL;
}

// ---------------------------------------------------------------------------
// ~AuditFile
//
AuditFile::~AuditFile()
{
  // Flush the current line.
  if(currentLine != NULL)
    endLine();

  // Put a last line in the Audit
  putLine(AUDITEVENT_STOP_AUDIT, AUDIT_MAX_LEVEL);

  // Close Audit File
  fclose(auditFile);
  auditFile = NULL;
}

// ---------------------------------------------------------------------------
// endLine
//
void 
AuditFile::endLine(unsigned short Eventstatus,
		   const char *Reason)
{
  if(currentLine != NULL) {
 
    currentLine->status = Eventstatus;
 
    if(Reason != NULL)
      currentLine->addComment(Reason);
 
    // Log depending on level and non-zero status.
    if((currentLine->level >= AUDIT_CURRENT_LEVEL) ||
       (currentLine->status != 0))
      currentLine->write(auditFile);
 
    delete currentLine;
    currentLine = NULL;
  }
}

// ---------------------------------------------------------------------------
// addToLine
//
void 
AuditFile::addToLine(const char *Comment)
{
  currentLine->addComment(Comment);
}

// ---------------------------------------------------------------------------
// addToLine
//
void 
AuditFile::addToLinef(const char *Format, ...)
{
  va_list argptr; // Variable Argument list, see stdarg.h

  if((currentLine != NULL) &&
     (Format != NULL)) {
    va_start(argptr, Format);
    vsprintf(va_Buffer, Format, argptr);
    va_end(argptr);
 
    currentLine->addComment(va_Buffer);
  }
}

// ---------------------------------------------------------------------------
// putLine
//
void 
AuditFile::putLine(unsigned short EventType,
		   unsigned short EventLevel,
		   unsigned short Eventstatus,
		   const char *Reason)
{
  if(EventLevel < AUDIT_CURRENT_LEVEL)
    return;

  AuditLine *TempLine = new AuditLine();

  TempLine->type = EventType;
  TempLine->level = EventLevel;
  TempLine->status = Eventstatus;

  if(Reason != NULL) TempLine->addComment(Reason);
  TempLine->write(auditFile);

  delete TempLine;
}

// ---------------------------------------------------------------------------
// startLine
//
void 
AuditFile::startLine(FederationHandle Federation,
		     FederateHandle Federate,
		     unsigned short EventType)
{
  // Check already valid opened line 
  if(currentLine != NULL) {
    fprintf(stderr, "Audit Error : Current line already valid !\n");
    return;
  }

  currentLine = new AuditLine();

  currentLine->federation = Federation;
  currentLine->federate = Federate;
  currentLine->type = EventType;
  currentLine->level = AUDIT_MIN_LEVEL;
}

// ---------------------------------------------------------------------------
// setLevel
//
void 
AuditFile::setLevel(unsigned short EventLevel)
{
  if(currentLine != NULL) currentLine->level = EventLevel;
}

}

// $Id: AuditFile.cc,v 3.0.2.1 2002/11/22 18:44:01 breholee Exp $
