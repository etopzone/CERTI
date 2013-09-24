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

#ifdef NO_PRETTYDEBUG ///< Deprecated, use NDEBUG
#define NDEBUG
#endif

#include "certi.hh"
#include <iosfwd>
#include <string>
#include <fstream>

/** Do not use the pdUnused and pdLast Levels!!! Do not also specify
 *  any value for the elements, because order is used and missing
 *  value would cause crash.  pdLast must always be the last in the
 *  enum.  Key(see pdDebugKeys) */
enum pdDebugLevel  {pdUnused, /**< Do not use! : */
                    pdAnswer, /**< Server answer A */
                    pdCom, /**< Communication C */
                    pdDebug, /**< Debug D */
                    pdError, /**< Error E */
                    pdGendoc, /**< Gendoc G */
                    pdInit, /**< Initialization I */
                    pdMessage, /**< Message Type M */
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
#define pdDebugKeysString ":ACDEGIMPRSTWXZ"

// This is the standard way to issue debug messages into the debug streams.
// Use that as follows:
//
// static PrettyDebug D("Name", "Header");
//
// [...]
//
//   Debug(D, pdDebug) << "This is a usual std::ostream shich could be used like that\n";
//
// Note that this is about the least runtime overhead you can have with dynamic debug messages
// as the only thing that happens exactly once is the check against the null pointer in
// the stream return. That is even optimized into fast inline path and a more expensive part
// in case we really need the some output.
// The next thing is that it can be used with *everything* that could be output to
// an std::ostream without the need to maintain an extra debug stream type.
// In the current case this make certi compile on win64 :)
#define Debug(pd, level) \
if (std::ostream* os = pd.getStream(level)) *os

//---------------------------------------------------------------------------
class CERTI_EXPORT PrettyDebug {
public:
    PrettyDebug(const char* name, const char* header);
    ~PrettyDebug();

    void enableDebugLevel(pdDebugLevel Level);
    void enableDebugLevel(pdDebugLevel Level, std::ostream& stream);
    void disableDebugLevel(pdDebugLevel Level);

    // Get the ostream to print debug messages to.
    // Returns zero if debugging is not enabled for the given level.
    std::ostream* getStream(pdDebugLevel level)
    {
        // make sure this is an unsigned value to so just one check is sufficient
        if (unsigned(pdLast) <= unsigned(level))
            return 0;
        if (!_streams[level])
            return 0;
        return getStreamPrintHeader(level);
    }

    static void setFederateName(const std::string& inName)
    { _federateName = inName; }
    static void setFederateName(const char* inName)
    { if (inName) _federateName = inName; }

#ifdef NDEBUG
    void Out(pdDebugLevel level, const char *format, ...) {}
#else
    void Out(pdDebugLevel level, const char *format, ...);
    void Mes(pdDebugLevel level, const char type, const short testMess, const char *context);
#endif

private:
    // Print the message header for the given level and return the apropriate stream
    std::ostream* getStreamPrintHeader(pdDebugLevel level);

    std::string _name;
    std::string _header;
    std::ostream* _streams[pdLast];

    static void initStreams();
    static std::ostream _defaultOutputStream;
    static std::string _federateName;
    static std::filebuf* fb;
};

#endif // PRETTYDEBUG_HH

// $Id: PrettyDebug.hh,v 4.11 2013/09/24 14:27:57 erk Exp $
