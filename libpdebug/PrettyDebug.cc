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
// $Id: PrettyDebug.cc,v 3.1 2002/12/11 00:47:34 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "PrettyDebug.hh"

//---------------------------------------------------------------------------

const char *pdInitMessage        = " variable read, content = ";
const char *pdTooLongInitMessage = "Env Var read(string too long).";
const char *pdDebugKeys          =  pdDebugKeysString;
const char *pdEmptyMessage       = "Pretty Debug received an empty Message.";

// --------------------
// -- ParseEnvString --
// --------------------

// Parse the environment variable value to find debug keys and enable
// debug levels
// The internal LEnvVar and LMessage variables must already have been set.
void pdCDebug::ParseEnvString(const char *Name)
{
  unsigned int i;
  
  char *Value = getenv(Name); // The value of the variable Name
  char  Key;           	       // A character from the value compared to keys.
  char *Pos;		       // The position of Key in pdDebugKeys
  char *DebugKeys = strdup(pdDebugKeys); // strchr needs a non const string
  
  // Clear current level map
  for(i = pdUnused; i < pdLast + 1; i++)
    Level_Map [i] = false;
  
  if(Value != NULL) {
    
    // Print Debug Init Message
    int length = strlen(LEnvVar) + strlen(pdInitMessage) + strlen(Value);
    if(length < pdMaxMessageSize) {
      strcpy(LMessage + LHeaderSize, LEnvVar);
      strcat(LMessage, pdInitMessage);
      strcat(LMessage, Value);
      strcat(LMessage, "\n");
    } else {
      strcpy(LMessage + LHeaderSize, pdTooLongInitMessage);
    }
    
    // Compare each char of Value to content of the pgDebugKeys string,
    // to enable matching debug levels.
    for(i = 0; i < strlen(Value); i++) {
      Key = Value [i];
      Pos = strchr(DebugKeys, Key);
      if(Pos != NULL)
	Level_Map [Pos - DebugKeys] = true;
    }
  }
  
  free(DebugKeys);
  return;
}

// -----------------
// -- Constructor --
// -----------------
 
// Constructor. Initialize the debug process according to the value of
// the 'Name' environment variable. The 'Header' message is put in front
// of all printed debug messages. It can be a module name or whatever you need.

pdCDebug::pdCDebug(const char * Name, const char * Header)
{
  int MaxSize;
  if(Name == NULL) {
    pdServer.Print("Error in pgCDebug constructor, no Name specified.");
    exit(EXIT_FAILURE);
  }
  
  LEnvVar = strdup(Name);
  
  if(Header != NULL) {
    LHeaderSize = strlen(Header);
    MaxSize     = LHeaderSize + pdMaxMessageSize + 2;
    // A final printed message is Header+Message+\n(+\0)
    
    LMessage =(char *) malloc(MaxSize * sizeof(char));
    strcpy(LMessage, Header);
  }
  else {
    LHeaderSize = 0;
    MaxSize     = pdMaxMessageSize + 2;
    // A final printed message is Message+\n(+\0)
    
    LMessage =(char *) malloc(MaxSize * sizeof(char));
  }
  
  if((LEnvVar == NULL) ||(LMessage == NULL) ) {
    pdServer.Print("Error in pgCDebug constructor memory allocation.");
    exit(EXIT_FAILURE);
  }
  
  ParseEnvString(Name);
}

// ----------------
// -- Destructor --
// ----------------

pdCDebug::~pdCDebug()
{
  free(LEnvVar);
  free(LMessage);
}

// ----------
// -- Mode --
// ----------

// Return true if Level is enabled, else return false.

bool pdCDebug::Mode(pdDebugLevel Level)
{
  return Level_Map [Level];
}

// ---------
// -- Out --
// ---------

// If Level is enabled, Message is sent to the DebugServer,
// preceded with the Header specified in the Constructor.
// If the NO_PRETTYDEBUG constant is defined, the Out method has beed
// declared inline, and its body set to {}(see PrettyDebug.hh).

#ifndef NO_PRETTYDEBUG
void pdCDebug::Out(pdDebugLevel Level, const char * Format, ...)
{
  va_list argptr; // Variable Argument list, see stdarg.h

  if(Level_Map [Level] == true) {
    
    // Cat Header and Message strings
    if(Format != NULL) {
      va_start(argptr, Format);
      vsprintf(LMessage + LHeaderSize, Format, argptr);
      va_end(argptr);
    }
    else
      strcpy(LMessage + LHeaderSize, pdEmptyMessage);
    
    // Add trailing \n
    strcat(LMessage, "\n");
    
    pdServer.Print(LMessage);
  }
}
#endif










