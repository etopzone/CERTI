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
// $Id: Billard.cc,v 3.4 2003/10/27 10:51:38 breholee Exp $
// ----------------------------------------------------------------------------

#include "Billard.hh"

#include <config.h>
#include "Objects.hh"
#include "PrettyDebug.hh"

#include <unistd.h>

using std::string ;
using std::endl ;
using std::cout ;

static pdCDebug D("BILLARD", __FILE__);

// ----------------------------------------------------------------------------
Billard::Billard(string federate_name)
    : rtiamb(), objects(rtiamb, *this, 500, 100),
      federateName(federate_name),
      handle(0), creator(false), nbTicks(0),
      regulating(false), constrained(false), localTime(0.0), TIME_STEP(1.0),
      XMAX(500), YMAX(100)
{
    //    fedamb.init(&objects);
}

// ----------------------------------------------------------------------------
/** Get the federate handle
 */
FederateHandle
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
Billard::join(string federation_name, string fdd_name)
{
    federationName = federation_name ;

    // create federation
    try {
        rtiamb.createFederationExecution(federation_name.c_str(),
                                         fdd_name.c_str());
        D.Out(pdInit, "Federation execution created.");
        creator = true ;
    }
    catch (FederationExecutionAlreadyExists& e) {
        D.Out(pdInit, "Federation execution already created.");
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
        catch (FederateAlreadyExecutionMember& e) {
            D[pdExcept] << "Federate " << federateName.c_str()
                        << "already exists." << endl ;

            throw ;
        }
        catch (FederationExecutionDoesNotExist& e) {
            D.Out(pdExcept, "Federate %s : FederationExecutionDoesNotExist.",
                  federateName.c_str());
            // sleep(1);
        }
        catch (Exception& e) {
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
        catch (Exception& e) {
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
    rtiamb.tick();
    nbTicks++ ;
}

// ----------------------------------------------------------------------------
/** Set time regulation (time regulating and time constrained)
 */
void
Billard::setTimeRegulation(bool start_constrained, bool start_regulating)
{
    D.Out(pdInit, "Time Regulation setup");

    if (start_constrained) {
        if (!constrained) {
            rtiamb.enableTimeConstrained();
            constrained = true ;
            D.Out(pdInit, "Time Constrained enabled.");
        }
        rtiamb.modifyLookahead(TIME_STEP);
    }
    else {
        if (constrained) {
            rtiamb.disableTimeConstrained();
            constrained = false ;
            D.Out(pdInit, "Time Constrained disabled.");
        }
    }

    if (start_regulating) {
        if (!regulating) {
            for (;;) {
                rtiamb.queryFederateTime(localTime);

                try {
                    rtiamb.enableTimeRegulation(localTime, TIME_STEP);
                    regulating = true ;
                    break ;
                }
                catch (FederationTimeAlreadyPassed) {
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
                        catch (RTIinternalError) {
                            printf("RTIinternalError Raised in tick.\n");
                            exit(-1);
                        }
                    }
                }
                catch (RTIinternalError) {
                    printf("RTIinternalError Raised in setTimeRegulating.\n");
                    exit(-1);
                }
            }
        }
    }
    else {
        if (regulating) {
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
            catch (Exception& e) {
                D.Out(pdExcept, "******** Exception ticking the RTI : %d ", &e);
            }
        D.Out(pdDebug, "paused");

        try {
            rtiamb.synchronizationPointAchieved("Init");
        }
        catch (Exception& e) {
            D.Out(pdExcept, "**** Exception achieving a synchronization "
                  "point by creator : %d", &e);
        }

        while (paused)
            try {
                tick();
            }
            catch (Exception& e) {
                D.Out(pdExcept, "**** Exception ticking the RTI : %d.", &e);
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
                catch (Exception& e) {
                    D.Out(pdExcept,
                          "******** Exception ticking the RTI : %d.", &e);
                }
            }
        }
        D.Out(pdInit, "Federate paused");

        try {
            // Federate ends its synchronization.
            rtiamb.synchronizationPointAchieved("Init");
            D.Out(pdInit, "Pause achieved.");
        }
        catch (Exception& e) {
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
            catch (Exception& e) {
                D.Out(pdExcept, "******** Exception ticking the RTI : %d.", &e);
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
    objects.init(seed);
}

// ----------------------------------------------------------------------------
/** init ball with values
    \param x Ball X value
    \param y Ball Y value
 */
void
Billard::init(int x, int y)
{
    objects.init(x, y);
}

// ----------------------------------------------------------------------------
/** create objects, regions, etc.
 */
void
Billard::declare()
{
    // only objects in this class
    objects.declare(federateName);
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
    catch (Exception& e) {
        D.Out(pdExcept, "******* Exception sur timeAdvanceRequest.");
    }

    while (!granted) {
        try {
            tick();
        }
        catch (Exception& e) {
            D.Out(pdExcept, "******** Exception ticking the RTI : %d.", &e);
        }
    }
    try {
        rtiamb.queryFederateTime(localTime);
    }
    catch (Exception& e) {
        D.Out(pdExcept,
              "**** Exception asking for federate local time : ", &e);
    }

    D.Out(pdTrace, "Time advanced, local time is now %.2f.",
          localTime.getTime());

    RTIfedTime next_step(localTime + TIME_STEP);

    objects.erase();
    objects.step(next_step);
    objects.display();
    objects.update(next_step);
}

// ----------------------------------------------------------------------------
/** resign the federation
 */
void
Billard::resign()
{
    objects.destroy(localTime);
    D.Out(pdTerm, "Local objects deleted.");

    setTimeRegulation(false, false);

    try {
        rtiamb.resignFederationExecution(
            DELETE_OBJECTS_AND_RELEASE_ATTRIBUTES);
        D.Out(pdTerm, "Just resigned from federation");
    }
    catch (Exception &e) {
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
            catch (FederatesCurrentlyJoined) {
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
    AttributeHandleSet *AttributeSet = AttributeHandleSetFactory::create(3);
    AttributeSet->add(AttrXID);
    AttributeSet->add(AttrYID);

    // Subscribe to Bille objects.
    D[pdDebug] << "subscribe: class " << BilleClassID << ", attributes "
	       << AttrXID << " and " << AttrYID << "... " ;
    rtiamb.subscribeObjectClassAttributes(BilleClassID, *AttributeSet, RTI_TRUE);
    D[pdDebug] << "done." << endl ;

    // Publish Boule Objects.
    AttributeSet->add(AttrColorID);
    rtiamb.publishObjectClass(BouleClassID, *AttributeSet);

    // Publish and subscribe to Bing interactions
    rtiamb.subscribeInteractionClass(BingClassID, RTI_TRUE);
    rtiamb.publishInteractionClass(BingClassID);

    AttributeSet->empty();

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
Billard::sendInteraction(double dx, double dy, const FedTime& InteractionTime,
                     ObjectHandle id)
{
    char buf[512] ;
    ParameterHandleValuePairSet *parameterSet=NULL ;

    parameterSet = ParameterSetFactory::create(3);

    sprintf(buf, "%ld", id);
    parameterSet->add(ParamBoulID, buf, strlen(buf)+1);

    D.Out(pdDebug, "SendInteraction");
    D.Out(pdDebug, "SendInteraction - ParamBoulID= %u", ParamBoulID);
    D.Out(pdDebug, "SendInteraction - x= %d", id);
    D.Out(pdDebug, "SendInteraction - buf= %s", buf);

    // D.Out(pdDebug, "SendInteraction - ParamBoulID= %u, x= %f, buf= %s",
    // ParamBoulID, Id, buf);

    sprintf(buf, "%f", dx);
    parameterSet->add(ParamDXID, buf, strlen(buf)+1);
    D.Out(pdDebug, "SendInteraction - ParamDXID= %u, x= %f, buf= %s",
          ParamDXID, dx, buf);

    sprintf(buf, "%f", dy);
    parameterSet->add(ParamDYID, buf, strlen(buf)+1);
    D.Out(pdDebug, "SendInteraction - ParamDYID= %u, x= %f, buf= %s",
          ParamDYID, dy, buf);

    D.Out(pdRegister, "Sending interaction(DX= %f, DY= %f).", dx, dy);

    try {
        rtiamb.sendInteraction(BingClassID, *parameterSet, InteractionTime, "");
    }
    catch (Exception& e) {
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
Billard::sendUpdate(double x, double y, int color, const FedTime& UpdateTime,
                ObjectHandle id)
{
    char buf[512] ;
    AttributeHandleValuePairSet *attributeSet ;

    attributeSet = AttributeSetFactory::create(3);

    D.Out(pdTrace, "SendUpdate.");

    sprintf(buf, "%f", x);
    attributeSet->add(AttrXID, buf, strlen(buf)+1);
    D.Out(pdDebug, "SendUpdate - AttrXID= %u, x= %f, size= %u",
          AttrXID, x, attributeSet->size());

    sprintf(buf, "%f", y);
    attributeSet->add(AttrYID, buf, strlen(buf)+1);
    D.Out(pdDebug, "SendUpdate - AttrYID= %u, y= %f, size= %u",
          AttrYID, y, attributeSet->size());

    sprintf(buf, "%d", color);
    attributeSet->add(AttrColorID, buf, strlen(buf)+1);
    D.Out(pdDebug, "SendUpdate - AttrColorID= %u, color= %f, size= %u",
          AttrColorID, color, attributeSet->size());

    try {
        rtiamb.updateAttributeValues(id, *attributeSet, UpdateTime, "");
        // if (log)
        // logfile << string(((RTIfedTime) UpdateTime).getTime()) << " : UAV "
        // << string(Local.x) << " " << string(Local.y) << endl ;
    }
    catch (Exception& e) {
        D.Out(pdExcept, "**** Exception updating attribute values: %d", &e);
    }

    delete attributeSet ;
}

// ----------------------------------------------------------------------------
/** Register a ball instance
    \param s Object name to provide to the RTI
    \return The created object handle
 */
ObjectHandle
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
Billard::discoverObjectInstance(ObjectHandle theObject,
                            ObjectClassHandle theObjectClass,
                            const char */*theObjectName*/)
    throw (CouldNotDiscover,
           ObjectClassNotKnown,
           InvalidFederationTime,
           FederateInternalError)
{
    if (theObjectClass != BilleClassID) {
        D.Out(pdError, "Object of Unknown Class discovered.");
        throw RTIinternalError();
    }
    objects.discover(theObject);
}

// ----------------------------------------------------------------------------
/** Callback announce synchronization point
 */
void
Billard::announceSynchronizationPoint(const char *label, const char *tag)
    throw (FederateInternalError)
{
    if (strcmp(label, "Init") == 0) {
        paused = RTI_TRUE ;
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
    throw (FederateInternalError)
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
Billard::receiveInteraction(InteractionClassHandle theInteraction,
                        const ParameterHandleValuePairSet& theParameters,
                        const FedTime& /*theTime*/,
                        const char */*theTag*/,
                        EventRetractionHandle /*theHandle*/)
    throw (InteractionClassNotKnown,
           InteractionParameterNotKnown,
           InvalidFederationTime,
           FederateInternalError)
{
    char *parmValue ;
    ULong valueLength ;
    int dx1 = 0 ;
    int dy1 = 0 ;
    ObjectHandle h1 = 0 ;
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
        ParameterHandle handle = theParameters.getHandle(j);

        valueLength = theParameters.getValueLength(j);
        parmValue = new char[valueLength] ;
        theParameters.getValue(j, parmValue, valueLength);

        if (handle == ParamDXID) {
            if (parmValue != NULL) {
                dx1 = atoi(parmValue);
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
                    dy1 = atoi(parmValue);
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
                        h1 = atoi(parmValue);
                        bille = true ;
                    }
                    else
                        D.Out(pdError, "Unrecognized parameter handle");
                }
    }
    if (bille) {
        objects.receive(h1, dx1, dy1);
    }
}

// ----------------------------------------------------------------------------
/** Callback : reflect attribute values
 */
void
Billard::reflectAttributeValues(ObjectHandle theObject,
                            const AttributeHandleValuePairSet& theAttributes,
                            const FedTime& theTime,
                            const char */*theTag*/,
                            EventRetractionHandle /*theHandle*/)
    throw (ObjectNotKnown,
           AttributeNotKnown,
           InvalidFederationTime,
           FederateInternalError)
{
    D.Out(pdTrace, "reflectAttributeValues");

    int i=0 ;
    float oldx, oldy ;

    float x1 = 0 ;
    float y1 = 0 ;

    ULong valueLength ;
    char *attrValue ;


    D.Out(pdDebug, "reflectAttributeValues - nb attributs= %d",
          theAttributes.size());


    for (unsigned int j=0 ; j<theAttributes.size(); j++) {

        AttributeHandle handle = theAttributes.getHandle(j);
        valueLength = theAttributes.getValueLength(j);
        attrValue = new char[valueLength] ;
        theAttributes.getValue(j, attrValue, valueLength);

        if (handle == AttrXID) {
            if (attrValue != NULL) {
                x1 = atof(attrValue);
                delete[] attrValue ;
            }
            else
                D.Out(pdError, "Fed: ERREUR: missing Attribute.");
        }
        else if (handle == AttrYID) {
            if (attrValue != NULL) {
                y1 = atof(attrValue);
                delete[] attrValue ;
            }
            else
                D.Out(pdError, "Fed: ERREUR: missing Attribute.");
        }
        else
            D.Out(pdError, "Fed: ERREUR: handle inconnu.");
    }

    objects.reflect(theObject, (int) x1, (int) y1);
}

// ----------------------------------------------------------------------------
/** Callback : remove object instance
 */
void
Billard::removeObjectInstance(ObjectHandle theObject,
                          const FedTime &,
                          const char *,
                          EventRetractionHandle)
    throw (ObjectNotKnown, InvalidFederationTime, FederateInternalError)
{
    objects.remove(theObject);
}

// ----------------------------------------------------------------------------
/** Callback : time advance granted
 */
void
Billard::timeAdvanceGrant(const FedTime& /*theTime*/)
    throw (InvalidFederationTime, TimeAdvanceWasNotInProgress,
           FederationTimeAlreadyPassed, FederateInternalError)
{    
    granted = true ;
}

// $Id: Billard.cc,v 3.4 2003/10/27 10:51:38 breholee Exp $
