/***********************************************************************
IEEE 1516.1 High Level Architecture Interface Specification C++ API
File: RTI_RangeBounds.h
***********************************************************************/
#ifndef RTI_RangeBounds_h
#define RTI_RangeBounds_h
#include <RTI_SpecificConfig.h>
class RTI_EXPORT RTI_RangeBounds
{
public:
   RTI_RangeBounds();
   RTI_RangeBounds(unsigned long lowerBound,
          unsigned long upperBound);
   ~RTI_RangeBounds()
     throw ();
   RTI_RangeBounds(RTI_RangeBounds const & rhs);
   RTI_RangeBounds &
   operator=(RTI_RangeBounds const & rhs);
   unsigned long
   getLowerBound() const;
   unsigned long
   getUpperBound() const;
   void
   setLowerBound(unsigned long lowerBound);
   void
   setUpperBound(unsigned long upperBound);
   private:
   unsigned long _lowerBound;
   unsigned long _upperBound;
};
#ifdef RTI_USE_INLINE
#include "RTI_RangeBounds.i"
#endif // RTI_USE_INLINE
#endif // RTI_RangeBounds_h
