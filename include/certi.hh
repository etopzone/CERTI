// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
//
// This program is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// ----------------------------------------------------------------------------

#ifndef CERTI_HH_INCLUDED
#define CERTI_HH_INCLUDED
#if defined(_WIN32)
    #ifdef _MSC_VER
        // MSVC Pragma
        // Visual C++ does not support declarations using exceptions specification
        #pragma warning(disable: 4290)
        #pragma warning(disable: 4251)
        #pragma message("MSVC specific - disabled pragma 4251 and 4290 (exception specification ignored)")
        #define _CRT_SECURE_NO_DEPRECATE
        #pragma message("JvY - Used _CRT_SECURE_NO_DEPRECATE")
        // Visual C++ 2012 or greater now includes stdint
        // http://stackoverflow.com/questions/126279/c99-stdint-h-header-and-ms-visual-studio
        #if _MSC_VER >= 1700
            #include <stdint.h>
        #else
            #ifndef FAKED_INTTYPES_DEFINED
            #define FAKED_INTTYPES_DEFINED
            typedef unsigned __int64    uint64_t;
            typedef __int64             int64_t;
            typedef unsigned __int32    uint32_t;
            typedef __int32             int32_t;
            typedef unsigned __int16    uint16_t;
            typedef __int16             int16_t;
            typedef unsigned __int8     uint8_t;
            typedef __int8              int8_t;
        #endif
        #endif
        #ifdef _M_X64
           #define  CERTI_INT64_CONSTANT(val)  (val##L)
           #define  CERTI_INT64_FORMAT         "l"
        #else
           #define  CERTI_INT64_CONSTANT(val)  (val##LL)
           #define  CERTI_INT64_FORMAT         "ll"
        #endif
        typedef float			float_t;
        typedef double			double_t;
        #ifndef __func__
        #define __func__ "<unknown>"
        #endif
    #else
       #include <inttypes.h>
       #ifdef __x86_64__
          #define  CERTI_INT64_CONSTANT(val)  (val##L)
          #define  CERTI_INT64_FORMAT         "l"
       #else
          #define  CERTI_INT64_CONSTANT(val)  (val##LL)
          #define  CERTI_INT64_FORMAT         "ll"
       #endif
    #endif
    #define ANY_DLL_EXPORT __declspec(dllexport)
    #define ANY_DLL_IMPORT __declspec(dllimport)
    #define ANY_DLL_LOCAL
#else
    #include <inttypes.h>
    #ifdef __x86_64__
       #define  CERTI_INT64_CONSTANT(val)  (val##L)
       #define  CERTI_INT64_FORMAT         "l"
    #else
       #define  CERTI_INT64_CONSTANT(val)  (val##LL)
       #define  CERTI_INT64_FORMAT         "ll"
    #endif
    
    #ifndef ANY_DLL_EXPORT
        #if (__GNUC__ >= 4)
        #define ANY_DLL_EXPORT __attribute__ ((visibility("default")))
        #define ANY_DLL_IMPORT __attribute__ ((visibility("default")))
        #define ANY_DLL_LOCAL  __attribute__ ((visibility("hidden")))
    #else
        #define ANY_DLL_EXPORT
        #define ANY_DLL_IMPORT
        #define ANY_DLL_LOCAL
        #endif
    #endif
#endif

#if defined(CERTI_EXPORTS)
    #define CERTI_EXPORT ANY_DLL_EXPORT
#else
    #define CERTI_EXPORT ANY_DLL_IMPORT
#endif

#define CERTI_UINT64_SWAP_LE_BE_CONSTANT(val)	((uint64_t) ( \
      (((uint64_t) (val) &						\
	(uint64_t) CERTI_INT64_CONSTANT(0x00000000000000ffU)) << 56) |	\
      (((uint64_t) (val) &						\
	(uint64_t) CERTI_INT64_CONSTANT(0x000000000000ff00U)) << 40) |	\
      (((uint64_t) (val) &						\
	(uint64_t) CERTI_INT64_CONSTANT(0x0000000000ff0000U)) << 24) |	\
      (((uint64_t) (val) &						\
	(uint64_t) CERTI_INT64_CONSTANT(0x00000000ff000000U)) <<  8) |	\
      (((uint64_t) (val) &						\
	(uint64_t) CERTI_INT64_CONSTANT(0x000000ff00000000U)) >>  8) |	\
      (((uint64_t) (val) &						\
	(uint64_t) CERTI_INT64_CONSTANT(0x0000ff0000000000U)) >> 24) |	\
      (((uint64_t) (val) &						\
	(uint64_t) CERTI_INT64_CONSTANT(0x00ff000000000000U)) >> 40) |	\
      (((uint64_t) (val) &						\
	(uint64_t) CERTI_INT64_CONSTANT(0xff00000000000000U)) >> 56)))

