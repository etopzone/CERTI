// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This file is part of Billard
//
// Billard is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// Billard is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
// ----------------------------------------------------------------------------

#include <config.h>

#include "Billard.hh"
#include "MessageBuffer.hh"
#include "PrettyDebug.hh"

#ifndef _MSC_VER
#include <unistd.h>
#endif

#ifdef _WIN32
#include <windows.h>
#define sleep(a) Sleep(a * 1000)
#define usleep(a) Sleep(a / 1000)
#endif

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <memory>

using std::unique_ptr;
using std::string;
using std::endl;
using std::cout;
using std::vector;

static PrettyDebug D("BILLARD", __FILE__);

// ----------------------------------------------------------------------------
/** Constructor
 */
Billard::Billard(std::string federate_name)
    : rtiamb()
    , local(0)
    , federateName(federate_name)
    , handle(0)
    , creator(false)
    , nbTicks(0)
    , regulating(false)
    , constrained(false)
    , notimestamp(false)
    , // i.e. with time
    localTime(0.0)
    , TIME_STEP(1.0)
    , XMAX(500)
    , YMAX(100)
{
}

// ----------------------------------------------------------------------------
/** Destructor
 */
Billard::~Billard() throw(RTI::FederateInternalError)
{
}

// ----------------------------------------------------------------------------
/** Get the federate handle
 */
RTI::FederateHandle Billard::getHandle() const
{
    return handle;
}

// ----------------------------------------------------------------------------
/** Join the federation
    \param federation_name Federation name
    \param fdd_name Federation designator (.fed file)
 */
void Billard::join(std::string federation_name, std::string fdd_name)
{
    federationName = federation_name;

    // create federation
    try {
        rtiamb.createFederationExecution(federation_name.c_str(), fdd_name.c_str());
        Debug(D, pdInit) << "Federation execution created." << std::endl;
        creator = true;
    }
    catch (RTI::FederationExecutionAlreadyExists& e) {
        printf("BILLARD Note : %s Reason is : %s. OK I can join it\n", e._name, e._reason);
        Debug(D, pdInit) << "Federation execution already created." << std::endl;
    }
    catch (RTI::CouldNotOpenFED& e) {
        printf("BILLARD ERROR : %s Reason is : %s\n", e._name, e._reason);
        Debug(D, pdExcept) << "BILLARD : Could not use FED file." << std::endl;
        delete &rtiamb; // Says RTIA to stop.
        exit(0);
    }

    // join federation
    bool joined = false;
    int nb = 5;

    while (!joined && nb > 0) {
        nb--;
        try {
            handle = rtiamb.joinFederationExecution(federateName.c_str(), federation_name.c_str(), this);
            joined = true;
            break;
        }
        catch (RTI::FederateAlreadyExecutionMember& e) {
            Debug(D, pdExcept) << "Federate " << federateName << "already exists." << endl;

            throw;
        }
        catch (RTI::FederationExecutionDoesNotExist& e) {
            Debug(D, pdExcept) << "Federate " << federateName << ": FederationExecutionDoesNotExist." << std::endl;
            // sleep(1);
        }
        catch (RTI::Exception& e) {
            Debug(D, pdExcept) << "Federate " << federateName << " : Join Federation Execution failed : " << &e
                               << std::endl;
            throw;
        }
    }
}

// ----------------------------------------------------------------------------
/** Creator put federation in pause.
 */
void Billard::pause()
{
    if (creator) {
        Debug(D, pdInit) << "Pause requested" << std::endl;
        try {
            rtiamb.registerFederationSynchronizationPoint("Init", "Waiting all players.");
        }
        catch (RTI::Exception& e) {
            Debug(D, pdExcept) << "Federate " << federateName << " : Register Synchronization Point failed : " << endl;
        }
    }
}

// ----------------------------------------------------------------------------
/** Creator put federation in pause for synchronization with a friend
 */
