/***********************************************************************
IEEE 1516.1 High Level Architecture Interface Specification C++ API
File: RTI_SaveFailureReason.h
***********************************************************************/
#ifndef RTI_SaveFailureReason_h
#define RTI_SaveFailureReason_h
class RTI_bool;
#include <RTI_SpecificConfig.h>
#include <RTI_string>
// Type safe class used to represent type of data order.
class RTI_EXPORT RTI_SaveFailureReason
{
  public:
   RTI_SaveFailureReason(RTI_SaveFailureReason const & rhs);
   static
   RTI_SaveFailureReason const
   rtiUnableToSave();
   static
   RTI_SaveFailureReason const
   federateReportedFailureDuringSave();
   static
   RTI_SaveFailureReason const
   federateResignedDuringSave();
   static
   RTI_SaveFailureReason const
   rtiDetectedFailureDuringSave();
   static
   RTI_SaveFailureReason const
   saveTimeCannotBeHonored();
   RTI_SaveFailureReason &
   operator=(RTI_SaveFailureReason const & rhs);
   RTI_bool
   operator==(RTI_SaveFailureReason const & rhs) const;
   RTI_bool
   operator!=(RTI_SaveFailureReason const & rhs) const;
   RTI_wstring const
   toString() const;
 private:
   RTI_SaveFailureReason(unsigned saveFailureReason);
   unsigned _saveFailureReason;
};
// These constants save a little typing for users.
// They can be used much like a enum, but in a type-safe way
RTI_SaveFailureReason const
RTI_RTI_UNABLE_TO_SAVE =
RTI_SaveFailureReason::rtiUnableToSave();
RTI_SaveFailureReason const
RTI_FEDERATE_REPORTED_FAILURE_DURING_SAVE =
RTI_SaveFailureReason::federateReportedFailureDuringSave();
RTI_SaveFailureReason const
RTI_FEDERATE_RESIGNED_DURING_SAVE =
RTI_SaveFailureReason::federateResignedDuringSave();
RTI_SaveFailureReason const
RTI_RTI_DETECTED_FAILURE_DURING_SAVE =
RTI_SaveFailureReason::rtiDetectedFailureDuringSave();
RTI_SaveFailureReason const
RTI_SAVE_TIME_CANNOT_BE_HONORED =
RTI_SaveFailureReason::saveTimeCannotBeHonored();
#ifdef RTI_USE_INLINE
#include "RTI_SaveFailureReason.i"
#endif // RTI_USE_INLINE
#endif // RTI_SaveFailureReason_h
