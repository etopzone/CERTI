// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: AuditFile.hh,v 3.3 2003/01/08 13:22:43 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_AUDIT_FILE_HH
#define _CERTI_AUDIT_FILE_HH

#include <config.h>

#include <fstream>
using std::ofstream;
using std::ios;

#include <iostream>
using std::cerr;
using std::endl;

#include <cstdarg> // vsprintf, va_*

#include "AuditLine.hh"
#include "audit.hh"
#include "RTItypes.hh"

namespace certi {

// ---------------------------------------------------------------------------
//! AuditFile class is used to store information into a file for later usage.
/*! First call the startLine method to give background information about the
    next current audit line. The line is then prepared internally, but not yet
    written to the audit file. You can then set the Level of the line
    separately. If the Level is below the fixed Audit level, nothing will be
    stored or written for this event (except in the case of an exception). The
    default level is the lowest one.  Each call to addToLine or to addToLine
    adds the parameter string to the current line. Then a last call to EndLine
    will set the line's status (or Result) and flush the line into the Audit
    file.
*/
class AuditFile
{
public:
    AuditFile(const char *); // Open LogFileName for writing.
    ~AuditFile(void);

    void startLine(FederationHandle, FederateHandle, unsigned short EventType);
    void setLevel(unsigned short EventLevel);
    void addToLine(const char *Comment);
    void addToLinef(const char *Format, ...);
    void endLine(unsigned short Eventstatus = e_NO_EXCEPTION,
                 const char *Reason = NULL);
    void putLine(unsigned short EventType,
                 unsigned short EventLevel,
                 unsigned short Eventstatus = e_NO_EXCEPTION,
                 const char *Reason = NULL);

protected:
    ofstream  *auditFile;   //!< Stream pointer to output file.
    AuditLine *currentLine; //!< Line currently being processed.
    char va_Buffer [1024];  //!< Static buffer for va_printf operations.
};

}

#endif // _CERTI_AUDIT_FILE_HH

// $Id: AuditFile.hh,v 3.3 2003/01/08 13:22:43 breholee Exp $