void Billard::pause_friend()
{
    if (creator) {
        Debug(D, pdInit) << "Pause requested for friend" << std::endl;
        try {
            // For testing purpose
            RTI::FederateHandle numfed(0);
            RTI::FederateHandleSet* federateSet = RTI::FederateHandleSetFactory::create(1);
            cout << "Now we test Register Federation Synchronisation Point on some federates" << endl;
            cout << "Please enter a federate handle (zero means none)" << endl;
            cout << "This federate will be synchronized with the creator and not the others" << endl;
            /* FIXME we should use C++ I/O and not mix C I/O with  C++ I/O  
             while ( not (std::cin >> numfed) || numfed < 0) {
	         std::cout << "That's not a number; ";
		     std::cin.clear();
		     std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		     }
		     */
            (void) scanf("%lu", &numfed);
            if (numfed != 0) {
                // We store numfed into the federate set
                federateSet->add(numfed);
                rtiamb.registerFederationSynchronizationPoint("Friend", "Synchro with a friend", *federateSet);
            }
        }
        catch (RTI::Exception& e) {
            Debug(D, pdExcept) << "Federate " << federateName << " : Register Synchronization Point failed" << endl;
        }
    }
}
// ----------------------------------------------------------------------------
/** tick the RTI
 */
void Billard::tick()
{
    usleep(0);
    rtiamb.tick();
    nbTicks++;
}
void Billard::tick2()
{
    rtiamb.tick2();
    nbTicks++;
}

// ----------------------------------------------------------------------------
/** Set time regulation (time regulating and time constrained)
    @param start_constrained boolean, if true federate is constrained
    @param start_regulating boolean, if true federate is regulating
 */
void Billard::setTimeRegulation(bool start_constrained, bool start_regulating)
{
    Debug(D, pdInit) << "Time Regulation setup" << std::endl;

    if (start_constrained) {
        if (!constrained) {
            // change from no constrained to constrained
            rtiamb.enableTimeConstrained();
            constrained = true;
            Debug(D, pdInit) << "Time Constrained enabled." << std::endl;
        }
        //rtiamb.modifyLookahead(TIME_STEP);
    }
    else {
        if (constrained) {
            // change from constrained to no constrained
            rtiamb.disableTimeConstrained();
            constrained = false;
            Debug(D, pdInit) << "Time Constrained disabled." << std::endl;
        }
    }

    if (start_regulating) {
        if (!regulating) {
            // change from no regulating to regulating
            for (;;) {
                rtiamb.queryFederateTime(localTime);

                try {
                    rtiamb.enableTimeRegulation(localTime, TIME_STEP);
                    regulating = true;
                    break;
                }
                catch (RTI::FederationTimeAlreadyPassed) {
                    // Si Je ne suis pas le premier, je vais les rattraper.
                    rtiamb.queryFederateTime(localTime);

                    RTIfedTime requestTime(((RTIfedTime&) localTime).getTime());
                    requestTime += TIME_STEP;

                    granted = false;
                    rtiamb.timeAdvanceRequest(requestTime);
                    while (!granted) {
                        try {
                            tick();
                        }
                        catch (RTI::RTIinternalError) {
                            printf("RTIinternalError Raised in tick.\n");
                            exit(-1);
                        }
                    }
                }
                catch (RTI::RTIinternalError) {
                    printf("RTIinternalError Raised in setTimeRegulating.\n");
                    exit(-1);
                }
            }
        }
    }
    else {
        if (regulating) {
            // change from regulating to no regulating
            rtiamb.disableTimeRegulation();
            regulating = false;
        }
    }
}

// ----------------------------------------------------------------------------
/** Synchronize with other federates
 */
