/***********************************************************************
IEEE 1516.1 High Level Architecture Interface Specification C++ API
File: RTI_ResignAction.h
***********************************************************************/
#ifndef RTI_ResignAction_h
#define RTI_ResignAction_h
class RTI_bool;
#include <RTI_SpecificConfig.h>
#include <RTI_string>
// Type safe class used to represent action taken on resign.
class RTI_EXPORT RTI_ResignAction
{
public:
   RTI_ResignAction(RTI_ResignAction const & rhs);
   static
   RTI_ResignAction const
   unconditionallyDivestAttributes();
   static
   RTI_ResignAction const
   deleteObjects();
   static
   RTI_ResignAction const
   cancelPendingOwnershipAcquisitions();
   static
   RTI_ResignAction const
   deleteObjectsThenDivest();
   static
   RTI_ResignAction const
   cancelThenDeleteThenDivest();
   static
   RTI_ResignAction const
   noAction();
   RTI_ResignAction &
   operator=(RTI_ResignAction const & rhs);
   RTI_bool
   operator==(RTI_ResignAction const & rhs) const;
   RTI_bool
   operator!=(RTI_ResignAction const & rhs) const;
   RTI_wstring const
   toString() const;
  private:
   RTI_ResignAction(unsigned _resignAction);
   unsigned _resignAction;
};

// These constants save a little typing for users.
// They can be used much like a enum, but in a type-safe way
RTI_ResignAction const
RTI_UNCONDITIONALLY_DIVEST_ATTRIBUTES
= RTI_ResignAction::unconditionallyDivestAttributes();
RTI_ResignAction const
RTI_DELETE_OBJECTS
= RTI_ResignAction::deleteObjects();
RTI_ResignAction const
RTI_CANCEL_PENDING_OWNERSHIP_ACQUISITIONS
= RTI_ResignAction::cancelPendingOwnershipAcquisitions();
RTI_ResignAction const
RTI_DELETE_OBJECTS_THEN_DIVEST
= RTI_ResignAction::deleteObjectsThenDivest();
RTI_ResignAction const
RTI_CANCEL_THEN_DELETE_THEN_DIVEST
= RTI_ResignAction::cancelThenDeleteThenDivest();
RTI_ResignAction const
RTI_NO_ACTION
= RTI_ResignAction::noAction();
#ifdef RTI_USE_INLINE
#include "RTI_ResignAction.i"
#endif // RTI_USE_INLINE
#endif // RTI_ResignAction_h