#ifdef HOST_IS_BIG_ENDIAN
  #define CERTI_UINT64_SWAP_LE_BE(val) (CERTI_UINT64_SWAP_LE_BE_CONSTANT(val))
  #define CERTI_UINT64_TO_BE(val)	   (val)
  #define CERTI_UINT64_TO_LE(val)	   (CERTI_UINT64_SWAP_LE_BE (val))
#else
  #define CERTI_UINT64_SWAP_LE_BE(val) (CERTI_UINT64_SWAP_LE_BE_CONSTANT(val))
  #define CERTI_UINT64_TO_BE(val)	   (CERTI_UINT64_SWAP_LE_BE (val))
  #define CERTI_UINT64_TO_LE(val)      (val)
#endif

#define CERTI_UINT64_FROM_BE(val)	            (CERTI_UINT64_TO_BE(val))
#define CERTI_UINT64_FROM_LE(val)	            (CERTI_UINT64_TO_LE(val))
#define CERTI_ENCODE_DOUBLE_TO_UINT64BE(val)    (CERTI_UINT64_TO_BE   (*(uint64_t*)(val)))
#define CERTI_DECODE_DOUBLE_FROM_UINT64BE(val)  (CERTI_UINT64_FROM_BE (*(uint64_t*)(val)))

#include <string>
#include <vector>
#include <sstream>

/**
 * @defgroup libCERTI The CERTI library.
 * @ingroup CERTI_Libraries
 * The CERTI library contains CERTI specific HLA implementation
 */


namespace certi {

typedef uint32_t ExtentIndex;

typedef uint32_t OrderType;
typedef uint32_t TransportType;
typedef uint8_t RtiVersion;

typedef double TickTime;

typedef std::string ObjectName_t ;
typedef std::vector<char> AttributeValue_t;
typedef std::vector<char> ParameterValue_t;

enum ResignAction {
    RELEASE_ATTRIBUTES = 1,
    DELETE_OBJECTS,
    DELETE_OBJECTS_AND_RELEASE_ATTRIBUTES,
    NO_ACTION
};

enum ObjectRemovalReason {
    OUT_OF_REGION = 1,
    OBJECT_DELETED,
    NO_LONGER_SUBSCRIBED
};

// Constants (former HLA constants)
const TransportType RELIABLE = 1 ;
const TransportType BEST_EFFORT = 2 ;

const OrderType RECEIVE = 1 ;
const OrderType TIMESTAMP = 2 ;

const RtiVersion HLA_1_3 = 1;
const RtiVersion IEEE_1516_2000 = 2;
const RtiVersion IEEE_1516_2010 = 3;

// Constants
const int MAX_BACKLOG = 256 ;


/**
 * Helper class to simplify string construction. Implemented as
 * a stringstream wrapper.
 *
 * For example:
 * throw AttributeNotDefined(stringize() << "value: " << number);
 */
template<typename C>
struct basic_stringize
{
  template<typename T>
  basic_stringize<C> & operator << (const T& t)
  {
    m_s << t;
    return *this;
  }

  // note: must not return reference
  [[deprecated("Use std::to_string instead")]] operator const std::basic_string<C>() const
  {
    return m_s.str();
  }

private:
  std::basic_stringstream<C> m_s;
};

typedef basic_stringize<char> stringize;
typedef basic_stringize<wchar_t> wstringize;

} // namespace certi

// Define this if you want the complete object and interaction tree to
// be displayed as the FED file is read.
// #define CREAD_DISPLAYS_ROOTOBJ

// If you want statistics to be printed at the end of the execution,
// uncomment the next line.
#define RTI_PRINTS_STATISTICS

// You can comment the next line out if you don't want to use Multicast.
// #define FEDERATION_USES_MULTICAST

// If this macro is defined, some messages will be sent on UDP sockets
// by the RTIG to the RTIAs. Some messages types are NULL messages, and
// all Reflect Attributes Values messages, and Receive Interaction messages,
// even those with data asking for a "RELIABLE" service level.
// #define HLA_USES_UDP

// The next macro must contain the path name of the Audit File. It should
// be an absolute path, but it may be a relative path for testing reasons.
#define RTIG_AUDIT_FILENAME "RTIG.log"