void Billard::synchronize(int autostart)
{
    Debug(D, pdInit) << "Synchronize" << std::endl;

    if (creator) {
        // Wait a signal from user and stop the pause synchronization.

        if (autostart == 0) {
            cout << "Press ENTER to start execution..." << endl;
            getchar();
        }
        else {
            while (autostart >= 0) {
                sleep(1);
                printf("\rAutostart : %5d", autostart);
                fflush(stdout);
                autostart--;
            }
            printf("\n");
        }

        Debug(D, pdInit) << "Creator can resume execution..." << std::endl;
        while (!paused)
            try {
                Debug(D, pdInit) << "not paused" << std::endl;
                tick();
            }
            catch (RTI::Exception& e) {
                Debug(D, pdExcept) << "******** Exception ticking the RTI : " << &e << std::endl;
                throw;
            }
        Debug(D, pdDebug) << "paused" << std::endl;

        try {
            rtiamb.synchronizationPointAchieved("Init");
        }
        catch (RTI::Exception& e) {
            Debug(D, pdExcept) << "**** Exception achieving a synchronization point by creator : " << &e << std::endl;
        }

        while (paused)
            try {
                tick();
            }
            catch (RTI::Exception& e) {
                Debug(D, pdExcept) << "**** Exception ticking the RTI : " << &e << std::endl;
                throw;
            }
    }
    else {
        if (autostart != 0) {
            printf("Ignoring autostart (federate is not creator)\n");
        }
        printf("Synchronization...\n");

        if (!paused) {
            Debug(D, pdInit) << "Federate not paused: too early" << std::endl;
            while (!paused) {
                try {
                    tick();
                }
                catch (RTI::Exception& e) {
                    Debug(D, pdExcept) << "******** Exception ticking the RTI : " << &e << std::endl;
                    throw;
                }
            }
        }
        Debug(D, pdInit) << "Federate paused" << std::endl;

        try {
            // Federate ends its synchronization.
            rtiamb.synchronizationPointAchieved("Init");
            Debug(D, pdInit) << "Pause achieved." << std::endl;
        }
        catch (RTI::Exception& e) {
            Debug(D, pdExcept) << "**** Exception achieving a synchronization point : " << &e << std::endl;
        }

        Debug(D, pdInit) << "Federate waiting end of pause..." << std::endl;
        while (paused) {
            try {
                tick();
            }
            catch (RTI::Exception& e) {
                Debug(D, pdExcept) << "******** Exception ticking the RTI : " << &e << std::endl;
                throw;
            }
        }
        Debug(D, pdInit) << "End of pause" << std::endl;
    }

    Debug(D, pdInit) << "Federation is synchronized." << std::endl;
}

// ----------------------------------------------------------------------------
/** init ball with seed
 */
void Billard::init(int seed)
{
    local.init(seed);
}

// ----------------------------------------------------------------------------
/** init ball with values
    \param x Ball X value
    \param y Ball Y value
 */
void Billard::init(int x, int y)
{
    local.init(x, y);
}

// ----------------------------------------------------------------------------
/** create objects, regions, etc.
 */
void Billard::declare()
{
    local.ID = registerBallInstance(federateName.c_str());
    // test, quelle est la classe de l'objet cree
    cout << "the class of the new created object is" << rtiamb.getObjectClass(local.ID) << endl;
}

// ----------------------------------------------------------------------------
/** one simulation step advance)
 */
void Billard::step()
{
    granted = false;

    try {
        rtiamb.queryFederateTime(localTime);
    }
    catch (RTI::Exception& e) {
        Debug(D, pdExcept) << "**** Exception asking for federate local time : " << &e << std::endl;
    }

    try {
        RTIfedTime time_aux(localTime.getTime() + TIME_STEP.getTime());

        Debug(D, pdDebug) << "time_aux : " << time_aux.getTime()
                          << " - localtime : " << ((RTIfedTime&) localTime).getTime()
                          << " - timestep : " << ((RTIfedTime&) TIME_STEP).getTime() << std::endl;
        granted = false;
        rtiamb.timeAdvanceRequest(time_aux);
    }
    catch (RTI::Exception& e) {
        Debug(D, pdExcept) << "******* Exception sur timeAdvanceRequest." << std::endl;
    }

    for (vector<Ball>::iterator it = remote.begin(); it != remote.end(); ++it) {
        it->active = false;
    }

    while (!granted) {
        try {
            tick2();
        }
        catch (RTI::Exception& e) {
            Debug(D, pdExcept) << "******** Exception ticking the RTI : " << &e << std::endl;
            throw;
        }
    }

    RTIfedTime next_step(localTime + TIME_STEP);

    local.erase();

    vector<Ball>::iterator it;

    for (it = remote.begin(); it != remote.end(); ++it) {
        if (it->ID != 0 && it->active && local.checkBallCollision(&(*it))) {
#ifdef X_DISPLAY_MISSING
            std::cout << "Collision between Ball <" << local.ID << "> and <" << it->ID << ">" << std::endl;
#endif
            sendInteraction(local.dx, local.dy, next_step, it->ID);
            // On prend la vitesse de l'autre sauf dans le cas ou
            // on avait deja la meme. Dans ce cas, on inverse la notre.
            if ((local.dx == it->dx) && (local.dy == it->dy)) {
                local.dx = -local.dx;
                local.dy = -local.dy;
            }
            else
                local.setDirection(it->dx, it->dy);
        }
        it->display();
        Debug(D, pdTrace) << "no collision." << std::endl;
    }

    // Teste la collision avec le bord
    Debug(D, pdTrace) << "Border collisions..." << std::endl;
    local.checkBorderCollision(XMAX, YMAX);

    local.x += local.dx;
    local.y += local.dy;

    checkRegions();

    local.display();

    sendUpdate(local.x, local.y, (int) local.color, next_step, local.ID);

    Debug(D, pdTrace) << "End of step" << std::endl;
}

