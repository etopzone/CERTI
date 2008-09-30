/***********************************************************************
IEEE 1516.1 High Level Architecture Interface Specification C++ API
File: RTI_RTIambassadorFactory.h
***********************************************************************/
#ifndef RTI_RTIambassadorFactory_h
#define RTI_RTIambassadorFactory_h
#include <RTI_memory>
#include <RTI_vector>
#include <RTI_string>
class RTI_RTIinternalError;
class RTI_RTIambassador;
class RTI_RTIambassadorFactory

{
public:
   RTI_RTIambassadorFactory();
   virtual
   ~RTI_RTIambassadorFactory()
     throw ();
   // 10.35
   RTI_auto_ptr< RTI_RTIambassador >
   createRTIambassador(RTI_vector< RTI_wstring,
                                 RTI___STL_DEFAULT_ALLOCATOR(RTI_wstring) >
                      & args)
     throw (RTI_RTIinternalError);
};
#endif // RTI_RTIambassadorFactory_h
