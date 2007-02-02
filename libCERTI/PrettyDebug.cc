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
// $Id: PrettyDebug.cc,v 4.0.2.1 2007/02/02 14:19:35 rousse Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "PrettyDebug.hh"

#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <stdarg.h>
#include <iostream>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>

using std::cout ;
using std::cerr ;
using std::endl ;

DebugOStream DebugOStream::nullOutputStream(cout);
DebugOStream PrettyDebug::defaultOutputStream(cerr);

//Fix this pointer to the default initialisation of pointers for your compiler
DebugOStream* PrettyDebug::nullOutputStreamPtr = 0 ; 
DebugOStream& PrettyDebug::nullOutputStream = DebugOStream::nullOutputStream;

std::string PrettyDebug::federateName_ = "" ;

// ----------------------------------------------------------------------------
/** Print the message to the default output ostream.
 *  This function does NOT add any trailing \n. */
void
PrettyDebug::Print(DebugOStream& theOutputStream,
                   const char* theHeaderMessage, const char * Message)
{
   char buffer[256] ;
   struct timeval tv;
   gettimeofday( &tv, NULL );

   sprintf( buffer, "HLALOG - %d.%06d - %s",tv.tv_sec,tv.tv_usec,federateName_.c_str() ) ;

    if (Message != NULL)
      theOutputStream << buffer << " - " << theHeaderMessage << "> " << Message;
    else
      theOutputStream << buffer << " - " << theHeaderMessage << pdSEmptyMessage;
}

// ----------------------------------------------------------------------------
/** Parse the environment variable value to find debug keys and enable
 *  debug levels.  The internal LEnvVar and LMessage variables must
 *  already have been set. */
void
PrettyDebug::ParseEnvString(const char *Name)
{
    unsigned int i;
    const char *pdInitMessage = " variable read, content = ";
    const char *pdTooLongInitMessage = "Env Var read(string too long).";
    char *Value = getenv(Name); // The value of the variable Name
    char  Key; // A character from the value compared to keys.
    char *Pos; // The position of Key in pdDebugKeys
    char *DebugKeys = (char *) pdDebugKeysString ; // needs a non const string
  
    for (i = pdUnused; i < pdLast + 1; i++) // Clear current level map
        Level_Map[i] = PrettyDebug::nullOutputStreamPtr;
  
    if (Value != 0) { // Print Debug Init Message
        int length = strlen(LEnvVar) + strlen(pdInitMessage) + strlen(Value);
        if (length < pdMaxMessageSize)
            cout << LEnvVar << pdInitMessage << Value << endl;
        else
            cout << LEnvVar << pdTooLongInitMessage << endl;
        
         // Compare each char of Value to content of the pgDebugKeys
         // string, to enable matching debug levels.
        for (i = 0; i < strlen(Value); i++) {
            Key = Value[i];
            Pos = strchr(DebugKeys, Key);
            if (Pos != 0)
                Level_Map[Pos - DebugKeys] = &(PrettyDebug::defaultOutputStream);
        }
    }
    return;
}

// ---------------------------------------------------------------------------- 
/** Constructor. Initialize the debug process according to the value
 *  of the 'Name' environment variable.  The 'Header' message is put
 *  in front of all printed debug messages. It can be a module name or
 *  whatever you need. */
PrettyDebug::PrettyDebug(const char *Name, const char *Header)
{
    if (Name == 0) {
        PrettyDebug::Print(PrettyDebug::defaultOutputStream, "", 
                        "Error in pgCDebug constructor, no Name specified.\n");
        exit(EXIT_FAILURE);
    }

    LEnvVar = strdup(Name);
    if (Header != 0)
        HeaderMessage = strdup(Header);
    else
        HeaderMessage = strdup("");

     //Initialisation de LMessage a 0. Il est alloue la premiere fois
     //dans la methode Out.
    LMessage = 0;

    if ((LEnvVar == 0) || (HeaderMessage == 0)) {
        PrettyDebug::Print(PrettyDebug::defaultOutputStream, "", 
                        "Error in pgCDebug constructor memory allocation.\n");
        exit(EXIT_FAILURE);
    }
    ParseEnvString(Name);
}

