/***********************************************************************
  IEEE 1516.1 High Level Architecture Interface Specification C++ API
  File: RTI/1516.h
***********************************************************************/

// ****************************************************************
// ********************** Adjusted for CERTI **********************
// ****************************************************************
// This file is one of the SISO HLA 1516 header files that was edited
// by CERTI for compilation. By default, there exists a circular
// dependency between the RTI library (provided by CERTI) and the
// LogicalTime implementation (to be provided by the federate).
// In order to break this dependency some minor changes have been
// applied: The LogicalTime, LogicalTimeInterval and 
// LogicalTimeFactory classes are now exported by the LogicalTime 
// implementation. All three classes have been given an inline (empty)
// destructor.
// While these changes may break (direct) DLL compatibility, they
// greatly ease the implementation of the RTI1516 library for CERTI.
// ****************************************************************
// ********************** Adjusted for CERTI **********************
// ****************************************************************


//
// This file is simply a convenience provided for those developers that would
// like to include everything all at once
//

#ifndef RTI_1516_h
#define RTI_1516_h

// Identification of the API version number.  
#define HLA_SPECIFICATION_NAME "1516"
#define HLA_API_MAJOR_VERSION 2   
#define HLA_API_MINOR_VERSION 0   

// This file contains platform specific configuration info.
#include <RTI/SpecificConfig.h>

// These file include declarations/definitions for ISO 14882 standard C++
// classes, renamed for portability.
#include <string>
#include <set>
#include <map>
#include <vector>
#include <memory>

// This file contains standard RTI type declarations/definitions.
#include <RTI/Exception.h>
#include <RTI/Handle.h>
#include <RTI/Enums.h>
#include <RTI/RangeBounds.h>

// This file contains standard RTI type declarations/definitions which depend on
// RTI implementation specific declarations/definitions.
#include <RTI/Typedefs.h>
#include <RTI/certiLogicalTime.h>
#include <RTI/certiLogicalTimeFactory.h>
#include <RTI/certiLogicalTimeInterval.h>

namespace rti1516
{
   // Vendor-specific name and version of the RTI implementation
   std::wstring RTI_EXPORT RTIname(); 
   std::wstring RTI_EXPORT RTIversion();
}

#include <RTI/FederateAmbassador.h>
#include <RTI/RTIambassador.h>
#include <RTI/RTIambassadorFactory.h>

#endif // RTI_1516_h