// ----------------------------------------------------------------------------
/** Check DDM regions
 */
void Billard::checkRegions()
{
    // Nothing, in this default demo
}

// ----------------------------------------------------------------------------
/** resign the federation
 */
void Billard::resign()
{
    try {
        rtiamb.deleteObjectInstance(local.ID, localTime, "DO");
        Debug(D, pdRegister) << "Local object deleted from federation." << std::endl;
    }
    catch (RTI::Exception& e) {
        Debug(D, pdExcept) << "**** Exception delete object : " << &e << std::endl;
    }

    Debug(D, pdTerm) << "Local objects deleted." << std::endl;

    setTimeRegulation(false, false);

    try {
        rtiamb.resignFederationExecution(RTI::DELETE_OBJECTS_AND_RELEASE_ATTRIBUTES);
        Debug(D, pdTerm) << "Just resigned from federation" << std::endl;
    }
    catch (RTI::Exception& e) {
        Debug(D, pdExcept) << "** Exception during resignFederationExecution by federate" << std::endl;
    }
    // Detruire la federation

    if (creator) {
        for (;;) {
            tick();
            try {
                Debug(D, pdTerm) << "Asking from federation destruction..." << std::endl;
                rtiamb.destroyFederationExecution(federationName.c_str());

                Debug(D, pdTerm) << "Federation destruction granted." << std::endl;
                break;
            }
            catch (RTI::FederatesCurrentlyJoined) {
                sleep(5);
            }
        }
    }
    Debug(D, pdTerm) << "Destroying RTIAmbassador and FedAmbassador." << std::endl;
    Debug(D, pdTerm) << "Federation terminated." << std::endl;
}

// ----------------------------------------------------------------------------
/** Carry out publications and subscriptions
 */
void Billard::publishAndSubscribe()
{
    // Get all class and attributes handles
    getHandles();

    // Add PositionX et PositionY to the attribute set
    unique_ptr<RTI::AttributeHandleSet> attributes(RTI::AttributeHandleSetFactory::create(3));
    attributes->add(AttrXID);
    attributes->add(AttrYID);

    // Subscribe to Bille objects.
    Debug(D, pdDebug) << "subscribe: class " << BilleClassID << ", attributes " << AttrXID << " and " << AttrYID
                      << "... " << endl;
    rtiamb.subscribeObjectClassAttributes(BilleClassID, *attributes, RTI::RTI_TRUE);
    Debug(D, pdDebug) << "done." << endl;

    // Publish Boule Objects.
    attributes->add(AttrColorID);
    rtiamb.publishObjectClass(BouleClassID, *attributes);

    // Publish and subscribe to Bing interactions
    rtiamb.subscribeInteractionClass(BingClassID, RTI::RTI_TRUE);
    rtiamb.publishInteractionClass(BingClassID);

    Debug(D, pdInit) << "Local Objects and Interactions published and subscribed." << std::endl;
}

