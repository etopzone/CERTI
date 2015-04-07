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
#include "PrettyDebug.hh"
#include "MessageBuffer.hh"

#ifndef _MSC_VER
#include <unistd.h>
#endif

#ifdef _WIN32
#include <windows.h>
#define    sleep(a)              Sleep(a * 1000)
#define    usleep(a)             Sleep(a / 1000)
#endif

#include <memory>
#include <iostream>
#include <cstdlib>
#include <cassert>

using std::auto_ptr ;
using std::string ;
using std::endl ;
using std::cout ;
using std::vector ;

static PrettyDebug D("BILLARD", __FILE__);

// ----------------------------------------------------------------------------
/** Constructor
 */
Billard::Billard(std::string federate_name)
    : rtiamb(),
      local(0),
      federateName(federate_name),
      handle(0),
      creator(false),
      nbTicks(0),
      regulating(false),
      constrained(false),
      notimestamp(false),      // i.e. with time
      localTime(0.0),
      TIME_STEP(1.0),
      XMAX(500),
      YMAX(100)
{
}

// ----------------------------------------------------------------------------
/** Destructor
 */
Billard::~Billard()
    throw (RTI::FederateInternalError)
{
}

// ----------------------------------------------------------------------------
/** Get the federate handle
 */
RTI::FederateHandle
Billard::getHandle() const
{
    return handle ;
}

// ----------------------------------------------------------------------------
/** Join the federation
    \param federation_name Federation name
    \param fdd_name Federation designator (.fed file)
 */
void
Billard::join(std::string federation_name, std::string fdd_name)
{
    federationName = federation_name ;

    // create federation
    try {
        rtiamb.createFederationExecution(federation_name.c_str(),
                                         fdd_name.c_str());
        D.Out(pdInit, "Federation execution created.");
        creator = true ;
    }
    catch (RTI::FederationExecutionAlreadyExists& e) {
        printf("BILLARD Note : %s Reason is : %s. OK I can join it\n",e._name,e._reason);
        D.Out(pdInit, "Federation execution already created.");
    }
    catch (RTI::CouldNotOpenFED& e) {
        printf("BILLARD ERROR : %s Reason is : %s\n",e._name,e._reason);
        D.Out(pdExcept, "BILLARD : Could not use FED file.");
        delete &rtiamb ; // Says RTIA to stop.
        exit(0);
    }

    // join federation
    bool joined = false ;
    int nb = 5 ;

    while (!joined && nb > 0) {
        nb-- ;
        try {
            handle = rtiamb.joinFederationExecution(federateName.c_str(),
                                                    federation_name.c_str(),
                                                    this);
            joined = true ;
            break ;
        }
        catch (RTI::FederateAlreadyExecutionMember& e) {
            Debug(D, pdExcept) << "Federate " << federateName.c_str()
                        << "already exists." << endl ;

            throw ;
        }
        catch (RTI::FederationExecutionDoesNotExist& e) {
            D.Out(pdExcept, "Federate %s : FederationExecutionDoesNotExist.",
                  federateName.c_str());
            // sleep(1);
        }
        catch (RTI::Exception& e) {
            D.Out(pdExcept,
                  "Federate %s :Join Federation Execution failed : %d .",
                  federateName.c_str(), &e);
            throw ;
        }
    }
}

// ----------------------------------------------------------------------------
/** Creator put federation in pause.
 */
void
Billard::pause()
{
    if (creator) {
        D.Out(pdInit, "Pause requested");
        try {
            rtiamb.registerFederationSynchronizationPoint("Init", "Waiting all players.");
        }
        catch (RTI::Exception& e) {
            Debug(D, pdExcept) << "Federate " << federateName
                        << " : Register Synchronization Point failed : %d"
                        << endl ;
        }
    }
}

// ----------------------------------------------------------------------------
/** Creator put federation in pause for synchronization with a friend
 */
