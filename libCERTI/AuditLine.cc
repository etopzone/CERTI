// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
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
// $Id: AuditLine.cc,v 3.2 2002/12/11 00:47:33 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "AuditLine.hh"

namespace certi {

// ---------------------------------------------------------------------------
// addComment
//
void AuditLine::addComment(const char *String)
{
  if(String == NULL)
    return;

  // BUG: Should check overflow
  std::strcat(comment, String);
}

// ---------------------------------------------------------------------------
// AuditLine
//
AuditLine::AuditLine()
{
  // Set the Date :
  date = time(NULL);

  // Clear attributes and Comment line
  federation = 0;
  federate = 0;
  type = 0;
  level = 0;
  status = 0;

  comment [0] = 0;
}

// ---------------------------------------------------------------------------
// ~AuditLine
//
AuditLine::~AuditLine()
{
}

// ---------------------------------------------------------------------------
// write
//
void AuditLine::write(FILE *AuditFile)
{
  fprintf(AuditFile, "%lu %2hu %3lu %3hu %2hu %3hu %s\n",
	  date, // Usually 9 digits at this time !
	  federation, // 2 digits
	  federate, // 3 digits
	  type, // 3 digits
	  level, // 2 digits
	  status, // 3 digits
	  comment); // ended by a new line character.

  fflush(AuditFile);
}

}

// $Id: AuditLine.cc,v 3.2 2002/12/11 00:47:33 breholee Exp $

