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
// $Id: PrettyDebug.hh,v 3.2 2003/03/21 18:18:48 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_PRETTYDEBUG_HH
#define _CERTI_PRETTYDEBUG_HH

#include <config.h>

#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "DebugOStream.hh"

#define pdSEmptyMessage "Pretty Debug Server empty Message."

// Pretty Debug Basic types
typedef unsigned TDebugLevel;

// Alias
class PrettyDebug ;
typedef PrettyDebug pdCDebug ;

// Pretty Debug Constants
#define pdMaxMessageSize 255  // greater then pdTooLongInitMessage length!

/** Do not use the pdUnused and pdLast Levels!!! Do not also specify
 *  any value for the elements, because order is used and missing
 *  value would cause crash.  pdLast must always be the last in the
 *  enum.  Key(see pdDebugKeys) */
enum pdDebugLevel  {pdUnused, /**< Do not use! : */
                    pdAnswer, /**< Server answer A */
                    pdCom, /**< Communication C */
                    pdDebug, /**< Debug D */
                    pdError, /**< Error E */
                    pdInit, /**< Initialization I */
                    pdProtocol, /**< Protocol P */
                    pdRegister, /**< Object Registration R */
                    pdRequest, /**< Client Request S */
                    pdTerm, /**< Terminate T */
                    pdWarning, /**< Warning W */
                    pdExcept, /**< Exceptions X */
                    pdTrace, /**< Trace Z */
                    pdLast}; /**< Do not use \0 */

// The following keys are used in the environment variable to enable
// debug level.  For example, if ENVVAR=D:C, the pdDebug and the pdCom
// debug level are enabled, and all others are not enabled.

// KEEP THE SAME ORDER AS IN THE pdDebugLevel ENUM!
#define pdDebugKeysString ":ACDEIPRSTWXZ\0"


//---------------------------------------------------------------------------
class PrettyDebug
{
private:
    char* LEnvVar; /**< Name of the environment variable to look for. */
    char* LMessage; /**< The container of all printed debug
                       messages. Start with the Header specified at
                       construction time */
    char* HeaderMessage; /**< The header message specified at
                            construction time. */
 
    static DebugOStream defaultOutputStream;
    static DebugOStream* nullOutputStreamPtr;
    static DebugOStream& nullOutputStream;
 
    /** If Level_Map[Level] != &PrettyDebug::nullOutputStream, then the
       debug message must be printed in the ostream addressed. */
    DebugOStream* Level_Map [pdLast + 1];

    static void Print(DebugOStream& theOutputStream, 
                      const char* theHeaderMessage, 
                      const char * Message);

    // Parse the environment variable value to find debug keys and
    // enable debug levels
    void ParseEnvString(const char * Name);

public:  
    PrettyDebug(const char * Name, const char * Header);
    ~PrettyDebug();
  
    bool Mode(pdDebugLevel Level);

    void enableDebugLevel(pdDebugLevel Level, 
                          DebugOStream& theOutputStream = PrettyDebug::defaultOutputStream);
    void disableDebugLevel(pdDebugLevel Level);
  
    DebugOStream& operator[](pdDebugLevel Level) //Inline method
    {
        DebugOStream* theReturnedOutputStreamPtr = (Level_Map[Level]);
        if (theReturnedOutputStreamPtr == PrettyDebug::nullOutputStreamPtr) {
            return(PrettyDebug::nullOutputStream);
        }
        *theReturnedOutputStreamPtr << HeaderMessage;
        return(*theReturnedOutputStreamPtr);
    }

#ifdef NO_PRETTYDEBUG
    inline void Out(pdDebugLevel Level, const char *Format, ...) {};
#else
    void Out(pdDebugLevel Level, const char *Format, ...);
#endif

};

#endif // _CERTI_PRETTYDEBUG_HH

// $Id: PrettyDebug.hh,v 3.2 2003/03/21 18:18:48 breholee Exp $
