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

#include <fstream>
#include <iomanip>
#include <iostream>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <process.h>
#include <sys/timeb.h>
#include <time.h>
#else
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif

std::ostream PrettyDebug::the_default_output_stream(std::cerr.rdbuf());
std::string PrettyDebug::the_federate_name = "";
std::filebuf* PrettyDebug::the_file_buffer = nullptr;

PrettyDebug::PrettyDebug(const std::string& name, const std::string& header) : my_name{name}, my_header{header}
{
    PrettyDebug::initStreams();

    // note that if we do not have a name, we cannot enable any debug message ...
    if (!my_name.empty()) {
        const char* value = getenv(my_name.c_str());
        if (value) {
            // Compare each char of Value to content of the pgDebugKeys
            // string, to enable matching debug levels.
            char debugKeys[] = pdDebugKeysString;
            unsigned valueLen = strlen(value);
            for (unsigned i = 0; i < valueLen; ++i) {
                const char* pos = strchr(debugKeys, value[i]);
                if (pos) {
                    my_streams[pos - debugKeys] = &the_default_output_stream;
                }
            }
        }
    }
}

void PrettyDebug::enableDebugLevel(pdDebugLevel level)
{
    enableDebugLevel(level, std::cerr);
}

void PrettyDebug::enableDebugLevel(pdDebugLevel level, std::ostream& stream)
{
    if (unsigned(pdLast) <= unsigned(level)) {
        return;
    }

    my_streams[level] = &stream;
}

void PrettyDebug::disableDebugLevel(pdDebugLevel level)
{
    if (unsigned(pdLast) <= unsigned(level)) {
        return;
    }

    my_streams[level] = nullptr;
}

std::ostream* PrettyDebug::getStream(pdDebugLevel level, const int line)
{
    // make sure this is an unsigned value to so just one check is sufficient
    if (unsigned(pdLast) <= unsigned(level)) {
        return nullptr;
    }
    if (!my_streams[level]) {
        return nullptr;
    }
    return getStreamPrintHeader(level, line);
}

void PrettyDebug::setFederateName(const std::string& federate_name)
{
    the_federate_name = federate_name;
}

void PrettyDebug::Out(pdDebugLevel level, const char* format, ...)
{
#ifndef NDEBUG
    // Fast return if streams are not enabled for this level.
    std::ostream* stream = getStreamPrintHeader(level, 0);
    if (!stream) {
        return;
    }

    if (!format) {
        *stream << "Pretty Debug received an empty Message.\n";
    }
    else {
        char buffer[512];

        va_list argptr;
        va_start(argptr, format);
        vsnprintf(buffer, sizeof(buffer), format, argptr);
        va_end(argptr);

        *stream << buffer << '\n';
    }
#endif
}

std::ostream* PrettyDebug::getStreamPrintHeader(pdDebugLevel level, const int line)
{
    // Print a standard message header and return the stream where the rest should go.

    std::ostream* stream = my_streams[level];
    if (!stream) {
        return nullptr;
    }

    *stream << "HLALOG - ";

// System dependent timestamp
#ifdef _WIN32
    struct _timeb timebuffer;
    _ftime(&timebuffer);
    *stream << timebuffer.time << '.' << std::setw(3) << std::right << std::setfill('0') << timebuffer.millitm;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    *stream << tv.tv_sec << '.' << std::setw(6) << std::right << std::setfill('0') << tv.tv_usec;
#endif

    *stream << ' ' << the_federate_name << " - " << my_header << ":" << line << " > ";

    return stream;
}

void PrettyDebug::initStreams()
{
    if (!the_file_buffer) {
        const char* value = getenv("CERTI_DEBUG_FILE");
        if (value) {
            std::ostringstream filename;
            int pid
#ifdef _WIN32
                = _getpid();
#else
                = (int) getpid();
#endif
            filename << value << "-" << pid;
            the_file_buffer = new std::filebuf();
            the_file_buffer->open(filename.str().c_str(), std::ios_base::out | std::ios_base::trunc);
        }
    }
    /* replace buffer stream if we have one */
    if (the_file_buffer) {
        the_default_output_stream.rdbuf(the_file_buffer);
    }
}
