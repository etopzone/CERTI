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
// $Id: PrettyDebug.hh,v 3.1 2002/12/11 00:47:34 breholee Exp $
// ---------------------------------------------------------------------------

// PrettyDebug Include File
// ************************
//
// PrettyDebug is here to help you filter or inhibit the debug messages
// you need to develop your applications.
// 
// To use PrettyDebug is simple. First include this file :
//   #include "PrettuDebug.h"
//
// In your main loop, create a CPrettyDebug Object:
//   static pdCDebug D("MY_ENV_VAR", "(module.c) - ");
//
// "MY_ENV_VAR" is the name of the environment variable PrettyDebug is going 
// to search for. If it is found, its content will set or inhibit the
// debug levels.(see below)
//
// "(module.c) - " is a header that will be printed in front of all debug
// messages. You may want to give the same length to all your headers.
//
// Then each time you need to print a message, just type:
//  D.Out(pdWarning, "The warning message.");
//
// pdwarning is one of the pdDebugLevel available.(see below). You can change 
// this list of levels to fit your needs.
// 
// The second argument is the message you need to print. It should not end
// with a trailing \n. Its size must be shorted than the pdMaxMessageSize
// constant defined below

#ifndef PrettyDebugH
#define PrettyDebugH

#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "PrettyDebugS.hh"

// Les cas a traiter __CONSOLE__, __cplusplus__, ...

// If your compiler knows about the 'bool' type and the 'true' and 'false' 
// values, then you should uncomment the following line.

//#define __BOOL_IS_DEFINED //on supprime cette ligne pour la demo avec rting

// If you want to save memory and gain speed, you can turn PrettyDebug OFF
// by defining the NO_PRETTYDEBUG constant.

// #define NO_PRETTYDEBUG

//---------------------------------------------------------------------------
// Pretty Debug Basic types
//---------------------------------------------------------------------------
typedef unsigned TDebugLevel;

// #ifndef __BOOL_IS_DEFINED
// typedef unsigned char bool;
// #define true  1
// #define false 0
// #endif

//---------------------------------------------------------------------------
// Pretty Debug Constants
//---------------------------------------------------------------------------

#define pdMaxMessageSize 255  // greater then pdTooLongInitMessage length!

// Do not use the pdUnused and pdLast Levels!!! Do not also specify any
// value for the elements, because order is used and missing value would
// cause crash. pdLast must always be the last in the enum.
//						     Key(see pdDebugKeys)
enum pdDebugLevel  {pdUnused, // Do not use!  		:
		    pdAnswer, // Server answer          A
		    pdCom,    // Communication  	C
		    pdDebug,  // Debug 			D
		    pdError,  // Error		      	E
		    pdInit,   // Initialization         I
		    pdProtocol,// Protocol              P
		    pdRegister,// Object Registration   R
		    pdRequest,// Client Request         S
		    pdTerm,   // Terminate              T
		    pdWarning,// Warning                W
		    pdExcept, // Exceptions             X
		    pdTrace,  // Trace                  Z
		    pdLast};  // Do not use     	\0


// The following keys are used in the environment variable to
// enable debug level. For example, if ENVVAR=D:C, the pdDebug and
// the pdCom debug level are enabled, and all others are not enabled.

// KEEP THE SAME ORDER AS IN THE pdDebugLevel ENUM!

#define pdDebugKeysString ":ACDEIPRSTWXZ\0"

//---------------------------------------------------------------------------
// CDebug Class
//---------------------------------------------------------------------------

class pdCDebug {
 private:
  char * LEnvVar;
  // Name of the environment variable to look for.

  char * LMessage;
  // The container of all printed debug messages.
  // Start with the Header specified at construction time

  int    LHeaderSize;
  // Size of thhe header in LMessage.
  
  bool Level_Map [pdLast + 1];
  // If Level_Map[Level] == true, then the debug message
  // must be printed.
  
  void ParseEnvString(const char * Name);
  // Parse the environment variable value to find debug keys
  // and enable debug levels

  static pdCDebugServer pdServer ;
public:

  pdCDebug(const char * Name, const char * Header);
  // Constructor. Initialize the debug process according to
  // the value of the 'Name' environment variable. The 
  // 'Header' message is put in front of all printed debug
  // messages. It can be a module name or whatever you need.
  
  ~pdCDebug();
  // Destructor
  
  bool Mode(pdDebugLevel Level);
  // Return true if Level is enabled, else return false.
  
#ifdef NO_PRETTYDEBUG
  inline void Out(pdDebugLevel Level, const char *Format, ...) {};
#else
  void Out(pdDebugLevel Level, const char *Format, ...);
#endif
  // If Level is enabled, Message is sent to the DebugServer,
  // preceded with the Header specified in the Constructor.
  // Message should NOT end with a \n.
  // Message size should be shorter than or equal to pdMaxMessageSize.

};

//---------------------------------------------------------------------------
#endif