// ----------------------------------------------------------------------------
/** get handles of objet/interaction classes
 */
void Billard::getHandles()
{
    Debug(D, pdDebug) << "Get handles..." << endl;
    BilleClassID = rtiamb.getObjectClassHandle(CLA_BILLE);
    BouleClassID = rtiamb.getObjectClassHandle(CLA_BOULE);
    Debug(D, pdInit) << "BilleClassID = " << BilleClassID << ", BouleClassID = " << BouleClassID << std::endl;

    // Attributs des classes d'Objets
    AttrXID = rtiamb.getAttributeHandle(ATT_POSITION_X, BilleClassID);
    AttrYID = rtiamb.getAttributeHandle(ATT_POSITION_Y, BilleClassID);
    AttrColorID = rtiamb.getAttributeHandle(ATT_COLOR, BouleClassID);
    Debug(D, pdInit) << "AttrXID = " << AttrXID << ", AttrYID = " << AttrYID << ", AttrColorID = " << AttrColorID
                     << std::endl;

    // Interactions
    BingClassID = rtiamb.getInteractionClassHandle(INT_BING);
    ParamBoulID = rtiamb.getParameterHandle(PAR_BOUL, BingClassID);
    ParamDXID = rtiamb.getParameterHandle(PAR_DX, BingClassID);
    ParamDYID = rtiamb.getParameterHandle(PAR_DY, BingClassID);
    Debug(D, pdInit) << "BingClassID = " << BingClassID << ", DX_ID = " << ParamDXID << ", DY_ID = " << ParamDYID
                     << std::endl;
}

// ----------------------------------------------------------------------------
/*! Envoie une interaction, dont les parametres DX et DY ont pour valeur les
  dx et dy de la bille Local, et dont l'etiquette temporelle vaut
  InteractionTime.
*/
void Billard::sendInteraction(double dx, double dy, const RTI::FedTime& InteractionTime, RTI::ObjectHandle id)
{
    libhla::MessageBuffer buffer;

    RTI::ParameterHandleValuePairSet* parameterSet = RTI::ParameterSetFactory::create(3);

    buffer.reset();
    buffer.write_int32(id);
    buffer.updateReservedBytes();
    parameterSet->add(ParamBoulID, static_cast<char*>(buffer(0)), buffer.size());

    Debug(D, pdDebug) << "SendInteraction" << std::endl;
    Debug(D, pdDebug) << "SendInteraction - ParamBoulID= " << ParamBoulID << std::endl;
    Debug(D, pdDebug) << "SendInteraction - x= " << id << std::endl;

    buffer.reset();
    buffer.write_double(dx);
    buffer.updateReservedBytes();
    parameterSet->add(ParamDXID, static_cast<char*>(buffer(0)), buffer.size());

    Debug(D, pdDebug) << "SendInteraction - ParamDXID= " << ParamDXID << ", x= " << dx << std::endl;

    buffer.reset();
    buffer.write_double(dy);
    buffer.updateReservedBytes();
    parameterSet->add(ParamDYID, static_cast<char*>(buffer(0)), buffer.size());

    Debug(D, pdDebug) << "SendInteraction - ParamDYID= " << ParamDYID << ", x= " << dy << std::endl;

    try {
        if (notimestamp) {
            Debug(D, pdRegister) << "Sending interaction without time (DX= " << dx << ", DY= " << dy << ")."
                                 << std::endl;
            rtiamb.sendInteraction(BingClassID, *parameterSet, "");
        }
        else {
            Debug(D, pdRegister) << "Sending interaction wit time (DX= " << dx << ", DY= " << dy << ")." << std::endl;
            rtiamb.sendInteraction(BingClassID, *parameterSet, InteractionTime, "");
        }
    }
    catch (RTI::Exception& e) {
        std::cout << "sendInteraction raise exception " << e._name << "(" << e._reason << ")" << std::endl;
        Debug(D, pdExcept) << "**** Exception sending interaction : " << &e << std::endl;
    }

    delete parameterSet;
}

// ----------------------------------------------------------------------------
/** Updates a ball by sending entity position and color.
    \param x X position
    \param y Y position
    \param color Color
    \param UpdateTime Event time
    \param id Object handle (ball)
 */
