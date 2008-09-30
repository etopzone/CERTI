/***********************************************************************
IEEE 1516.1 High Level Architecture Interface Specification C++ API
File: RTI_1516.h
***********************************************************************/
//
// This file is simply a convenience provided for those developers that would
// like to include everything all at once
//
#ifndef RTI_1516_h
#define RTI_1516_h
// These file include declarations/definitions for ISO 14882 standard C++
// classes, renamed for portability.
#include <RTI_string>
#include <RTI_set>
#include <RTI_map>
#include <RTI_vector>
#include <RTI_memory>
// This file contains platform specific configuration info.
#include <RTI_SpecificConfig.h>
#include <RTI_bool.h>
// This file contains standard RTI type declarations/definitions.
#include <RTI_exception.h>
#include <RTI_Handle.h>
#include <RTI_Value.h>
#include <RTI_ResignAction.h>
#include <RTI_TransportationType.h>
#include <RTI_OrderType.h>
#include <RTI_SaveFailureReason.h>
#include <RTI_SaveStatus.h>
#include <RTI_SynchronizationFailureReason.h>
#include <RTI_RestoreStatus.h>
#include <RTI_RestoreFailureReason.h>
#include <RTI_ServiceGroupIndicator.h>
#include <RTI_RangeBounds.h>
// This file has RTI implementation specific declarations/definitions.
#include <RTI_SpecificTypedefs.h>
 // This file contains standard RTI type declarations/definitions which depend on
 // RTI implementation specific declarations/definitions.
 #include <RTI_Typedefs.h>
 #include <RTI_LogicalTime.h>
 #include <RTI_LogicalTimeFactory.h>
 #include <RTI_LogicalTimeInterval.h>
 #include <RTI_LogicalTimeIntervalFactory.h>
 static char const * const HLA_VERSION = "1516.1.5";
 #include <RTI_FederateAmbassador.h>
 #include <RTI_RTIambassador.h>
 // This file provides RTI implementation specific decalarations and definitions
 // that need to follow the other header files.
 #include <RTI_SpecificPostamble.h>
 #endif // RTI_1516_h
