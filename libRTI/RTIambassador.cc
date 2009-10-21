// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This file is part of CERTI-libRTI
//
// CERTI-libRTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libRTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: RTIambassador.cc,v 3.109 2009/10/21 20:04:46 erk Exp $
// ----------------------------------------------------------------------------

#include "RTI.hh"
#include "fedtime.hh"

#include "RTIambPrivateRefs.hh"
#include "RTItypesImp.hh"

#include "Message.hh"
#include "PrettyDebug.hh"

#include "config.h"

#ifdef _WIN32
#include <stdio.h>
#include <string.h>
#else 
#include <unistd.h>
#endif
#include <iostream>
#include <signal.h>
#include <cassert>
#include <cerrno>
#include <typeinfo>

using std::cout ;
using std::cerr ;
using std::endl ;

namespace {

static PrettyDebug D("LIBRTI", __FILE__);
static PrettyDebug G("GENDOC",__FILE__) ;

using namespace certi ;

std::vector<RTI::Handle>
build_region_handles(RTI::Region **regions, int nb)
    throw (RTI::RegionNotKnown)
{
    std::vector<RTI::Handle> vect(nb);
    for (int i = 0 ; i < nb ; ++i) {
    	RTI::Region *region = regions[i] ;
    	try {
    		vect[i] = dynamic_cast<RegionImp *>(region)->getHandle();
    	}
    	catch (std::bad_cast) {
    		throw RTI::RegionNotKnown("");
    	}
    }
    return vect ;
}

RTI::Handle
get_handle(const RTI::Region &region)
    throw (RTI::RegionNotKnown, RTI::RTIinternalError)
{
    try {
	return dynamic_cast<const RegionImp &>(region).getHandle();
    }
    catch (std::bad_cast) {
	throw RTI::RegionNotKnown("");
    }
    throw RTI::RTIinternalError("");
}

char *
hla_strdup(const std::string &s)
    throw (RTI::RTIinternalError)
{
    try {
        size_t len = s.length();
        // the HLA 1.3 standard defines, that char* must be free-ed by delete[]
        char *result = new char[len+1];
        strncpy(result, s.c_str(), len);
        result[len] = '\0';

        return result;
    }
    catch (std::bad_alloc) {
        throw RTI::RTIinternalError("Cannot allocate memory.");
    }
    throw RTI::RTIinternalError("");
}

} // anonymous namespace

// ----------------------------------------------------------------------------
//! Start RTIambassador processes for communication with RTIG.
/*! When a new RTIambassador is created in the application, a new process rtia
  is launched. This process is used for data exchange with rtig server.
  This process connects to rtia after one second delay (UNIX socket).
*/

RTI::RTIambassador::RTIambassador()
    throw (RTI::MemoryExhausted, RTI::RTIinternalError)
{
    G.Out(pdGendoc,"enter RTIambassador::RTIambassador");
    PrettyDebug::setFederateName( "Federate" );
    std::stringstream msg;

    privateRefs = new RTIambPrivateRefs();

    privateRefs->socketUn = new SocketUN(stIgnoreSignal);
	
    privateRefs->is_reentrant = false ;

    std::vector<std::string> rtiaList;
    const char* env = getenv("CERTI_RTIA");
    if (env && strlen(env))
      rtiaList.push_back(std::string(env));
    env = getenv("CERTI_HOME");
    if (env && strlen(env))
      rtiaList.push_back(std::string(env) + "/bin/rtia");
    rtiaList.push_back(PACKAGE_INSTALL_PREFIX "/bin/rtia");
    rtiaList.push_back("rtia");

#if defined(RTIA_USE_TCP)
    int port = privateRefs->socketUn->listenUN();
    if (port == -1) {
      D.Out( pdError, "Cannot listen to RTIA connection. Abort." );
      throw RTI::RTIinternalError( "Cannot listen to RTIA connection" );
    }
#else
    int pipeFd = privateRefs->socketUn->socketpair();
    if (pipeFd == -1) {
      D.Out( pdError, "Cannot get socketpair to RTIA connection. Abort." );
      throw RTI::RTIinternalError( "Cannot get socketpair to RTIA connection" );
    }
#endif

#ifdef _WIN32
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory( &si, sizeof(si) );
  si.cb = sizeof(si);
  ZeroMemory( &pi, sizeof(pi) );

#if !defined(RTIA_USE_TCP)
  SOCKET newPipeFd;
  if (!DuplicateHandle(GetCurrentProcess(),
                       (HANDLE)pipeFd,
                       GetCurrentProcess(),
                       (HANDLE*)&newPipeFd,
                       0,
                       TRUE, // Inheritable
                       DUPLICATE_SAME_ACCESS)) {
    D.Out( pdError, "Cannot duplicate socket for RTIA connection. Abort." );
    throw RTI::RTIinternalError( "Cannot duplicate socket for RTIA connection. Abort." );
  }
#endif

  bool success = false;
  for (unsigned i = 0; i < rtiaList.size(); ++i) {
    std::stringstream stream;
#if defined(RTIA_USE_TCP)
    stream << rtiaList[i] << ".exe -p " << port;
#else
    stream << rtiaList[i] << ".exe -f " << newPipeFd;
#endif

    // Start the child process. 
    if (CreateProcess( NULL, // No module name (use command line). 
                       (char*)stream.str().c_str(),	// Command line. 
                       NULL,					// Process handle not inheritable. 
                       NULL,					// Thread handle not inheritable. 
                       TRUE,					// Set handle inheritance to TRUE.
                       0,   					// No creation flags. 
                       NULL,					// Use parent's environment block. 
                       NULL,					// Use parent's starting directory. 
                       &si,					// Pointer to STARTUPINFO structure.
                       &pi ))					// Pointer to PROCESS_INFORMATION structure.
      {
        success = true;
        break;
      }
  }
  if (!success) {
    msg << "CreateProcess - GetLastError()=<"
        << GetLastError() <<"> "
        << "Cannot connect to RTIA.exe";		
    throw RTI::RTIinternalError( msg.str().c_str());
  }
    
   privateRefs->handle_RTIA = pi.hProcess;
   privateRefs->pid_RTIA = pi.dwProcessId;

#if !defined(RTIA_USE_TCP)
   closesocket(pipeFd);
   closesocket(newPipeFd);
#endif

#else

    sigset_t nset, oset;
    // temporarily block termination signals
    // note: this is to prevent child processes from receiving termination signals
    sigemptyset(&nset);
    sigaddset(&nset, SIGINT);
    sigprocmask(SIG_BLOCK, &nset, &oset);

    switch((privateRefs->pid_RTIA = fork())) {
      case -1: // fork failed.
        perror("fork");
        // unbock the above blocked signals
        sigprocmask(SIG_SETMASK, &oset, NULL);
#if !defined(RTIA_USE_TCP)
        close(pipeFd);
#endif
        throw RTI::RTIinternalError("fork failed in RTIambassador constructor");
        break ;

      case 0: // child process (RTIA).
        // close all open filedescriptors except the pipe one
        for (int fdmax = sysconf(_SC_OPEN_MAX), fd = 3; fd < fdmax; ++fd) {
#if !defined(RTIA_USE_TCP)
          if (fd == pipeFd)
            continue;
#endif
          close(fd);
        }
        for (unsigned i = 0; i < rtiaList.size(); ++i)
        {
          std::stringstream stream;
#if defined(RTIA_USE_TCP)
          stream << port;
          execlp(rtiaList[i].c_str(), rtiaList[i].c_str(), "-p", stream.str().c_str(), NULL);
#else
          stream << pipeFd;
          execlp(rtiaList[i].c_str(), rtiaList[i].c_str(), "-f", stream.str().c_str(), NULL);
#endif
        }
        // unbock the above blocked signals
        sigprocmask(SIG_SETMASK, &oset, NULL);
        msg << "Could not launch RTIA process (execlp): "
            << strerror(errno)
            << endl
            << "Maybe RTIA is not in search PATH environment.";        
        throw RTI::RTIinternalError(msg.str().c_str());
        
      default: // father process (Federe).
        // unbock the above blocked signals
        sigprocmask(SIG_SETMASK, &oset, NULL);
#if !defined(RTIA_USE_TCP)
        close(pipeFd);
#endif
        break ;
    }
#endif

#if defined(RTIA_USE_TCP)
    if (privateRefs->socketUn->acceptUN(10*1000) == -1) {
#ifdef WIN32
      TerminateProcess(privateRefs->pid_RTIA, 0);
#else
      kill( privateRefs->pid_RTIA, SIGINT );
#endif
      throw RTI::RTIinternalError( "Cannot connect to RTIA" );
    }
#endif

    G.Out(pdGendoc,"exit  RTIambassador::RTIambassador");
}

// ----------------------------------------------------------------------------
//! Closes processes.
/*! When destructor is called, kill rtia process.
 */
RTI::RTIambassador::~RTIambassador()
    throw (RTI::RTIinternalError)
{
    Message req, rep ;

    req.type = Message::CLOSE_CONNEXION;
    G.Out(pdGendoc,"        ====>executeService CLOSE_CONNEXION");
    privateRefs->executeService(&req, &rep);
    // after the response is received, the privateRefs->socketUn must not be used

    delete privateRefs;
}

// ----------------------------------------------------------------------------
//! Evoke callback (HLA1.3).
/*! Non-blocking callback evocation, returns immediately with or without 1 callback.
 *  @return Always returns false.
 */
RTI::Boolean
RTI::RTIambassador::tick()
    throw (RTI::SpecifiedSaveLabelDoesNotExist,
           RTI::ConcurrentAccessAttempted,
           RTI::RTIinternalError)
{
#if defined(LEGACY_LIBRTI)
    __tick_kernel(RTI_FALSE, 0.0, 0.0);
#elif defined (HLA13NG_LIBRTI)
    // may suffer from starving
    __tick_kernel(RTI_TRUE, 0.0, std::numeric_limits<double>::infinity());
#else
#error "At least one LIBRTI flag must be defined."
#endif
   return RTI_FALSE;
}

// ----------------------------------------------------------------------------
//! Evoke callback (CERTI extension).
/*! Blocks until a callback delivery and then evokes a single callback.
 *  Waits until a callback delivery, be careful.
 *  @return true if additional callbacks pending, false otherwise
 *  @warning This is a non-standard extension of the HLA 1.3 API.
 */
RTI::Boolean
RTI::RTIambassador::tick2()
    throw (RTI::SpecifiedSaveLabelDoesNotExist,
           RTI::ConcurrentAccessAttempted,
           RTI::RTIinternalError)
{
   __tick_kernel(RTI_FALSE, std::numeric_limits<double>::infinity(), 0.0);
   return RTI_FALSE;
}

// ----------------------------------------------------------------------------
//! Generic callback evocation (CERTI extension).
/*! Blocks up to "minimum" seconds until a callback delivery and then evokes a
 *  single callback.
 *  @return true if additional callbacks pending, false otherwise
 *  @warning This is a non-standard extension of the HLA 1.3 API.
 */