void
Billard::pause_friend()
{
    if (creator) {
        D.Out(pdInit, "Pause requested for friend");
        try {
            // For testing purpose
             RTI::FederateHandle numfed(0) ;
             RTI::FederateHandleSet *federateSet = RTI::FederateHandleSetFactory::create(1) ;
             cout << "Now we test Register Federation Synchronisation Point on some federates" << endl ;
             cout << "Please enter a federate handle (zero means none)" << endl ;
             cout << "This federate will be synchronized with the creator and not the others" << endl;
            /* FIXME we should use C++ I/O and not mix C I/O with  C++ I/O  
             while ( not (std::cin >> numfed) || numfed < 0) {
	         std::cout << "That's not a number; ";
		     std::cin.clear();
		     std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		     }
		     */
             scanf("%lu",&numfed);
             if (numfed != 0)
                 {
                 // We store numfed into the federate set
                 federateSet->add(numfed) ;
                 rtiamb.registerFederationSynchronizationPoint("Friend","Synchro with a friend",
                                                          *federateSet) ;
                 }
        }
        catch (RTI::Exception& e) {
            Debug(D, pdExcept) << "Federate " << federateName
                        << " : Register Synchronization Point failed : %d"
                        << endl ;
        }
    }
}
// ----------------------------------------------------------------------------
/** tick the RTI
 */
void
Billard::tick()
{
    usleep( 0 ) ;
    rtiamb.tick();
    nbTicks++ ;
}
void
Billard::tick2()
{
    rtiamb.tick2();
    nbTicks++ ;
}

// ----------------------------------------------------------------------------
/** Set time regulation (time regulating and time constrained)
    @param start_constrained boolean, if true federate is constrained
    @param start_regulating boolean, if true federate is regulating
 */
