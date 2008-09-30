/***********************************************************************
IEEE 1516.1 High Level Architecture Interface Specification C++ API
File: RTI_RestoreFailureReason.h
***********************************************************************/
#ifndef RTI_RestoreFailureReason_h
#define RTI_RestoreFailureReason_h
class RTI_bool;
#include <RTI_SpecificConfig.h>
#include <RTI_string>
// Type safe class used to represent the reason a restore failed
class RTI_EXPORT RTI_RestoreFailureReason
{
  public:
   RTI_RestoreFailureReason(RTI_RestoreFailureReason const & rhs);
   static
   RTI_RestoreFailureReason const
   rtiUnableToRestore();
   static
   RTI_RestoreFailureReason const
   federateReportedFailureDuringRestore();
   static
   RTI_RestoreFailureReason const
   federateResignedDuringRestore();
   static
   RTI_RestoreFailureReason const
   rtiDetectedFailureDuringRestore();
   RTI_RestoreFailureReason &
   operator=(RTI_RestoreFailureReason const & rhs);
   RTI_bool
   operator==(RTI_RestoreFailureReason const & rhs) const;
   RTI_bool
   operator!=(RTI_RestoreFailureReason const & rhs) const;
   RTI_wstring const
   toString() const;
  private:
   RTI_RestoreFailureReason(unsigned RestoreFailureReason);
   unsigned _RestoreFailureReason;
};
// These constants Restore a little typing for users.
// They can be used much like a enum, but in a type-safe way
RTI_RestoreFailureReason const
RTI_RTI_UNABLE_TO_RESTORE =
RTI_RestoreFailureReason::rtiUnableToRestore();
RTI_RestoreFailureReason const
RTI_FEDERATE_REPORTED_FAILURE_DURING_RESTORE =
RTI_RestoreFailureReason::federateReportedFailureDuringRestore();
RTI_RestoreFailureReason const
RTI_FEDERATE_RESIGNED_DURING_RESTORE =
RTI_RestoreFailureReason::federateResignedDuringRestore();
RTI_RestoreFailureReason const
RTI_RTI_DETECTED_FAILURE_DURING_RESTORE =
RTI_RestoreFailureReason::rtiDetectedFailureDuringRestore();
#ifdef RTI_USE_INLINE
#include "RTI_RestoreFailureReason.i"
#endif // RTI_USE_INLINE
#endif // RTI_RestoreFailureReason_h