RTI::Boolean
RTI::RTIambassador::__tick_kernel(RTI::Boolean multiple, TickTime minimum, TickTime maximum)
    throw (RTI::SpecifiedSaveLabelDoesNotExist,
           RTI::ConcurrentAccessAttempted,
           RTI::RTIinternalError)
{
    Message vers_RTI, vers_Fed ;

    // Request callback(s) from the local RTIA
    vers_RTI.type = Message::TICK_REQUEST ;
    vers_RTI.setBoolean(multiple);
    vers_RTI.setMinTickTime(minimum);
    vers_RTI.setMaxTickTime(maximum);

    try {
        vers_RTI.send(privateRefs->socketUn,privateRefs->msgBufSend);
    }
    catch (NetworkError &e) {
        std::stringstream msg;
        msg << "NetworkError in tick() while sending TICK_REQUEST: " << e._reason;

        throw RTI::RTIinternalError(msg.str().c_str());
    }

    // Read response(s) from the local RTIA until Message::TICK_REQUEST is received.
    while (1) {
        try {
            vers_Fed.receive(privateRefs->socketUn,privateRefs->msgBufReceive);
        }
        catch (NetworkError &e) {
            std::stringstream msg;
            msg << "NetworkError in tick() while receiving response: " << e._reason;

            throw RTI::RTIinternalError(msg.str().c_str());
        }

        //D.Mes( pdMessage, 'M', vers_Fed.type);
        vers_Fed.trace("RTI::RTIambassador::tick ");

        // If the type is TICK_REQUEST, the __tick_kernel() has terminated.
        if (vers_Fed.type == Message::TICK_REQUEST) {
            if (vers_Fed.getExceptionType() != e_NO_EXCEPTION) {
                // tick() may only throw exceptions defined in the HLA standard
                // the RTIA is responsible for sending 'allowed' exceptions only
                privateRefs->processException(&vers_Fed);
            }
            return RTI::Boolean(vers_Fed.getBoolean());
        }

        try {
            // Otherwise, the RTI calls a FederateAmbassador service.
            privateRefs->callFederateAmbassador(&vers_Fed);
        }
        catch (RTI::RTIinternalError) {
            // RTIA awaits TICK_REQUEST_NEXT, terminate the tick() processing
            privateRefs->sendTickRequestStop();
            // ignore the response and re-throw the original exception
            throw;
        }

        try {
            // Request next callback from the RTIA
            Message tick_next;
            tick_next.type = Message::TICK_REQUEST_NEXT;

            tick_next.send(privateRefs->socketUn, privateRefs->msgBufSend);
        }
        catch (NetworkError &e) {
            std::stringstream msg;
            msg << "NetworkError in tick() while sending TICK_REQUEST_NEXT: " << e._reason;

            throw RTI::RTIinternalError(msg.str().c_str());
        }
    } // while(1)
}

// ----------------------------------------------------------------------------
//! Evoke multiple callbacks (HLA1.3).
/*! Blocks up to "minimum" seconds until a callback delivery, then envokes
 *  multiple callbacks until no callback is pending, or until "maximum"
 *  duration is reached.
 *  @return true if additional callbacks pending, false otherwise
 */
RTI::Boolean
RTI::RTIambassador::tick(TickTime minimum, TickTime maximum)
    throw (RTI::SpecifiedSaveLabelDoesNotExist, RTI::ConcurrentAccessAttempted,
           RTI::RTIinternalError)
{
    return __tick_kernel(RTI_TRUE, minimum, maximum);
}

// ----------------------------------------------------------------------------
//! Get Region Token.
RTI::RegionToken
RTI::RTIambassador::getRegionToken(Region *region)
    throw (RTI::FederateNotExecutionMember, RTI::ConcurrentAccessAttempted,
           RTI::RegionNotKnown, RTI::RTIinternalError)
{
    return get_handle(*region);
}

// ----------------------------------------------------------------------------
//! Get Region.
RTI::Region *
RTI::RTIambassador::getRegion(RegionToken)
    throw (RTI::FederateNotExecutionMember, RTI::ConcurrentAccessAttempted,
           RTI::RegionNotKnown, RTI::RTIinternalError)
{
    throw RTI::RTIinternalError("unimplemented service getRegion");
}

// ----------------------------------------------------------------------------
// Create Federation Execution.
/** Realization of the Create Federation Execution federation management service
    (HLA 1.3).
    Send a CREATE_FEDERATION_EXECUTION request type to inform rtia process a
    new federation is being created.
    @param executionName execution name of the federation to be created
    @param FED           FED file name (path seen by rtig)
*/
void
//RTI::
RTI::RTIambassador::createFederationExecution(const char *executionName,
					      const char *FED)
    throw (RTI::RTIinternalError, RTI::ConcurrentAccessAttempted, 
	   RTI::ErrorReadingFED, RTI::CouldNotOpenFED, 
	   RTI::FederationExecutionAlreadyExists)
{
Message req, rep ;

G.Out(pdGendoc,"enter RTIambassador::createFederationExecution");

req.type = Message::CREATE_FEDERATION_EXECUTION ;
req.setFederationName(executionName);
req.setFEDid(FED);

/*#ifdef _WIN32
	if(!stricmp(FED,executionName)) {
#else
	if(!strcasecmp(FED,exeName)) {
#endif
}*/
G.Out(pdGendoc,"             ====>executeService CREATE_FEDERATION_EXECUTION");

privateRefs->executeService(&req, &rep);

G.Out(pdGendoc,"exit RTIambassador::createFederationExecution");

}

// ----------------------------------------------------------------------------
// Destroy Federation Execution.
/** Realization of the Destroy Federation Execution federation management service
    (HLA 1.3).
    Send a DESTROY_FEDERATION_EXECUTION request type to remove a federation
    execution from the RTI set of federation executions.
    \param executionName execution name of the federation to be destroyed
*/
void
RTI::RTIambassador::destroyFederationExecution(const char *executionName)
    throw (RTI::RTIinternalError, RTI::ConcurrentAccessAttempted,
	   RTI::FederationExecutionDoesNotExist, RTI::FederatesCurrentlyJoined)
{
    Message req, rep ;

    G.Out(pdGendoc,"enter RTIambassador::destroyFederationExecution");

    req.type = Message::DESTROY_FEDERATION_EXECUTION ;
    req.setFederationName(executionName);

    G.Out(pdGendoc,"        ====>executeService DESTROY_FEDERATION_EXECUTION");

    privateRefs->executeService(&req, &rep);

    G.Out(pdGendoc,"exit RTIambassador::destroyFederationExecution");

}

// ----------------------------------------------------------------------------
//! Join Federation Execution.
FederateHandle
RTI::RTIambassador::joinFederationExecution(const char *yourName,
				       const char *executionName,
				       FederateAmbassadorPtr fedamb)
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress,
	   RTI::ConcurrentAccessAttempted, RTI::ErrorReadingFED, RTI::CouldNotOpenFED, 
	   RTI::FederationExecutionDoesNotExist, RTI::FederateAlreadyExecutionMember)
{
    Message req, rep ;

    G.Out(pdGendoc,"enter RTIambassador::joinFederationExecution");

    if ( yourName == NULL || strlen(yourName) == 0 )
        throw RTI::RTIinternalError("Incorrect or empty federate name");
    if ( executionName == NULL || strlen(executionName) == 0 )
        throw RTI::RTIinternalError("Incorrect or empty federation name");

    privateRefs->fed_amb = (FederateAmbassador *) fedamb ;

    req.type = Message::JOIN_FEDERATION_EXECUTION ;
    req.setFederateName(yourName);
    req.setFederationName(executionName);
    G.Out(pdGendoc,"        ====>executeService JOIN_FEDERATION_EXECUTION");
    privateRefs->executeService(&req, &rep);
    G.Out(pdGendoc,"exit  RTIambassador::joinFederationExecution");
    return rep.getFederate();
}

// ----------------------------------------------------------------------------
//! Resign Federation Execution.
void
RTI::RTIambassador::resignFederationExecution(ResignAction theAction)
    throw (RTI::FederateOwnsAttributes,
           RTI::FederateNotExecutionMember,
           RTI::InvalidResignAction,
           RTI::ConcurrentAccessAttempted,
           RTI::RTIinternalError)
{
    Message req, rep ;

    G.Out(pdGendoc,"enter RTIambassador::resignFederationExecution");

    req.type = Message::RESIGN_FEDERATION_EXECUTION ;
    req.setResignAction(theAction);

    G.Out(pdGendoc,"        ====>executeService RESIGN_FEDERATION_EXECUTION");
    privateRefs->executeService(&req, &rep);

    G.Out(pdGendoc,"exit RTIambassador::resignFederationExecution");

}

// ----------------------------------------------------------------------------
//! Register Federation Synchronization Point without set of federates 
//  (all federates in the federation execution are informed)
/**
    @param label Synchronization point label
    @param the_tag User-supplied tag
*/
void
RTI::RTIambassador::registerFederationSynchronizationPoint(const char *label,
                                                      const char *the_tag)
    throw (RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    G.Out(pdGendoc,"enter RTIambassador::registerFederationSynchronizationPoint for all federates");

    req.type = Message::REGISTER_FEDERATION_SYNCHRONIZATION_POINT ;
    req.setLabel(label);
    if ( the_tag == NULL )
       {
       throw RTI::RTIinternalError ("Calling registerFederationSynchronizationPoint with Tag NULL");
       }
    req.setTag(the_tag);
    // boolean false because without set of federates
    req.setBoolean(false);
    G.Out(pdGendoc,"        ====>executeService REGISTER_FEDERATION_SYNCHRONIZATION_POINT");
    privateRefs->executeService(&req, &rep);

    G.Out(pdGendoc,"exit RTIambassador::registerFederationSynchronizationPoint for all federates");

}

// ----------------------------------------------------------------------------
//! Register Federation Synchronization Point with set of federates (UNIMPLEMENTED)
//  (only federates in the set are informed)
/**
    @param label Synchronization point label
    @param theTag User-supplied tag
    @param set_of_fed set of federate designators
*/
void
RTI::RTIambassador::registerFederationSynchronizationPoint(const char *label,
						      const char *theTag,
						      const FederateHandleSet &set_of_fed)
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress,
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember)
{

    Message req, rep ;

    G.Out(pdGendoc,"enter RTIambassador::registerFederationSynchronizationPoint for some federates");

    req.type = Message::REGISTER_FEDERATION_SYNCHRONIZATION_POINT ;
    req.setLabel(label);
    if ( theTag == NULL )
       {
       throw RTI::RTIinternalError ("Calling registerFederationSynchronizationPoint with Tag NULL");
       }
    req.setTag(theTag);
    // boolean must be true because federate set exists but if size=0 (set empty)
    // we have to put boolean false (HLA 1.3 compliance to inform ALL federates)
    if ( set_of_fed.size() == 0 )
        {
        req.setBoolean(false) ;
        }
    else
        {
        req.setBoolean(true);
        // and then we have to store this set (after the size)
        req.handleArraySize = set_of_fed.size() ;
        req.handleArray.resize(req.handleArraySize) ;
        for (unsigned long i=0 ; i<set_of_fed.size() ; i++)
            {
            req.handleArray[i] = set_of_fed.getHandle(i) ;
            }
        }
    G.Out(pdGendoc,"        ====>executeService REGISTER_FEDERATION_SYNCHRONIZATION_POINT");
    privateRefs->executeService(&req, &rep);

    G.Out(pdGendoc,"exit RTIambassador::registerFederationSynchronizationPoint for some federates");

}

// ----------------------------------------------------------------------------
//! Synchronization Point Achieved
void
RTI::RTIambassador::synchronizationPointAchieved(const char *label)
    throw (RTI::SynchronizationPointLabelWasNotAnnounced,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    G.Out(pdGendoc,"enter RTIambassador::synchronizationPointAchieved");

    req.type = Message::SYNCHRONIZATION_POINT_ACHIEVED ;
    req.setLabel(label);
    G.Out(pdGendoc,"        ====>executeService SYNCHRONIZATION_POINT_ACHIEVED");
    privateRefs->executeService(&req, &rep);

    G.Out(pdGendoc,"exit  RTIambassador::synchronizationPointAchieved");
}