void
Billard::setTimeRegulation(bool start_constrained, bool start_regulating)
{
    D.Out(pdInit, "Time Regulation setup");

    if (start_constrained) {
        if (!constrained) {
            // change from no constrained to constrained
            rtiamb.enableTimeConstrained();
            constrained = true ;
            D.Out(pdInit, "Time Constrained enabled.");
        }
        //rtiamb.modifyLookahead(TIME_STEP);
    }
    else {
        if (constrained) {
            // change from constrained to no constrained
            rtiamb.disableTimeConstrained();
            constrained = false ;
            D.Out(pdInit, "Time Constrained disabled.");
        }
    }

    if (start_regulating) {
        if (!regulating) {
            // change from no regulating to regulating
            for (;;) {
                rtiamb.queryFederateTime(localTime);

                try {
                    rtiamb.enableTimeRegulation(localTime, TIME_STEP);
                    regulating = true ;
                    break ;
                }
                catch (RTI::FederationTimeAlreadyPassed) {
                    // Si Je ne suis pas le premier, je vais les rattraper.
                    rtiamb.queryFederateTime(localTime);

                    RTIfedTime requestTime(((RTIfedTime&)localTime).getTime());
                    requestTime += TIME_STEP ;

		    granted = false ;
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
            regulating = false ;
        }
    }
}

// ----------------------------------------------------------------------------
/** Synchronize with other federates
 */
void
Billard::synchronize(int autostart)
{
    D.Out(pdInit, "Synchronize");

    if (creator) {
        // Wait a signal from user and stop the pause synchronization.

        if (autostart == 0) {
            cout << "Press ENTER to start execution..." << endl ;
            getchar();
        }
        else {
            while (autostart >= 0) {
                sleep(1);
                printf("\rAutostart : %5d", autostart);
                fflush(stdout);
                autostart-- ;
            }
            printf("\n");
        }

        D.Out(pdInit, "Creator can resume execution...");
        while (!paused)
            try {
                D.Out(pdInit, "not paused");
                tick();
            }
            catch (RTI::Exception& e) {
                D.Out(pdExcept, "******** Exception ticking the RTI : %d ", &e);
                throw ;
            }
        D.Out(pdDebug, "paused");

        try {
            rtiamb.synchronizationPointAchieved("Init");
        }
        catch (RTI::Exception& e) {
            D.Out(pdExcept, "**** Exception achieving a synchronization "
                  "point by creator : %d", &e);
        }
       
        while (paused)
            try {
                tick();
            }
            catch (RTI::Exception& e) {
                D.Out(pdExcept, "**** Exception ticking the RTI : %d.", &e);
                throw ;
            }
    }
    else {
        if (autostart != 0) {
            printf("Ignoring autostart (federate is not creator)\n");
        }
        printf("Synchronization...\n");

        if (!paused) {
            D.Out(pdInit,
                  "Federate not paused: too early");
            while (!paused) {
                try {
                    tick();
                }
                catch (RTI::Exception& e) {
                    D.Out(pdExcept,
                          "******** Exception ticking the RTI : %d.", &e);
                    throw ;
                }
            }
        }
        D.Out(pdInit, "Federate paused");

        try {
            // Federate ends its synchronization.
            rtiamb.synchronizationPointAchieved("Init");
            D.Out(pdInit, "Pause achieved.");
        }
        catch (RTI::Exception& e) {
            D.Out(pdExcept,
                  "**** Exception achieving a synchronization point : %d",
                  &e);
        }

        D.Out(pdInit,
              "Federate waiting end of pause...");
        while (paused) {
            try {
                tick();
            }
            catch (RTI::Exception& e) {
                D.Out(pdExcept, "******** Exception ticking the RTI : %d.", &e);
                throw ;
            }
        }
        D.Out(pdInit, "End of pause");
    }

    D.Out(pdInit, "Federation is synchronized.");

}

// ----------------------------------------------------------------------------
/** init ball with seed
 */
void
Billard::init(int seed)
{
    local.init(seed);
}

// ----------------------------------------------------------------------------
/** init ball with values
    \param x Ball X value
    \param y Ball Y value
 */
void
Billard::init(int x, int y)
{
    local.init(x, y);
}

// ----------------------------------------------------------------------------
/** create objects, regions, etc.
 */
void
Billard::declare()
{
    local.ID = registerBallInstance(federateName.c_str());
   // test, quelle est la classe de l'objet cree
   cout << "the class of the new created object is" <<
rtiamb.getObjectClass (local.ID) << endl ;
}

// ----------------------------------------------------------------------------
/** one simulation step advance)
 */
void
Billard::step()
{
    granted = false ;

    try {
        rtiamb.queryFederateTime(localTime);
    }
    catch (RTI::Exception& e) {
        D.Out(pdExcept,
              "**** Exception asking for federate local time : ", &e);
    }

    try {
        RTIfedTime time_aux(localTime.getTime()+TIME_STEP.getTime());

        D.Out(pdDebug, "time_aux : %.2f - localtime : %.2f - "
              "timestep : %.2f", time_aux.getTime(),
              ((RTIfedTime&)localTime).getTime(),
              ((RTIfedTime&)TIME_STEP).getTime());
        granted = false ;
        rtiamb.timeAdvanceRequest(time_aux);
    }
    catch (RTI::Exception& e) {
        D.Out(pdExcept, "******* Exception sur timeAdvanceRequest.");
    }

    for (vector<Ball>::iterator it = remote.begin(); it != remote.end(); ++it) {
        it->active = false ;
    }

    while (!granted) {
        try {
            tick2();
        }
        catch (RTI::Exception& e) {
            D.Out(pdExcept, "******** Exception ticking the RTI : %d.", &e);
            throw ;
        }
    }

    RTIfedTime next_step(localTime + TIME_STEP);

    local.erase();

    vector<Ball>::iterator it ;

    for (it = remote.begin(); it != remote.end(); ++it) {
        if (it->ID != 0 && it->active && local.checkBallCollision(&(*it))) {
#ifdef X_DISPLAY_MISSING
            std::cout << "Collision between Ball <" << local.ID << "> and <"<<it->ID<<">" << std::endl;
#endif
            sendInteraction(local.dx, local.dy, next_step, it->ID);
            // On prend la vitesse de l'autre sauf dans le cas ou
            // on avait deja la meme. Dans ce cas, on inverse la notre.
            if ((local.dx == it->dx) && (local.dy == it->dy)) {
                local.dx = -local.dx ;
                local.dy = -local.dy ;
            }
            else
                local.setDirection(it->dx, it->dy);
        }
        it->display();
        D.Out(pdTrace, "no collision.");
    }

    // Teste la collision avec le bord
    D.Out(pdTrace, "Border collisions...");
    local.checkBorderCollision(XMAX, YMAX);

    local.x += local.dx ;
    local.y += local.dy ;

    checkRegions();

    local.display();

    sendUpdate(local.x, local.y, (int) local.color, next_step, local.ID);

    D.Out(pdTrace, "End of step");
}

// ----------------------------------------------------------------------------
/** Check DDM regions
 */
void
Billard::checkRegions()
{
    // Nothing, in this default demo
}

// ----------------------------------------------------------------------------
/** resign the federation
 */
void
Billard::resign()
{
    try {
        rtiamb.deleteObjectInstance(local.ID, localTime, "DO");
        D.Out(pdRegister, "Local object deleted from federation.");
    }
    catch (RTI::Exception &e) {
        D.Out(pdExcept, "**** Exception delete object : %d", &e);
    }

    D.Out(pdTerm, "Local objects deleted.");

    setTimeRegulation(false, false);

    try {
        rtiamb.resignFederationExecution(
            RTI::DELETE_OBJECTS_AND_RELEASE_ATTRIBUTES);
        D.Out(pdTerm, "Just resigned from federation");
    }
    catch (RTI::Exception &e) {
        D.Out(pdExcept,
              "** Exception during resignFederationExecution by federate");
    }
    // Detruire la federation

    if (creator) {
        for (;;) {
            tick();
            try {
                D.Out(pdTerm, "Asking from federation destruction...");
                rtiamb.destroyFederationExecution(federationName.c_str());

                D.Out(pdTerm, "Federation destruction granted.");
                break ;
            }
            catch (RTI::FederatesCurrentlyJoined) {
                sleep(5);
            }
        }
    }
    D.Out(pdTerm, "Destroying RTIAmbassador and FedAmbassador.");
    D.Out(pdTerm, "Federation terminated.");
}

// ----------------------------------------------------------------------------
/** Carry out publications and subscriptions
 */
void
Billard::publishAndSubscribe()
{
    // Get all class and attributes handles
    getHandles();

    // Add PositionX et PositionY to the attribute set
    auto_ptr<RTI::AttributeHandleSet> attributes(RTI::AttributeHandleSetFactory::create(3));
    attributes->add(AttrXID);
    attributes->add(AttrYID);

    // Subscribe to Bille objects.
    Debug(D, pdDebug) << "subscribe: class " << BilleClassID << ", attributes "
	       << AttrXID << " and " << AttrYID << "... " << endl ;
    rtiamb.subscribeObjectClassAttributes(BilleClassID, *attributes, RTI::RTI_TRUE);
    Debug(D, pdDebug) << "done." << endl ;

    // Publish Boule Objects.
    attributes->add(AttrColorID);
    rtiamb.publishObjectClass(BouleClassID, *attributes);

    // Publish and subscribe to Bing interactions
    rtiamb.subscribeInteractionClass(BingClassID, RTI::RTI_TRUE);
    rtiamb.publishInteractionClass(BingClassID);

    D.Out(pdInit, "Local Objects and Interactions published and subscribed.");
}

// ----------------------------------------------------------------------------
/** get handles of objet/interaction classes
 */
void
Billard::getHandles()
{
    Debug(D, pdDebug) << "Get handles..." << endl ;
    BilleClassID = rtiamb.getObjectClassHandle(CLA_BILLE);
    BouleClassID = rtiamb.getObjectClassHandle(CLA_BOULE);
    D.Out(pdInit, "BilleClassID = %d, BouleClassID = %d.",
          BilleClassID, BouleClassID);

    // Attributs des classes d'Objets
    AttrXID = rtiamb.getAttributeHandle(ATT_POSITION_X, BilleClassID);
    AttrYID = rtiamb.getAttributeHandle(ATT_POSITION_Y, BilleClassID);
    AttrColorID = rtiamb.getAttributeHandle(ATT_COLOR, BouleClassID);
    D.Out(pdInit, "AttrXID = %d, AttrYID = %d, AttrColorID = %d.",
          AttrXID, AttrYID, AttrColorID);

    // Interactions
    BingClassID = rtiamb.getInteractionClassHandle(INT_BING);
    ParamBoulID = rtiamb.getParameterHandle(PAR_BOUL, BingClassID);
    ParamDXID = rtiamb.getParameterHandle(PAR_DX, BingClassID);
    ParamDYID = rtiamb.getParameterHandle(PAR_DY, BingClassID);
    D.Out(pdInit, "BingClassID = %d, DX_ID = %d, DY_ID = %d",
          BingClassID, ParamDXID, ParamDYID);
}

// ----------------------------------------------------------------------------
/*! Envoie une interaction, dont les parametres DX et DY ont pour valeur les
  dx et dy de la bille Local, et dont l'etiquette temporelle vaut
  InteractionTime.
*/
void
Billard::sendInteraction(double dx, double dy, const RTI::FedTime& InteractionTime,
			 RTI::ObjectHandle id)
{
    libhla::MessageBuffer buffer;
    RTI::ParameterHandleValuePairSet *parameterSet=NULL ;

    parameterSet = RTI::ParameterSetFactory::create(3);

    buffer.reset();
    buffer.write_int32(id);
    buffer.updateReservedBytes();              
    parameterSet->add(ParamBoulID, static_cast<char*>(buffer(0)), buffer.size());

    D.Out(pdDebug, "SendInteraction");
    D.Out(pdDebug, "SendInteraction - ParamBoulID= %u", ParamBoulID);
    D.Out(pdDebug, "SendInteraction - x= %d", id);    

    buffer.reset();
	buffer.write_double(dx);
	buffer.updateReservedBytes();
    parameterSet->add(ParamDXID, static_cast<char*>(buffer(0)), buffer.size());

    D.Out(pdDebug, "SendInteraction - ParamDXID= %u, x= %f",
          ParamDXID, dx);
    
    buffer.reset();
    buffer.write_double(dy);
    buffer.updateReservedBytes();
    parameterSet->add(ParamDYID, static_cast<char*>(buffer(0)), buffer.size());

    D.Out(pdDebug, "SendInteraction - ParamDYID= %u, x= %f",
          ParamDYID, dy);

    try {
        if ( notimestamp )
            {
            D.Out(pdRegister, "Sending interaction without time (DX= %f, DY= %f).", dx, dy);
            rtiamb.sendInteraction(BingClassID, *parameterSet, "");
            }
        else
            {
            D.Out(pdRegister, "Sending interaction wit time (DX= %f, DY= %f).", dx, dy);
            rtiamb.sendInteraction(BingClassID, *parameterSet, InteractionTime, "");
            }
    }
    catch (RTI::Exception& e) {
        std::cout<<"sendInteraction raise exception "<<e._name<<"("<<e._reason<<")"<<std::endl;
        D.Out(pdExcept, "**** Exception sending interaction : %d", &e);
    }

    delete parameterSet ;
}

// ----------------------------------------------------------------------------
/** Updates a ball by sending entity position and color.
    \param x X position
    \param y Y position
    \param color Color
    \param UpdateTime Event time
    \param id Object handle (ball)
 */
void
Billard::sendUpdate(double x, double y, int color, const RTI::FedTime& UpdateTime,
		    RTI::ObjectHandle id)
{    
    libhla::MessageBuffer buffer;
    RTI::AttributeHandleValuePairSet *attributeSet ;

    attributeSet = RTI::AttributeSetFactory::create(3);

    D.Out(pdTrace, "SendUpdate.");
   
    buffer.reset();
    buffer.write_double(x);
    buffer.updateReservedBytes();
    attributeSet->add(AttrXID, static_cast<char*>(buffer(0)),buffer.size());    
    D.Out(pdDebug, "SendUpdate - AttrXID= %u, x= %f, size= %u, attribute size=%d",
          AttrXID, x, attributeSet->size(),buffer.size());
    
    buffer.reset();
    buffer.write_double(y);	
    buffer.updateReservedBytes();
    attributeSet->add(AttrYID, static_cast<char*>(buffer(0)),buffer.size());
    D.Out(pdDebug, "SendUpdate - AttrYID= %u, y= %f, size= %u",
          AttrYID, y, buffer.size());

    buffer.reset();
    buffer.write_int32(color);	
    buffer.updateReservedBytes();
    attributeSet->add(AttrColorID, static_cast<char*>(buffer(0)),buffer.size());
   
    D.Out(pdDebug, "SendUpdate - AttrColorID= %u, color= %f, size= %u",
          AttrColorID, color, buffer.size());

    try {
        if ( notimestamp )
            {
            rtiamb.updateAttributeValues(id, *attributeSet, "coucou");
            }
        else
            {
            rtiamb.updateAttributeValues(id, *attributeSet, UpdateTime, "coucou");
            }
    }
    catch (RTI::Exception& e) {
        std::cout<<"Exception "<<e._name<<" ("<<e._reason<<")"<<std::endl;
        D.Out(pdExcept, "**** Exception updating attribute values: %d", &e);
    }

    delete attributeSet ;
}

// ----------------------------------------------------------------------------
/** Register a ball instance
    \param s Object name to provide to the RTI
    \return The created object handle
 */
RTI::ObjectHandle
Billard::registerBallInstance(const char *s)
{
    return rtiamb.registerObjectInstance(BouleClassID, s);
}

// ============================================================================
// FEDERATE AMBASSADOR CALLBACKS
// ============================================================================

// ----------------------------------------------------------------------------
/** Callback : discover object instance
 */
void
Billard::discoverObjectInstance(RTI::ObjectHandle theObject,
				RTI::ObjectClassHandle theObjectClass,
				const char */*theObjectName*/)
    throw (RTI::CouldNotDiscover, RTI::ObjectClassNotKnown, 
	   RTI::FederateInternalError)
{
    if (theObjectClass != BilleClassID) {
        D.Out(pdError, "Object of Unknown Class discovered.");
        std::string msg = stringize() << "Unknown objectClass < "<<theObjectClass <<">";
        throw RTI::FederateInternalError(msg.c_str());
    }
    
    bool already = false ;
    for (vector<Ball>::iterator i = remote.begin(); i != remote.end(); ++i) {
	if (i->ID == theObject) {
	    cout << "Rediscovered object " << theObject << endl ;
	    already = true ;
	}
    }
    if (!already) {

	cout << "Discovered object handle = " << theObject <<", name = "<< rtiamb.getObjectInstanceName(theObject) <<endl ;
	remote.push_back(Ball(theObject));
    }
}

// ----------------------------------------------------------------------------
/** Callback announce synchronization point
 */
void
Billard::announceSynchronizationPoint(const char *label, const char */*tag*/)
    throw (RTI::FederateInternalError)
{
    if (strcmp(label, "Init") == 0) {
        paused = true ;
        D.Out(pdProtocol, "announceSynchronizationPoint.");
    }
#ifdef TEST_RFSP
    else if (strcmp(label, "Friend") == 0) {
        std::cout<<"**** I am happy : I have a friend ****"<<std::endl;
        paused = true ;
        D.Out(pdProtocol, "announceSynchronizationPoint (friend).");
    } 
#endif       
    else {
        cout << "Unexpected synchronization label" << endl ;
        exit(1);
    }
}

// ----------------------------------------------------------------------------
/** Callback : federation synchronized
 */
void
Billard::federationSynchronized(const char *label)
    throw (RTI::FederateInternalError)
{
    if (strcmp(label, "Init") == 0) {
        paused = false ;
        D.Out(pdProtocol,
              "CALLBACK : federationSynchronized with label %s", label);
    }
}

// ----------------------------------------------------------------------------
/** Callback : receive interaction
 */
void
Billard::receiveInteraction(RTI::InteractionClassHandle theInteraction,
                        const RTI::ParameterHandleValuePairSet& theParameters,
                        const RTI::FedTime& /*theTime*/,
                        const char */*theTag*/,
                        RTI::EventRetractionHandle /*theHandle*/)
    throw (RTI::InteractionClassNotKnown,
           RTI::InteractionParameterNotKnown,
           RTI::InvalidFederationTime,
           RTI::FederateInternalError)
{
	libhla::MessageBuffer buffer;
    RTI::ULong valueLength ;
    int dx1 = 0 ;
    int dy1 = 0 ;
    RTI::ObjectHandle h1 = 0 ;
    bool bille = false ;

    D.Out(pdTrace, "Fed : receiveInteraction");
    if (theInteraction != BingClassID) {
        D.Out(pdError,
              "CALLBACK receiveInteraction : Unknown Interaction received");
        exit(-1);
    }

    D.Out(pdDebug, "receiveInteraction - nb attributs= %d",
          theParameters.size());

    for (unsigned int j = 0 ; j < theParameters.size(); ++j) {
        RTI::ParameterHandle parmHandle = theParameters.getHandle(j);

        valueLength = theParameters.getValueLength(j);
        assert(valueLength>0);
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
                    h1 = buffer.read_int32(); ;
                    bille = true ;
                }
                else {
                     D.Out(pdError, "Unrecognized parameter handle");
                }
            }
        }
    }
    if (bille) {
        if (h1 == local.ID) {
            local.dx = dx1 ;
            local.dy = dy1 ;
        }
    }
}

