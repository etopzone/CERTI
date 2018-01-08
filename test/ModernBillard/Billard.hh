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

#include "Ball.hh"
#include "ColoredBall.hh"
#include <RTI/NullFederateAmbassador.h>
#include <RTI/RTI1516.h>
#include <RTI/RTI1516fedTime.h>

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

class Billard : public rti1516e::NullFederateAmbassador {
public:
    Billard(const std::string& federate_name, rti1516e::RTIambassador& ambassador);

    virtual ~Billard() noexcept = default;

    void init(int);

    void init(int, int);

    virtual void declare();

    /** Join the federation
     * \param federation_name Federation name
     * \param fdd_name Federation designator (.fed file)
     */
    void join(std::string federation_name, std::string fdd_name);

    void pause();

    void pause_friend();

    virtual void publishAndSubscribe();

    void resign();

    void setTimeRegulation(bool constrained, bool regulating);

    void setVerbose(bool flag)
    {
        verbose = flag;
    }

    void setNotimestamp(bool flag)
    {
        notimestamp = flag;
    }

    void step();

    virtual void checkRegions();

    void synchronize(int);

    void tick();

    void tick2();

    rti1516e::FederateHandle getHandle() const;

    // Callbacks
    virtual void
    announceSynchronizationPoint(std::wstring const& label,
                                 rti1516e::VariableLengthData const& theUserSuppliedTag)
    throw(rti1516e::FederateInternalError) override;

    virtual void
    federationSynchronized(std::wstring const& label,
                           rti1516e::FederateHandleSet const& failedToSyncSet) throw(rti1516e::FederateInternalError) override;

    virtual void timeAdvanceGrant(rti1516e::LogicalTime const& theTime) throw(FederateInternalError) override;

    virtual void discoverObjectInstance(rti1516e::ObjectInstanceHandle theObject,
                                        rti1516e::ObjectClassHandle theObjectClass,
                                        std::wstring const& theObjectInstanceName) throw(rti1516e::FederateInternalError) override;

    virtual void reflectAttributeValues(rti1516e::ObjectInstanceHandle theObject,
                                        rti1516e::AttributeHandleValueMap const& theAttributeValues,
                                        rti1516e::VariableLengthData const& theUserSuppliedTag,
                                        rti1516e::OrderType sentOrder,
                                        rti1516e::TransportationType theType,
                                        rti1516e::SupplementalReflectInfo theReflectInfo) throw(rti1516e::FederateInternalError) override;

    virtual void reflectAttributeValues(rti1516e::ObjectInstanceHandle theObject,
                                        rti1516e::AttributeHandleValueMap const& theAttributeValues,
                                        rti1516e::VariableLengthData const& theUserSuppliedTag,
                                        rti1516e::OrderType sentOrder,
                                        rti1516e::TransportationType theType,
                                        rti1516e::LogicalTime const& theTime,
                                        rti1516e::OrderType receivedOrder,
                                        rti1516e::SupplementalReflectInfo theReflectInfo) throw(rti1516e::FederateInternalError) override;

    virtual void receiveInteraction(rti1516e::InteractionClassHandle theInteraction,
                                    rti1516e::ParameterHandleValueMap const& theParameterValues,
                                    rti1516e::VariableLengthData const& theUserSuppliedTag,
                                    rti1516e::OrderType sentOrder,
                                    rti1516e::TransportationType theType,
                                    rti1516e::SupplementalReceiveInfo theReceiveInfo) throw(rti1516e::FederateInternalError) override;

    virtual void receiveInteraction(rti1516e::InteractionClassHandle theInteraction,
                                    rti1516e::ParameterHandleValueMap const& theParameterValues,
                                    rti1516e::VariableLengthData const& theUserSuppliedTag,
                                    rti1516e::OrderType sentOrder,
                                    rti1516e::TransportationType theType,
                                    rti1516e::LogicalTime const& theTime,
                                    rti1516e::OrderType receivedOrder,
                                    rti1516e::SupplementalReceiveInfo theReceiveInfo) throw(rti1516e::FederateInternalError) override;

    virtual void removeObjectInstance(rti1516e::ObjectInstanceHandle theObject,
                                      rti1516e::VariableLengthData const& theUserSuppliedTag,
                                      rti1516e::OrderType sentOrder,
                                      rti1516e::SupplementalRemoveInfo theRemoveInfo) throw(rti1516e::FederateInternalError) override;

    rti1516e::ObjectInstanceHandle registerBallInstance(const char*);

    void sendInteraction(double, double, const rti1516e::LogicalTime&, rti1516e::ObjectInstanceHandle);

    void sendInteraction(double, double, rti1516e::ObjectInstanceHandle);

    void sendUpdate(double, double, int, const rti1516e::LogicalTime&, rti1516e::ObjectInstanceHandle);

    bool getCreator()
    {
        return creator;
    };

protected:
    std::string my_federate_name;

    virtual void getHandles();

    rti1516e::RTIambassador& my_amb;
    ColoredBall local{0};
    std::vector<Ball> remote;

    std::string federationName{""}; /// setup at join

    rti1516e::FederateHandle handle;
    bool creator{false};
    long nbTicks{0};

    bool regulating{false};
    bool constrained{false};
    bool notimestamp{false}; // true no timestamp i.e.false means with time
    rti1516e::LogicalTime localTime;
    const rti1516e::LogicalTime TIME_STEP{1.0};

    const int XMAX{500};
    const int YMAX{100};

    bool paused;
    bool granted;
    bool verbose;

    // Handles
    rti1516e::ObjectClassHandle BilleClassID;
    rti1516e::ObjectClassHandle BouleClassID;
    rti1516e::AttributeHandle AttrXID;
    rti1516e::AttributeHandle AttrYID;
    rti1516e::AttributeHandle AttrColorID;
    rti1516e::InteractionClassHandle BingClassID;
    rti1516e::ParameterHandle ParamDXID;
    rti1516e::ParameterHandle ParamDYID;
    rti1516e::ParameterHandle ParamBoulID;
};

#endif // CERTI_BILLARD_HH

// $Id: Billard.hh,v 3.14 2010/03/19 13:54:03 erk Exp $
