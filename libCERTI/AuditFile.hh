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
// $Id: AuditFile.hh,v 3.0.2.1 2002/11/22 18:44:01 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_AUDIT_FILE_HH
#define _CERTI_AUDIT_FILE_HH

#include <stdio.h>
#include <sys/types.h>
#include <stdarg.h>

#include "AuditLine.hh"
#include "audit.hh"
#include "RTItypes.hh"

namespace certi {

class AuditFile
{
  
  // ATTRIBUTES ------------------------------------------------------------
protected:
  FILE *auditFile;
  AuditLine *currentLine;
  char va_Buffer [1024]; // Static buffer for va_printf operations

  // METHODS ---------------------------------------------------------------
public:
  AuditFile(const char *LogFileName); // Open LogFileName for writing.
  ~AuditFile();
    
  // First call the startLine method to give background information
  // about the next current audit line. The line is then prepared
  // internally, but not yey written to the audit file.  You can
  // then set the Level of the line seperatly. If the Level is below
  // the fixed Audit level, nothing will be stored or written for
  // this event(except in the case of an exception). The default
  // level is the lowest one.  Each call to addToLine or to
  // addToLine adds the parameter string to the current line. Then
  // a last call to EndLine will set the line's status(or Result)
  // and flush the line into the Audit file.
  void startLine(FederationHandle, FederateHandle, unsigned short EventType);
  void setLevel(unsigned short EventLevel);
  void addToLine(const char *Comment);
  void addToLinef(const char *Format, ...);
  void endLine(unsigned short Eventstatus = e_NO_EXCEPTION,
	       const char *Reason = NULL); 

  // Sometimes, you may want to directly put a line in the audit
  // without calling 3 methods : you can also use the followinf
  // PutLine method in case of an emergency. The line is written
  // immediatly, even before any currently builded audit line. The
  // federation and federate numbers are set to(0,0).
  void putLine(unsigned short EventType,
	       unsigned short EventLevel,
	       unsigned short Eventstatus = e_NO_EXCEPTION,
	       const char *Reason = NULL);
};

}

#endif // _CERTI_AUDIT_FILE_HH

// $Id: AuditFile.hh,v 3.0.2.1 2002/11/22 18:44:01 breholee Exp $
