/***********************************************************************
  IEEE 1516.1 High Level Architecture Interface Specification C++ API
  File: RTI/SpecificConfig.h
***********************************************************************/

// Purpose: This file contains definitions that are used to isolate
// platform-specific elements of the API.  It is not implementation-specific.

#ifndef RTI_SpecificConfig_h
#define RTI_SpecificConfig_h

#ifdef _MSC_VER
#ifndef FAKED_INTTYPES_DEFINED
#define FAKED_INTTYPES_DEFINED
typedef unsigned __int64  uint64_t;
typedef signed __int64    int64_t;
typedef unsigned __int32  uint32_t;
typedef signed __int32    int32_t;
typedef unsigned __int16  uint16_t;
typedef signed __int16    int16_t;
typedef unsigned __int8   uint8_t;
typedef signed __int8     int8_t;
typedef short int         int_least16_t;
#endif
#else
#include <inttypes.h>
#endif

// DLL visibility macros helpers
#if defined(_WIN32) || defined(__CYGWIN__)
    #define ANY_DLL_EXPORT __declspec(dllexport)
    #define ANY_DLL_IMPORT __declspec(dllimport)
    #define ANY_DLL_LOCAL
#else
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

#if defined(_WIN32)
    #if defined(_MSC_VER) && defined(RTI_DISABLE_WARNINGS)
    // disable warning about truncating template instantiation symbol names
    #pragma warning(disable: 4786)
    // disable warning about exceptions not being part of a method's signature
    #pragma warning(disable: 4290)
    // disable warnings about a "dllexport" class using a regular class
    #pragma warning(disable: 4251)
    #endif
   //
   // On Windows, BUILDING_RTI should be defined only when compiling
   // the RTI DLL (i.e. by RTI developers).  BUILDING_FEDTIME should
   // be defined only when building a libfedtime DLL.  STATIC_RTI
   // should be defined when building a static (non-DLL) RTI library,
   // or when building a federate that wants to statically link to
   // an RTI library.  STATIC_FEDTIME should be defined when building
   // a static (non-DLL) fedtime library, or when building a federate
   // that wants to statically link to a fedtime library.
   //
   #if defined(STATIC_RTI)
       #define RTI_EXPORT
   #else
       #if defined(BUILDING_RTI)
          // define the proper qualifiers to import/export symbols from/to DLL
          #define RTI_EXPORT __declspec(dllexport)
       #else // !BUILDING_RTI
          #define RTI_EXPORT __declspec(dllimport)
       #endif // BUILDING_RTI
   #endif // STATIC_RTI

   #if defined(STATIC_FEDTIME)
       #define FEDTIME_EXPORT
   #else
       #if defined(BUILDING_FEDTIME)
           // define the proper qualifiers to import/export symbols from/to DLL
           #define FEDTIME_EXPORT __declspec(dllexport)
       #else // !BUILDING_FEDTIME
           #define FEDTIME_EXPORT __declspec(dllimport)
       #endif // BUILDING_FEDTIME
   #endif // STATIC_FEDTIME
#else // !_WIN32
// no special qualfifers are necessary on non-WIN32 platforms
   #if defined(RTI_EXPORTS)
       #define RTI_EXPORT ANY_DLL_EXPORT
   #else
       #define RTI_EXPORT ANY_DLL_IMPORT
   #endif
   #if defined(FedTime_EXPORTS)
       #define FEDTIME_EXPORT ANY_DLL_EXPORT
   #else
       #define FEDTIME_EXPORT ANY_DLL_IMPORT
   #endif
#endif

#endif // RTI_SpecificConfig_h
