// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This file is part of CERTI
//
// CERTI is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// $Id: Billard.hh,v 3.14 2010/03/19 13:54:03 erk Exp $
// ----------------------------------------------------------------------------

#ifndef CERTI_BILLARD_HH
#define CERTI_BILLARD_HH

#include "RTI.hh"
#include "NullFederateAmbassador.hh"
#include "fedtime.hh"
#include "Ball.hh"
#include "ColoredBall.hh"

#include <vector>

// Classes
#define CLA_BILLE "Bille"
#define CLA_BOULE "Boule"

// Attributes
#define ATT_POSITION_X "PositionX"
#define ATT_POSITION_Y "PositionY"
#define ATT_COLOR "Color"

// Interactions
#define INT_BING "Bing"

// Parameters
#define PAR_DX "DX"
#define PAR_DY "DY"
#define PAR_BOUL "BoulNum"

// Types
#define TYP_FLOAT "float"
#define TYP_INT "int"

// Test
// TEST_RFSP defined is used for testing registerFederationSynchronizationPoint
//           with a set of federates.
#define TEST_RFSP
// Here no test. Comment next line if you want to test requestFederationSave
#undef TEST_RFSP



class Billard : public NullFederateAmbassador
{
public:

    Billard(std::string);
    virtual ~Billard() throw (RTI::FederateInternalError);

    void init(int);
    void init(int, int);
    virtual void declare();
    void join(std::string, std::string);
    void pause();
    void pause_friend();
    virtual void publishAndSubscribe();
    void resign();
    void setTimeRegulation(bool constrained, bool regulating);
    void setVerbose(bool flag) { verbose = flag ; }
    void setNotimestamp(bool flag) { notimestamp = flag ; }
    void step();
    virtual void checkRegions();
    void synchronize(int);
    void tick();
    void tick2();

    RTI::FederateHandle getHandle() const ;

    // Callbacks
    void announceSynchronizationPoint(const char *label, const char *tag)
        throw (RTI::FederateInternalError);

    void federationSynchronized(const char *label)
        throw (RTI::FederateInternalError);

    void timeAdvanceGrant(const RTI::FedTime& theTime)
        throw (RTI::FederateInternalError, RTI::TimeAdvanceWasNotInProgress, 
	       RTI::InvalidFederationTime);

    void discoverObjectInstance(RTI::ObjectHandle theObject,
                                RTI::ObjectClassHandle theObjectClass,
                                const char *theObjectName)
        throw (RTI::FederateInternalError, RTI::ObjectClassNotKnown, RTI::CouldNotDiscover);

    void reflectAttributeValues(RTI::ObjectHandle theObject,
                                const RTI::AttributeHandleValuePairSet& theAttributes,
                                const RTI::FedTime& theTime, const char *theTag,
                                RTI::EventRetractionHandle theHandle)
        throw (RTI::ObjectNotKnown, RTI::AttributeNotKnown,
	       RTI::InvalidFederationTime, RTI::FederateInternalError);

    void reflectAttributeValues(RTI::ObjectHandle,
                                const RTI::AttributeHandleValuePairSet &,
                                const char *)
        throw (RTI::ObjectNotKnown, RTI::AttributeNotKnown,
	       RTI::FederateInternalError);

    void receiveInteraction(RTI::InteractionClassHandle theInteraction,
                            const RTI::ParameterHandleValuePairSet& theParameters,
                            const RTI::FedTime& theTime, const char *theTag,
                            RTI::EventRetractionHandle theHandle)
        throw (RTI::InteractionClassNotKnown, RTI::InteractionParameterNotKnown,
               RTI::InvalidFederationTime, RTI::FederateInternalError);

    void receiveInteraction(RTI::InteractionClassHandle,
                            const RTI::ParameterHandleValuePairSet &,
                            const char *)
        throw (RTI::InteractionClassNotKnown, RTI::InteractionParameterNotKnown,
	       RTI::FederateInternalError) { };

    void removeObjectInstance(RTI::ObjectHandle theObject, const RTI::FedTime& theTime,
			      const char *theTag,
			      RTI::EventRetractionHandle theHandle)
	throw (RTI::ObjectNotKnown, RTI::InvalidFederationTime, RTI::FederateInternalError);

    void removeObjectInstance(RTI::ObjectHandle, const char *)
	throw (RTI::ObjectNotKnown, RTI::FederateInternalError) { };

    RTI::ObjectHandle registerBallInstance(const char *);

    void sendInteraction(double, double, const RTI::FedTime &, RTI::ObjectHandle);
    void sendInteraction(double, double, RTI::ObjectHandle);

    void sendUpdate(double, double, int, const RTI::FedTime &, RTI::ObjectHandle);

    bool getCreator(){return creator;};

    /**
     * Helper class to simplify string construction. Implemented as
     * a stringstream wrapper.
     *
     * For example:
     * throw AttributeNotDefined(stringize() << "value: " << number);
     */
    template<typename C>
    struct basic_stringize
    {
      template<typename T>
      basic_stringize<C> & operator << (const T& t)
      {
        m_s << t;
        return *this;
      }

      // note: must not return reference
      operator const std::basic_string<C>() const
      {
        return m_s.str();
      }

    private:
      std::basic_stringstream<C> m_s;
    };

    typedef basic_stringize<char> stringize;
    typedef basic_stringize<wchar_t> wstringize;

protected:
    virtual void getHandles();
 
    RTI::RTIambassador rtiamb ;
    ColoredBall local ;
    std::vector<Ball> remote ;

    std::string federateName ;
    std::string federationName ;

    RTI::FederateHandle handle ;
    bool creator ;
    long nbTicks ;

    bool regulating ;
    bool constrained ;
    bool notimestamp ;   // true no timestamp i.e.false means with time
    RTIfedTime localTime ;
    const RTIfedTime TIME_STEP ;

    const int XMAX ;
    const int YMAX ;

    bool paused ;
    bool granted ;
    bool verbose ;

    // Handles
    RTI::ObjectClassHandle BilleClassID ;
    RTI::ObjectClassHandle BouleClassID ;
    RTI::AttributeHandle AttrXID ;
    RTI::AttributeHandle AttrYID ;
    RTI::AttributeHandle AttrColorID ;
    RTI::InteractionClassHandle BingClassID ;
    RTI::ParameterHandle ParamDXID ;
    RTI::ParameterHandle ParamDYID ;
    RTI::ParameterHandle ParamBoulID ;
};

#endif // CERTI_BILLARD_HH

// $Id: Billard.hh,v 3.14 2010/03/19 13:54:03 erk Exp $