void Billard::sendUpdate(double x, double y, int color, const RTI::FedTime& UpdateTime, RTI::ObjectHandle id)
{
    libhla::MessageBuffer buffer;
    RTI::AttributeHandleValuePairSet* attributeSet;

    attributeSet = RTI::AttributeSetFactory::create(3);

    Debug(D, pdTrace) << "SendUpdate." << std::endl;

    buffer.reset();
    buffer.write_double(x);
    buffer.updateReservedBytes();
    attributeSet->add(AttrXID, static_cast<char*>(buffer(0)), buffer.size());
    Debug(D, pdDebug) << "SendUpdate - AttrXID= " << AttrXID << ", x= " << x << ", size= " << buffer.size()
                      << std::endl;

    buffer.reset();
    buffer.write_double(y);
    buffer.updateReservedBytes();
    attributeSet->add(AttrYID, static_cast<char*>(buffer(0)), buffer.size());
    Debug(D, pdDebug) << "SendUpdate - AttrYID= " << AttrYID << ", y= " << y << ", size= " << buffer.size()
                      << std::endl;

    buffer.reset();
    buffer.write_int32(color);
    buffer.updateReservedBytes();
    attributeSet->add(AttrColorID, static_cast<char*>(buffer(0)), buffer.size());

    Debug(D, pdDebug) << "SendUpdate - AttrColorID= " << AttrColorID << ", color= " << color
                      << ", size= " << buffer.size() << std::endl;

    try {
        if (notimestamp) {
            rtiamb.updateAttributeValues(id, *attributeSet, "coucou");
        }
        else {
            rtiamb.updateAttributeValues(id, *attributeSet, UpdateTime, "coucou");
        }
    }
    catch (RTI::Exception& e) {
        std::cout << "Exception " << e._name << " (" << e._reason << ")" << std::endl;
        Debug(D, pdExcept) << "**** Exception updating attribute values: " << &e << std::endl;
    }

    delete attributeSet;
}

// ----------------------------------------------------------------------------
/** Register a ball instance
    \param s Object name to provide to the RTI
    \return The created object handle
 */
RTI::ObjectHandle Billard::registerBallInstance(const char* s)
{
    return rtiamb.registerObjectInstance(BouleClassID, s);
}

// ============================================================================
// FEDERATE AMBASSADOR CALLBACKS
// ============================================================================

// ----------------------------------------------------------------------------
/** Callback : discover object instance
 */
void Billard::discoverObjectInstance(RTI::ObjectHandle theObject,
                                     RTI::ObjectClassHandle theObjectClass,
                                     const char* /*theObjectName*/) throw(RTI::CouldNotDiscover,
                                                                          RTI::ObjectClassNotKnown,
                                                                          RTI::FederateInternalError)
{
    if (theObjectClass != BilleClassID) {
        Debug(D, pdError) << "Object of Unknown Class discovered." << std::endl;
        std::string msg = stringize() << "Unknown objectClass < " << theObjectClass << ">";
        throw RTI::FederateInternalError(msg.c_str());
    }

    bool already = false;
    for (vector<Ball>::iterator i = remote.begin(); i != remote.end(); ++i) {
        if (i->ID == theObject) {
            cout << "Rediscovered object " << theObject << endl;
            already = true;
        }
    }
    if (!already) {
        cout << "Discovered object handle = " << theObject << ", name = " << rtiamb.getObjectInstanceName(theObject)
             << endl;
        remote.push_back(Ball(theObject));
    }
}

// ----------------------------------------------------------------------------
/** Callback announce synchronization point
 */
void Billard::announceSynchronizationPoint(const char* label, const char* /*tag*/) throw(RTI::FederateInternalError)
{
    if (strcmp(label, "Init") == 0) {
        paused = true;
        Debug(D, pdProtocol) << "announceSynchronizationPoint." << std::endl;
    }
#ifdef TEST_RFSP
    else if (strcmp(label, "Friend") == 0) {
        std::cout << "**** I am happy : I have a friend ****" << std::endl;
        paused = true;
        Debug(D, pdProtocol) << "announceSynchronizationPoint (friend)." << std::endl;
    }
#endif
    else {
        cout << "Unexpected synchronization label" << endl;
        exit(1);
    }
}

