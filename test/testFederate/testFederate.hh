#ifndef TEST_FEDERATE_HH
#define TEST_FEDERATE_HH

#if defined(HLA13)
  #include <RTI.hh>
  #include <NullFederateAmbassador.hh>
  typedef NullFederateAmbassador NullFederateAmbassador_t;
  typedef RTI::RTIambassador RTIambassador_t;
  #include <fedtime.hh>
  #define NSRTI(a) "RTI::a"
#elif defined(IEEE1516_2000)
  #include <RTI/RTI1516.h>
  #include <RTI/Enums.h>
  #include <RTI/NullFederateAmbassador.h>
  typedef rti1516::NullFederateAmbassador NullFederateAmbassador_t;
  typedef rti1516::RTIambassador RTIambassador_t ;
  #include <RTI/RTI1516fedTime.h>
  #define NSRTI(a) "rti1516::##a"
#elif defined(IEEE1516_2010)
  #include <RTI/RTI1516.h>
  #include <RTI/Enums.h>
  #include <RTI/NullFederateAmbassador.h>
  typedef rti1516e::NullFederateAmbassador NullFederateAmbassador_t;
  typedef rti1516e::RTIambassador RTIambassador_t;
  #define NSRTI(a) "rti1516e::##a"
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


    bool createFederationExecution(std::wstring FederationName, std::wstring FOMFile);
    bool destroyFederationExecution();

    bool joinFederationExecution();
    bool resignFederationExecution();

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
    uint32_t         verboseLevel;
    std::wstring     name;
    std::wstring     federationName;
    std::wstring     FOMFile;
    RTIambassador_t* rtiAmb;
    bool             status;

    /* This method have standard specific initialization */
    bool specificInitialize();
private:
};

#endif
