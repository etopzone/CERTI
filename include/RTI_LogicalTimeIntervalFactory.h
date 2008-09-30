/***********************************************************************
IEEE 1516.1 High Level Architecture Interface Specification C++ API
File: RTI_LogicalTimeIntervalFactory.h
***********************************************************************/
#ifndef RTI_LogicalTimeIntervalFactory_h
#define RTI_LogicalTimeIntervalFactory_h
//  The classes associated with logical time allow a federation to provide
//  their own representation for logical time and logical time interval. The
//  federation is responsible to inherit from the abstract classes declared
//  below. The encoded time classes are used to hold the arbitrary bit
//  representation of the logical time and logical time intervals.
class RTI_EXPORT RTI_LogicalTimeIntervalFactory
{
  public:
   virtual
   ~RTI_LogicalTimeIntervalFactory()
     throw ();
   virtual
   RTI_auto_ptr< RTI_LogicalTimeInterval >
   makeZero() = 0;
   virtual
   RTI_auto_ptr< RTI_LogicalTimeInterval >
   epsilon() = 0;
   virtual
   RTI_auto_ptr< RTI_LogicalTimeInterval >
   decode(RTI_EncodedLogicalTimeInterval const & encodedLogicalTimeInterval)
     throw (RTI_CouldNotDecode) = 0;
};
#endif // RTI_LogicalTimeIntervalFactory_h
