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

#ifndef PRETTYDEBUG_HH
#define PRETTYDEBUG_HH

#include <fstream>
#include <iosfwd>
#include <string>

#include "certi.hh"

/** Do not use the pdUnused and pdLast Levels!!! Do not also specify
 * any value for the elements, because order is used and missing
 * value would cause crash.  pdLast must always be the last in the
 * enum.  Key(see pdDebugKeys)
 */
enum pdDebugLevel {
    pdUnused, /// Do not use! :
    pdAnswer, /// Server answer A
    pdCom, /// Communication C
    pdDebug, /// Debug D
    pdError, /// Error E
    pdGendoc, /// Gendoc G
    pdInit, /// Initialization I
    pdMessage, /// Message Type M
    pdProtocol, /// Protocol P
    pdRegister, /// Object Registration R
    pdRequest, /// Client Request S
    pdTerm, /// Terminate T
    pdWarning, /// Warning W
    pdExcept, /// Exceptions X
    pdTrace, /// Trace Z
    pdLast /// Do not use \0
};

/** The following keys are used in the environment variable to enable
 * debug level.  For example, if ENVVAR=D:C, the pdDebug and the pdCom
 * debug level are enabled, and all others are not enabled.
 * Pay attention to keep this in the same order as the enum above.
 */

#define pdDebugKeysString ":ACDEGIMPRSTWXZ"

/* This is the standard way to issue debug messages into the debug streams.
 * Use that as follows:
 *
 * static PrettyDebug D("Name", "Header");
 * //...
 * 
 * Debug(D, pdDebug) << "This is a usual std::ostream shich could be used like that\n";
 *
 * Note that this is about the least runtime overhead you can have with dynamic debug messages
 * as the only thing that happens exactly once is the check against the null pointer in
 * the stream return. That is even optimized into fast inline path and a more expensive part
 * in case we really need the some output.
 * The next thing is that it can be used with *everything* that could be output to
 * an std::ostream without the need to maintain an extra debug stream type.
 * In the current case this make certi compile on win64 :)
 */
#define Debug(pd, level)                                                                                               \
    if (std::ostream* os = pd.getStream(level, __LINE__))                                                              \
    *os

class CERTI_EXPORT PrettyDebug {
public:
    /** Constructor.
     *
     * Initialize the debug process according to the value
     * of the 'name' environment variable.  The 'header' message is put
     * in front of all printed debug messages. It can be a module name or
     * whatever you need.
     */
    PrettyDebug(const std::string& name, const std::string& header);
    ~PrettyDebug() = default;

    /** Enable the echoing of the given debug level with the default
     * ostream or modify the ostream if the debug level was already
     * enabled.
     */
    void enableDebugLevel(pdDebugLevel level);

    /** Enable the echoing of the given debug level with the provided
     * ostream or modify the ostream if the debug level was already
     * enabled.  @attention: only the address of the given ostream is
     * stored, it is not copied so the ostream MUST exist until the
     * destruction of the PrettyDebug or until the level is disabled
     */
    void enableDebugLevel(pdDebugLevel level, std::ostream& stream);

    /** Disable the echoing of the given debug level. */
    void disableDebugLevel(pdDebugLevel level);

    /** Get the ostream to print debug messages to.
     * Returns nullptr if debugging is not enabled for the given level.
     */
    std::ostream* getStream(pdDebugLevel level, const int line);

    static void setFederateName(const std::string& federate_name);

    /** If level is enabled, Message is sent to the DebugServer, preceded
     * with the Header specified in the Constructor.  If the NDEBUG
     * constant is defined, the Out method does not do anything
     */
    [[deprecated("Use Debug(X, level) instead of X.Out(level)")]] void Out(pdDebugLevel level, const char* format, ...);

private:
    static void initStreams();

    static std::ostream the_default_output_stream;
    static std::string the_federate_name;
    static std::filebuf* the_file_buffer;

    /** Print the message header for the given level and return the apropriate stream
     * If level is enabled, print the debug header for this level and return
     * a std::ostream pointer to print debug messates to. If the given level is not
     * active returns a null pointer.
     */
    std::ostream* getStreamPrintHeader(pdDebugLevel level, const int line);

    std::string my_name{""};
    std::string my_header{""};
    std::ostream* my_streams[pdLast]{nullptr};
};

#endif // PRETTYDEBUG_HH

// $Id: PrettyDebug.hh,v 4.11 2013/09/24 14:27:57 erk Exp $
