// HLA 1.3 Header "RTI.hh"
// $Id: RTI.hh,v 3.8 2007/08/10 14:45:49 erk Exp $

#ifndef RTI_hh
#define RTI_hh

#if defined(_WIN32)
	#include <windows.h>
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
	
	#if defined(CERTI_EXPORTS)
		#define CERTI_EXPORT __declspec(dllexport)
	#else
		#define CERTI_EXPORT __declspec(dllimport)
	#endif
#else
	#define RTI_EXPORT  
	#define CERTI_EXPORT  
#endif

#ifdef RTI_USES_STD_FSTREAM
#include <fstream>
#define RTI_STD std
#else
#include <fstream.h>
#define RTI_STD
#endif

struct RTIambPrivateRefs ;
struct RTIambPrivateData ;

/**
 * @defgroup libRTI the RTI library (normative API).
 * @ingroup HLA_Libraries 
 * The API comes directly from HLA specifications. 
 * @{
 */ 

class RTI
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

RTI_STD::ostream CERTI_EXPORT & 
operator<<(RTI_STD::ostream &, RTI::Exception *);

RTI_STD::ostream CERTI_EXPORT & 
operator<<(RTI_STD::ostream &, RTI::Exception const &);

/** @} */

#endif