// ----------------------------------------------------------------------------
/** Callback : reflect attribute values with time
 */
void
Billard::reflectAttributeValues(
    RTI::ObjectHandle theObject,
    const RTI::AttributeHandleValuePairSet& theAttributes,
    const RTI::FedTime& /*theTime*/,
    const char */*theTag*/,
    RTI::EventRetractionHandle /*theHandle*/)
    throw (RTI::ObjectNotKnown,
           RTI::AttributeNotKnown,
           RTI::InvalidFederationTime,
           RTI::FederateInternalError)
{
    D.Out(pdTrace, "reflectAttributeValues with time");

    libhla::MessageBuffer buffer;
    float x1 = 0 ;
    float y1 = 0 ;

    RTI::ULong valueLength ;

    D.Out(pdDebug, "reflectAttributeValues - nb attributs= %d",
          theAttributes.size());

    for (unsigned int j=0 ; j<theAttributes.size(); j++) {

        RTI::AttributeHandle parmHandle = theAttributes.getHandle(j);
        valueLength = theAttributes.getValueLength(j);
        assert(valueLength>0);
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
            D.Out(pdError, "Fed: ERREUR: handle inconnu.");
    }
    //cout << x1 << " - " << y1 << endl;
    vector<Ball>::iterator it = remote.begin() ;
    while (it != remote.end() && it->ID != theObject)
      ++it ;

    if (it == remote.end())
        D.Out(pdError, "Fed: error, id not found (%d).", theObject);
    else {
        it->erase();
        /* We only want a single attribute per RAV*/
        if (x1 !=0) {
          float oldx = it->x ;
          it->x = x1 ;
          it->dx = it->x - oldx;
        }
        if (y1 !=0) {
          float oldy = it->y ;
          it->y = y1 ;
          it->dy = it->y - oldy;
        }
        it->active = true ;
    }
}

