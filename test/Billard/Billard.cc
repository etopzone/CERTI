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
#include "Certi_Win.h"

#include "Billard.hh"
#include "PrettyDebug.hh"

#ifndef _WIN32
#include <unistd.h>
#endif

using std::auto_ptr ;
using std::string ;
using std::endl ;
using std::cout ;
using std::vector ;

static pdCDebug D("BILLARD", __FILE__);

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
        D.Out(pdInit, "Federation execution already created.");
    }
    catch (RTI::CouldNotOpenFED& e) {
        D.Out(pdExcept, "BILLARD : Could not use FED file.");
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
            D[pdExcept] << "Federate " << federateName.c_str()
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
            rtiamb.registerFederationSynchronizationPoint(
                "Init", "Waiting all players.");
        }
        catch (RTI::Exception& e) {
            D[pdExcept] << "Federate " << federateName.c_str()
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
        rtiamb.modifyLookahead(TIME_STEP);
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
}

// ----------------------------------------------------------------------------
/** one simulation step advance)
 */
void
Billard::step()
{
    granted = false ;
    rtiamb.queryFederateTime(localTime);

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
    try {
        rtiamb.queryFederateTime(localTime);
    }
    catch (RTI::Exception& e) {
        D.Out(pdExcept,
              "**** Exception asking for federate local time : ", &e);
    }

    D.Out(pdTrace, "Time advanced, local time is now %.2f.",
          localTime.getTime());

    RTIfedTime next_step(localTime + TIME_STEP);

    local.erase();

    vector<Ball>::iterator it ;

    for (it = remote.begin(); it != remote.end(); ++it) {
        if (it->ID != 0 && it->active && local.checkBallCollision(&(*it))) {
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
    D[pdDebug] << "subscribe: class " << BilleClassID << ", attributes "
	       << AttrXID << " and " << AttrYID << "... " << endl ;
    rtiamb.subscribeObjectClassAttributes(BilleClassID, *attributes, RTI::RTI_TRUE);
    D[pdDebug] << "done." << endl ;

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
    D[pdDebug] << "Get handles..." << endl ;
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
    char buf[512] ;
    RTI::ParameterHandleValuePairSet *parameterSet=NULL ;

    parameterSet = RTI::ParameterSetFactory::create(3);

    // OLD : sprintf(buf, "%ld", id);
    // OLD : parameterSet->add(ParamBoulID, buf, strlen(buf)+1);
    memcpy(buf,&id,sizeof(id));
    parameterSet->add(ParamBoulID, buf, sizeof(id));

    D.Out(pdDebug, "SendInteraction");
    D.Out(pdDebug, "SendInteraction - ParamBoulID= %u", ParamBoulID);
    D.Out(pdDebug, "SendInteraction - x= %d", id);
    D.Out(pdDebug, "SendInteraction - buf= %s", buf);

    // D.Out(pdDebug, "SendInteraction - ParamBoulID= %u, x= %f, buf= %s",
    // ParamBoulID, Id, buf);

    // OLD : sprintf(buf, "%f", dx);
    // OLD : parameterSet->add(ParamDXID, buf, strlen(buf)+1);
    memcpy(buf,&dx,sizeof(dx)); 
    parameterSet->add(ParamDXID, buf, sizeof(dx));

    D.Out(pdDebug, "SendInteraction - ParamDXID= %u, x= %f, buf= %s",
          ParamDXID, dx, buf);

    // OLD : sprintf(buf, "%f", dy);
    // OLD : parameterSet->add(ParamDYID, buf, strlen(buf)+1);
    memcpy(buf,&dy,sizeof(dy));
    parameterSet->add(ParamDYID, buf, sizeof(dy));

    D.Out(pdDebug, "SendInteraction - ParamDYID= %u, x= %f, buf= %s",
          ParamDYID, dy, buf);

    D.Out(pdRegister, "Sending interaction(DX= %f, DY= %f).", dx, dy);

    try {
        rtiamb.sendInteraction(BingClassID, *parameterSet, InteractionTime, "");
    }
    catch (RTI::Exception& e) {
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
    char buf[512] ;
    RTI::AttributeHandleValuePairSet *attributeSet ;

    attributeSet = RTI::AttributeSetFactory::create(3);

    D.Out(pdTrace, "SendUpdate.");

    // OLD : sprintf(buf, "%f", x);
    // OLD : attributeSet->add(AttrXID, buf, strlen(buf)+1);
    memcpy(buf,&x,sizeof(x));
    attributeSet->add(AttrXID, buf,sizeof(x));    
    D.Out(pdDebug, "SendUpdate - AttrXID= %u, x= %f, size= %u, attribute size=%d",
          AttrXID, x, attributeSet->size(),sizeof(x));

    // OLD : sprintf(buf, "%f", y);
    // OLD : attributeSet->add(AttrYID, buf, strlen(buf)+1);
    memcpy(buf,&y,sizeof(y));
    attributeSet->add(AttrYID, buf,sizeof(y));
    D.Out(pdDebug, "SendUpdate - AttrYID= %u, y= %f, size= %u",
          AttrYID, y, attributeSet->size());

    // OLD : sprintf(buf, "%d", color);
    // OLD : attributeSet->add(AttrColorID, buf, strlen(buf)+1);
    memcpy(buf,&color,sizeof(color));
    attributeSet->add(AttrColorID, buf,sizeof(color));
   
    D.Out(pdDebug, "SendUpdate - AttrColorID= %u, color= %f, size= %u",
          AttrColorID, color, attributeSet->size());

    try {
        if ( notimestamp )
            {
            rtiamb.updateAttributeValues(id, *attributeSet, "");
            }
        else
            {
            rtiamb.updateAttributeValues(id, *attributeSet, UpdateTime, "");
            }
    }
    catch (RTI::Exception& e) {
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
        throw RTI::FederateInternalError("");
    }
    
    bool already = false ;
    for (vector<Ball>::iterator i = remote.begin(); i != remote.end(); ++i) {
	if (i->ID == theObject) {
	    cout << "Rediscovered object " << theObject << endl ;
	    already = true ;
	}
    }
    if (!already) {
	cout << "Discovered object " << theObject << endl ;
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
    char *parmValue ;
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
        RTI::ParameterHandle handle = theParameters.getHandle(j);

        valueLength = theParameters.getValueLength(j);
        parmValue = new char[valueLength] ;
        theParameters.getValue(j, parmValue, valueLength);

        if (handle == ParamDXID) {
            if (parmValue != NULL) {
                // OLD : dx1 = atoi(parmValue);
                double d_dx1 ;
                memcpy(&d_dx1,parmValue,valueLength) ;
                dx1 = d_dx1 ;
                // Local.dx = atof(parmValue);
                D.Out(pdDebug, "receiveInteraction(*) - dx= %s", parmValue);
                delete[] parmValue ;
            }
            else
                D.Out(pdError, "Missing Attribute in RAV.");
        }
        else
            if (handle == ParamDYID) {
                if (parmValue != NULL) {
                    // OLD : dy1 = atoi(parmValue);
                    double d_dy1 ;
                    memcpy(&d_dy1,parmValue,valueLength) ;
                    dy1 = d_dy1 ;
                    // Local.dy = atof(parmValue);
                    D.Out(pdDebug, "receiveInteraction(*) - dy= %s", parmValue);
                    delete[] parmValue ;
                }
                else
                    D.Out(pdError, "Missing Attribute in RAV.");
            }
            else
                if (handle == ParamBoulID) {
                    if (parmValue != NULL) {
                        // OLD : h1 = atoi(parmValue);
                        RTI::ObjectHandle d_h1 ;
                        memcpy(&d_h1,parmValue,valueLength) ;
                        h1 = d_h1 ;
                        bille = true ;
                    }
                    else
                        D.Out(pdError, "Unrecognized parameter handle");
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

    float x1 = 0 ;
    float y1 = 0 ;

    RTI::ULong valueLength ;
    char *attrValue ;

    D.Out(pdDebug, "reflectAttributeValues - nb attributs= %d",
          theAttributes.size());

    for (unsigned int j=0 ; j<theAttributes.size(); j++) {

        RTI::AttributeHandle handle = theAttributes.getHandle(j);
        valueLength = theAttributes.getValueLength(j);
        attrValue = new char[valueLength] ;
        theAttributes.getValue(j, attrValue, valueLength);

        if (handle == AttrXID) {
            if (attrValue != NULL) {
                // OLD : x1 = atof(attrValue);
                double d_x1 ;
                memcpy(&d_x1,attrValue,valueLength) ;
                x1 = d_x1 ;
                delete[] attrValue ;
            }
            else
                D.Out(pdError, "Fed: ERREUR: missing Attribute.");
        }
        else if (handle == AttrYID) {
            if (attrValue != NULL) {
                // OLD : y1 = atof(attrValue);
                double d_y1 ;
                memcpy(&d_y1,attrValue,valueLength) ;
                y1 = d_y1 ;
                delete[] attrValue ;
            }
            else
                D.Out(pdError, "Fed: ERREUR: missing Attribute.");
        }
        else
            D.Out(pdError, "Fed: ERREUR: handle inconnu.");
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

    RTI::ULong valueLength ;
    char *attrValue ;

    D.Out(pdDebug, "reflectAttributeValues - nb attributs= %d",
          theAttributes.size());

    for (unsigned int j=0 ; j<theAttributes.size(); j++) {

        RTI::AttributeHandle handle = theAttributes.getHandle(j);
        valueLength = theAttributes.getValueLength(j);
        attrValue = new char[valueLength] ;
        theAttributes.getValue(j, attrValue, valueLength);

        if (handle == AttrXID) {
            if (attrValue != NULL) {
                // OLD : x1 = atof(attrValue);
                double d_x1 ;
                memcpy(&d_x1,attrValue,valueLength) ;
                x1 = d_x1 ;
                delete[] attrValue ;
            }
            else
                D.Out(pdError, "Fed: ERREUR: missing Attribute.");
        }
        else if (handle == AttrYID) {
            if (attrValue != NULL) {
                // OLD : y1 = atof(attrValue);
                double d_y1 ;
                memcpy(&d_y1,attrValue,valueLength) ;
                y1 = d_y1 ;
                delete[] attrValue ;
            }
            else
                D.Out(pdError, "Fed: ERREUR: missing Attribute.");
        }
        else
            D.Out(pdError, "Fed: ERREUR: handle inconnu.");
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
Billard::timeAdvanceGrant(const RTI::FedTime& /*theTime*/)
    throw (RTI::InvalidFederationTime, RTI::TimeAdvanceWasNotInProgress, 
	   RTI::FederateInternalError)
{    
    granted = true ;
}
