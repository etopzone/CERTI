/***********************************************************************
IEEE 1516.1 High Level Architecture Interface Specification C++ API
File: RTI_bool.h
***********************************************************************/
#ifndef RTI_bool_h
#define RTI_bool_h
#include <RTI_SpecificConfig.h>
#include <RTI_string>
// This is a type-safe boolean class.Not all platforms support the bool
// type. Consequently, one must be provided.
//
class RTI_EXPORT RTI_bool
{
  public:
   RTI_bool();
   RTI_bool(unsigned boolean);
   RTI_bool(RTI_bool const & b);
   ~RTI_bool()
     throw();
   static
   RTI_bool const
   True();
   static
   RTI_bool const
   False();
   operator unsigned() const;
   RTI_bool const
   operator==(RTI_bool const & rhs) const;
   RTI_bool const
   operator!=(RTI_bool const & rhs) const;
   RTI_wstring
   toString() const;
  private:
unsigned _boolean;
};
#ifdef RTI_USE_INLINE
#include "RTI_bool.i"
#endif // RTI_USE_INLINE
RTI_bool const RTI_true= RTI_bool::True();
RTI_bool const RTI_false = RTI_bool::False();
#endif // RTI_bool_h