// ----------------------------------------------------------------------------
//! Request Federation Save with time.
void
RTI::RTIambassador::requestFederationSave(const char *label,
                                     const RTI::FedTime& theTime)
    throw (RTI::FederationTimeAlreadyPassed,
           RTI::InvalidFederationTime,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    G.Out(pdGendoc,"enter RTIambassador::requestFederationSave with time");

    req.type = Message::REQUEST_FEDERATION_SAVE ;
    req.setFedTime(certi_cast<RTIfedTime>()(theTime).getTime());
    req.setLabel(label);
    // boolean true means time constrained
    req.setBoolean(true);

    G.Out(pdGendoc,"        ====>executeService REQUEST_FEDERATION_SAVE");
    privateRefs->executeService(&req, &rep);

    G.Out(pdGendoc,"exit RTIambassador::requestFederationSave with time");
}

// ----------------------------------------------------------------------------
//! Request Federation Save without time.
void
RTI::RTIambassador::requestFederationSave(const char *label)
    throw (RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    G.Out(pdGendoc,"enter RTIambassador::requestFederationSave without time");

    req.type = Message::REQUEST_FEDERATION_SAVE ;
    req.setLabel(label);
    // boolean false means without time
    req.setBoolean(false);

    G.Out(pdGendoc,"      ====>executeService REQUEST_FEDERATION_SAVE");

    privateRefs->executeService(&req, &rep);

    G.Out(pdGendoc,"exit  RTIambassador::requestFederationSave without time");
}

// ----------------------------------------------------------------------------
//! Federate Save Begun.
void
RTI::RTIambassador::federateSaveBegun()
    throw (RTI::SaveNotInitiated,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    G.Out(pdGendoc,"enter RTIambassador::federateSaveBegun");

    req.type = Message::FEDERATE_SAVE_BEGUN ;
    G.Out(pdGendoc,"      ====>executeService FEDERATE_SAVE_BEGUN");
    privateRefs->executeService(&req, &rep);

    G.Out(pdGendoc,"exit  RTIambassador::federateSaveBegun");
}

// ----------------------------------------------------------------------------
//! Federate Save Complete.
void
RTI::RTIambassador::federateSaveComplete()
    throw (RTI::SaveNotInitiated,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    G.Out(pdGendoc,"enter RTIambassador::federateSaveComplete");

    req.type = Message::FEDERATE_SAVE_COMPLETE ;
    G.Out(pdGendoc,"      ====>executeService FEDERATE_SAVE_COMPLETE");
    privateRefs->executeService(&req, &rep);
    G.Out(pdGendoc,"exit  RTIambassador::federateSaveComplete");
}

// ----------------------------------------------------------------------------
// Federate Save Not Complete.
void
RTI::RTIambassador::federateSaveNotComplete()
    throw (RTI::SaveNotInitiated,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{

    Message req, rep ;

    G.Out(pdGendoc,"enter RTIambassador::federateSaveNotComplete");
    G.Out(pdGendoc,"      ====>executeService FEDERATE_SAVE_NOT_COMPLETE");
    req.type = Message::FEDERATE_SAVE_NOT_COMPLETE ;

    privateRefs->executeService(&req, &rep);

    G.Out(pdGendoc,"exit  RTIambassador::federateSaveNotComplete");
}

// ----------------------------------------------------------------------------
//! Request Restore.
void
RTI::RTIambassador::requestFederationRestore(const char *label)
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember)
{
    Message req, rep ;

    G.Out(pdGendoc,"enter RTIambassador::requestFederationRestore");

    req.type = Message::REQUEST_FEDERATION_RESTORE ;
    req.setLabel(label);
    G.Out(pdGendoc,"      ====>executeService REQUEST_FEDERATION_RESTORE");
    privateRefs->executeService(&req, &rep);
    G.Out(pdGendoc,"exit  RTIambassador::requestFederationRestore");
}

// ----------------------------------------------------------------------------
//! Restore Complete.
void
RTI::RTIambassador::federateRestoreComplete()
    throw (RTI::RTIinternalError, RTI::SaveInProgress,
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember,
	   RTI::RestoreNotRequested)
{
    Message req, rep ;

    G.Out(pdGendoc,"enter RTIambassador::federateRestoreComplete");

    req.type = Message::FEDERATE_RESTORE_COMPLETE ;
    G.Out(pdGendoc,"      ====>executeService FEDERATE_RESTORE_COMPLETE");
    privateRefs->executeService(&req, &rep);
    G.Out(pdGendoc,"exit  RTIambassador::federateRestoreComplete");
}

// ----------------------------------------------------------------------------
//! Federate Restore Not Complete.
void
RTI::RTIambassador::federateRestoreNotComplete()
    throw (RTI::RTIinternalError, RTI::SaveInProgress,
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember,
	   RTI::RestoreNotRequested)
{
    Message req, rep ;

    G.Out(pdGendoc,"enter RTIambassador::federateRestoreNotComplete");
    G.Out(pdGendoc,"      ====>executeService FEDERATE_RESTORE_NOT_COMPLETE");
    req.type = Message::FEDERATE_RESTORE_NOT_COMPLETE ;

    privateRefs->executeService(&req, &rep);
    G.Out(pdGendoc,"exit  RTIambassador::federateRestoreNotComplete");
}

// ----------------------------------------------------------------------------
// Publish Object Class
void
RTI::RTIambassador::publishObjectClass(ObjectClassHandle theClass,
                                  const AttributeHandleSet& attributeList)
    throw (RTI::RTIinternalError, 
	   RTI::RestoreInProgress, RTI::SaveInProgress, RTI::ConcurrentAccessAttempted, 
	   RTI::FederateNotExecutionMember, RTI::OwnershipAcquisitionPending, 
	   RTI::AttributeNotDefined, RTI::ObjectClassNotDefined)
{
    Message req, rep ;

    G.Out(pdGendoc,"enter RTIambassador::publishObjectClass");

    req.type = Message::PUBLISH_OBJECT_CLASS ;
    req.setObjectClass(theClass);
    req.setAHS(certi_cast<AttributeHandleSetImp>()(attributeList).getAttributeHandles());
    G.Out(pdGendoc,"      ====>executeService PUBLISH_OBJECT_CLASS");
    privateRefs->executeService(&req, &rep);
    G.Out(pdGendoc,"exit  RTIambassador::publishObjectClass");
}

// ----------------------------------------------------------------------------
// UnPublish Object Class
void
RTI::RTIambassador::unpublishObjectClass(ObjectClassHandle theClass)
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember, 
	   RTI::OwnershipAcquisitionPending, RTI::ObjectClassNotPublished, 
	   RTI::ObjectClassNotDefined)
{
    Message req, rep ;

    G.Out(pdGendoc,"enter RTIambassador::unpublishObjectClass");

    req.type = Message::UNPUBLISH_OBJECT_CLASS ;
    req.setObjectClass(theClass);
    G.Out(pdGendoc,"      ====>executeService UNPUBLISH_OBJECT_CLASS");
    privateRefs->executeService(&req, &rep);
    G.Out(pdGendoc,"exit  RTIambassador::unpublishObjectClass");
}


// ----------------------------------------------------------------------------
// Publish Interaction Class
void
RTI::RTIambassador::publishInteractionClass(InteractionClassHandle theInteraction)
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember, 
	   RTI::InteractionClassNotDefined)
{
    Message req, rep ;

    req.type = Message::PUBLISH_INTERACTION_CLASS ;
    req.setInteractionClass(theInteraction);
    G.Out(pdGendoc,"      ====>executeService PUBLISH_INTERACTION_CLASS");
    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Publish Interaction Class
void
RTI::RTIambassador::unpublishInteractionClass(InteractionClassHandle theInteraction)
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember, 
	   RTI::InteractionClassNotPublished, RTI::InteractionClassNotDefined)
{
    Message req, rep ;

    req.type = Message::UNPUBLISH_INTERACTION_CLASS ;
    req.setInteractionClass(theInteraction);
    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Subscribe Object Class Attributes
void
RTI::RTIambassador::
subscribeObjectClassAttributes(ObjectClassHandle theClass,
                               const AttributeHandleSet& attributeList,
                               RTI::Boolean active)
    throw (RTI::RTIinternalError, 
	   RTI::RestoreInProgress, RTI::SaveInProgress, RTI::ConcurrentAccessAttempted, 
	   RTI::FederateNotExecutionMember, RTI::AttributeNotDefined, 
	   RTI::ObjectClassNotDefined)
{
    Message req, rep ;
    G.Out(pdGendoc,"enter RTIambassador::subscribeObjectClassAttributes");
    req.type = Message::SUBSCRIBE_OBJECT_CLASS_ATTRIBUTES ;
    req.setObjectClass(theClass);
    req.setAHS(certi_cast<AttributeHandleSetImp>()(attributeList).getAttributeHandles());
    req.setBoolean(active);

    privateRefs->executeService(&req, &rep);
    G.Out(pdGendoc,"exit  RTIambassador::subscribeObjectClassAttributes");
}

// ----------------------------------------------------------------------------
// UnSubscribe Object Class Attribute
void
RTI::RTIambassador::unsubscribeObjectClass(ObjectClassHandle theClass)
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember, 
	   RTI::ObjectClassNotSubscribed, RTI::ObjectClassNotDefined)
{
    Message req, rep ;

    req.type = Message::UNSUBSCRIBE_OBJECT_CLASS ;
    req.setObjectClass(theClass);
    privateRefs->executeService(&req, &rep);
}


// ----------------------------------------------------------------------------
// Subscribe Interaction Class
void
RTI::RTIambassador::subscribeInteractionClass(InteractionClassHandle theClass,
                                         RTI::Boolean /*active*/)
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, 
	   RTI::SaveInProgress, RTI::FederateLoggingServiceCalls, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember, 
	   RTI::InteractionClassNotDefined)
{
    Message req, rep ;

    req.type = Message::SUBSCRIBE_INTERACTION_CLASS ;
    req.setInteractionClass(theClass);
    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// UnSubscribe Interaction Class
void
RTI::RTIambassador::unsubscribeInteractionClass(InteractionClassHandle theClass)
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember, 
	   RTI::InteractionClassNotSubscribed, RTI::InteractionClassNotDefined)
{
    Message req, rep ;

    req.type = Message::UNSUBSCRIBE_INTERACTION_CLASS ;
    req.setInteractionClass(theClass);
    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Register Object
ObjectHandle
RTI::RTIambassador::registerObjectInstance(ObjectClassHandle theClass,
                                      const char *theObjectName)
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, 
	   RTI::SaveInProgress, RTI::ConcurrentAccessAttempted, 
	   RTI::FederateNotExecutionMember, RTI::ObjectAlreadyRegistered, 
	   RTI::ObjectClassNotPublished, RTI::ObjectClassNotDefined)
{
    Message req, rep ;

    req.type = Message::REGISTER_OBJECT_INSTANCE ;
    req.setName(theObjectName);
    req.setObjectClass(theClass);
    privateRefs->executeService(&req, &rep);

    return rep.getObject();
}

// ----------------------------------------------------------------------------
ObjectHandle
RTI::RTIambassador::registerObjectInstance(ObjectClassHandle theClass)
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember, 
	   RTI::ObjectClassNotPublished, RTI::ObjectClassNotDefined)
{
    Message req, rep ;

    req.type = Message::REGISTER_OBJECT_INSTANCE ;
    req.setName("\0");
    req.setObjectClass(theClass);

    privateRefs->executeService(&req, &rep);

    return rep.getObject();
}

