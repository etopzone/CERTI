// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002  ONERA
//
// This file is part of CERTI-libRTI
//
// CERTI-libRTI is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libRTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: local_config.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

// Types utilises par le RTI : Constantes, exceptions, handles...

#ifndef _CERTI_RTI_CONFIG_HH
#define _CERTI_RTI_CONFIG_HH

#include <limits.h>

// -------------------------
// -- Do your tuning here --
// -------------------------

// Define it if you want the complete object and interaction tree to
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
//   Level 0 : NULL messages(time synchronization)
//   Level 1 : Update Attribute Values messages
//   Level 2 : Send Interaction messages
//   Level 6 : Register / Delete Objects messages
//   Level 7 : Publish / Subscribe messages
//   Level 8 : Pause, Resume, Set Time regulating/constrained messages
//   Level 9 : Create/Join/Resign/Destroy Federation msgs, Connections.
//   Level 10: RTIG and Audit start/stop.

#define AUDIT_CURRENT_LEVEL 5

// ------------------------------
// -- GSSAPI Security Services --
// ------------------------------

// Comment out HLA_USES_GSSAPI to choose NOT to use GSSAPI services.

//#define HLA_USES_GSSAPI


// Server Principal Name 
//(must be like "<rtip principal name>@<network machine name>" )

#define HLA_SERVER_PRINCIPAL_NAME "RTIG@brive"


// Expiration delay for GSSAPI sessions(must be greater then the longer
// lifetime of a federate). [In seconds]

#define HLA_GSS_SESSION_DURATION 3600


// You can use GSSAPI at least to authenticate federates(needed to use
// security levels), to sign all messages(source authentication), or
// to sign AND encrypt message. [last option]
// Just choose one of the three options below and affect it to the 
// RTI_GSSAPI_USAGE macro.

#define RTI_GSS_NOTHING     0
#define RTI_GSS_SIGN_ONLY   1
#define RTI_GSS_ENCRYPT     2

#define RTI_GSSAPI_USAGE   RTI_GSS_NOTHING

// Type of Security services needed : Can be any combination(OR) of the 
// following flags(see your GSSAPI documentation for further details) :
//
//   GSS_C_DELEG_FLAG     Credential delegation(not supported by HLA anyway)
//   GSS_C_MUTUAL_FLAG    Mutual authentication
//   GSS_C_REPLAY_FLAG    Replay detection
//   GSS_C_SEQUENCE_FLAG  Out of sequence message detection.
//
// Note: Currently, no option is suppported by our default GSSAPI mechanism.

#define HLA_GSS_FLAGS 0

// ---------------
// -- Constants --
// ---------------

#define MAX_FEDERATION               20
#define MAX_FEDERATE                 20
#define MAX_OBJECT_CLASSES          100
#define MAX_INTERACTION_CLASSES     100
#define MAX_ATTRIBUTES_PER_CLASS     20
#define MAX_PARAMETERS_PER_CLASS     20 // Must be the same as Attributes
#define MAX_BYTES_PER_VALUE          80
#define MAX_BYTES_PER_VALUETYPE      20
#define PRIVILEGE_TO_DELETE_HANDLE    0
#define ROOT_OBJECT_CLASS_HANDLE      0
#define ROOT_INTERACTION_CLASS_HANDLE 0
#define PRIVILEGE_TO_DELETE_NAME      "privilegeToDelete"

#define MAX_FEDERATE_NAME_LENGTH     64
#define MAX_FEDERATION_NAME_LENGTH   64
#define MAX_USER_TAG_LENGTH          32

#define EPSILON (1.0e-9)
#define POSITIVE_INFINITY      HUGE_VAL

// --------------------------------------
// -- Stop the tuning after this point --
// --------------------------------------

#endif // _CERTI_RTI_CONFIG_HH

// $Id: local_config.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $

