/***********************************************************************
IEEE 1516.1 High Level Architecture Interface Specification C++ API
File: RTI_OrderType.h
***********************************************************************/
#ifndef RTI_OrderType_h
#define RTI_OrderType_h
class RTI_bool;
#include <RTI_SpecificConfig.h>
#include <RTI_SpecificTypedefs.h> // for RTI_EncodedData
#include <RTI_string>
// Type safe class used to represent type of data order.
class RTI_EXPORT RTI_OrderType
{
  public:
   RTI_OrderType(RTI_EncodedData const & theEncodedOrderType);
   RTI_OrderType(RTI_OrderType const & rhs)
     throw();
   static
   RTI_OrderType const
   receive()
     throw();
   static
   RTI_OrderType const
   timestamp()
     throw();
   RTI_OrderType &
   operator=(RTI_OrderType const & rhs)
     throw ();
   RTI_bool
   operator==(RTI_OrderType const & rhs) const
     throw();
   RTI_bool
   operator!=(RTI_OrderType const & rhs) const
     throw();
   RTI_wstring const
   toString() const;
   RTI_EncodedData
   encode() const
     throw();
 private:
   RTI_OrderType(unsigned orderType)
     throw();
   unsigned _orderType;
};
// These constants save a little typing for users.
// They can be used much like a enum, but in a type-safe way
RTI_OrderType const
RTI_RECEIVE =
RTI_OrderType::receive();
RTI_OrderType const
RTI_TIMESTAMP =
RTI_OrderType::timestamp();
#ifdef RTI_USE_INLINE
#include "RTI_OrderType.i"
#endif // RTI_USE_INLINE
#endif // RTI_OrderType_h
