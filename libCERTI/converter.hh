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
// $Id: converter.hh,v 3.3 2003/06/27 17:26:29 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_CONVERTER_HH
#define _CERTI_CONVERTER_HH

namespace certi {

void getStringToObjectLength(const char *, unsigned long &);
void stringToObject(const char *, char *, unsigned long);
void objectToString(const char *, unsigned long, char *);
void getObjectToStringLength(const char *, unsigned long, unsigned long &);

} // namespace certi

#endif // _CERTI_CONVERTER_HH

// $Id: converter.hh,v 3.3 2003/06/27 17:26:29 breholee Exp $
