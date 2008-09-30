/***********************************************************************
IEEE 1516.1 High Level Architecture Interface Specification C++ API
File: RTI_Typedefs.h
***********************************************************************/
// Purpose: This file contains the standard RTI types that are prefixed
// with the string "RTI". These definitions/declarations are standard
// for all RTI implementations.
//
// The types declared here require the use of some RTI_Specific types.
#ifndef RTI_Typedefs_h
#define RTI_Typedefs_h
// The following type definitions use standard C++ classes for containers
// that are used in the RTI API.
#include <RTI_set>
#include <RTI_map>
#include <RTI_vector>
#include <RTI_SpecificTypedefs.h>
typedef RTI_set< RTI_AttributeHandle, RTI_less< RTI_AttributeHandle > ,
                RTI___STL_DEFAULT_ALLOCATOR (RTI_AttributeHandle)>
 RTI_AttributeHandleSet;
typedef RTI_set<RTI_FederateHandle, RTI_less<RTI_FederateHandle > ,
               RTI___STL_DEFAULT_ALLOCATOR (RTI_FederateHandle)>
 RTI_FederateHandleSet;
typedef RTI_set< RTI_DimensionHandle, RTI_less< RTI_DimensionHandle >,
                RTI___STL_DEFAULT_ALLOCATOR (RTI_DimensionHandle) >
 RTI_DimensionHandleSet;
typedef RTI_set< RTI_RegionHandle, RTI_less< RTI_RegionHandle >,
                RTI___STL_DEFAULT_ALLOCATOR (RTI_RegionHandle) >
 RTI_RegionHandleSet;
// RTI_AttributeHandleValueMap implements a constrained set of
// (attribute handle and value) pairs
typedef RTI_map< RTI_AttributeHandle, RTI_AttributeValue, RTI_less >
                RTI_AttributeHandle >, RTI___STL_DEFAULT_ALLOCATOR
               (RTI_AttributeHandle) >
 RTI_AttributeHandleValueMap;
// RTI_ParameterHandleValueMap implements a constrained set of
// (parameter handle and value) pairs
typedef RTI_map< RTI_ParameterHandle, RTI_ParameterValue, RTI_less<
                RTI_ParameterHandle >, RTI___STL_DEFAULT_ALLOCATOR
               (RTI_ParameterHandle) >
 RTI_ParameterHandleValueMap;
// RTI_AttributeHandleSetRegionHandleSetPairVector implements a collection of
// (attribute handle set and region set) pairs
typedef RTI_pair< RTI_AttributeHandleSet, RTI_RegionHandleSet >
  RTI_AttributeHandleSetRegionHandleSetPair;
typedef RTI_vector< RTI_AttributeHandleSetRegionHandleSetPair,
                   RTI___STL_DEFAULT_ALLOCATOR
                  (RTI_AttributeHandleSetRegionHandleSetPair) >
 RTI_AttributeHandleSetRegionHandleSetPairVector;
// RTI_FederateHandleSaveStatusPairVector implements a collection of
// (federate handle and save status) pairs
typedef RTI_pair< RTI_FederateHandle, RTI_SaveStatus >
 RTI_FederateHandleSaveStatusPair;
typedef RTI_vector< RTI_FederateHandleSaveStatusPair,
                   RTI___STL_DEFAULT_ALLOCATOR(RTI_FederateHandleSaveStatusPair >
 RTI_FederateHandleSaveStatusPairVector;
// RTI_FederateHandleRestoreStatusPairVector implements a collection of
// (federate handle and restore status) pairs
typedef RTI_pair< RTI_FederateHandle, RTI_RestoreStatus >
                 RTI_FederateHandleRestoreStatusPair;
typedef RTI_vector< RTI_FederateHandleRestoreStatusPair,
                   RTI___STL_DEFAULT_ALLOCATOR(RTI_FederateHandleRestore
StatusPair) >
 RTI_FederateHandleRestoreStatusPairVector;
#endif // RTI_Typedefs_h
