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
// $Id: converter.cc,v 3.0 2003/04/22 16:42:15 breholee Exp $
// ----------------------------------------------------------------------------

#include "converter.hh"

namespace certi {

// ----------------------------------------------------------------------------
//! Returns buffer size needed for storing message restored by stringToObject.
void
getStringToObjectLength(char *init_string, ULong& size)
{
    ULong counter = 0 ;
    ULong length = strlen(init_string);
    ULong i = 0 ;
    size = 0 ;
    while (i<length) {
        switch (init_string[i]) {
        case '\\':
            i++ ;
            while ((init_string[i]=='?') && (i<length)) {
                counter++ ;
                i++ ;
            }
            if ((counter%2)==0) size += (counter/2);
            else size += 1+((counter-1)/2);
            counter = 0 ;
            break ;
        default:
            size++ ;
            i++ ;
        }
    }
}

// ----------------------------------------------------------------------------
//! Restore string network message to the original text object.
/*! stringToObject is used to restore initial object as it was before being
  sent throw the network. Cases are as follows :
  - ? : if alone without \ prefixing caracter, restore to \0
  - \ : this caracter is used to tell that a caracter \ or ? needs replace it
  - other : keep caracter.

  Pattern is "\???\" for case \. If number of ? is odd, it must be
  replaced by a list of ? else it must be replaced by a list of \.
*/
void
stringToObject(char *init_string, char *end_string, ULong size)
{
    ULong counter = 0 ;
    ULong i = 0 ;
    ULong j = 0 ;
    ULong indice = 0 ;
    ULong length = strlen(init_string);

    memset(end_string, '\0', size);

    while (i<length) {
        switch(init_string[i]) {
        case '?':
            // No use to add '\0' since memset does it
            i++ ;
            indice++ ;
            break ;
        case '\\':
            i++ ;
            while ((init_string[i]=='?') && (i<length)) {
                counter++ ;
                i++ ;
            }
            i++ ;
            if ((counter%2)==0) {
                for (j=0 ; j<(counter/2); j++) {
                    end_string[indice]='\\' ;
                    indice++ ;
                }
            }
            else {
                for (j=0 ; j<(1+((counter-1)/2)); j++) {
                    end_string[indice]='?' ;
                    indice++ ;
                }
            }
            counter = 0 ;
            break ;
        default:
            end_string[indice]=init_string[i] ;
            i++ ;
            indice++ ;
        }
    }
}

// ===========================================================================
// Data Integrity
// ===========================================================================
//! Convert an object string to a network capable message.
/*! objectToString is used to convert data to a network message for sending.
  Special caracters are ?, \ and \0.
  - \0 : it is converted to a single ?,
  - ? : converted to pattern "\?[??]+\",
  - \ : converted to pattern "\??[??]+\".
  - other : keep caracter.

  Differenciation between ? and \ is made on parity number of ? contained
  between \.
*/
void
objectToString(const char *init_string,
               ULong size, char *end_string)
{
    ULong i = 0 ;
    ULong j = 0 ;

    while (i < size) {
        switch(init_string[i]) {
        case '\0':
            end_string[j++] = '?' ;
            i++ ;
            break ;
        case '?':
            end_string[j++] = '\\' ;
            end_string[j++] = '?' ;
            i++ ;
            while ((init_string[i] == '?') && (i < size)) {
                end_string[j++] = '?' ;
                end_string[j++] = '?' ;
                i++ ;
            }
            end_string[j++] = '\\' ;
            break ;
        case '\\':
            end_string[j++] = '\\' ;
            end_string[j++] = '?' ;
            end_string[j++] = '?' ;
            i++ ;
            while ((init_string[i] == '\\') && (i<size)) {
                end_string[j++] = '?' ;
                end_string[j++] = '?' ;
                i++ ;
            }
            end_string[j++] = '\\' ;
            break ;
        default:
            end_string[j++] = init_string[i] ;
            i++ ;
        }
    }
    end_string[j] = '\0' ;
}

// ----------------------------------------------------------------------------
//! Returns buffer size needed to store network message made by objectToString
void
getObjectToStringLength(char *init_string,
                        ULong init_size, ULong &size)
{
    ULong counter = 0 ;
    ULong i = 0 ;
    size = 0 ;

    while (i < init_size) {
        switch(init_string[i]) {
        case '?':
            i++ ;
            while ((init_string[i] == '?') && (i < init_size)) {
                counter++ ;
                i++ ;
            }
            size += 3 + 2 * counter ;
            counter = 0 ;
            break ;
        case '\\':
            i++ ;
            while ((init_string[i] == '\\') && (i < init_size)) {
                counter++ ;
                i++ ;
            }
            size += 4 + 2 * counter ;
            counter = 0 ;
        default:
            size++ ;
            i++ ;
        }
    }
    size++ ;
}

} // namespace certi

// $Id: converter.cc,v 3.0 2003/04/22 16:42:15 breholee Exp $