// ----------------------------------------------------------------------------
// Update Attribute Values with time
/** Realization of the Create Federation Execution federation management service
    with time (HLA 1.3).
    Provide current values to the federation for instance attributes owned by
    the federate.
    Un event retraction designator is returned.
    @param theObject     Object instance designator
    @param theAttributes Set of attribute designator and value pairs
    @param theTime       Federation time
    @param theTag        User supplied tag
*/
RTI::EventRetractionHandle
RTI::RTIambassador::
updateAttributeValues(ObjectHandle theObject,
                      const AttributeHandleValuePairSet& theAttributes,
                      const RTI::FedTime& theTime,
                      const char *theTag)
    throw (RTI::ObjectNotKnown,
           RTI::AttributeNotDefined,
           RTI::AttributeNotOwned,
           RTI::InvalidFederationTime,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    G.Out(pdGendoc,"enter RTIambassador::updateAttributeValues with time");
    Message req, rep ;

    req.type = Message::UPDATE_ATTRIBUTE_VALUES ;
    req.setObject(theObject);
    req.setFedTime(certi_cast<RTIfedTime>()(theTime).getTime());
    if ( theTag == NULL)
       {
       throw RTI::RTIinternalError ("Calling updateAttributeValues with Tag NULL");
       }
    req.setTag(theTag);
    req.setAHVPS(certi_cast<AttributeHandleValuePairSetImp>()(theAttributes).getAttributeHandleValuePairs());
    req.setBoolean(true);

    privateRefs->executeService(&req, &rep);
    G.Out(pdGendoc,"return  RTIambassador::updateAttributeValues with time");
    return rep.getEventRetraction();
}

// ----------------------------------------------------------------------------
// Update Attribute Values without time
/** Realization of the Create Federation Execution federation management service
    without time (HLA 1.3).
    Provide current values to the federation for instance attributes owned by
    the federate.
    @param theObject     Object instance designator
    @param theAttributes Set of attribute designator and value pairs
    @param theTag        User supplied tag
*/
void
RTI::RTIambassador::updateAttributeValues(ObjectHandle the_object,
                                     const AttributeHandleValuePairSet& theAttributes,
                                     const char *theTag)
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember, 
	   RTI::AttributeNotOwned, RTI::AttributeNotDefined, RTI::ObjectNotKnown)
{
    G.Out(pdGendoc,"enter RTIambassador::updateAttributeValues without time");
    Message req, rep ;

    req.type = Message::UPDATE_ATTRIBUTE_VALUES ;
    req.setObject(the_object);
    if ( theTag == NULL)
       {
       throw RTI::RTIinternalError ("Calling updateAttributeValues with Tag NULL");
       }
    req.setTag(theTag);
    req.setAHVPS(certi_cast<AttributeHandleValuePairSetImp>()(theAttributes).getAttributeHandleValuePairs());
    req.setBoolean(false);

    privateRefs->executeService(&req, &rep);
    G.Out(pdGendoc,"exit  RTIambassador::updateAttributeValues without time");
}

// ----------------------------------------------------------------------------
/** Send Interaction with time
    This service (HLA 1.3) send an interaction into the federation.
    As the federation time argument is supplied, an event retraction designator is returned.
    @param theInteraction Interaction class designator
    @param theParameters Set of interaction parameters designator and value pairs
    @param theTime Federation time
    @param theTag User-supplied tag
*/
RTI::EventRetractionHandle
RTI::RTIambassador::sendInteraction(InteractionClassHandle theInteraction,
                               const ParameterHandleValuePairSet& theParameters,
                               const RTI::FedTime& theTime,
                               const char *theTag)
    throw (RTI::InteractionClassNotDefined,
           RTI::InteractionClassNotPublished,
           RTI::InteractionParameterNotDefined,
           RTI::InvalidFederationTime,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.type = Message::SEND_INTERACTION ;
    req.setInteractionClass(theInteraction);
    req.setFedTime(certi_cast<RTIfedTime>()(theTime).getTime());
    if (theTag == NULL)
       {
       throw RTI::RTIinternalError ("Calling sendInteraction with Tag NULL") ;
       }
    req.setTag((std::string)theTag);
    req.setPHVPS(certi_cast<ParameterHandleValuePairSetImp>()(theParameters).getParameterHandleValuePairs());
    req.setRegion(0);
    req.setBoolean(true);

    privateRefs->executeService(&req, &rep);

    return rep.getEventRetraction();
}

// ----------------------------------------------------------------------------
/** Send Interaction without time
    This service (HLA 1.3) send an interaction into the federation.
    None is returned.
    @param theInteraction Interaction class designator
    @param theParameters Set of interaction parameters designator and value pairs
    @param theTag User-supplied tag
*/
void
RTI::RTIambassador::sendInteraction(InteractionClassHandle theInteraction,
                               const ParameterHandleValuePairSet &theParameters,
                               const char *theTag)
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember, 
	   RTI::InteractionParameterNotDefined, RTI::InteractionClassNotPublished, 
	   RTI::InteractionClassNotDefined)
{

    Message req, rep ;

    req.type = Message::SEND_INTERACTION ;
    req.setInteractionClass(theInteraction);
    if (theTag == NULL)
       {
       throw RTI::RTIinternalError ("Calling sendIntercation with Tag NULL") ;
       }
    req.setTag((std::string)theTag);
    req.setPHVPS(certi_cast<ParameterHandleValuePairSetImp>()(theParameters).getParameterHandleValuePairs());
    req.setRegion(0);
    req.setBoolean(false);
  
    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
/** Delete Object with time
    This service (HLA 1.3) deletes an object instance.
    As the federation time argument is supplied, an event retraction designator is returned.
    @param theObject Object designator
    @param theTime Federation time
    @param theTag user supplied tag
*/
RTI::EventRetractionHandle
RTI::RTIambassador::deleteObjectInstance(ObjectHandle theObject,
                                    const RTI::FedTime& theTime,
                                    const char *theTag)
    throw (RTI::ObjectNotKnown,
           RTI::DeletePrivilegeNotHeld,
           RTI::InvalidFederationTime,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.type = Message::DELETE_OBJECT_INSTANCE ;
    req.setObject(theObject);
    req.setFedTime(certi_cast<RTIfedTime>()(theTime).getTime());
    if (theTag == NULL)
       {
       throw RTI::RTIinternalError ("Calling deleteObjectInstance with Tag NULL") ;
       }
    req.setTag(theTag);
    req.setBoolean(true);

    privateRefs->executeService(&req, &rep);
    return rep.getEventRetraction();
}

// ----------------------------------------------------------------------------
/** Delete Object without time
    This service (HLA 1.3) deletes an object instance.
    @param theObject Object designator
    @param theTag user supplied tag
*/
void
RTI::RTIambassador::deleteObjectInstance(ObjectHandle theObject,
                                    const char *theTag)
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember, 
	   RTI::DeletePrivilegeNotHeld, RTI::ObjectNotKnown)
{
    Message req, rep ;

    req.type = Message::DELETE_OBJECT_INSTANCE ;
    req.setObject(theObject);
    if (theTag == NULL)
       {
       throw RTI::RTIinternalError ("Calling deleteObjectInstance with Tag NULL") ;
       }
    req.setTag(theTag);
   
    req.setBoolean(false);

    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Local Delete Object Instance
void
RTI::RTIambassador::localDeleteObjectInstance(ObjectHandle theObject)
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember, 
	   RTI::FederateOwnsAttributes, RTI::ObjectNotKnown)
{
    throw RTI::RTIinternalError("unimplemented service localDeleteObjectInstance");
    Message req, rep ;

    req.type = Message::LOCAL_DELETE_OBJECT_INSTANCE ;
    req.setObject(theObject);

    privateRefs->executeService(&req, &rep);
}


