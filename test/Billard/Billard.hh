// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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
// $Id: Billard.hh,v 3.4 2003/10/27 10:51:38 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef CERTI_BILLARD_HH
#define CERTI_BILLARD_HH

#include "RTI.hh"
#include "Objects.hh"
#include "BillardNull.hh"

#include <string>

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

class Billard : public BillardNull
{
public:
    Billard(std::string);

    void init(int);
    void init(int, int);
    virtual void declare();
    void join(std::string, std::string);
    void pause();
    void publishAndSubscribe();
    void resign();
    void setTimeRegulation(bool, bool);
    void setVerbose(bool flag) { verbose = flag ; }
    void step();
    void synchronize(int);
    void tick();

    FederateHandle getHandle() const ;

    // Callbacks
    void announceSynchronizationPoint(const char *label, const char *tag)
        throw (FederateInternalError);

    void federationSynchronized(const char *label)
        throw (FederateInternalError);

    void timeAdvanceGrant(const FedTime& theTime)
        throw (InvalidFederationTime, TimeAdvanceWasNotInProgress,
               FederationTimeAlreadyPassed, FederateInternalError);

    void discoverObjectInstance(ObjectHandle theObject,
                                ObjectClassHandle theObjectClass,
                                const char *theObjectName)
        throw (CouldNotDiscover, ObjectClassNotKnown, InvalidFederationTime,
               FederateInternalError);

    void reflectAttributeValues(ObjectHandle theObject,
                                const AttributeHandleValuePairSet& theAttributes,
                                const FedTime& theTime, const char *theTag,
                                EventRetractionHandle theHandle)
        throw (ObjectNotKnown, AttributeNotKnown, InvalidFederationTime,
               FederateInternalError);

    void receiveInteraction(InteractionClassHandle theInteraction,
                            const ParameterHandleValuePairSet& theParameters,
                            const FedTime& theTime, const char *theTag,
                            EventRetractionHandle theHandle)
        throw (InteractionClassNotKnown, InteractionParameterNotKnown,
               InvalidFederationTime, FederateInternalError);

    void removeObjectInstance(ObjectHandle theObject, const FedTime& theTime,
			      const char *theTag,
			      EventRetractionHandle theHandle)
	throw (ObjectNotKnown, InvalidFederationTime, FederateInternalError);

    ObjectHandle registerBallInstance(const char *);

    void sendInteraction(double, double, const FedTime &, ObjectHandle);
    void sendUpdate(double, double, int, const FedTime &, ObjectHandle);

protected:
    void getHandles();
 
    RTIambassador rtiamb ;
    Objects objects ;

    std::string federateName ;
    std::string federationName ;

    FederateHandle handle ;
    bool creator ;
    long nbTicks ;

    bool regulating ;
    bool constrained ;
    RTIfedTime localTime ;
    const RTIfedTime TIME_STEP ;

    const int XMAX ;
    const int YMAX ;
    bool paused ;
    bool granted ;
    bool verbose ;

    // Handles
    ObjectClassHandle BilleClassID ;
    ObjectClassHandle BouleClassID ;
    AttributeHandle AttrXID ;
    AttributeHandle AttrYID ;
    AttributeHandle AttrColorID ;
    InteractionClassHandle BingClassID ;
    ParameterHandle ParamDXID ;
    ParameterHandle ParamDYID ;
    ParameterHandle ParamBoulID ;
    SpaceHandle GeoID ;
};

#endif // CERTI_BILLARD_HH

// $Id: Billard.hh,v 3.4 2003/10/27 10:51:38 breholee Exp $