// Define the lower audit level you need, from AUDIT_MIN_LEVEL(0, all)
// to AUDIT_MAX_LEVEL(10, min audit logging).
// Level 0 : NULL messages(time synchronization)
// Level 1 : Update Attribute Values messages
// Level 2 : Send Interaction messages
// Level 6 : Register / Delete Objects messages
// Level 7 : Publish / Subscribe messages
// Level 8 : Pause, Resume, Set Time regulating/constrained messages
// Level 9 : Create/Join/Resign/Destroy Federation msgs, Connections.
// Level 10: RTIG and Audit start/stop.
#define AUDIT_CURRENT_LEVEL 5

// ------------------------------
// -- GSSAPI Security Services --
// ------------------------------

// Comment out HLA_USES_GSSAPI to choose NOT to use GSSAPI services.
//#define HLA_USES_GSSAPI

// Server Principal Name
//(must be like "<rtip principal name>@<network machine name>")
#define HLA_SERVER_PRINCIPAL_NAME "RTIG@server"

// Expiration delay for GSSAPI sessions(must be greater then the longer
// lifetime of a federate).[In seconds]
#define HLA_GSS_SESSION_DURATION 3600

// You can use GSSAPI at least to authenticate federates(needed to use
// security levels), to sign all messages(source authentication), or
// to sign AND encrypt message.[last option]
// Just choose one of the three options below and affect it to the
// RTI_GSSAPI_USAGE macro.
#define RTI_GSS_NOTHING 0
#define RTI_GSS_SIGN_ONLY 1
#define RTI_GSS_ENCRYPT 2

#define RTI_GSSAPI_USAGE RTI_GSS_NOTHING

// Type of Security services needed : Can be any combination(OR) of the
// following flags(see your GSSAPI documentation for further details) :
//
// GSS_C_DELEG_FLAG Credential delegation(not supported by HLA anyway)
// GSS_C_MUTUAL_FLAG Mutual authentication
// GSS_C_REPLAY_FLAG Replay detection
// GSS_C_SEQUENCE_FLAG Out of sequence message detection.
//
// Note: Currently, no option is suppported by our default GSSAPI mechanism.
#define HLA_GSS_FLAGS 0


/*
 * Basic bit swapping functions
 */
#define CERTI_UINT16_SWAP_BYTES(val)	((uint16_t) ( \
    (((uint16_t) (val) & (uint16_t) 0x00ffU) << 8) |  \
    (((uint16_t) (val) & (uint16_t) 0xff00U) >> 8)))

#define CERTI_UINT32_SWAP_BYTES(val)	((uint32_t) (     \
    (((uint32_t) (val) & (uint32_t) 0x000000ffU) << 24) | \
    (((uint32_t) (val) & (uint32_t) 0x0000ff00U) <<  8) | \
    (((uint32_t) (val) & (uint32_t) 0x00ff0000U) >>  8) | \
    (((uint32_t) (val) & (uint32_t) 0xff000000U) >> 24)))

#define CERTI_UINT64_SWAP_BYTES(val)	((uint64_t) (                   \
      (((uint64_t) (val) &						\
	(uint64_t) CERTI_INT64_CONSTANT(0x00000000000000ffU)) << 56) |	\
      (((uint64_t) (val) &						\
	(uint64_t) CERTI_INT64_CONSTANT(0x000000000000ff00U)) << 40) |	\
      (((uint64_t) (val) &						\
	(uint64_t) CERTI_INT64_CONSTANT(0x0000000000ff0000U)) << 24) |	\
      (((uint64_t) (val) &						\
	(uint64_t) CERTI_INT64_CONSTANT(0x00000000ff000000U)) <<  8) |	\
      (((uint64_t) (val) &						\
	(uint64_t) CERTI_INT64_CONSTANT(0x000000ff00000000U)) >>  8) |	\
      (((uint64_t) (val) &						\
	(uint64_t) CERTI_INT64_CONSTANT(0x0000ff0000000000U)) >> 24) |	\
      (((uint64_t) (val) &						\
	(uint64_t) CERTI_INT64_CONSTANT(0x00ff000000000000U)) >> 40) |	\
      (((uint64_t) (val) &						\
	(uint64_t) CERTI_INT64_CONSTANT(0xff00000000000000U)) >> 56)))
#endif // CERTI_HH_INCLUDED

// $Id: certi.hh,v 3.54 2013/09/24 14:28:49 erk Exp $
