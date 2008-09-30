/***********************************************************************
IEEE 1516.1 High Level Architecture Interface Specification C++ API
File: RTI_SynchronizationFailureReason.h
***********************************************************************/
#ifndef RTI_SynchronizationFailureReason_h
#define RTI_SynchronizationFailureReason_h
class RTI_bool;
#include <RTI_SpecificConfig.h>
#include <RTI_string>
// Type safe class used to represent type of data order.
class RTI_EXPORT RTI_SynchronizationFailureReason
{
  public:
   RTI_SynchronizationFailureReason(RTI_SynchronizationFailureReason
const & rhs);
   static
   RTI_SynchronizationFailureReason const
   synchronizationPointLabelNotUnique();
   static
   RTI_SynchronizationFailureReason const
   synchronizationSetMemberNotJoined();
   RTI_SynchronizationFailureReason &
   operator=(RTI_SynchronizationFailureReason const & rhs);
   RTI_bool
   operator==(RTI_SynchronizationFailureReason const & rhs) const;
   RTI_bool
   operator!=(RTI_SynchronizationFailureReason const & rhs) const;
   RTI_wstring const
   toString() const;
  private:
   RTI_SynchronizationFailureReason(unsigned SynchronizationFailureReason);
   unsigned _SynchronizationFailureReason;
};
// These constants Synchronization a little typing for users.
// They can be used much like a enum, but in a type-safe way
RTI_SynchronizationFailureReason const
RTI_SYNCHRONIZATION_POINT_LABEL_NOT_UNIQUE =
RTI_SynchronizationFailureReason::synchronizationPointLabelNotUnique();
RTI_SynchronizationFailureReason const
RTI_SYNCHRONIZATION_SET_MEMBER_NOT_JOINED =
RTI_SynchronizationFailureReason::synchronizationSetMemberNotJoined();
#ifdef RTI_USE_INLINE
#include "RTI_SynchronizationFailureReason.i"
#endif // RTI_USE_INLINE
#endif // RTI_SynchronizationFailureReason_h
