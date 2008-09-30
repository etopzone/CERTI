/***********************************************************************
IEEE 1516.1 High Level Architecture Interface Specification C++ API
File: RTI_Handle.h
***********************************************************************/
#ifndef RTI_Handle_h
#define RTI_Handle_h
class RTI_bool;
#include <RTI_SpecificConfig.h>
#include <RTI_string>
//  The RTIhandle class is used to provide the common interface to the different
//  RTI handle types used in the API. This interface includes a constructor,
//  assignment, equality, inequality, and less than operators. The encode method
//  returns a type safe EncodedHandleClass instance that can be used to exchange
//  handles between federates as attributes and parameters. The constructor takes
//  a EncodedHandleClass which enables the type safe creation of a RTIhandle from
//  encoded data passed to a federate. The template parameter class
//  ImplementationSpecificHandleClass provides RTI implementations the ability
//  to customize a private class member for their particular implementation.The
//  int template type parameter provides the ability to support strong typing.
template<class ImplementationSpecificHandleClass,
          class EncodedHandleClass,
          class ImplementationSpecificHandleClassFriend,
          int   i>
class RTI_EXPORT RTI_Handle
{
  public:
   explicit
   RTI_Handle(EncodedHandleClass encodedHandle);
   ~RTI_Handle()
      throw();
   RTI_Handle(RTI_Handle const & rhs);
   RTI_Handle &
   operator=(RTI_Handle const & rhs);
   RTI_bool
   operator==(RTI_Handle const & rhs) const;
   RTI_bool
   operator!=(RTI_Handle const & rhs) const;
   RTI_bool
   operator< (RTI_Handle const & rhs) const;
   EncodedHandleClass
   encode() const;
   RTI_wstring const
   toString() const;
  private:
   ImplementationSpecificHandleClass _impl;
   //
   // This class is the only class which can construct an RTI_Handle
   //
   friend ImplementationSpecificHandleClassFriend;
   RTI_Handle(ImplementationSpecificHandleClass const & impl);
};
#ifdef     RTI_USE_INLINE
#include "RTI_Handle.i"
#endif // RTI_USE_INLINE
#ifdef     RTI_TEMPLATES_REQUIRE_SOURCE
#include "RTI_Handle.cpp"
#endif // RTI_TEMPLATES_REQUIRE_SOURCE
#endif // RTI_Handle_h
