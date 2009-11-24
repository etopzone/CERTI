// HLA 1.3 Header "RTI.hh"
// $Id: RTI.hh,v 3.17 2009/11/24 21:44:48 erk Exp $

#ifndef RTI_hh
#define RTI_hh

#if defined(_WIN32)
	#pragma warning(disable: 4290)
	#pragma warning(disable: 4275)
	#pragma warning(disable: 4251)
	#pragma warning(disable: 4273)
	#pragma warning(disable: 4996)
	#if defined(RTI_EXPORTS)
		#define RTI_EXPORT __declspec(dllexport)
	#else
		#define RTI_EXPORT __declspec(dllimport)
	#endif
    #if defined(FedTime_EXPORTS)
        #define FEDTIME_EXPORT __declspec(dllexport)
    #else
        #define FEDTIME_EXPORT __declspec(dllimport)
    #endif
#else
	#define RTI_EXPORT
    #define FEDTIME_EXPORT
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

    class RTI_EXPORT RTIambassador
    {
    public:
#include "RTIambServices.hh"
	RTIambPrivateData *privateData ;
    private:
	RTIambPrivateRefs* privateRefs ;
    };

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
