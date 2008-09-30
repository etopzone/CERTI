/***********************************************************************
IEEE 1516.1 High Level Architecture Interface Specification C++ API
File: RTI_LogicalTimeFactory.h
***********************************************************************/
#ifndef RTI_LogicalTimeFactory_h
#define RTI_LogicalTimeFactory_h
//  The classes associated with logical time allow a federation to provide
//  their own representation for logical time and logical time interval. The
//  federation is responsible to inherit from the abstract classes declared
//  below. The encoded time classes are used to hold the arbitrary bit
//  representation of the logical time and logical time intervals.
class RTI_LogicalTime;
class RTI_CouldNotDecode;
class RTI_InternalError;
class RTI_EXPORT RTI_LogicalTimeFactory
{
  public:
   virtual
   ~RTI_LogicalTimeFactory()
     throw ();
   virtual
   RTI_auto_ptr< RTI_LogicalTime >
   makeInitial()
     throw (RTI_InternalError) = 0;
   virtual
   RTI_auto_ptr< RTI_LogicalTime >
   decode(RTI_EncodedLogicalTime const & encodedLogicalTime)
     throw (RTI_InternalError,
            RTI_CouldNotDecode) = 0;
};
#endif // RTI_LogicalTimeFactory_h
