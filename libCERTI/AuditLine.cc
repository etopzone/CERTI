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
// $Id: AuditLine.cc,v 3.6 2003/06/27 17:26:28 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "AuditLine.hh"

#include "RTItypes.hh"

#include <string>
#include <time.h>

using std::ofstream ;
using std::endl ;
using std::string ;

namespace certi {

// ----------------------------------------------------------------------------
//! addComment adds information to the comment parameter.
void
AuditLine::addComment(const char *str)
{
    comment += str ;
}

// ----------------------------------------------------------------------------
//! AuditLine constructor.
/*! Initialise internal parameters to null.
 */
AuditLine::AuditLine()
    : federation(0), federate(0), type(0), level(0), status(0)
{
    // Set the Date
    date = time(NULL);
}

// ----------------------------------------------------------------------------
//! AuditLine destructor. Nothing to be done.
AuditLine::~AuditLine()
{
}

// ----------------------------------------------------------------------------
//! Write a line to the audit file
/*! Write module writes a line to the AuditFile stream. Line ends by a newline.
  Formatting is as follows :
  - date : date of line processing start,
  - federation : federation involved,
  - federate : federate involved by message,
  - type : type of information,
  - level : level assigned to information,
  - status : status of processing,
  - comment : detailed comment.
*/
void
AuditLine::write(ofstream &audit_file)
{
    audit_file << date << ' ' << federation << ' ' << federate << ' '
               << type << ' ' << level << ' ' << status << ' '
               << comment << endl ;

    audit_file.flush();
}

} // namespace certi

// $Id: AuditLine.cc,v 3.6 2003/06/27 17:26:28 breholee Exp $