// ----------------------------------------------------------------------------
// Change Attribute Transportation Type
void
RTI::RTIambassador::
changeAttributeTransportationType(ObjectHandle theObject,
                                  const AttributeHandleSet& theAttributes,
                                  TransportationHandle theType)
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember, 
	   RTI::InvalidTransportationHandle, RTI::AttributeNotOwned, 
	   RTI::AttributeNotDefined, RTI::ObjectNotKnown)
{
    Message req, rep ;

    req.type = Message::CHANGE_ATTRIBUTE_TRANSPORTATION_TYPE ;
    req.setObject(theObject);
    req.setTransportation(theType);
    req.setAHS(certi_cast<AttributeHandleSetImp>()(theAttributes).getAttributeHandles());

    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Change Interaction Transportation Type
void
RTI::RTIambassador::
changeInteractionTransportationType(InteractionClassHandle theClass,
                                    TransportationHandle theType)
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, 
	   RTI::SaveInProgress, RTI::ConcurrentAccessAttempted, 
	   RTI::FederateNotExecutionMember, RTI::InvalidTransportationHandle, 
	   RTI::InteractionClassNotPublished, RTI::InteractionClassNotDefined)
{
    Message req, rep ;

    req.type = Message::CHANGE_INTERACTION_TRANSPORTATION_TYPE ;
    req.setInteractionClass(theClass);
    req.setTransportation(theType);

    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Request Attribute Value Update
void
RTI::RTIambassador::requestObjectAttributeValueUpdate(ObjectHandle theObject,
                                                 const AttributeHandleSet &ahs)
    throw (RTI::RTIinternalError, 
	   RTI::RestoreInProgress, RTI::SaveInProgress, RTI::ConcurrentAccessAttempted, 
	   RTI::FederateNotExecutionMember, RTI::AttributeNotDefined, 
	   RTI::ObjectNotKnown)
{
    Message req, rep ;

    G.Out(pdGendoc,"enter RTIambassador::requestObjectAttributeValueUpdate");
    req.type = Message::REQUEST_OBJECT_ATTRIBUTE_VALUE_UPDATE ;
    req.setObject(theObject);
    req.setAHS(certi_cast<AttributeHandleSetImp>()(ahs).getAttributeHandles());

    privateRefs->executeService(&req, &rep);
    G.Out(pdGendoc,"exit  RTIambassador::requestObjectAttributeValueUpdate");
}

// ----------------------------------------------------------------------------
void
RTI::RTIambassador::requestClassAttributeValueUpdate(ObjectClassHandle theClass,
                                                const AttributeHandleSet &attrs)
    throw (RTI::RTIinternalError, 
	   RTI::RestoreInProgress, RTI::SaveInProgress, RTI::ConcurrentAccessAttempted, 
	   RTI::FederateNotExecutionMember, RTI::AttributeNotDefined, 
	   RTI::ObjectClassNotDefined)
{

    Message req, rep ;

    G.Out(pdGendoc,"enter RTIambassador::requestClassAttributeValueUpdate");
    req.type = Message::REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE ;
    req.setObjectClass(theClass);
    req.setAHS(certi_cast<AttributeHandleSetImp>()(attrs).getAttributeHandles());

    privateRefs->executeService(&req, &rep);
    G.Out(pdGendoc,"exit  RTIambassador::requestClassAttributeValueUpdate");
}

// ----------------------------------------------------------------------------
// UnConditional Attribute Ownership Divestiture
void
RTI::RTIambassador::
unconditionalAttributeOwnershipDivestiture(ObjectHandle theObject,
                                           const AttributeHandleSet &attrs)
    throw (RTI::ObjectNotKnown,
           RTI::AttributeNotDefined,
           RTI::AttributeNotOwned,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.type = Message::UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE ;
    req.setObject(theObject);
    req.setAHS(certi_cast<AttributeHandleSetImp>()(attrs).getAttributeHandles());

    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Negotiated Attribute Ownership Divestiture
void
RTI::RTIambassador::
negotiatedAttributeOwnershipDivestiture(ObjectHandle theObject,
                                        const AttributeHandleSet& attrs,
                                        const char *theTag)
    throw (RTI::ObjectNotKnown,
           RTI::AttributeNotDefined,
           RTI::AttributeNotOwned,
           RTI::AttributeAlreadyBeingDivested,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.type = Message::NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE ;
    req.setObject(theObject);
    if (theTag == NULL)
       {
       throw RTI::RTIinternalError ("Calling negotiatedAttributeOwnershipDivestiture with Tag NULL") ;
       }
    req.setTag(theTag);
    req.setAHS(certi_cast<AttributeHandleSetImp>()(attrs).getAttributeHandles());

    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Attribute Ownership Acquisition
void
RTI::RTIambassador::
attributeOwnershipAcquisition(ObjectHandle theObject,
                              const AttributeHandleSet& desiredAttributes,
                              const char *theTag)
    throw (RTI::ObjectNotKnown,
           RTI::ObjectClassNotPublished,
           RTI::AttributeNotDefined,
           RTI::AttributeNotPublished,
           RTI::FederateOwnsAttributes,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.type = Message::ATTRIBUTE_OWNERSHIP_ACQUISITION ;
    req.setObject(theObject);
    if (theTag == NULL)
       {
       throw RTI::RTIinternalError ("Calling attributeOwnershipAcquisition with Tag NULL") ;
       }
    req.setTag(theTag);
    req.setAHS(certi_cast<AttributeHandleSetImp>()(desiredAttributes).getAttributeHandles());

    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Attribute Ownership Release Response
RTI::AttributeHandleSet*
RTI::RTIambassador::
attributeOwnershipReleaseResponse(ObjectHandle theObject,
                                  const AttributeHandleSet& attrs)
    throw (RTI::ObjectNotKnown,
           RTI::AttributeNotDefined,
           RTI::AttributeNotOwned,
           RTI::FederateWasNotAskedToReleaseAttribute,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.type = Message::ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE ;
    req.setObject(theObject);
    req.setAHS(certi_cast<AttributeHandleSetImp>()(attrs).getAttributeHandles());

    privateRefs->executeService(&req, &rep);

    if (rep.getExceptionType() == e_NO_EXCEPTION) {
        return new AttributeHandleSetImp(rep.getAHS());
    }

    return NULL ;
}

// ----------------------------------------------------------------------------
// Cancel Negotiated Attribute Ownership Divestiture
void
RTI::RTIambassador::
cancelNegotiatedAttributeOwnershipDivestiture(ObjectHandle theObject,
                                              const AttributeHandleSet& attrs)
    throw (RTI::ObjectNotKnown,
           RTI::AttributeNotDefined,
           RTI::AttributeNotOwned,
           RTI::AttributeDivestitureWasNotRequested,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.type = Message::CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE ;
    req.setObject(theObject);
    req.setAHS(certi_cast<AttributeHandleSetImp>()(attrs).getAttributeHandles());

    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Cancel Attribute Ownership Acquisition
void
RTI::RTIambassador::
cancelAttributeOwnershipAcquisition(ObjectHandle theObject,
                                    const AttributeHandleSet& attrs)
    throw (RTI::ObjectNotKnown,
           RTI::AttributeNotDefined,
           RTI::AttributeAlreadyOwned,
           RTI::AttributeAcquisitionWasNotRequested,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.type = Message::CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION ;
    req.setObject(theObject);
    req.setAHS(certi_cast<AttributeHandleSetImp>()(attrs).getAttributeHandles());

    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Attribute Ownership Acquisition If Available
void
RTI::RTIambassador::
attributeOwnershipAcquisitionIfAvailable(ObjectHandle theObject,
                                         const AttributeHandleSet& desired)
    throw (RTI::ObjectNotKnown,
           RTI::ObjectClassNotPublished,
           RTI::AttributeNotDefined,
           RTI::AttributeNotPublished,
           RTI::FederateOwnsAttributes,
           RTI::AttributeAlreadyBeingAcquired,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.type = Message::ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE ;
    req.setObject(theObject);
    req.setAHS(certi_cast<AttributeHandleSetImp>()(desired).getAttributeHandles());

    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Query Attribute Ownership
void
RTI::RTIambassador::
queryAttributeOwnership(ObjectHandle theObject,
                        AttributeHandle theAttribute)
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, 
	   RTI::SaveInProgress, RTI::ConcurrentAccessAttempted, 
	   RTI::FederateNotExecutionMember, RTI::AttributeNotDefined, 
	   RTI::ObjectNotKnown)
{
    Message req, rep ;

    req.type = Message::QUERY_ATTRIBUTE_OWNERSHIP ;
    req.setObject(theObject);
    req.setAttribute(theAttribute);

    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// 5.16 Is Attribute Owned By Federate
RTI::Boolean
RTI::RTIambassador::isAttributeOwnedByFederate(ObjectHandle theObject,
                                          AttributeHandle theAttribute)
    throw (RTI::ObjectNotKnown,
           RTI::AttributeNotDefined,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.type = Message::IS_ATTRIBUTE_OWNED_BY_FEDERATE ;
    req.setObject(theObject);
    req.setAttribute(theAttribute);

    privateRefs->executeService(&req, &rep);

    return ((strcmp((rep.getTag()).c_str(), "RTI_TRUE") == 0) ? RTI_TRUE : RTI_FALSE); 
}

// ----------------------------------------------------------------------------
// Enable Time Regulation
void
RTI::RTIambassador::enableTimeRegulation(const RTI::FedTime& theFederateTime,
                                    const RTI::FedTime& theLookahead)
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, 
	   RTI::SaveInProgress, RTI::FederateNotExecutionMember, 
	   RTI::ConcurrentAccessAttempted, RTI::InvalidLookahead, 
	   RTI::InvalidFederationTime, RTI::TimeAdvanceAlreadyInProgress, 
	   RTI::EnableTimeRegulationPending, RTI::TimeRegulationAlreadyEnabled)
{
    Message req, rep ;
    req.type = Message::ENABLE_TIME_REGULATION ;
    req.setFedTime(certi_cast<RTIfedTime>()(theFederateTime).getTime());
    req.setLookahead(certi_cast<RTIfedTime>()(theLookahead).getTime());
    req.setBoolean(true);
    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Disable Time Regulation
void
RTI::RTIambassador::disableTimeRegulation()
    throw (RTI::RTIinternalError, 
	   RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::FederateNotExecutionMember, RTI::ConcurrentAccessAttempted, 
	   RTI::TimeRegulationWasNotEnabled)
{
    Message req, rep ;
    req.type = Message::DISABLE_TIME_REGULATION ;
    req.setBoolean(false);
    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Enable Time Constrained
void
RTI::RTIambassador::enableTimeConstrained()
    throw (RTI::TimeConstrainedAlreadyEnabled, //not implemented
           RTI::EnableTimeConstrainedPending, //not implemented
           RTI::TimeAdvanceAlreadyInProgress, //not implemented
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.type = Message::ENABLE_TIME_CONSTRAINED ;
    req.setBoolean(true);
    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Disable Time Constrained
void
RTI::RTIambassador::disableTimeConstrained()
    throw (RTI::TimeConstrainedWasNotEnabled, //not implemented
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.type = Message::DISABLE_TIME_CONSTRAINED ;
    req.setBoolean(false);
    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Time Advance Request
void
RTI::RTIambassador::timeAdvanceRequest(const RTI::FedTime& theTime)
    throw (RTI::TimeAdvanceAlreadyInProgress,
           RTI::FederationTimeAlreadyPassed,
           RTI::InvalidFederationTime,
           RTI::EnableTimeRegulationPending, //not implemented
           RTI::EnableTimeConstrainedPending, //not implemented
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.type = Message::TIME_ADVANCE_REQUEST ;
    req.setFedTime(certi_cast<RTIfedTime>()(theTime).getTime());
    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Time Advance Request Available
void
RTI::RTIambassador::timeAdvanceRequestAvailable(const RTI::FedTime& theTime)
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember, 
	   RTI::EnableTimeConstrainedPending, RTI::EnableTimeRegulationPending, 
	   RTI::TimeAdvanceAlreadyInProgress, RTI::FederationTimeAlreadyPassed, 
	   RTI::InvalidFederationTime)
{
    Message req, rep ;

    req.type = Message::TIME_ADVANCE_REQUEST_AVAILABLE ;
    req.setFedTime(certi_cast<RTIfedTime>()(theTime).getTime());

    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Next Event Request
void
RTI::RTIambassador::nextEventRequest(const RTI::FedTime& theTime)
    throw (RTI::TimeAdvanceAlreadyInProgress,
           RTI::FederationTimeAlreadyPassed,
           RTI::InvalidFederationTime,
           RTI::EnableTimeRegulationPending, //not implemented
           RTI::EnableTimeConstrainedPending, //not implemented
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.type = Message::NEXT_EVENT_REQUEST ;
    req.setFedTime(certi_cast<RTIfedTime>()(theTime).getTime());
    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Next Event Request Available
void
RTI::RTIambassador::nextEventRequestAvailable(const RTI::FedTime& theTime)
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember, 
	   RTI::EnableTimeConstrainedPending, RTI::EnableTimeRegulationPending, 
	   RTI::TimeAdvanceAlreadyInProgress, RTI::FederationTimeAlreadyPassed, 
	   RTI::InvalidFederationTime)
{
    Message req, rep ;

    req.type = Message::NEXT_EVENT_REQUEST_AVAILABLE ;
    req.setFedTime(certi_cast<RTIfedTime>()(theTime).getTime());
    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Flush Queue Request
void
RTI::RTIambassador::flushQueueRequest(const RTI::FedTime& theTime)
    throw (RTI::TimeAdvanceAlreadyInProgress,
           RTI::FederationTimeAlreadyPassed,
           RTI::InvalidFederationTime,
           RTI::EnableTimeRegulationPending, //not implemented
           RTI::EnableTimeConstrainedPending, //not implemented
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    throw RTI::RTIinternalError("Unimplemented Service flushQueueRequest");
    Message req, rep ;

    req.type = Message::FLUSH_QUEUE_REQUEST ;
    req.setFedTime(certi_cast<RTIfedTime>()(theTime).getTime());

    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Enable Asynchronous Delivery
void
RTI::RTIambassador::enableAsynchronousDelivery()
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember, 
	   RTI::AsynchronousDeliveryAlreadyEnabled)
{
    // throw AsynchronousDeliveryAlreadyEnabled("Default value (non HLA)");

    Message req, rep ;

    req.type = Message::ENABLE_ASYNCHRONOUS_DELIVERY ;

    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Disable Asynchronous Delivery
void
RTI::RTIambassador::disableAsynchronousDelivery()
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember, 
	   RTI::AsynchronousDeliveryAlreadyDisabled)
{
    Message req, rep ;

    req.type = Message::DISABLE_ASYNCHRONOUS_DELIVERY ;

    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Query LBTS
void
RTI::RTIambassador::queryLBTS(RTI::FedTime& theTime)
    throw (RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.type = Message::QUERY_LBTS ;
    privateRefs->executeService(&req, &rep);

    certi_cast<RTIfedTime>()(theTime) = rep.getFedTime();
}

// ----------------------------------------------------------------------------
// Query Federate Time
void
RTI::RTIambassador::queryFederateTime(RTI::FedTime& theTime)
    throw (RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.type = Message::QUERY_FEDERATE_TIME ;
    privateRefs->executeService(&req, &rep);

    certi_cast<RTIfedTime>()(theTime) = rep.getFedTime();
}

// ----------------------------------------------------------------------------
// Query Minimum Next Event Time
void
RTI::RTIambassador::queryMinNextEventTime(RTI::FedTime& theTime)
    throw (RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.type = Message::QUERY_MIN_NEXT_EVENT_TIME ;
    privateRefs->executeService(&req, &rep);

    certi_cast<RTIfedTime>()(theTime) = rep.getFedTime();
}

// ----------------------------------------------------------------------------
// Modify Lookahead
void
RTI::RTIambassador::modifyLookahead(const RTI::FedTime& theLookahead)
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember, 
	   RTI::InvalidLookahead)
{
    Message req, rep ;

    req.type = Message::MODIFY_LOOKAHEAD ;
    req.setLookahead(certi_cast<RTIfedTime>()(theLookahead).getTime());

    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Query Lookahead
void
RTI::RTIambassador::queryLookahead(RTI::FedTime &theTime)
    throw (RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.type = Message::QUERY_LOOKAHEAD ;
    privateRefs->executeService(&req, &rep);

    certi_cast<RTIfedTime>()(theTime) = rep.getFederationTimeDelta().getTime();
}

// ----------------------------------------------------------------------------
// Retract
void
RTI::RTIambassador::retract(RTI::EventRetractionHandle handle)
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember, 
	   RTI::InvalidRetractionHandle)
{
    throw RTI::RTIinternalError("Unimplemented Service retract");
    Message req, rep ;

    req.type = Message::RETRACT ;
    req.setEventRetraction(handle);

    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Change Attribute Order Type
void
RTI::RTIambassador::changeAttributeOrderType(ObjectHandle theObject,
                                        const AttributeHandleSet& attrs,
                                        OrderingHandle theType)
    throw (RTI::RTIinternalError, 
	   RTI::RestoreInProgress, RTI::SaveInProgress, RTI::ConcurrentAccessAttempted, 
	   RTI::FederateNotExecutionMember, RTI::InvalidOrderingHandle, 
	   RTI::AttributeNotOwned, RTI::AttributeNotDefined, RTI::ObjectNotKnown)
{
    Message req, rep ;

    req.type = Message::CHANGE_ATTRIBUTE_ORDER_TYPE ;
    req.setObject(theObject);
    req.setOrdering(theType);
    req.setAHS(certi_cast<AttributeHandleSetImp>()(attrs).getAttributeHandles());

    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Change Interaction Order Type
void
RTI::RTIambassador::changeInteractionOrderType(InteractionClassHandle theClass,
                                          OrderingHandle theType)
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, 
	   RTI::SaveInProgress, RTI::ConcurrentAccessAttempted, 
	   RTI::FederateNotExecutionMember, RTI::InvalidOrderingHandle, 
	   RTI::InteractionClassNotPublished, RTI::InteractionClassNotDefined)
{
    Message req, rep ;

    req.type = Message::CHANGE_INTERACTION_ORDER_TYPE ;
    req.setInteractionClass(theClass);
    req.setOrdering(theType);

    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
/** Create a routing region for data distribution management.
    \param space The space handle of the region
    \param nb_extents The number of extents
    \return A Region object, associated with the created region
 */
RTI::Region *
RTI::RTIambassador::createRegion(SpaceHandle space, ULong nb_extents)
    throw (RTI::SpaceNotDefined,
           RTI::InvalidExtents,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;
    req.setType(Message::DDM_CREATE_REGION);
    req.setSpace(space);
    req.setNumber(nb_extents);
    privateRefs->executeService(&req, &rep);
    RTI::Region *region = new RegionImp(rep.getRegion(), space,
				   std::vector<Extent>(nb_extents,
						       Extent(rep.getNumber())));

    assert(region->getNumberOfExtents() == nb_extents);
    return region ;
}

// ----------------------------------------------------------------------------
/** Notify about region modification. Applies the changes done through
    the region services to the RTI.
    \param r The region to commit to the RTI
 */
void
RTI::RTIambassador::notifyAboutRegionModification(Region &r)
    throw (RTI::RegionNotKnown,
           RTI::InvalidExtents,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    try {
	RegionImp &region = dynamic_cast<RegionImp &>(r);
	Debug(D, pdDebug) << "Notify About Region " << region.getHandle()
		   << " Modification" << endl ;
	Message req, rep ;
	
	req.setType(Message::DDM_MODIFY_REGION);
	req.setRegion(region.getHandle());
	req.setExtents(region.getExtents());
	
	privateRefs->executeService(&req, &rep);
	region.commit();
    }
    catch (std::bad_cast) {
	throw RTI::RegionNotKnown("");
    }
    catch (RTI::Exception &e) {
	throw ;
    }
}

// ----------------------------------------------------------------------------
/** Delete region. Correctly destroys the region (through the RTI).
    \attention Always use this function to destroy a region. Do NOT
    use the C++ delete operator.
 */
void
RTI::RTIambassador::deleteRegion(Region *region)
    throw (RTI::RegionNotKnown,
           RTI::RegionInUse,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    if (region == 0) {
        throw RegionNotKnown("");
    }

    Message req, rep ;

    req.setType(Message::DDM_DELETE_REGION);
    try {
	req.setRegion(dynamic_cast<RegionImp *>(region)->getHandle());
    }
    catch (std::bad_cast) {
	throw RegionNotKnown("");
    }
    privateRefs->executeService(&req, &rep);

    delete region ;
}

// ----------------------------------------------------------------------------
// Register Object Instance With Region
ObjectHandle
RTI::RTIambassador::registerObjectInstanceWithRegion(ObjectClassHandle object_class,
                                                const char *tag,
                                                AttributeHandle attrs[],
                                                Region *regions[],
                                                ULong nb)
    throw (RTI::ObjectClassNotDefined,
           RTI::ObjectClassNotPublished,
           RTI::AttributeNotDefined,
           RTI::AttributeNotPublished,
           RTI::RegionNotKnown,
           RTI::InvalidRegionContext,
           RTI::ObjectAlreadyRegistered,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.setType(Message::DDM_REGISTER_OBJECT);
    req.setObjectClass(object_class);
    if ( tag != NULL )
       {
       req.setTag(tag);
       }
    req.setAHS(attrs, nb);
    req.setRegions(build_region_handles(regions, nb));

    privateRefs->executeService(&req, &rep);

    return rep.getObject();
}

// ----------------------------------------------------------------------------
ObjectHandle
RTI::RTIambassador::registerObjectInstanceWithRegion(ObjectClassHandle object_class,
                                                AttributeHandle attrs[],
                                                Region *regions[],
                                                ULong nb)
    throw (RTI::ObjectClassNotDefined,
           RTI::ObjectClassNotPublished,
           RTI::AttributeNotDefined,
           RTI::AttributeNotPublished,
           RTI::RegionNotKnown,
           RTI::InvalidRegionContext,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.setType(Message::DDM_REGISTER_OBJECT);
    req.setObjectClass(object_class);
    req.setAHS(attrs, nb);
    req.setRegions(build_region_handles(regions, nb));

    privateRefs->executeService(&req, &rep);

    return rep.getObject();
}

// ----------------------------------------------------------------------------
/** Associate region for updates. Make attributes of an object
    be updated through a routing region.
    @param region Region to use for updates
    @param object Object to associate to the region
    @param attributes Handles of the involved attributes
    @sa unassociateRegionForUpdates
*/
void
RTI::RTIambassador::associateRegionForUpdates(Region &region,
                                         ObjectHandle object,
                                         const AttributeHandleSet &attributes)
    throw (RTI::ObjectNotKnown,
           RTI::AttributeNotDefined,
           RTI::InvalidRegionContext,
           RTI::RegionNotKnown,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Debug(D, pdDebug) << "+ Associate Region for Updates" << endl ;

    Message req, rep ;
    
    req.type = Message::DDM_ASSOCIATE_REGION ;
    req.setObject(object);
    req.setRegion(get_handle(region));
    req.setAHS(certi_cast<AttributeHandleSetImp>()(attributes).getAttributeHandles());

    privateRefs->executeService(&req, &rep);
    Debug(D, pdDebug) << "- Associate Region for Updates" << endl ;
}

// ----------------------------------------------------------------------------
/** Unassociate region for updates. Make attributes of an object be updated
    through the default region (ie. Declaration Management services)
    @param region Region to unassociate
    @param object Object to unassociate
    @see associateRegionForUpdates
 */
void
RTI::RTIambassador::unassociateRegionForUpdates(Region &region,
                                           ObjectHandle object)
    throw (RTI::ObjectNotKnown,
           RTI::InvalidRegionContext,
           RTI::RegionNotKnown,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Debug(D, pdDebug) << "+ Unassociate Region for Updates" << endl ;
    Message req, rep ;

    req.type = Message::DDM_UNASSOCIATE_REGION ;
    req.setObject(object);
    req.setRegion(get_handle(region));

    privateRefs->executeService(&req, &rep);
    Debug(D, pdDebug) << "- Unassociate Region for Updates" << endl ;
}

// ----------------------------------------------------------------------------
/** Subscribe object class attributes with region.
    @param object_class ObjectClassHandle
    @param region Region to subscribe with
    @param attributes AttributeHandleSet involved in the subscription
    @param passive Boolean
    @sa unsubscribeObjectClassWithRegion
 */
void
RTI::RTIambassador::subscribeObjectClassAttributesWithRegion(
    ObjectClassHandle object_class,
    Region &region,
    const AttributeHandleSet &attributes,
    Boolean passive)
    throw (RTI::ObjectClassNotDefined,
           RTI::AttributeNotDefined,
           RTI::RegionNotKnown,
           RTI::InvalidRegionContext,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Debug(D, pdDebug) << "+ Subscribe Object Class Attributes with Region" << endl ;
    Message req, rep ;

    req.type = Message::DDM_SUBSCRIBE_ATTRIBUTES ;
    req.setObjectClass(object_class);
    req.setRegion(get_handle(region));
    req.setAHS(certi_cast<AttributeHandleSetImp>()(attributes).getAttributeHandles());
    req.setBoolean(passive);

    privateRefs->executeService(&req, &rep);
    Debug(D, pdDebug) << "- Subscribe Object Class Attributes with Region" << endl ;
}

// ----------------------------------------------------------------------------
/** Unsubscribe object class attributes with region.
    @param object_class ObjectClassHandle
    @param region Region to unsubscribe with
    @sa subscribeObjectClassAttributesWithRegion
 */
void
RTI::RTIambassador::unsubscribeObjectClassWithRegion(ObjectClassHandle object_class,
                                                Region &region)
    throw (RTI::ObjectClassNotDefined,
           RTI::RegionNotKnown,
           RTI::ObjectClassNotSubscribed,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Debug(D, pdDebug) << "+ Unsubscribe Object Class " << object_class
	       << " with Region" << endl ;
    Message req, rep ;

    req.type = Message::DDM_UNSUBSCRIBE_ATTRIBUTES ;
    req.setObjectClass(object_class);
    req.setRegion(get_handle(region));

    privateRefs->executeService(&req, &rep);
    Debug(D, pdDebug) << "- Unsubscribe Object Class with Region" << endl ;
}

// ----------------------------------------------------------------------------
// Subscribe Interaction Class With Region
void
RTI::RTIambassador::subscribeInteractionClassWithRegion(InteractionClassHandle ic,
                                                   Region &region,
                                                   RTI::Boolean passive)
    throw (RTI::InteractionClassNotDefined,
           RTI::RegionNotKnown,
           RTI::InvalidRegionContext,
           RTI::FederateLoggingServiceCalls,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.type = Message::DDM_SUBSCRIBE_INTERACTION ;
    req.setInteractionClass(ic);
    req.setRegion(get_handle(region));
    req.setBoolean(passive);

    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// UnSubscribe Interaction Class With Region
void
RTI::RTIambassador::unsubscribeInteractionClassWithRegion(InteractionClassHandle ic,
                                                     Region &region)
    throw (RTI::InteractionClassNotDefined,
           RTI::InteractionClassNotSubscribed,
           RTI::RegionNotKnown,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.type = Message::DDM_UNSUBSCRIBE_INTERACTION ;
    req.setInteractionClass(ic);
    req.setRegion(get_handle(region));

    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Send Interaction With Region
RTI::EventRetractionHandle
RTI::RTIambassador::sendInteractionWithRegion(InteractionClassHandle interaction,
                                         const ParameterHandleValuePairSet &par,
                                         const RTI::FedTime &time,
                                         const char *tag,
                                         const Region &region)
    throw (RTI::InteractionClassNotDefined,
           RTI::InteractionClassNotPublished,
           RTI::InteractionParameterNotDefined,
           RTI::InvalidFederationTime,
           RTI::RegionNotKnown,
           RTI::InvalidRegionContext,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.setType(Message::SEND_INTERACTION);
    req.setInteractionClass(interaction);
    req.setPHVPS(certi_cast<ParameterHandleValuePairSetImp>()(par).getParameterHandleValuePairs());
    req.setFedTime(certi_cast<RTIfedTime>()(time).getTime());
    if ( tag == NULL )
       {
       throw RTI::RTIinternalError ("Calling sendInteractionWithRegion with Tag NULL");
       }
    req.setTag(tag);
    req.setRegion(get_handle(region));

    privateRefs->executeService(&req, &rep);

    return rep.getEventRetraction();
}

// ----------------------------------------------------------------------------
void
RTI::RTIambassador::sendInteractionWithRegion(InteractionClassHandle interaction,
                                         const ParameterHandleValuePairSet &par,
                                         const char *tag,
                                         const Region &region)
    throw (RTI::InteractionClassNotDefined,
           RTI::InteractionClassNotPublished,
           RTI::InteractionParameterNotDefined,
           RTI::RegionNotKnown,
           RTI::InvalidRegionContext,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::SaveInProgress,
           RTI::RestoreInProgress,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.setType(Message::SEND_INTERACTION);
    req.setInteractionClass(interaction);
    req.setPHVPS(certi_cast<ParameterHandleValuePairSetImp>()(par).getParameterHandleValuePairs());
    if ( tag == NULL )
       {
       throw RTI::RTIinternalError ("Calling sendInteractionWithRegion with Tag NULL");
       }
    req.setTag(tag);
    req.setRegion(get_handle(region));

    privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
// Request Class Attribute Value Update With Region
void RTI::RTIambassador::
requestClassAttributeValueUpdateWithRegion(ObjectClassHandle /*object*/,
                                           const AttributeHandleSet &attrs,
                                           const Region &region)
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember, 
	   RTI::RegionNotKnown, RTI::AttributeNotDefined, RTI::ObjectClassNotDefined)
{
    throw RTI::RTIinternalError("unimplemented service requestClassAttributeValueUpdateWithRegion");

    Message req, rep ;
    req.setType(Message::DDM_REQUEST_UPDATE);
    req.setAHS(certi_cast<AttributeHandleSetImp>()(attrs).getAttributeHandles());
    req.setRegion(get_handle(region));
    privateRefs->executeService(&req, &rep);    
}

// ----------------------------------------------------------------------------
/** Get object class handle
    \param theName Name of the object class
 */
ObjectClassHandle
RTI::RTIambassador::getObjectClassHandle(const char *theName)
    throw (RTI::NameNotFound,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::RTIinternalError)
{
    Message req, rep ;

    G.Out(pdGendoc,"enter RTIambassador::getObjectClassHandle");

    req.type = Message::GET_OBJECT_CLASS_HANDLE ;
    req.setName(theName);
    privateRefs->executeService(&req, &rep);

    G.Out(pdGendoc,"exit RTIambassador::getObjectClassHandle");

    return rep.getObjectClass();
}

// ----------------------------------------------------------------------------
/** Get object class name.
    \param handle Handle of the object class
    \return The class name associated with the handle, memory has to
    be freed by the caller.
*/
char *
RTI::RTIambassador::getObjectClassName(ObjectClassHandle handle)
    throw (RTI::ObjectClassNotDefined,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::RTIinternalError)
{
    Message req, rep ;
    req.type = Message::GET_OBJECT_CLASS_NAME ;
    req.setObjectClass(handle);
    privateRefs->executeService(&req, &rep);
    return hla_strdup(rep.getName());
}

// ----------------------------------------------------------------------------
/** Get attribute handle.
    \param theName Name of the attribute
    \param whichClass Handle of the attribute's class
 */
AttributeHandle
RTI::RTIambassador::getAttributeHandle(const char *theName,
                                  ObjectClassHandle whichClass)
    throw (RTI::ObjectClassNotDefined,
           RTI::NameNotFound,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::RTIinternalError)
{
    G.Out(pdGendoc,"enter RTI::RTIambassador::getAttributeHandle");
    Message req, rep ;
    req.type = Message::GET_ATTRIBUTE_HANDLE ;
    req.setName(theName);
    req.setObjectClass(whichClass);
    privateRefs->executeService(&req, &rep);
    G.Out(pdGendoc,"exit  RTI::RTIambassador::getAttributeHandle");
    return rep.getAttribute();
}

// ----------------------------------------------------------------------------
/** Get attribute name.
    \param theHandle Handle of the attribute
    \param whichClass Handle of the attribute's class
 */

char *
RTI::RTIambassador::getAttributeName(AttributeHandle theHandle,
                                ObjectClassHandle whichClass)
    throw (RTI::ObjectClassNotDefined,
           RTI::AttributeNotDefined,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::RTIinternalError)
{
    Message req, rep ;
    req.type = Message::GET_ATTRIBUTE_NAME ;
    req.setAttribute(theHandle);
    req.setObjectClass(whichClass);
    privateRefs->executeService(&req, &rep);
    return hla_strdup(rep.getName());
}

// ----------------------------------------------------------------------------
// Get Interaction Class Handle
InteractionClassHandle
RTI::RTIambassador::getInteractionClassHandle(const char *theName)
    throw (RTI::NameNotFound,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.type = Message::GET_INTERACTION_CLASS_HANDLE ;
    req.setName(theName);

    privateRefs->executeService(&req, &rep);

    return rep.getInteractionClass();
}

// ----------------------------------------------------------------------------
// Get Interaction Class Name
char *
RTI::RTIambassador::getInteractionClassName(InteractionClassHandle theHandle)
    throw (RTI::InteractionClassNotDefined,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.type = Message::GET_INTERACTION_CLASS_NAME ;
    req.setInteractionClass(theHandle);

    privateRefs->executeService(&req, &rep);

    return hla_strdup(rep.getName());
}

// ----------------------------------------------------------------------------
// Get Parameter Handle
ParameterHandle
RTI::RTIambassador::getParameterHandle(const char *theName,
                                  InteractionClassHandle whichClass)
    throw (RTI::InteractionClassNotDefined,
           RTI::NameNotFound,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.type = Message::GET_PARAMETER_HANDLE ;
    req.setName(theName);
    req.setInteractionClass(whichClass);

    privateRefs->executeService(&req, &rep);

    return rep.getParameter();
}

// ----------------------------------------------------------------------------
// Get Parameter Name
char *
RTI::RTIambassador::getParameterName(ParameterHandle theHandle,
                                InteractionClassHandle whichClass)
    throw (RTI::InteractionClassNotDefined,
           RTI::InteractionParameterNotDefined,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::RTIinternalError)
{
    Message req, rep ;

    req.type = Message::GET_PARAMETER_NAME ;
    req.setParameter(theHandle);
    req.setInteractionClass(whichClass);

    privateRefs->executeService(&req, &rep);

    return hla_strdup(rep.getName());
}

// ----------------------------------------------------------------------------
// Get Object Instance Handle
ObjectHandle
RTI::RTIambassador::getObjectInstanceHandle(const char *theName)
    throw (RTI::RTIinternalError, RTI::ConcurrentAccessAttempted, 
   RTI::FederateNotExecutionMember, RTI::ObjectNotKnown)
{
    Message req, rep ;

    req.type = Message::GET_OBJECT_INSTANCE_HANDLE ;
    req.setName(theName);

    privateRefs->executeService(&req, &rep);

    return rep.getObject();
}

// ----------------------------------------------------------------------------
// Get Object Instance Name
char *
RTI::RTIambassador::getObjectInstanceName(ObjectHandle theHandle)
    throw (RTI::RTIinternalError, RTI::ConcurrentAccessAttempted, 
	   RTI::FederateNotExecutionMember, RTI::ObjectNotKnown)
{
    Message req, rep ;

    req.type = Message::GET_OBJECT_INSTANCE_NAME ;
    req.setObject(theHandle);

    privateRefs->executeService(&req, &rep);

    return hla_strdup(rep.getName());
}

// ----------------------------------------------------------------------------
/** Get routing space handle
    \param rs_name Name of the routing space
 */
SpaceHandle
RTI::RTIambassador::getRoutingSpaceHandle(const char *rs_name)
    throw (RTI::NameNotFound,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::RTIinternalError)
{
    Debug(D, pdDebug) << "Get routing space handle: " << rs_name << endl ;
    Message req, rep ;
    req.type = Message::GET_SPACE_HANDLE ;
    req.setName(rs_name);
    privateRefs->executeService(&req, &rep);
    return rep.getSpace();
}

// ----------------------------------------------------------------------------
/** Get routing space name
    \param handle Handle of the routing space
 */
char *
RTI::RTIambassador::getRoutingSpaceName(SpaceHandle handle)
    throw (RTI::SpaceNotDefined,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::RTIinternalError)
{
    Message req, rep ;
    req.type = Message::GET_SPACE_NAME ;
    req.setSpace(handle);
    privateRefs->executeService(&req, &rep);
    return hla_strdup(rep.getName());
}

// ----------------------------------------------------------------------------
/** Get dimension handle
    \param dimension Name of the dimension
    \param space The dimension's routing SpaceHandle
 */
DimensionHandle
RTI::RTIambassador::getDimensionHandle(const char *dimension,
                                  SpaceHandle space)
    throw (RTI::SpaceNotDefined,
           RTI::NameNotFound,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::RTIinternalError)
{
    Message req, rep ;
    req.type = Message::GET_DIMENSION_HANDLE ;
    req.setName(dimension);
    req.setSpace(space);
    privateRefs->executeService(&req, &rep);
    return rep.getDimension();
}

// ----------------------------------------------------------------------------
/** Get dimension name
    \param dimension Handle of the dimension
    \param space The dimension's routing space handle
 */
char *
RTI::RTIambassador::getDimensionName(DimensionHandle dimension,
                                SpaceHandle space)
    throw (RTI::SpaceNotDefined,
           RTI::DimensionNotDefined,
           RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
           RTI::RTIinternalError)
{
    Message req, rep ;
    req.type = Message::GET_DIMENSION_NAME ;
    req.setDimension(dimension);
    req.setSpace(space);
    privateRefs->executeService(&req, &rep);
    return hla_strdup(rep.getName());
}

// ----------------------------------------------------------------------------
/** Get attribute routing space handle
    \param attribute The attribute handle
    \param object_class The attribute's class handle
    \return The associated routing space handle
 */
SpaceHandle
RTI::RTIambassador::getAttributeRoutingSpaceHandle(AttributeHandle attribute,
                                              ObjectClassHandle object_class)
    throw (RTI::ObjectClassNotDefined,
	   RTI::AttributeNotDefined,
           RTI::FederateNotExecutionMember,
	   RTI::ConcurrentAccessAttempted,
           RTI::RTIinternalError)
{
    Message req, rep ;
    req.type = Message::GET_ATTRIBUTE_SPACE_HANDLE ;
    req.setAttribute(attribute);
    req.setObjectClass(object_class);
    privateRefs->executeService(&req, &rep);
    return rep.getSpace();
}

// ----------------------------------------------------------------------------
// Get Object Class
ObjectClassHandle
RTI::RTIambassador::getObjectClass(ObjectHandle theObject)
    throw (RTI::RTIinternalError, RTI::ConcurrentAccessAttempted, 
	   RTI::FederateNotExecutionMember, RTI::ObjectNotKnown)
{
    Message req, rep ;
    req.type = Message::GET_OBJECT_CLASS ;
    req.setObject(theObject);
    privateRefs->executeService(&req, &rep);
    return rep.getObjectClass();
}

// ----------------------------------------------------------------------------
/** Get interaction routing space handle
    \param inter The interaction handle
    \return The associated routing space
 */
SpaceHandle
RTI::RTIambassador::getInteractionRoutingSpaceHandle(InteractionClassHandle inter)
    throw (RTI::InteractionClassNotDefined,
	   RTI::FederateNotExecutionMember,
           RTI::ConcurrentAccessAttempted,
	   RTI::RTIinternalError)
{
    Message req, rep ;
    req.type = Message::GET_INTERACTION_SPACE_HANDLE ;
    req.setInteractionClass(inter);
    this->privateRefs->executeService(&req, &rep);
    return rep.getSpace();
}

// ----------------------------------------------------------------------------
// Get Transportation Handle
RTI::TransportationHandle
RTI::RTIambassador::getTransportationHandle(const char *theName)
    throw (RTI::RTIinternalError, RTI::ConcurrentAccessAttempted, 
	   RTI::FederateNotExecutionMember, RTI::NameNotFound)
{
    Message req, rep ;
    req.type = Message::GET_TRANSPORTATION_HANDLE ;
    req.setName(theName);
    privateRefs->executeService(&req, &rep);
    return rep.getTransportation();
}

// ----------------------------------------------------------------------------
// Get Transportation Name
char *
RTI::RTIambassador::getTransportationName(TransportationHandle theHandle)
    throw (RTI::RTIinternalError, RTI::ConcurrentAccessAttempted, 
	   RTI::FederateNotExecutionMember, RTI::InvalidTransportationHandle)
{
    Message req, rep ;
    req.type = Message::GET_TRANSPORTATION_NAME ;
    req.setTransportation(theHandle);
    privateRefs->executeService(&req, &rep);
    return hla_strdup(rep.getName());
}

// ----------------------------------------------------------------------------
// Get Ordering Handle
RTI::OrderingHandle
RTI::RTIambassador::getOrderingHandle(const char *theName)
    throw (RTI::RTIinternalError, RTI::ConcurrentAccessAttempted, 
	   RTI::FederateNotExecutionMember, RTI::NameNotFound)
{
    Message req, rep ;
    req.type = Message::GET_ORDERING_HANDLE ;
    req.setName(theName);
    privateRefs->executeService(&req, &rep);
    return rep.getOrdering();
}


// ----------------------------------------------------------------------------
// Get Ordering Name
char *
RTI::RTIambassador::getOrderingName(OrderingHandle theHandle)
    throw (RTI::RTIinternalError, RTI::ConcurrentAccessAttempted, 
	   RTI::FederateNotExecutionMember, RTI::InvalidOrderingHandle)
{
    Message req, rep ;
    req.type = Message::GET_ORDERING_NAME ;
    req.setOrdering(theHandle);
    privateRefs->executeService(&req, &rep);
    return hla_strdup(rep.getName());
}

/**
 * Sets the ClassRelevanceAdvisory (CRA) switch to true. The switch 
 * state is hold on the RTIG side. That's why the message
 * ENABLE_CLASS_RELEVANCE_ADVISORY_SWITCH 
 * is transmitted to RTIA. RTIA transmits the message towards RTIG.
 *
 * By default, the CRA switch is true. This causes a delivery of the
 * federate service startRegistrationForObjectClass to a publisher 
 * if there are any new subscribers for the federates published object 
 * classes. If there are no more subscribers a publisher gets the 
 * federate service stopRegistrationForObjectClass.
 *
 * By disabling the CRA switch the federate is no longer informed by 
 * subscriptions to its published object classes, i.e. the federate 
 * services startRegistrationForObjectClass and 
 * stopRegistrationForObjectClass respectively are not invoked.
 * @see disableClassRelevanceAdvisorySwitch()
 */
void
RTI::RTIambassador::enableClassRelevanceAdvisorySwitch()
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember)
{
    Message req, rep ;

    req.type = Message::ENABLE_CLASS_RELEVANCE_ADVISORY_SWITCH ;
    privateRefs->executeService(&req, &rep);
}

/**
 * Sets the ClassRelevanceAdvisory (CRA) switch to false. The switch
 * state is hold on the RTIG side. That's why the message
 * DISABLE_CLASS_RELEVANCE_ADVISORY_SWITCH 
 * is transmitted to RTIA. RTIA transmits the message towards RTIG.
 *
 * By default, the CRA switch is true. This causes a delivery of the
 * federate service startRegistrationForObjectClass to a publisher 
 * if there are any new subscribers for the federates published object 
 * classes. If there are no more subscribers a publisher gets the 
 * federate service stopRegistrationForObjectClass.
 * @see enableClassRelevanceAdvisorySwitch()
 *
 * By disabling the CRA switch the federate is no longer informed by 
 * subscriptions to its published object classes, i.e. the federate 
 * services startRegistrationForObjectClass and 
 * stopRegistrationForObjectClass respectively are not invoked.
 */
void
RTI::RTIambassador::disableClassRelevanceAdvisorySwitch()
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember)
{
    Message req, rep ;

    req.type = Message::DISABLE_CLASS_RELEVANCE_ADVISORY_SWITCH ;
    privateRefs->executeService(&req, &rep);
}

/**
 * Sets the AttributeRelevanceAdvisory (ARA) switch to true. The switch 
 * state is hold on the RTIG side. That's why the message
 * ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH 
 * is transmitted to RTIA. RTIA transmits the message towards RTIG.
 *
 * By default, the ARA switch is false. When enabling the ARA switch
 * the federate is informed by the federate service 
 * turnUpdatesOnForObjectInstance of new object instances within remote 
 * federates actively subscribed to its published attributes. If there
 * are no active subscribers for a set of instance-attributes the federate
 * receives the federate service turnUpdatesOffForObjectInstance. 
 *
 * By disabling the ARA switch the federate is no longer informed by 
 * subscriptions to its published attributes, i.e. the federate 
 * services turnUpdatesOnForObjectInstance and 
 * turnUpdatesOffForObjectInstance respectively are not invoked.
 * @see disableAttributeRelevanceAdvisorySwitch()
 */
void
RTI::RTIambassador::enableAttributeRelevanceAdvisorySwitch()
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember)
{
    Message req, rep ;

    req.type = Message::ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH ;
    privateRefs->executeService(&req, &rep);
}

/**
 * Sets the AttributeRelevanceAdvisory (ARA) switch to false. The switch 
 * state is hold on the RTIG side. That's why the message
 * DISABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH 
 * is transmitted to RTIA. RTIA transmits the message towards RTIG.
 *
 * By default, the ARA switch is false. When enabling the ARA switch
 * the federate is informed by the federate service 
 * turnUpdatesOnForObjectInstance of new object instances within remote 
 * federates actively subscribed to its published attributes. If there
 * are no active subscribers for a set of instance-attributes the federate
 * receives the federate service turnUpdatesOffForObjectInstance. 
 * @see enableAttributeRelevanceAdvisorySwitch()
 *
 * By disabling the ARA switch the federate is no longer informed by 
 * subscriptions to its published attributes, i.e. the federate 
 * services turnUpdatesOnForObjectInstance and 
 * turnUpdatesOffForObjectInstance respectively are not invoked.
 */
void
RTI::RTIambassador::disableAttributeRelevanceAdvisorySwitch()
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember)
{
    Message req, rep ;

    req.type = Message::DISABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH ;
    privateRefs->executeService(&req, &rep);
}

/**
 * Sets the AttributeScopeAdvisory (ASA) switch to true. The switch state 
 * is hold on the RTIG side. That's why the message
 * ENABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH 
 * is transmitted to RTIA. RTIA transmits the message towards RTIG.
 *
 * By default, the ASA switch is false. When enabling the ASA switch
 * the federate is informed by the federate services
 * attributesInScope and attributesOutScope respectively of discovered
 * or registrated but not owned attribute-instances intersecting or
 * leaving its subscription regions.
 *
 * By disabling the ASA switch the federate is no longer informed of
 * changes in attribute-instance scope, i.e. the federate 
 * services attributesInScope and attributesOutScope respectively are 
 * not invoked.
 * @see disableAttributeScopeAdvisorySwitch()
 */
void RTI::RTIambassador::enableAttributeScopeAdvisorySwitch()
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember)
{
    Message req, rep ;

    req.type = Message::ENABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH ;
    privateRefs->executeService(&req, &rep);
}

/**
 * Sets the AttributeScopeAdvisory (ASA) switch to false. The switch state 
 * is hold on the RTIG side. That's why the message
 * DISABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH 
 * is transmitted to RTIA. RTIA transmits the message towards RTIG.
 *
 * By default, the ASA switch is false. When enabling the ASA switch
 * the federate is informed by the federate services
 * attributesInScope and attributesOutScope respectively of discovered
 * or registrated but not owned attribute-instances intersecting or
 * leaving its subscription regions.
 * @see enableAttributeScopeAdvisorySwitch()
 *
 * By disabling the ASA switch the federate is no longer informed of
 * changes in attribute-instance scope, i.e. the federate 
 * services attributesInScope and attributesOutScope respectively are 
 * not invoked.
 */
void
RTI::RTIambassador::disableAttributeScopeAdvisorySwitch()
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember)
{
    Message req, rep ;

    req.type = Message::DISABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH ;
    privateRefs->executeService(&req, &rep);
}

/**
 * Sets the InteractionRelevanceAdvisory (IRA) switch to true. The switch 
 * state is hold on the RTIG side. That's why the message
 * ENABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH 
 * is transmitted to RTIA. RTIA transmits the message towards RTIG.
 *
 * By default, the IRA switch is true. This causes a delivery of the
 * federate service turnInteractionsOn to a publisher if there are 
 * any new subscribers for the federates published interaction 
 * classes. If there are no more subscribers a publisher gets the 
 * federate service turnInteractionsOff(). 
 *
 * By disabling the IRA switch the federate is no longer informed by 
 * subscriptions to its published interaction classes, i.e. the federate 
 * services turnInteractionsOn and turnInteractionsOff respectively are 
 * not invoked.
 * @see disableInteractionRelevanceAdvisorySwitch()
 */
void
RTI::RTIambassador::enableInteractionRelevanceAdvisorySwitch()
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember)
{
    Message req, rep ;

    req.type = Message::ENABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH ;
    privateRefs->executeService(&req, &rep);
}

/**
 * Sets the InteractionRelevanceAdvisory (IRA) switch to false. The switch 
 * state is hold on the RTIG side. That's why the message
 * DISABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH 
 * is transmitted to RTIA. RTIA transmits the message towards RTIG.
 *
 * By default, the IRA switch is true. This causes a delivery of the
 * federate service turnInteractionsOn to a publisher if there are 
 * any new subscribers for the federates published interaction 
 * classes. If there are no more subscribers a publisher gets the 
 * federate service turnInteractionsOff(). 
 * @see enableInteractionRelevanceAdvisorySwitch()
 *
 * By disabling the IRA switch the federate is no longer informed by 
 * subscriptions to its published interaction classes, i.e. the federate 
 * services turnInteractionsOn and turnInteractionsOff respectively are 
 * not invoked.
 */
void
RTI::RTIambassador::disableInteractionRelevanceAdvisorySwitch()
    throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress, 
	   RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember)
{
    Message req, rep ;

    req.type = Message::DISABLE_INTERACTION_RELEVANCE_ADVISORY_SWITCH ;
    privateRefs->executeService(&req, &rep);
}

// $Id: RTIambassador.cc,v 3.109 2009/10/21 20:04:46 erk Exp $
