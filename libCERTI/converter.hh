// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libRTI
//
// CERTI-libRTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libRTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: converter.hh,v 3.2 2003/05/15 20:40:11 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_CONVERTER_HH
#define _CERTI_CONVERTER_HH

#include <config.h>

// Project
#include "RTItypes.hh"

// Standard libraries
#include <string.h>

namespace certi {

void getStringToObjectLength(const char *, ULong &);
void stringToObject(const char *, char *, ULong);
void objectToString(const char *, ULong, char *);
void getObjectToStringLength(const char *, ULong, ULong &);

} // namespace certi

#endif // _CERTI_CONVERTER_HH

// $Id: converter.hh,v 3.2 2003/05/15 20:40:11 breholee Exp $