// ----------------------------------------------------------------------------
/** Destructor. */
PrettyDebug::~PrettyDebug()
{
    free(LEnvVar);
    if (LMessage != 0) {
        free(LMessage);
    }
    if (HeaderMessage != 0) {
        free(HeaderMessage);
    }
}

// ----------------------------------------------------------------------------
/** Return true if Level is enabled, else return false. */
bool
PrettyDebug::Mode(pdDebugLevel Level)
{
    return (Level_Map[Level] != PrettyDebug::nullOutputStreamPtr);
}

// ----------------------------------------------------------------------------
/** Enable the echoing of the given debug level with the provided
 *  ostream or modify the ostream if the debug level was already
 *  enabled.  @attention: only the address of the given ostream is
 *  stored, it is not copied so the ostream MUST exist until the
 *  destruction of the PrettyDebug or until the level is disabled */
void
PrettyDebug::enableDebugLevel(pdDebugLevel Level, DebugOStream& theOutputStream)
{
    Level_Map[Level] = &(theOutputStream);
    return;
}

// ----------------------------------------------------------------------------
/** Disable the echoing of the given debug level. */
void
PrettyDebug::disableDebugLevel(pdDebugLevel Level)
{
    Level_Map[Level] = PrettyDebug::nullOutputStreamPtr;
    return;
}
     
// ----------------------------------------------------------------------------
#ifndef NDEBUG
/** If Level is enabled, Message is sent to the DebugServer, preceded
   with the Header specified in the Constructor.  If the NDEBUG
   constant is defined, the Out method has beed declared inline, and
   its body set to {} (see PrettyDebug.hh).

   \attention: Probleme, rien ne garantit qu'on ne depassera pas le
   nombre max de char dans le vsprintf. Mieux vaut utiliser la
   syntaxe C++ 
*/
void
PrettyDebug::Out(pdDebugLevel Level, const char * Format, ...)
{
    const char *pdEmptyMessage = "Pretty Debug received an empty Message.";
    DebugOStream* theOutputStreamPtr = Level_Map[Level];

    //Warnings messages
    //if (LEnvVar == 0) cout << endl << "ENV VAR NULL CALLING OUT METHOD ";
    //if (HeaderMessage == 0) cout << "HEADER NULL CALLING OUT METHOD" << endl;

    if (theOutputStreamPtr != PrettyDebug::nullOutputStreamPtr) {
        if (Format != 0) { // Cat Header and Message strings 
            if (LMessage == 0) { // A final printed message is
                                 // Header+Message+\n(+\0) 
                //Optimisation, on pourrait peut-etre mettre LMessage
                //en static.  Mais il ne faudra pas que dans un D.Out
                //qu'on appelle un autre D.Out car autrement, seul un
                //message serait affiche.

                LMessage = (char *) malloc((pdMaxMessageSize+2) * sizeof(char));

                if (LMessage == 0) { 
                    PrettyDebug::Print(PrettyDebug::defaultOutputStream,
                                    HeaderMessage, 
                                    "Error in pgCDebug Out method while "
                                    "allocating initial memory for messages\n");
                    exit(EXIT_FAILURE);
                }
//                else
//                    PrettyDebug::Print(PrettyDebug::defaultOutputStream, 
//                                    HeaderMessage, "Allocated initial memory "
//                                    "for Message buffer.\n");
            }
            va_list argptr; // Variable Argument list, see stdarg.h
            va_start(argptr, Format);
            //Probleme, rien ne garantit qu'on ne depassera pas le
            //nombre max de char dans le vsprintf.
            vsprintf(LMessage, Format, argptr); 
            va_end(argptr);
        }
        else
            strcpy(LMessage, pdEmptyMessage);
        strcat(LMessage, "\n"); // Add trailing \n
        PrettyDebug::Print(*theOutputStreamPtr, HeaderMessage, LMessage);
    }
    return;
}

#endif // NDEBUG

// $Id: PrettyDebug.cc,v 4.0.2.1 2007/02/02 14:19:35 rousse Exp $
