// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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
// $Id: certi.hh,v 3.38.4.1 2009/01/05 13:34:50 gotthardp Exp $
// ----------------------------------------------------------------------------
#include <vector>

#ifndef CERTI_HH_INCLUDED
#define CERTI_HH_INCLUDED
#if defined(_WIN32)
    #include <windows.h>
    #include <algorithm>
    #include <process.h>
    #define	sleep(a)	      Sleep(a * 1000)
    #define	usleep(a)	      Sleep(a / 1000)
    typedef unsigned short        ushort;
    typedef	int	              pid_t;
    #define  strcasecmp           strcmp
    #define	STAT_FUNCTION         _stat
    #define	STAT_STRUCT           struct _stat
    /* TCP/IP related typedef and includes */
    #ifndef _WINSOCK2API_
        #ifndef _WINSOCKAPI_
	   #include <winsock2.h>
	#endif
    #endif
    typedef u_long			in_addr_t;
    typedef unsigned short		in_port_t;
    #ifdef _MSC_VER
        typedef unsigned __int64        uint64_t;
        typedef __int64			 int64_t;
        typedef unsigned __int32	uint32_t;
        typedef __int32			 int32_t;
        typedef unsigned __int16	uint16_t;
        typedef __int16			 int16_t;
        typedef unsigned __int8	         uint8_t;
        typedef __int8			  int8_t;
        #ifdef _M_X64
           #define  CERTI_INT64_CONSTANT(val)  (val##L)
           #define  CERTI_INT64_FORMAT         "l"
        #else
           #define  CERTI_INT64_CONSTANT(val)  (val##LL)
           #define  CERTI_INT64_FORMAT         "ll"
        #endif
        #ifndef __func__
        #define __func__ "<unknown>"
        #endif
    #else
       #include <stdint.h>
       #ifdef __x86_64__
          #define  CERTI_INT64_CONSTANT(val)  (val##L)
          #define  CERTI_INT64_FORMAT         "l"
       #else
          #define  CERTI_INT64_CONSTANT(val)  (val##LL)
          #define  CERTI_INT64_FORMAT         "ll"
       #endif
    #endif
    #if defined(CERTI_EXPORTS)
		#define CERTI_EXPORT __declspec(dllexport)
	#else
		#define CERTI_EXPORT __declspec(dllimport)
	#endif
#else
    #include <stdint.h>
    #define  STAT_FUNCTION		stat
    #define  STAT_STRUCT		struct stat
    #ifdef __x86_64__
       #define  CERTI_INT64_CONSTANT(val)  (val##L)
       #define  CERTI_INT64_FORMAT         "l"
    #else
       #define  CERTI_INT64_CONSTANT(val)  (val##LL)
       #define  CERTI_INT64_FORMAT         "ll"
    #endif
	#define CERTI_EXPORT
#endif

#include "RTI.hh"

/**
 * @defgroup libCERTI The CERTI library.
 * @ingroup CERTI_Libraries
 * The CERTI library contains CERTI specific HLA implementation
 */

// next used in RTIG::processIncomingMessage method
#define BUFFER_EXCEPTION_REASON_SIZE 256

namespace certi {

typedef unsigned short UShort;
typedef unsigned long ULong;
typedef long Long;
typedef ULong Handle;
typedef Handle ObjectClassHandle;
typedef Handle InteractionClassHandle;
typedef ULong ExtentIndex;
typedef Handle AttributeHandle;
typedef Handle ParameterHandle;
typedef Handle ObjectHandle;
typedef Handle DimensionHandle;
typedef Handle FederateHandle;
typedef Handle OrderType;
typedef Long SpaceHandle;
typedef Handle TransportType;

typedef RTI::EventRetractionHandle EventRetractionHandle;

typedef UShort ObjectHandlecount ;
typedef std::string ObjectName_t ;
typedef std::string AttributeValue_t;
typedef std::string ParameterValue_t;

enum ObjectRemovalReason {
    OUT_OF_REGION = 1,
    OBJECT_DELETED,
    NO_LONGER_SUBSCRIBED
};
typedef Handle RegionHandle ;

// Constants (former HLA constants)
const TransportType RELIABLE = 1 ;
const TransportType BEST_EFFORT = 2 ;
const OrderType RECEIVE = 1 ;
const OrderType TIMESTAMP = 2 ;

// Constants
const int MAX_BACKLOG = 256 ;

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

// $Id: certi.hh,v 3.38.4.1 2009/01/05 13:34:50 gotthardp Exp $