// ----------------------------------------------------------------------------
/** Callback : federation synchronized
 */
void Billard::federationSynchronized(const char* label) throw(RTI::FederateInternalError)
{
    if (strcmp(label, "Init") == 0) {
        paused = false;
        Debug(D, pdProtocol) << "CALLBACK : federationSynchronized with label " << label << std::endl;
    }
}

// ----------------------------------------------------------------------------
/** Callback : receive interaction
 */
void Billard::receiveInteraction(RTI::InteractionClassHandle theInteraction,
                                 const RTI::ParameterHandleValuePairSet& theParameters,
                                 const RTI::FedTime& /*theTime*/,
                                 const char* /*theTag*/,
                                 RTI::EventRetractionHandle /*theHandle*/) throw(RTI::InteractionClassNotKnown,
                                                                                 RTI::InteractionParameterNotKnown,
                                                                                 RTI::InvalidFederationTime,
                                                                                 RTI::FederateInternalError)
{
    libhla::MessageBuffer buffer;
    RTI::ULong valueLength;
    int dx1 = 0;
    int dy1 = 0;
    RTI::ObjectHandle h1 = 0;
    bool bille = false;

    Debug(D, pdTrace) << "Fed : receiveInteraction" << std::endl;
    if (theInteraction != BingClassID) {
        Debug(D, pdError) << "CALLBACK receiveInteraction : Unknown Interaction received" << std::endl;
        exit(-1);
    }

    Debug(D, pdDebug) << "receiveInteraction - nb attributs= " << theParameters.size() << std::endl;

    for (unsigned int j = 0; j < theParameters.size(); ++j) {
        RTI::ParameterHandle parmHandle = theParameters.getHandle(j);

        valueLength = theParameters.getValueLength(j);
        assert(valueLength > 0);
        buffer.resize(valueLength);
        buffer.reset();
        theParameters.getValue(j, static_cast<char*>(buffer(0)), valueLength);
        buffer.assumeSizeFromReservedBytes();

        if (parmHandle == ParamDXID) {
            dx1 = static_cast<int>(buffer.read_double());
        }
        else {
            if (parmHandle == ParamDYID) {
                dy1 = static_cast<int>(buffer.read_double());
            }

            else {
                if (parmHandle == ParamBoulID) {
                    h1 = buffer.read_int32();
                    ;
                    bille = true;
                }
                else {
                    Debug(D, pdError) << "Unrecognized parameter handle" << std::endl;
                }
            }
        }
    }
    if (bille) {
        if (h1 == local.ID) {
            local.dx = dx1;
            local.dy = dy1;
        }
    }
}

// ----------------------------------------------------------------------------
/** Callback : reflect attribute values with time
 */
void Billard::reflectAttributeValues(RTI::ObjectHandle theObject,
                                     const RTI::AttributeHandleValuePairSet& theAttributes,
                                     const RTI::FedTime& /*theTime*/,
                                     const char* /*theTag*/,
                                     RTI::EventRetractionHandle /*theHandle*/) throw(RTI::ObjectNotKnown,
                                                                                     RTI::AttributeNotKnown,
                                                                                     RTI::InvalidFederationTime,
                                                                                     RTI::FederateInternalError)
{
    Debug(D, pdTrace) << "reflectAttributeValues with time" << std::endl;

    libhla::MessageBuffer buffer;
    float x1 = 0;
    float y1 = 0;

    RTI::ULong valueLength;

    Debug(D, pdDebug) << "reflectAttributeValues - nb attributs= " << theAttributes.size() << std::endl;

    for (unsigned int j = 0; j < theAttributes.size(); j++) {
        RTI::AttributeHandle parmHandle = theAttributes.getHandle(j);
        valueLength = theAttributes.getValueLength(j);
        assert(valueLength > 0);
        buffer.resize(valueLength);
        buffer.reset();
        theAttributes.getValue(j, static_cast<char*>(buffer(0)), valueLength);
        buffer.assumeSizeFromReservedBytes();

        if (parmHandle == AttrXID) {
            x1 = buffer.read_double();
        }
        else if (parmHandle == AttrYID) {
            y1 = buffer.read_double();
        }
        else
            Debug(D, pdError) << "Fed: ERREUR: handle inconnu." << std::endl;
    }
    //cout << x1 << " - " << y1 << endl;
    vector<Ball>::iterator it = remote.begin();
    while (it != remote.end() && it->ID != theObject)
        ++it;

    if (it == remote.end()) {
        Debug(D, pdError) << "Fed: error, id not found: " << theObject << std::endl;
    }
    else {
        it->erase();
        /* We only want a single attribute per RAV*/
        if (x1 != 0) {
            float oldx = it->x;
            it->x = x1;
            it->dx = it->x - oldx;
        }
        if (y1 != 0) {
            float oldy = it->y;
            it->y = y1;
            it->dy = it->y - oldy;
        }
        it->active = true;
    }
}

