/***********************************************************************
IEEE 1516.1 High Level Architecture Interface Specification C++ API
File: RTI_ServiceGroupIndicator.h
***********************************************************************/
#ifndef RTI_ServiceGroupIndicator_h
#define RTI_ServiceGroupIndicator_h
class RTI_bool;
#include <RTI_SpecificConfig.h>
#include <RTI_string>
// Type safe class used to represent the service group
class RTI_EXPORT RTI_ServiceGroupIndicator
{
public:
   RTI_ServiceGroupIndicator(RTI_ServiceGroupIndicator const & rhs);
   static
   RTI_ServiceGroupIndicator const
   federationManagement();
   static
   RTI_ServiceGroupIndicator const
   declarationManagement();
   static
   RTI_ServiceGroupIndicator const
   objectManagement();
   static
   RTI_ServiceGroupIndicator const
   ownershipManagement();
   static
   RTI_ServiceGroupIndicator const
   timeManagement();
   static
   RTI_ServiceGroupIndicator const
   dataDistributionManagement();
   static
   RTI_ServiceGroupIndicator const
   supportServices();
   RTI_ServiceGroupIndicator &
   operator=(RTI_ServiceGroupIndicator const & rhs);
   RTI_bool
   operator==(RTI_ServiceGroupIndicator const & rhs) const;
   RTI_bool
   operator!=(RTI_ServiceGroupIndicator const & rhs) const;
   RTI_wstring const
   toString() const;
  private:
   RTI_ServiceGroupIndicator(unsigned _ServiceGroupIndicator);
   unsigned _ServiceGroupIndicator;
};
// These constants save a little typing for users.
// They can be used much like a enum, but in a type-safe way
RTI_ServiceGroupIndicator const
RTI_FEDERATION_MANAGEMENT
= RTI_ServiceGroupIndicator::federationManagement();
RTI_ServiceGroupIndicator const
RTI_DECLARATION_MANAGEMENT
= RTI_ServiceGroupIndicator::declarationManagement();
RTI_ServiceGroupIndicator const
RTI_OBJECT_MANAGEMENT
= RTI_ServiceGroupIndicator::objectManagement();
RTI_ServiceGroupIndicator const
RTI_OWNERSHIP_MANAGEMENT
= RTI_ServiceGroupIndicator::ownershipManagement();
RTI_ServiceGroupIndicator const
RTI_TIME_MANAGEMENT
= RTI_ServiceGroupIndicator::timeManagement();
RTI_ServiceGroupIndicator const
RTI_DATA_DISTRIBUTION_MANAGEMENT
= RTI_ServiceGroupIndicator::dataDistributionManagement();
RTI_ServiceGroupIndicator const
RTI_SUPPORT_SERVICES
= RTI_ServiceGroupIndicator::supportServices();
#ifdef RTI_USE_INLINE
#include "RTI_ServiceGroupIndicator.i"
#endif // RTI_USE_INLINE
#endif // RTI_ServiceGroupIndicator_h