// ----------------------------------------------------------------------------
/** Callback : reflect attribute values without time
 */
void
Billard::reflectAttributeValues(
    RTI::ObjectHandle theObject,
    const RTI::AttributeHandleValuePairSet& theAttributes,
    const char */*theTag*/)
    throw (RTI::ObjectNotKnown,
           RTI::AttributeNotKnown,
           RTI::FederateInternalError)
{
    D.Out(pdTrace, "reflectAttributeValues without time");

    float x1 = 0 ;
    float y1 = 0 ;
    libhla::MessageBuffer buffer;

    RTI::ULong valueLength ;   

    D.Out(pdDebug, "reflectAttributeValues - nb attributs= %d",
          theAttributes.size());

    for (unsigned int j=0 ; j<theAttributes.size(); j++) {

        RTI::AttributeHandle parmHandle = theAttributes.getHandle(j);
        valueLength = theAttributes.getValueLength(j);
        assert(valueLength>0);
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
            D.Out(pdError, "Fed: ERREUR: handle inconnu.");
        }
    }
    
    vector<Ball>::iterator it = remote.begin() ;
    while (it != remote.end() && it->ID != theObject)
	++it ;

    if (it == remote.end())
        D.Out(pdError, "Fed: error, id not found (%d).", theObject);
    else {
        it->erase();

        float oldx = it->x ;
        it->x = x1 ;
        it->dx = it->x - oldx ;

        float oldy = it->y ;
        it->y = y1 ;
        it->dy = it->y - oldy ;

	it->active = true ;
    }
}
// ----------------------------------------------------------------------------
/** Callback : remove object instance
 */
void
Billard::removeObjectInstance(RTI::ObjectHandle theObject,
			      const RTI::FedTime &,
			      const char *,
			      RTI::EventRetractionHandle)
    throw (RTI::ObjectNotKnown, RTI::InvalidFederationTime, RTI::FederateInternalError)
{
    vector<Ball>::iterator it ;

    for (it = remote.begin(); it != remote.end(); ++it) {
        if (it->ID == theObject) {
            it->erase();
            remote.erase(it);
            return ;
        }
    }
}

// ----------------------------------------------------------------------------
/** Callback : time advance granted
 */
void
Billard::timeAdvanceGrant(const RTI::FedTime& theTime)
    throw (RTI::InvalidFederationTime, RTI::TimeAdvanceWasNotInProgress, 
	   RTI::FederateInternalError)
{    
    granted = true ;
    localTime = theTime ;
    D.Out(pdTrace, "Time advanced, local time is now %.2f.",
          localTime.getTime());
}

