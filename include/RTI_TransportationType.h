/***********************************************************************
IEEE 1516.1 High Level Architecture Interface Specification C++ API
File: RTI_TransportationType.h
***********************************************************************/
#ifndef RTI_TransportationType_h
#define RTI_TransportationType_h
class RTI_bool;
#include <RTI_SpecificConfig.h>
#include <RTI_SpecificTypedefs.h> // for RTI_EncodedData
#include <RTI_string>
// Type safe class used to represent type of data transportation.
class RTI_EXPORT RTI_TransportationType
{
  public:
   RTI_TransportationType(RTI_EncodedData const & rhs);
   RTI_TransportationType(RTI_TransportationType const & rhs)
     throw ();
   static
   RTI_TransportationType const
   reliable()
     throw();
   static
   RTI_TransportationType const
   bestEffort()
     throw();
   RTI_TransportationType &
   operator =(RTI_TransportationType const & rhs)
     throw();
   RTI_bool
   operator ==(RTI_TransportationType const & rhs) const
     throw();
   RTI_bool
   operator !=(RTI_TransportationType const & rhs) const
     throw();
   RTI_wstring const
   toString() const;
   RTI_EncodedData
   encode() const
     throw();
 private:
   RTI_TransportationType(unsigned transportationType)
     throw();
   unsigned _transportationType;
};
// These constants save a little typing for users.
// They can be used much like a enum, but in a type-safe way
RTI_TransportationType const
RTI_RELIABLE =
RTI_TransportationType::reliable();
RTI_TransportationType const
RTI_BEST_EFFORT =
RTI_TransportationType::bestEffort();
#ifdef RTI_USE_INLINE
#include "RTI_TransportationType.i"
#endif // RTI_USE_INLINE
#endif // RTI_TransportationType_h
