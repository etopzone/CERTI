// HLA 1.3 Header "RTI.hh"

#ifndef RTI_hh
#define RTI_hh

#if defined(_WIN32) || defined(__CYGWIN__)
	#pragma warning(disable: 4290)
	#pragma warning(disable: 4275)
	#pragma warning(disable: 4251)
	#pragma warning(disable: 4273)
	#pragma warning(disable: 4996)
	#define ANY_DLL_EXPORT __declspec(dllexport)
	#define ANY_DLL_IMPORT __declspec(dllimport)
	#define ANY_DLL_LOCAL
#else
    #if (__GNUC__ >= 4)
       #define ANY_DLL_EXPORT __attribute__ ((visibility("default")))
       #define ANY_DLL_IMPORT __attribute__ ((visibility("default")))
       #define ANY_DLL_LOCAL  __attribute__ ((visibility("hidden")))
    #else
       #define ANY_DLL_EXPORT
       #define ANY_DLL_IMPORT
       #define ANY_DLL_LOCAL
    #endif
#endif

#if defined(RTI_EXPORTS)
    #define RTI_EXPORT ANY_DLL_EXPORT
#else
    #define RTI_EXPORT ANY_DLL_IMPORT
#endif
#if defined(FedTime_EXPORTS)
    #define FEDTIME_EXPORT ANY_DLL_EXPORT
#else
    #define FEDTIME_EXPORT ANY_DLL_IMPORT
#endif

#ifdef RTI_USES_STD_FSTREAM
#include <fstream>
#define RTI_STD std
#else
#include <fstream.h>
#define RTI_STD
#endif

class RTIambPrivateRefs ;
struct RTIambPrivateData ;

/**
 * @defgroup libRTI RTI library (normative API).
 * @ingroup HLA_Libraries
 * The API comes directly from HLA specifications.
 * @{
 */

class RTI_EXPORT RTI
{
public:
#include "baseTypes.hh"
#include "RTItypes.hh"

	/**
	 * RTI (RunTime Infrastructure) Ambassador class.
	 * The RTIambassador class is the interface used by an
	 * HLA federate to call the RTI. The RTI may call the
	 * federate back using the Federate Ambassador class
	 * @see FederateAmbassador.
	 */
    class RTI_EXPORT RTIambassador
    {
    public:
#include "RTIambServices.hh"
	RTIambPrivateData *privateData ;
    private:
	RTIambPrivateRefs* privateRefs ;
    };

    /**
     * Federate Ambassador class.
     * The FederateAmbassador is the structure RTI callback class.
     * When the RTI needs to contact the federate it calls one
     * of the FederateAmbassador class methods.
     */
    class RTI_EXPORT FederateAmbassador
    {
    public:
#include "federateAmbServices.hh"
    };
};

RTI_STD::ostream RTI_EXPORT &
operator<<(RTI_STD::ostream &os, RTI::Exception *ex);

RTI_STD::ostream RTI_EXPORT &
operator<<(RTI_STD::ostream &os, RTI::Exception const &ex);

/** @} */

#endif
