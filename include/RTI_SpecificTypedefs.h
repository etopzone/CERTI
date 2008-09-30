/***********************************************************************
IEEE 1516.1 High Level Architecture Interface Specification C++ API
File: RTI_SpecificTypedefs.h
***********************************************************************/
// Purpose: This file contains definitions for RTI types that are specific to
// the RTI implementation. The parameterized classes RTI_Handle and RTI_Value
// enable the RTI implementation to insert a private member. The private
// member declaration must be exposed in this header file. These
// implementation specific declarations and definitions should not be
// utilized since it is non-standard with respect to RTI implementations.
#ifndef RTI_SpecificTypedefs_h
#define RTI_SpecificTypedefs_h
// The following type definitions correspond to standard types used in the API
// (e.g., FederateHandle) that are described through the use of RTI
// implementation specific representations.
//
// The following definitions represent an example of specific representations
// that could be used by an RTI implementation.
#include <RTI_SpecificConfig.h>
//#include <RTI_Value.h>
template < class T, int i > class RTI_Value;
//
// This class is an example of something that is RTI implementor specific.
//
#include <RTI_VariableLengthValueClass.h>
//
// The existence of these typedefs is required by the API Specification.
// However, their particular definition is implementation-specific.RTI-user
// code merely uses the typedefs. What their underlying type is, however, is up
// to the RTI implementor.
//
typedef RTI_Value< RTI_VariableLengthValueClass, 1 > RTI_AttributeValue;
typedef RTI_Value< RTI_VariableLengthValueClass, 2 > RTI_ParameterValue;
typedef RTI_Value< RTI_VariableLengthValueClass, 3 > RTI_UserSuppliedTag;
typedef RTI_Value< RTI_VariableLengthValueClass, 4 > RTI_EncodedData;
//
// The names of these classes are left up to the RTI implementor.
//
class RTI_FederateHandleFactory;
class RTI_ObjectClassHandleFactory;
class RTI_InteractionClassHandleFactory;
class RTI_ObjectInstanceHandleFactory;
class RTI_AttributeHandleFactory;
class RTI_ParameterHandleFactory;
class RTI_DimensionHandleFactory;
class RTI_MessageRetractionHandleFactory;
class RTI_RegionHandleFactory;
//#include <RTI_Handle.h>
template<class ImplementationSpecificHandleClass,
class EncodedHandleClass,
class ImplementationSpecificHandleClassFriend,
inti>
class RTI_Handle;
//
// The existence of these typedefs is required by the API Specification.
// However, their particular definition is implementation-specific.
//
typedef
RTI_Handle< long, RTI_EncodedData, RTI_FederateHandleFactory, 1 >
RTI_FederateHandle;
typedef
RTI_Handle< long, RTI_EncodedData, RTI_ObjectClassHandleFactory, 2 >
RTI_ObjectClassHandle;
typedef
RTI_Handle< long, RTI_EncodedData, RTI_InteractionClassHandleFactory, 3 >
RTI_InteractionClassHandle;
typedef
RTI_Handle< long, RTI_EncodedData, RTI_ObjectInstanceHandleFactory, 4 >
RTI_ObjectInstanceHandle;
typedef
RTI_Handle< long, RTI_EncodedData, RTI_AttributeHandleFactory, 5 >
RTI_AttributeHandle;
typedef
RTI_Handle< long, RTI_EncodedData, RTI_ParameterHandleFactory, 6 >
RTI_ParameterHandle;
typedef
RTI_Handle< long, RTI_EncodedData, RTI_DimensionHandleFactory, 7 >
RTI_DimensionHandle;
typedef
RTI_Handle< long, RTI_EncodedData, RTI_MessageRetractionHandleFactory, 8 >
RTI_MessageRetractionHandle;
typedef
RTI_Handle< long, RTI_EncodedData, RTI_RegionHandleFactory, 11 >
RTI_RegionHandle;
#endif // RTI_SpecificTypedefs_h
