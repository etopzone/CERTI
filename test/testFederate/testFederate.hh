#ifndef TEST_FEDERATE_HH
#define TEST_FEDERATE_HH

#if defined(HLA13)
  #include <RTI.hh>
  #include <NullFederateAmbassador.hh>
  typedef NullFederateAmbassador NullFederateAmbassador_t;
  typedef RTI::RTIambassador RTIambassador_t;
  #include <fedtime.hh>
  typedef RTI::FedTime LogicalTime_t;
  #define NSRTI(a) "RTI::a"
  typedef RTI::ObjectClassHandle              OC_Handle_t;
  typedef RTI::AttributeHandle                A_Handle_t;
  typedef RTI::InteractionClassHandle         IC_Handle_t;
  typedef RTI::ParameterHandle                P_Handle_t;
  typedef RTI::ParameterHandleValuePairSet    PHV_t;
#elif defined(IEEE1516_2000)
  #include <RTI/RTI1516.h>
  #include <RTI/Enums.h>
  #include <RTI/NullFederateAmbassador.h>
  typedef rti1516::NullFederateAmbassador NullFederateAmbassador_t;
  typedef rti1516::RTIambassador RTIambassador_t;
  #include <RTI/RTI1516fedTime.h>
  typedef rti1516::LogicalTime LogicalTime_t;
  #define NSRTI(a) "rti1516::##a"
  typedef rti1516::ObjectClassHandle              OC_Handle_t;
  typedef rti1516::AttributeHandle                A_Handle_t;
  typedef rti1516::InteractionClassHandle         IC_Handle_t;
  typedef rti1516::ParameterHandle                P_Handle_t;
  typedef rti1516::ParameterHandleValueMap        PHV_t;
#elif defined(IEEE1516_2010)
  #include <RTI/RTI1516.h>
  #include <RTI/Enums.h>
  #include <RTI/NullFederateAmbassador.h>
  typedef rti1516e::NullFederateAmbassador NullFederateAmbassador_t;
  typedef rti1516e::RTIambassador RTIambassador_t;
  #define NSRTI(a) "rti1516e::##a"
  typedef rti1516e::ObjectClassHandle              OC_Handle_t;
  typedef rti1516e::AttributeHandle                A_Handle_t;
  typedef rti1516e::InteractionClassHandle         IC_Handle_t;
  typedef rti1516e::ParameterHandle                P_Handle_t;
  typedef rti1516e::ParameterHandleValueMap        PHV_t;
#else
  #error "None of HLA13, IEEE1516_2000 or IEEE1516_2010 defined --> ERROR"
#endif

#include <string>
#include <sstream>
#include <stdint.h>

/**
* The test federate class.
* It inherits from the NullFederateAmbassador in order to implements RTI callbacks.
*/
class testFederate : public NullFederateAmbassador_t {
public:
    testFederate(std::wstring federateName) {
        verboseLevel = 0;
        name         = federateName;
        federateName = L"Not Set";
        FOMFile      = L"Not Set";
        rtiAmb       = NULL;
        connected    = true;
        status       = true;
        specificInitialize();
    };

#ifdef HLA13
    virtual ~testFederate() throw (RTI::FederateInternalError)
#elif IEEE1516_2000
    virtual ~testFederate() throw ()
#elif IEEE1516_2010
    virtual ~testFederate() throw ()
#endif
            {if (NULL != rtiAmb) delete rtiAmb;};


    /* Federation Management */
    bool createFederationExecution(std::wstring FederationName, std::wstring FOMFile);
    bool destroyFederationExecution();

    bool joinFederationExecution();
    bool resignFederationExecution();

    /* Declaration Management */
    /* Get handles */
    bool getHandles();
    bool publishObjectClassAttribute();
    bool unpublishObjectClassAttribute();
    bool publishInteractionClass();
    bool unpublishInteractionClass();

    bool subscribeObjectClassAttribute();
    bool unsubscribeObjectClassAttribute();
    bool subscribreInteractionClass();
    bool unsubscribreInteractionClass();

    /* Time Management I/F */
    bool evoke();
    bool TAR();
    bool TARA();
    bool enableTR();
    bool disableTR();
    bool enableAD();
    bool disableAD();


    void setVerbosityLevel(uint32_t level) {verboseLevel = level;};

    static const std::string getString(const std::wstring& wstr) {;
        return std::string(wstr.begin(),wstr.end());
    }

    static const std::wstring getWString(const char* cstr) {
        std::wstringstream ss;
        ss << cstr;
        return std::wstring(ss.str());
    }

protected:
    uint32_t              verboseLevel;
    std::wstring          name;
    std::wstring          federationName;
    std::wstring          FOMFile;
    RTIambassador_t*      rtiAmb;
    bool             connected;
    bool             status;

    /* Handles */
    OC_Handle_t  OCH_Data;
    A_Handle_t   AH_Attr1;
    A_Handle_t   AH_Attr2;
    IC_Handle_t  ICH_Message;
    P_Handle_t   PH_Param1;
    P_Handle_t   PH_Param2;
    /* This method have standard specific initialization */
    bool specificInitialize();
private:
};

#endif
