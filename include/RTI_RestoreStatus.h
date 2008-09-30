/***********************************************************************
IEEE 1516.1 High Level Architecture Interface Specification C++ API
File: RTI_RestoreStatus.h
***********************************************************************/
#ifndef RTI_RestoreStatus_h
#define RTI_RestoreStatus_h
class RTI_bool;
#include <RTI_SpecificConfig.h>
#include <RTI_string>
// Type safe class used to represent save status of an individual federate.
class RTI_EXPORT RTI_RestoreStatus
{
public:
  RTI_RestoreStatus(RTI_RestoreStatus const & rhs);
  static
  RTI_RestoreStatus const
  noRestoreInProgress();
  static
  RTI_RestoreStatus const
  federateRestoreRequestPending();
  static
  RTI_RestoreStatus const
  federateWaitingForRestoreToBegin();
  static
  RTI_RestoreStatus const
  federatePreparedToRestore();
  static
  RTI_RestoreStatus const
  federateRestoring();
  static
  RTI_RestoreStatus const
  federateWaitingForFederationToRestore();
  RTI_RestoreStatus &
  operator=(RTI_RestoreStatus const & rhs);
  RTI_bool
   operator==(RTI_RestoreStatus const & rhs) const;
   RTI_bool
   operator!=(RTI_RestoreStatus const & rhs) const;
   RTI_wstring const
   toString() const;
private:
   RTI_RestoreStatus(unsigned _RestoreStatus);
   unsigned _RestoreStatus;
};
// These constants save a little typing for users.
// They can be used much like a enum, but in a type-safe way
RTI_RestoreStatus const
RTI_NO_RESTORE_IN_PROGRESS
= RTI_RestoreStatus::noRestoreInProgress();
RTI_RestoreStatus const
RTI_FEDERATE_RESTORE_REQUEST_PENDING
= RTI_RestoreStatus::federateRestoreRequestPending();
RTI_RestoreStatus const
RTI_FEDERATE_WAITING_FOR_RESTORE_TO_BEGIN
= RTI_RestoreStatus::federateWaitingForRestoreToBegin();
RTI_RestoreStatus const
RTI_FEDERATE_PREPARED_TO_RESTORE
= RTI_RestoreStatus::federatePreparedToRestore();
RTI_RestoreStatus const
RTI_FEDERATE_RESTORING
= RTI_RestoreStatus::federateRestoring();
RTI_RestoreStatus const
RTI_FEDERATE_WAITING_FOR_FEDERATION_TO_RESTORE
= RTI_RestoreStatus::federateWaitingForFederationToRestore();
#ifdef RTI_USE_INLINE
#include "RTI_RestoreStatus.i"
#endif // RTI_USE_INLINE
#endif // RTI_RestoreStatus_h
