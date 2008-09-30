/***********************************************************************
IEEE 1516.1 High Level Architecture Interface Specification C++ API
File: RTI_SaveStatus.h
***********************************************************************/
#ifndef RTI_SaveStatus_h
#define RTI_SaveStatus_h
class RTI_bool;
#include <RTI_SpecificConfig.h>
#include <RTI_string>
// Type safe class used to represent save status of an individual federate.
class RTI_EXPORT RTI_SaveStatus
{
public:
   RTI_SaveStatus(RTI_SaveStatus const & rhs);
   static
   RTI_SaveStatus const
   noSaveInProgress();
   static
   RTI_SaveStatus const
   federateInstructedToSave();
   static
   RTI_SaveStatus const
   federateSaving();
   static
   RTI_SaveStatus const
   federateWaitingForFederationToSave();
   RTI_SaveStatus &
   operator=(RTI_SaveStatus const & rhs);
   RTI_bool
   operator==(RTI_SaveStatus const & rhs) const;
   RTI_bool
   operator!=(RTI_SaveStatus const & rhs) const;
   RTI_wstring const
   toString() const;
  private:
   RTI_SaveStatus(unsigned _SaveStatus);
   unsigned _SaveStatus;
};
// These constants save a little typing for users.
// They can be used much like a enum, but in a type-safe way
RTI_SaveStatus const
RTI_NO_SAVE_IN_PROGRESS
= RTI_SaveStatus::noSaveInProgress();
RTI_SaveStatus const
RTI_FEDERATE_INSTRUCTED_TO_SAVE
= RTI_SaveStatus::federateInstructedToSave();
RTI_SaveStatus const
RTI_FEDERATE_SAVING
= RTI_SaveStatus::federateSaving();
RTI_SaveStatus const
RTI_FEDERATE_WAITING_FOR_FEDERATION_TO_SAVE
= RTI_SaveStatus::federateWaitingForFederationToSave();
#ifdef RTI_USE_INLINE
#include "RTI_SaveStatus.i"
#endif // RTI_USE_INLINE
#endif // RTI_SaveStatus_h
