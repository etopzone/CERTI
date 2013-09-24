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
// ----------------------------------------------------------------------------

#include "PrettyDebug.hh"

#include <iostream>
#include <fstream>
#include <iomanip>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
# include <time.h>
# include <sys/timeb.h>
# include <process.h>
#else
# include <sys/time.h>
# include <sys/types.h>
# include <unistd.h>
#endif

std::ostream PrettyDebug::_defaultOutputStream(std::cerr.rdbuf());
std::string PrettyDebug::_federateName;
std::filebuf* PrettyDebug::fb = NULL;

void PrettyDebug::initStreams() {
    if (!fb) {
        const char *value = getenv("CERTI_DEBUG_FILE");
        if  (value) {
            std::ostringstream filename;
            int pid
#ifdef _WIN32
              = _getpid();
#else
              =(int)getpid();
#endif
            filename << value <<"-" <<pid;
            fb = new std::filebuf();
            fb->open(filename.str().c_str(), std::ios_base::out | std::ios_base::trunc);
        }
    }
    /* replace buffer stream if we have one */
    if (fb) {
        _defaultOutputStream.rdbuf(fb);
    }
}

// ----------------------------------------------------------------------------
/** Constructor. Initialize the debug process according to the value
 *  of the 'name' environment variable.  The 'header' message is put
 *  in front of all printed debug messages. It can be a module name or
 *  whatever you need. */
PrettyDebug::PrettyDebug(const char* name, const char* header)
{
    PrettyDebug::initStreams();

    if (header)
        _header = header;

    for (unsigned i = pdUnused; i < unsigned(pdLast); ++i) {
        _streams[i] = 0;
    }

    // note that if we do not have a name, we cannot enable any debug message ...
    if (name) {
        const char *value = getenv(name);
        if (value) {
            // Compare each char of Value to content of the pgDebugKeys
            // string, to enable matching debug levels.
            char debugKeys[] = pdDebugKeysString;
            unsigned valueLen = strlen(value);
            for (unsigned i = 0; i < valueLen; ++i) {
                const char* pos = strchr(debugKeys, value[i]);
                if (pos) {
                    _streams[pos - debugKeys] = &_defaultOutputStream;
                }
            }
        }
    }
}

// ----------------------------------------------------------------------------
/** Destructor. */
PrettyDebug::~PrettyDebug()
{
}

// ----------------------------------------------------------------------------
/** Enable the echoing of the given debug level with the default
 *  ostream or modify the ostream if the debug level was already
 *  enabled. */
void
PrettyDebug::enableDebugLevel(pdDebugLevel level)
{
    enableDebugLevel(level, std::cerr);
}

// ----------------------------------------------------------------------------
/** Enable the echoing of the given debug level with the provided
 *  ostream or modify the ostream if the debug level was already
 *  enabled.  @attention: only the address of the given ostream is
 *  stored, it is not copied so the ostream MUST exist until the
 *  destruction of the PrettyDebug or until the level is disabled */
void
PrettyDebug::enableDebugLevel(pdDebugLevel level, std::ostream& stream)
{
    if (unsigned(pdLast) <= unsigned(level))
        return;
    _streams[level] = &stream;
}

// ----------------------------------------------------------------------------
/** Disable the echoing of the given debug level. */
void
PrettyDebug::disableDebugLevel(pdDebugLevel level)
{
    if (unsigned(pdLast) <= unsigned(level))
        return;
    _streams[level] = 0;
}

#ifndef NDEBUG
// ----------------------------------------------------------------------------
/** If level is enabled, Message is sent to the DebugServer, preceded
   with the Header specified in the Constructor.  If the NDEBUG
   constant is defined, the Out method has beed declared inline, and
   its body set to {} (see PrettyDebug.hh).
*/
void
PrettyDebug::Out(pdDebugLevel level, const char *format, ...)
{
    // Fast return if streams are not enabled for this level.
    std::ostream* stream = getStreamPrintHeader(level);
    if (!stream)
        return;

    if (!format) {
        *stream << "Pretty Debug received an empty Message.\n";
    } else {
        char buffer[512];

        va_list argptr;
        va_start(argptr, format);
        vsnprintf(buffer, sizeof(buffer), format, argptr);
        va_end(argptr);

        *stream << buffer << '\n';
    }
}
#endif

// ----------------------------------------------------------------------------
/** If level is enabled, print the debug header for this level and return
    a std::ostream pointer to print debug messates to. If the given level is not
    active returns a null pointer.
*/
std::ostream*
PrettyDebug::getStreamPrintHeader(pdDebugLevel level)
{
    // Print a standard message header and return the stream where the rest should go.

    std::ostream* stream = _streams[level];
    if (!stream)
        return 0;

    *stream << "HLALOG - ";

    // System dependent timestamp
#ifdef _WIN32
    struct _timeb timebuffer;
    _ftime(&timebuffer);
    *stream << timebuffer.time << '.' << std::setw(3) << std::right
            << std::setfill('0') << timebuffer.millitm;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    *stream << tv.tv_sec << '.' << std::setw(6) << std::right
            << std::setfill('0') << tv.tv_usec;
#endif

    *stream << ' ' << _federateName << " - " << _header << "> ";

    return stream;
}

// $Id: PrettyDebug.cc,v 4.13 2013/09/24 14:27:50 erk Exp $
