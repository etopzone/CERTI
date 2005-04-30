// HLA 1.3 Header "RTI.hh"
// $Id: RTI.hh,v 3.6 2005/04/30 16:30:43 breholee Exp $

#ifndef RTI_hh
#define RTI_hh

#if defined(_WIN32)
#pragma warning(disable: 4290)
#pragma warning(disable: 4275)
#pragma warning(disable: 4251)
#if defined(BUILDING_RTI)
#define RTI_EXPORT __declspec(dllexport)
#else
#define RTI_EXPORT __declspec(dllimport)
#endif
#if defined(BUILDING_FEDTIME)
#define RTI_EXPORT_FEDTIME __declspec(dllexport)
#else
#define RTI_EXPORT_FEDTIME __declspec(dllimport)
#endif
#else
#define RTI_EXPORT
#define RTI_EXPORT_FEDTIME
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

RTI_STD::ostream RTI_EXPORT & 
operator<<(RTI_STD::ostream &, RTI::Exception *);

RTI_STD::ostream RTI_EXPORT & 
operator<<(RTI_STD::ostream &, RTI::Exception const &);

#endif
