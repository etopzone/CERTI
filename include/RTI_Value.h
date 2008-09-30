/***********************************************************************
IEEE 1516.1 High Level Architecture Interface Specification C++ API
File: RTI_Value.h
***********************************************************************/
#ifndef RTI_Value_h
#define RTI_Value_h
#include <RTI_SpecificConfig.h>
//  The RTI_Value class is used to as a generic value holder that contains a
//  pointer to the memory location and the size of the value. This class is
//  used for attributes, parameters, and user supplied tags.The constructor
//  takes a pointer to the data value and the size of the data. The key methods
//  on this class is the data method which returns a constant pointer to the
//  value memory location, and the size method which returns the size in bytes
//  of the value. The templatized class T provides RTI implementations the
//  ability to customize their particular implementation. The int template type
//  parameter provides the ability to support strong typing.
template < class T, int i >
RTI_EXPORT class RTI_Value
{
public:
   RTI_Value (void const * data, size_t size);
   ~RTI_Value()
     throw ();
   RTI_Value(RTI_Value const & rhs);
   RTI_Value &
   operator=(RTI_Value const & rhs);
   void const *
   data() const;
   size_t
   size() const;
private:
   T _impl;
};
#ifdef RTI_USE_INLINE
#include "RTI_Value.i"
#endif // RTI_USE_INLINE
#ifdef RTI_TEMPLATES_REQUIRE_SOURCE
#include "RTI_Value.cpp"
#endif // RTI_TEMPLATES_REQUIRE_SOURCE
#endif // RTI_Value_h
