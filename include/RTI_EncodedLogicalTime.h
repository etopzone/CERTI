/***********************************************************************
IEEE 1516.1 High Level Architecture Interface Specification C++ API
File: RTI_EncodedLogicalTime.h
***********************************************************************/
#ifndef RTI_EncodedLogicalTime_h
#define RTI_EncodedLogicalTime_h
#include <RTI_SpecificConfig.h>
class RTI_EncodedLogicalTime
{
public:
   virtual ~RTI_EncodedLogicalTime() throw () {}
   virtual voidconst * data() const = 0;
   virtual size_tsize()     const = 0;
};
#endif // RTI_EncodedLogicalTime_h