// ----------------------------------------------------------------------------
/** Callback : reflect attribute values without time
 */
void Billard::reflectAttributeValues(RTI::ObjectHandle theObject,
                                     const RTI::AttributeHandleValuePairSet& theAttributes,
                                     const char* /*theTag*/) throw(RTI::ObjectNotKnown,
                                                                   RTI::AttributeNotKnown,
                                                                   RTI::FederateInternalError)
{
    Debug(D, pdTrace) << "reflectAttributeValues without time" << std::endl;

    float x1 = 0;
    float y1 = 0;
    libhla::MessageBuffer buffer;

    RTI::ULong valueLength;

    Debug(D, pdDebug) << "reflectAttributeValues - nb attributs= " << theAttributes.size() << std::endl;

    for (unsigned int j = 0; j < theAttributes.size(); j++) {
        RTI::AttributeHandle parmHandle = theAttributes.getHandle(j);
        valueLength = theAttributes.getValueLength(j);
        assert(valueLength > 0);
        buffer.resize(valueLength);
        buffer.reset();
        theAttributes.getValue(j, static_cast<char*>(buffer(0)), valueLength);
        buffer.assumeSizeFromReservedBytes();

        if (parmHandle == AttrXID) {
            x1 = buffer.read_double();
        }
        else if (parmHandle == AttrYID) {
            y1 = buffer.read_double();
        }
        else {
            Debug(D, pdError) << "Fed: ERREUR: handle inconnu." << std::endl;
        }
    }

    vector<Ball>::iterator it = remote.begin();
    while (it != remote.end() && it->ID != theObject)
        ++it;

    if (it == remote.end()) {
        Debug(D, pdError) << "Fed: error, id not found: " << theObject << std::endl;
    }
    else {
        it->erase();

        float oldx = it->x;
        it->x = x1;
        it->dx = it->x - oldx;

        float oldy = it->y;
        it->y = y1;
        it->dy = it->y - oldy;

        it->active = true;
    }
}
// ----------------------------------------------------------------------------
/** Callback : remove object instance
 */
void Billard::removeObjectInstance(RTI::ObjectHandle theObject,
                                   const RTI::FedTime&,
                                   const char*,
                                   RTI::EventRetractionHandle) throw(RTI::ObjectNotKnown,
                                                                     RTI::InvalidFederationTime,
                                                                     RTI::FederateInternalError)
{
    vector<Ball>::iterator it;

    for (it = remote.begin(); it != remote.end(); ++it) {
        if (it->ID == theObject) {
            it->erase();
            remote.erase(it);
            return;
        }
    }
}

// ----------------------------------------------------------------------------
/** Callback : time advance granted
 */
void Billard::timeAdvanceGrant(const RTI::FedTime& theTime) throw(RTI::InvalidFederationTime,
                                                                  RTI::TimeAdvanceWasNotInProgress,
                                                                  RTI::FederateInternalError)
{
    granted = true;
    localTime = theTime;
    Debug(D, pdTrace) << "Time advanced, local time is now " << localTime.getTime() << std::endl;
}
