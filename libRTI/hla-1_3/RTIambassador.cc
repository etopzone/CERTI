// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2014  ONERA
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
// ----------------------------------------------------------------------------

#include "RTI.hh"
#include "fedtime.hh"

#include "RTIambPrivateRefs.hh"
#include "RTItypesImp.hh"

#include "Message.hh"
#include "M_Classes.hh"
#include "PrettyDebug.hh"

#include "config.h"

#ifdef _WIN32
#include <stdio.h>
#include <string.h>
#else 
#include <unistd.h>
#endif
#include <cstdlib>
#include <iostream>
#include <signal.h>
#include <cassert>
#include <cerrno>
#include <typeinfo>
#include <memory>

#ifdef CERTI_REALTIME_EXTENSIONS
#ifdef _WIN32
#error "CERTI Realtime extensions are not currently supported on Windows"
#else
#include <sched.h>
#include <sys/mman.h>
#endif
#endif

using std::cout ;
using std::cerr ;
using std::endl ;

namespace {

static PrettyDebug D("LIBRTI", __FILE__);
static PrettyDebug G("GENDOC",__FILE__) ;

using namespace certi ;

std::vector<certi::RegionHandle>
build_region_handles(RTI::Region **regions, int nb)
throw (RTI::RegionNotKnown)
{
	std::vector<certi::RegionHandle> vect(nb);
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
} /* end of build_region_handles */

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
} /* end of get_handle */

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
	catch (std::bad_alloc&) {
		throw RTI::RTIinternalError("Cannot allocate memory.");
	}
	throw RTI::RTIinternalError("");
} /* end of hla_strdup */

template<typename T>
void
assignAHVToRequest(const std::vector<RTI::AttributeHandle>& AHV, T& request) {
	request.setAttributesSize(AHV.size());
	for (uint32_t i=0;i<AHV.size();++i) {
		request.setAttributes(AHV[i],i);
	}
} /* end of assignAHVToRequest */

template<typename T>
void
assignAHVPSToRequest(const std::vector<std::pair<RTI::AttributeHandle, AttributeValue_t> >& AHVPSv, T& request) {

	uint32_t size = AHVPSv.size();
	request.setAttributesSize(size);
	request.setValuesSize(size);

	for (uint32_t i = 0 ; i < size ; ++i) {
		// FIXME why iterate from the end?
		request.setAttributes(AHVPSv[size-1-i].first,i);
		request.setValues(AHVPSv[size-1-i].second,i);
	}
}  /* end of assignAHVPSToRequest */

template<typename T>
void
assignPHVPSToRequest(const std::vector<std::pair<RTI::ParameterHandle, ParameterValue_t> > &PHVPSv, T& request) {

	uint32_t size = PHVPSv.size();
	request.setParametersSize(size);
	request.setValuesSize(size);

	for (uint32_t i = 0 ; i < size ; ++i) {
		// FIXME why iterate from the end?
		request.setParameters(PHVPSv[size-1-i].first,i);
		request.setValues(PHVPSv[size-1-i].second,i);
	}
} /* end of assignPHVPSToRequest */
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
	PrettyDebug::setFederateName( "LibRTI::UnjoinedFederate" );
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

#ifndef RTIA_CONSOLE_SHOW
	/*
	 * Avoid displaying console window
	 * when running RTIA.
	 */
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
#endif

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
		throw RTI::RTIinternalError(msg.str().c_str());
	}

	privateRefs->handle_RTIA = pi.hProcess;

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
#ifdef _WIN32
		TerminateProcess(privateRefs->handle_RTIA, 0);
#else
		kill( privateRefs->pid_RTIA, SIGINT );
#endif
		throw RTI::RTIinternalError( "Cannot connect to RTIA" );
	}
#endif

	M_Open_Connexion req, rep ;
	req.setVersionMajor(CERTI_Message::versionMajor);
	req.setVersionMinor(CERTI_Message::versionMinor);

	G.Out(pdGendoc,"        ====>executeService OPEN_CONNEXION");
	privateRefs->executeService(&req, &rep);

	G.Out(pdGendoc,"exit  RTIambassador::RTIambassador");
}

// ----------------------------------------------------------------------------
//! Closes processes.
/*! When destructor is called, kill rtia process.
 */
RTI::RTIambassador::~RTIambassador()
throw (RTI::RTIinternalError)
{
	M_Close_Connexion req, rep ;

	G.Out(pdGendoc,"        ====>executeService CLOSE_CONNEXION");
	privateRefs->executeService(&req, &rep);
	// after the response is received, the privateRefs->socketUn must not be used

	//TerminateProcess(privateRefs->handle_RTIA, 0);

	delete privateRefs;
}

// ----------------------------------------------------------------------------
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
RTI::Boolean
RTI::RTIambassador::__tick_kernel(RTI::Boolean multiple, TickTime minimum, TickTime maximum)
throw (RTI::SpecifiedSaveLabelDoesNotExist,
        RTI::ConcurrentAccessAttempted,
        RTI::RTIinternalError)
        {
    M_Tick_Request vers_RTI;
    std::auto_ptr<Message> vers_Fed(NULL);

    // Request callback(s) from the local RTIA
    vers_RTI.setMultiple(multiple);
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
            vers_Fed.reset(M_Factory::receive(privateRefs->socketUn));
        }
        catch (NetworkError &e) {
            std::stringstream msg;
            msg << "NetworkError in tick() while receiving response: " << e._reason;
            throw RTI::RTIinternalError(msg.str().c_str());
        }

        // If the type is TICK_REQUEST, the __tick_kernel() has terminated.
        if (vers_Fed->getMessageType() == Message::TICK_REQUEST) {
            if (vers_Fed->getExceptionType() != e_NO_EXCEPTION) {
                // tick() may only throw exceptions defined in the HLA standard
                // the RTIA is responsible for sending 'allowed' exceptions only
                privateRefs->processException(vers_Fed.get());
            }
            return RTI::Boolean(static_cast<M_Tick_Request*>(vers_Fed.get())->getMultiple());
        }

        try {
            // Otherwise, the RTI calls a FederateAmbassador service.
            privateRefs->callFederateAmbassador(vers_Fed.get());
        }
        catch (RTI::RTIinternalError&) {
            // RTIA awaits TICK_REQUEST_NEXT, terminate the tick() processing
            privateRefs->sendTickRequestStop();
            // ignore the response and re-throw the original exception
            throw;
        }

        try {
            // Request next callback from the RTIA
            M_Tick_Request_Next tick_next;
            tick_next.send(privateRefs->socketUn, privateRefs->msgBufSend);
        }
        catch (NetworkError &e) {
            std::stringstream msg;
            msg << "NetworkError in tick() while sending TICK_REQUEST_NEXT: " << e._reason;

            throw RTI::RTIinternalError(msg.str().c_str());
        }
    } // while(1)
    // This statement may never be reached but it please the compiler
    // for 'non void function with no return'
    return RTI::Boolean(false);
        }

// ----------------------------------------------------------------------------
RTI::Boolean
RTI::RTIambassador::tick(TickTime minimum, TickTime maximum)
throw (RTI::SpecifiedSaveLabelDoesNotExist, RTI::ConcurrentAccessAttempted,
		RTI::RTIinternalError)
		{
	return __tick_kernel(RTI_TRUE, minimum, maximum);
		}

#ifdef CERTI_REALTIME_EXTENSIONS
// ----------------------------------------------------------------------------
void 
RTI::RTIambassador::setPriorityforRTIAProcess(int priority, unsigned int sched_type)
    throw(RTIinternalError) {

#ifdef _WIN32
   throw RTIinternalError("Not Implemented on Windows");
#else
   struct sched_param sparm ;
   int cr ;

   sparm.sched_priority =  priority ;
   cr = sched_setscheduler(privateRefs->pid_RTIA,sched_type,&sparm);
   if (cr !=0) {
      throw RTIinternalError("RTIA process changing priority did not work");
      if (errno==EPERM) {
	 throw RTIinternalError("The calling process has no SU permission for that") ;
	 }
      else if (errno==ESRCH){
	 throw RTIinternalError( "The process id does not exist") ;
	 }
      throw RTIinternalError( "Unknown policy specified") ;
      }
#endif
} 

// ----------------------------------------------------------------------------
void 
RTI::RTIambassador::setAffinityforRTIAProcess(cpu_set_t mask)
    throw(RTIinternalError) {
#ifdef _WIN32
   throw RTIinternalError("Not Implemented on Windows");
#else
if (sched_setaffinity(privateRefs->pid_RTIA, sizeof(cpu_set_t), &mask))
    throw RTIinternalError( "RTIA process Error : sched_setaffinity") ;
#endif  
}
#endif

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
	M_Create_Federation_Execution req, rep ;

	G.Out(pdGendoc,"enter RTIambassador::createFederationExecution");

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
	M_Destroy_Federation_Execution req, rep ;

	G.Out(pdGendoc,"enter RTIambassador::destroyFederationExecution");

	req.setFederationName(executionName);

	G.Out(pdGendoc,"        ====>executeService DESTROY_FEDERATION_EXECUTION");

	privateRefs->executeService(&req, &rep);

	G.Out(pdGendoc,"exit RTIambassador::destroyFederationExecution");

		}

// ----------------------------------------------------------------------------
//! Join Federation Execution.
RTI::FederateHandle
RTI::RTIambassador::joinFederationExecution(const char *yourName,
		const char *executionName,
		FederateAmbassadorPtr fedamb)
throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress,
		RTI::ConcurrentAccessAttempted, RTI::ErrorReadingFED, RTI::CouldNotOpenFED,
		RTI::FederationExecutionDoesNotExist, RTI::FederateAlreadyExecutionMember)
		{
	M_Join_Federation_Execution req, rep ;

	G.Out(pdGendoc,"enter RTIambassador::joinFederationExecution");

	if ( yourName == NULL || strlen(yourName) == 0 )
		throw RTI::RTIinternalError("Incorrect or empty federate name");
	if ( executionName == NULL || strlen(executionName) == 0 )
		throw RTI::RTIinternalError("Incorrect or empty federation name");

	privateRefs->fed_amb = (FederateAmbassador *) fedamb ;

	req.setFederateName(yourName);
	req.setFederationName(executionName);
	G.Out(pdGendoc,"        ====>executeService JOIN_FEDERATION_EXECUTION");
	privateRefs->executeService(&req, &rep);
	G.Out(pdGendoc,"exit  RTIambassador::joinFederationExecution");
	PrettyDebug::setFederateName( "LibRTI::"+std::string(yourName));
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
	M_Resign_Federation_Execution req, rep ;

	G.Out(pdGendoc,"enter RTIambassador::resignFederationExecution");

	req.setResignAction(static_cast<certi::ResignAction>(theAction));

	G.Out(pdGendoc,"        ====>executeService RESIGN_FEDERATION_EXECUTION");
	privateRefs->executeService(&req, &rep);

	G.Out(pdGendoc,"exit RTIambassador::resignFederationExecution");

		}

void
RTI::RTIambassador::registerFederationSynchronizationPoint(const char *label,
		const char *the_tag)
throw (RTI::FederateNotExecutionMember,
		RTI::ConcurrentAccessAttempted,
		RTI::SaveInProgress,
		RTI::RestoreInProgress,
		RTI::RTIinternalError) {
	M_Register_Federation_Synchronization_Point req, rep ;

	G.Out(pdGendoc,"enter RTIambassador::registerFederationSynchronizationPoint for all federates");

	req.setLabel(label);
	// no federate set
	req.setFederateSetSize(0);
	if ( the_tag == NULL )
	{
		throw RTI::RTIinternalError ("Calling registerFederationSynchronizationPoint with Tag NULL");
	}
	req.setTag(the_tag);
	G.Out(pdGendoc,"        ====>executeService REGISTER_FEDERATION_SYNCHRONIZATION_POINT");
	privateRefs->executeService(&req, &rep);

	G.Out(pdGendoc,"exit RTIambassador::registerFederationSynchronizationPoint for all federates");

} /* end of RTI::RTIambassador::registerFederationSynchronizationPoint */

void
RTI::RTIambassador::registerFederationSynchronizationPoint(const char *label,
		const char *theTag,
		const FederateHandleSet &set_of_fed)
throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress,
		RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember) {

	M_Register_Federation_Synchronization_Point req, rep ;

	G.Out(pdGendoc,"enter RTIambassador::registerFederationSynchronizationPoint for some federates");

	req.setLabel(label);
	if ( theTag == NULL )
	{
		throw RTI::RTIinternalError ("Calling registerFederationSynchronizationPoint with Tag NULL");
	}
	req.setTag(theTag);
	// Federate set exists but if size=0 (set empty)
	// We have to send the size even if federate set size is 0
	// (HLA 1.3 compliance to inform ALL federates)

	req.setFederateSetSize(set_of_fed.size());
	for (uint32_t i=0 ; i<set_of_fed.size() ; i++) {
		req.setFederateSet(set_of_fed.getHandle(i),i);
	}

	G.Out(pdGendoc,"        ====>executeService REGISTER_FEDERATION_SYNCHRONIZATION_POINT");
	privateRefs->executeService(&req, &rep);

	G.Out(pdGendoc,"exit RTIambassador::registerFederationSynchronizationPoint for some federates");

} /* end of RTI::RTIambassador::registerFederationSynchronizationPoint */

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
	M_Synchronization_Point_Achieved req, rep ;

	G.Out(pdGendoc,"enter RTIambassador::synchronizationPointAchieved");

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
	M_Request_Federation_Save req, rep ;

	G.Out(pdGendoc,"enter RTIambassador::requestFederationSave with time");

	req.setDate(certi_cast<RTIfedTime>()(theTime).getTime());
	req.setLabel(label);

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
	M_Request_Federation_Save req, rep ;

	G.Out(pdGendoc,"enter RTIambassador::requestFederationSave without time");

	req.setLabel(label);
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
	M_Federate_Save_Begun req, rep ;

	G.Out(pdGendoc,"enter RTIambassador::federateSaveBegun");

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
	M_Federate_Save_Complete req, rep ;

	G.Out(pdGendoc,"enter RTIambassador::federateSaveComplete");
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

	M_Federate_Save_Not_Complete req, rep ;

	G.Out(pdGendoc,"enter RTIambassador::federateSaveNotComplete");
	G.Out(pdGendoc,"      ====>executeService FEDERATE_SAVE_NOT_COMPLETE");
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
	M_Request_Federation_Restore req, rep ;

	G.Out(pdGendoc,"enter RTIambassador::requestFederationRestore");
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
	M_Federate_Restore_Complete req, rep ;

	G.Out(pdGendoc,"enter RTIambassador::federateRestoreComplete");

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
	M_Federate_Restore_Not_Complete req, rep ;

	G.Out(pdGendoc,"enter RTIambassador::federateRestoreNotComplete");
	G.Out(pdGendoc,"      ====>executeService FEDERATE_RESTORE_NOT_COMPLETE");

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
	M_Publish_Object_Class req, rep ;
	const std::vector<unsigned long>& AHSI = certi_cast<AttributeHandleSetImp>()(attributeList).getAttributeHandles();
	G.Out(pdGendoc,"enter RTIambassador::publishObjectClass");

	req.setObjectClass(theClass);
	req.setAttributesSize(AHSI.size());
	for (uint32_t i=0; i<AHSI.size(); ++i) {
		req.setAttributes(AHSI[i],i);
	}
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
	M_Unpublish_Object_Class req, rep ;

	G.Out(pdGendoc,"enter RTIambassador::unpublishObjectClass");
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
	M_Publish_Interaction_Class req, rep ;

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
	M_Unpublish_Interaction_Class req, rep ;

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
	M_Subscribe_Object_Class_Attributes req, rep ;
	G.Out(pdGendoc,"enter RTIambassador::subscribeObjectClassAttributes");
	const std::vector<unsigned long>& AHSI = certi_cast<AttributeHandleSetImp>()(attributeList).getAttributeHandles();

	req.setObjectClass(theClass);
	req.setAttributesSize(AHSI.size());
	for (uint32_t i=0; i<AHSI.size();++i) {
		req.setAttributes(AHSI[i],i);
	}
	req.setActive(active);

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
	M_Unsubscribe_Object_Class req, rep ;

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
	M_Subscribe_Interaction_Class req, rep ;
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
	M_Unsubscribe_Interaction_Class req, rep ;

	req.setInteractionClass(theClass);
	privateRefs->executeService(&req, &rep);
		}

// ----------------------------------------------------------------------------
// Register Object
RTI::ObjectHandle
RTI::RTIambassador::registerObjectInstance(ObjectClassHandle theClass,
		const char *theObjectName)
throw (RTI::RTIinternalError, RTI::RestoreInProgress,
		RTI::SaveInProgress, RTI::ConcurrentAccessAttempted,
		RTI::FederateNotExecutionMember, RTI::ObjectAlreadyRegistered,
		RTI::ObjectClassNotPublished, RTI::ObjectClassNotDefined)
		{
	M_Register_Object_Instance req, rep ;

	req.setObjectName(theObjectName);
	req.setObjectClass(theClass);
	privateRefs->executeService(&req, &rep);

	return rep.getObject();
		}

// ----------------------------------------------------------------------------
RTI::ObjectHandle
RTI::RTIambassador::registerObjectInstance(ObjectClassHandle theClass)
throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress,
		RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember,
		RTI::ObjectClassNotPublished, RTI::ObjectClassNotDefined)
		{
	M_Register_Object_Instance req, rep ;

	req.setObjectClass(theClass);
	privateRefs->executeService(&req, &rep);
	return rep.getObject();
		}

// ----------------------------------------------------------------------------
// Update Attribute Values with time
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
	M_Update_Attribute_Values req, rep ;
	RTI::EventRetractionHandle_s eventRetraction;
	const std::vector<AttributeHandleValuePair_t>& AHVPS = certi_cast<AttributeHandleValuePairSetImp>()(theAttributes).getAttributeHandleValuePairs();
	req.setObject(theObject);
	req.setDate(certi_cast<RTIfedTime>()(theTime).getTime());
	/*
	 * Tolerate NULL tag (DMSO RTI-NG behavior)
	 * by not transmitting the tag in the CERTI message.
	 */
	if ( theTag != NULL) {
		req.setTag(theTag);
	}
	req.setAttributesSize(AHVPS.size());
	req.setValuesSize(AHVPS.size());
	for (uint32_t i=0;i<AHVPS.size();++i) {
		req.setAttributes(AHVPS[i].first,i);
		req.setValues(AHVPS[i].second,i);
	}

	privateRefs->executeService(&req, &rep);
	G.Out(pdGendoc,"return  RTIambassador::updateAttributeValues with time");
	eventRetraction.sendingFederate = rep.getEventRetraction().getSendingFederate();
	eventRetraction.theSerialNumber = rep.getEventRetraction().getSN();
	return eventRetraction;
		}

// ----------------------------------------------------------------------------
void
RTI::RTIambassador::updateAttributeValues(ObjectHandle the_object,
		const AttributeHandleValuePairSet& theAttributes,
		const char *theTag)
throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress,
		RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember,
		RTI::AttributeNotOwned, RTI::AttributeNotDefined, RTI::ObjectNotKnown)
		{
	G.Out(pdGendoc,"enter RTIambassador::updateAttributeValues without time");
	M_Update_Attribute_Values req, rep ;
	const std::vector<AttributeHandleValuePair_t>& AHVPS = certi_cast<AttributeHandleValuePairSetImp>()(theAttributes).getAttributeHandleValuePairs();

	req.setObject(the_object);
	/*
	 * Tolerate NULL tag (DMSO RTI-NG behavior)
	 * by not transmitting the tag in the CERTI message.
	 */
	if ( theTag != NULL) {
		req.setTag(theTag);
	}
	req.setAttributesSize(AHVPS.size());
	req.setValuesSize(AHVPS.size());
	for (uint32_t i=0;i<AHVPS.size();++i) {
		req.setAttributes(AHVPS[i].first,i);
		req.setValues(AHVPS[i].second,i);
	}
	privateRefs->executeService(&req, &rep);
	G.Out(pdGendoc,"exit  RTIambassador::updateAttributeValues without time");
		}

// ----------------------------------------------------------------------------
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
	M_Send_Interaction req, rep ;
	RTI::EventRetractionHandle    eventRetraction;
	req.setInteractionClass(theInteraction);
	req.setDate(certi_cast<RTIfedTime>()(theTime).getTime());
	/*
	 * Tolerate NULL tag (DMSO RTI-NG behavior)
	 * by not transmitting the tag in the CERTI message.
	 */
	if ( theTag != NULL) {
		req.setTag(theTag);
	}

	assignPHVPSToRequest(certi_cast<ParameterHandleValuePairSetImp>()(theParameters).getParameterHandleValuePairs(),req);
	req.setRegion(0);

	privateRefs->executeService(&req, &rep);

	eventRetraction.sendingFederate = rep.getEventRetraction().getSendingFederate();
	eventRetraction.theSerialNumber = rep.getEventRetraction().getSN();
	return eventRetraction;

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

	M_Send_Interaction req, rep ;

	req.setInteractionClass(theInteraction);
	/*
	 * Tolerate NULL tag (DMSO RTI-NG behavior)
	 * by not transmitting the tag in the CERTI message.
	 */
	if ( theTag != NULL) {
		req.setTag(theTag);
	}

	assignPHVPSToRequest(certi_cast<ParameterHandleValuePairSetImp>()(theParameters).getParameterHandleValuePairs(),req);
	req.setRegion(0);
	privateRefs->executeService(&req, &rep);
}

// ----------------------------------------------------------------------------
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
	M_Delete_Object_Instance req, rep ;
    RTI::EventRetractionHandle eventRetraction;

	req.setObject(theObject);
	req.setDate(certi_cast<RTIfedTime>()(theTime).getTime());
	if (theTag == NULL)
	{
		throw RTI::RTIinternalError ("Calling deleteObjectInstance with Tag NULL") ;
	}
	req.setTag(theTag);

	privateRefs->executeService(&req, &rep);
	eventRetraction.sendingFederate = rep.getEventRetraction().getSendingFederate();
		eventRetraction.theSerialNumber = rep.getEventRetraction().getSN();
		return eventRetraction;

		}

// ----------------------------------------------------------------------------
void
RTI::RTIambassador::deleteObjectInstance(ObjectHandle theObject,
		const char *theTag)
throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress,
		RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember,
		RTI::DeletePrivilegeNotHeld, RTI::ObjectNotKnown)
		{
	M_Delete_Object_Instance req, rep ;

	req.setObject(theObject);
	if (theTag == NULL)
	{
		throw RTI::RTIinternalError ("Calling deleteObjectInstance with Tag NULL") ;
	}
	req.setTag(theTag);
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
	M_Local_Delete_Object_Instance req, rep ;

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
	M_Change_Attribute_Transportation_Type req, rep ;
	const std::vector<AttributeHandle>& AHS = certi_cast<AttributeHandleSetImp>()(theAttributes).getAttributeHandles();
	req.setObject(theObject);
	req.setTransportationType(theType);
	req.setAttributesSize(AHS.size());
	for (uint32_t i=0;i<AHS.size();++i) {
		req.setAttributes(AHS[i],i);
	}

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
	M_Change_Interaction_Transportation_Type req, rep ;

	req.setInteractionClass(theClass);
	req.setTransportationType(theType);

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
	M_Request_Object_Attribute_Value_Update req, rep ;

	G.Out(pdGendoc,"enter RTIambassador::requestObjectAttributeValueUpdate");
	req.setObject(theObject);
	assignAHVToRequest(certi_cast<AttributeHandleSetImp>()(ahs).getAttributeHandles(),req);

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

	M_Request_Class_Attribute_Value_Update req, rep ;
	const std::vector<AttributeHandle>& AHSv = certi_cast<AttributeHandleSetImp>()(attrs).getAttributeHandles();
	G.Out(pdGendoc,"enter RTIambassador::requestClassAttributeValueUpdate");
	req.setObjectClass(theClass);
	req.setAttributesSize(attrs.size());
	for (uint32_t i=0;i<attrs.size();++i) {
		req.setAttributes(AHSv[i],i);
	}

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
	M_Unconditional_Attribute_Ownership_Divestiture req, rep ;
	const std::vector<AttributeHandle>& AHSv = certi_cast<AttributeHandleSetImp>()(attrs).getAttributeHandles();
	req.setObject(theObject);

	req.setAttributesSize(attrs.size());
	for (uint32_t i=0;i<attrs.size();++i) {
		req.setAttributes(AHSv[i],i);
	}

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
		RTI::RTIinternalError) {
	M_Negotiated_Attribute_Ownership_Divestiture req, rep ;
	const std::vector<AttributeHandle>& AHSv = certi_cast<AttributeHandleSetImp>()(attrs).getAttributeHandles();

	req.setObject(theObject);
	if (theTag == NULL) {
		throw RTI::RTIinternalError ("Calling negotiatedAttributeOwnershipDivestiture with Tag NULL") ;
	}
	req.setTag(theTag);
	req.setAttributesSize(attrs.size());
	for (uint32_t i=0;i<attrs.size();++i) {
		req.setAttributes(AHSv[i],i);
	}

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
	M_Attribute_Ownership_Acquisition req, rep ;

	req.setObject(theObject);
	if (theTag == NULL)
	{
		throw RTI::RTIinternalError ("Calling attributeOwnershipAcquisition with Tag NULL") ;
	}
	req.setTag(theTag);
	assignAHVToRequest(certi_cast<AttributeHandleSetImp>()(desiredAttributes).getAttributeHandles(),req);
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
	M_Attribute_Ownership_Release_Response req, rep ;
	AttributeHandleSetImp* retval;

	req.setObject(theObject);
	assignAHVToRequest(certi_cast<AttributeHandleSetImp>()(attrs).getAttributeHandles(),req);

	privateRefs->executeService(&req, &rep);

	if (rep.getExceptionType() == e_NO_EXCEPTION) {
		retval = new AttributeHandleSetImp(rep.getAttributesSize());
		for (uint32_t i=0;i<rep.getAttributesSize();++i) {
			retval->add(rep.getAttributes()[i]);
		}
		return retval;
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
	M_Cancel_Negotiated_Attribute_Ownership_Divestiture req, rep ;

	req.setObject(theObject);
	assignAHVToRequest(certi_cast<AttributeHandleSetImp>()(attrs).getAttributeHandles(),req);

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
	M_Cancel_Attribute_Ownership_Acquisition req, rep ;

	req.setObject(theObject);
	assignAHVToRequest(certi_cast<AttributeHandleSetImp>()(attrs).getAttributeHandles(),req);

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
	M_Attribute_Ownership_Acquisition_If_Available req, rep ;

	req.setObject(theObject);
	assignAHVToRequest(certi_cast<AttributeHandleSetImp>()(desired).getAttributeHandles(),req);

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
	M_Query_Attribute_Ownership req, rep ;

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
	M_Is_Attribute_Owned_By_Federate req, rep ;

	req.setObject(theObject);
	req.setAttribute(theAttribute);

	privateRefs->executeService(&req, &rep);

	return (rep.getTag() == "RTI_TRUE") ? RTI_TRUE : RTI_FALSE;
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
	M_Enable_Time_Regulation req, rep ;

	req.setDate(certi_cast<RTIfedTime>()(theFederateTime).getTime());
	req.setLookahead(certi_cast<RTIfedTime>()(theLookahead).getTime());

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
	M_Disable_Time_Regulation req, rep ;

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
	M_Enable_Time_Constrained req, rep ;

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
	M_Disable_Time_Constrained req, rep ;
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
	M_Time_Advance_Request req, rep ;

	req.setDate(certi_cast<RTIfedTime>()(theTime).getTime());
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
	M_Time_Advance_Request_Available req, rep ;

	req.setDate(certi_cast<RTIfedTime>()(theTime).getTime());

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
	M_Next_Event_Request req, rep ;

	req.setDate(certi_cast<RTIfedTime>()(theTime).getTime());
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
	M_Next_Event_Request_Available req, rep ;

	req.setDate(certi_cast<RTIfedTime>()(theTime).getTime());
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
	M_Flush_Queue_Request req, rep ;

	req.setDate(certi_cast<RTIfedTime>()(theTime).getTime());

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

	M_Enable_Asynchronous_Delivery req, rep ;


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
	M_Disable_Asynchronous_Delivery req, rep ;

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
	M_Query_Lbts req, rep ;

	privateRefs->executeService(&req, &rep);

	certi_cast<RTIfedTime>()(theTime) = rep.getDate().getTime();
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
	M_Query_Federate_Time req, rep ;

	privateRefs->executeService(&req, &rep);

	certi_cast<RTIfedTime>()(theTime) = rep.getDate().getTime();
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
	M_Query_Min_Next_Event_Time req, rep ;

	privateRefs->executeService(&req, &rep);

	certi_cast<RTIfedTime>()(theTime) = rep.getDate().getTime();
		}

// ----------------------------------------------------------------------------
// Modify Lookahead
void
RTI::RTIambassador::modifyLookahead(const RTI::FedTime& theLookahead)
throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress,
		RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember,
		RTI::InvalidLookahead)
		{
	M_Modify_Lookahead req, rep ;

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
	M_Query_Lookahead req, rep ;

	// Set Lookahead to a stupid value in the query
	// in order to avoid uninitiliazed value
	req.setLookahead(-1.0);
	privateRefs->executeService(&req, &rep);

	certi_cast<RTIfedTime>()(theTime) = rep.getLookahead();
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
	M_Retract req, rep ;
	EventRetraction event;

	event.setSN(handle.theSerialNumber);
	event.setSendingFederate(handle.sendingFederate);
	req.setEventRetraction(event);

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
	M_Change_Attribute_Order_Type req, rep ;

	req.setObject(theObject);
	req.setOrder(theType);
	assignAHVToRequest(certi_cast<AttributeHandleSetImp>()(attrs).getAttributeHandles(),req);

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
	M_Change_Interaction_Order_Type req, rep ;

	req.setInteractionClass(theClass);
	req.setOrder(theType);

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
	M_Ddm_Create_Region req, rep ;
	req.setSpace(space);
	req.setExtentSetSize(nb_extents);
	privateRefs->executeService(&req, &rep);
	RTI::Region *region = new RegionImp(rep.getRegion(), space,
			std::vector<Extent>(nb_extents,
					Extent(rep.getExtentSetSize())));

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
		M_Ddm_Modify_Region req, rep ;

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

	M_Ddm_Delete_Region req, rep ;

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
RTI::ObjectHandle
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
	M_Ddm_Register_Object req, rep ;

	req.setObjectClass(object_class);
	if ( tag != NULL )
	{
		req.setTag(tag);
	}

	req.setAttributesSize(nb);
	for (uint32_t i=0;i<nb;++i) {
		req.setAttributes(attrs[i],0);
	}
	req.setRegions(build_region_handles(regions, nb));

	privateRefs->executeService(&req, &rep);

	return rep.getObject();
		}

// ----------------------------------------------------------------------------
RTI::ObjectHandle
RTI::RTIambassador::registerObjectInstanceWithRegion(ObjectClassHandle object_class,
		RTI::AttributeHandle attrs[],
		RTI::Region *regions[],
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
	M_Ddm_Register_Object req, rep ;

	req.setObjectClass(object_class);
	req.setAttributesSize(nb);
	for (uint32_t i=0;i<nb;++i) {
		req.setAttributes(attrs[i],0);
	}

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

	M_Ddm_Associate_Region req, rep ;

	req.setObject(object);
	req.setRegion(get_handle(region));
	assignAHVToRequest(certi_cast<AttributeHandleSetImp>()(attributes).getAttributeHandles(),req);

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
	M_Ddm_Unassociate_Region req, rep ;

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
	M_Ddm_Subscribe_Attributes req, rep ;

	req.setObjectClass(object_class);
	req.setRegion(get_handle(region));
	assignAHVToRequest(certi_cast<AttributeHandleSetImp>()(attributes).getAttributeHandles(),req);
	if (passive)
		req.passiveOn();
	else
		req.passiveOff();

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
	M_Ddm_Unsubscribe_Attributes req, rep ;

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
	M_Ddm_Subscribe_Interaction req, rep ;

	req.setInteractionClass(ic);
	req.setRegion(get_handle(region));
	if (passive)
		req.passiveOn();
	else
		req.passiveOff();

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
	M_Ddm_Unsubscribe_Interaction req, rep ;

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
	M_Send_Interaction req, rep ;
    EventRetractionHandle  event;
	req.setInteractionClass(interaction);
	assignPHVPSToRequest(certi_cast<ParameterHandleValuePairSetImp>()(par).getParameterHandleValuePairs(),req);
	req.setDate(certi_cast<RTIfedTime>()(time).getTime());
	if ( tag == NULL )
	{
		throw RTI::RTIinternalError ("Calling sendInteractionWithRegion with Tag NULL");
	}
	req.setTag(tag);
	req.setRegion(get_handle(region));

	privateRefs->executeService(&req, &rep);
	event.theSerialNumber = rep.getEventRetraction().getSN();
	event.sendingFederate = rep.getEventRetraction().getSendingFederate();
	return event;
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
	M_Send_Interaction req, rep ;

	req.setInteractionClass(interaction);
	assignPHVPSToRequest(certi_cast<ParameterHandleValuePairSetImp>()(par).getParameterHandleValuePairs(),req);
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

	M_Ddm_Request_Update req, rep ;
	assignAHVToRequest(certi_cast<AttributeHandleSetImp>()(attrs).getAttributeHandles(),req);
	req.setRegion(get_handle(region));
	privateRefs->executeService(&req, &rep);
		}

// ----------------------------------------------------------------------------
/** Get object class handle
    \param theName Name of the object class
 */
RTI::ObjectClassHandle
RTI::RTIambassador::getObjectClassHandle(const char *theName)
throw (RTI::NameNotFound,
		RTI::FederateNotExecutionMember,
		RTI::ConcurrentAccessAttempted,
		RTI::RTIinternalError)
		{
	M_Get_Object_Class_Handle req, rep ;

	G.Out(pdGendoc,"enter RTIambassador::getObjectClassHandle");

	req.setClassName(theName);
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
	M_Get_Object_Class_Name req, rep ;

	req.setObjectClass(handle);
	privateRefs->executeService(&req, &rep);
	return hla_strdup(rep.getClassName());
		}

// ----------------------------------------------------------------------------
/** Get attribute handle.
    \param theName Name of the attribute
    \param whichClass Handle of the attribute's class
 */
RTI::AttributeHandle
RTI::RTIambassador::getAttributeHandle(const char *theName,
		ObjectClassHandle whichClass)
throw (RTI::ObjectClassNotDefined,
		RTI::NameNotFound,
		RTI::FederateNotExecutionMember,
		RTI::ConcurrentAccessAttempted,
		RTI::RTIinternalError)
		{
	G.Out(pdGendoc,"enter RTI::RTIambassador::getAttributeHandle");
	M_Get_Attribute_Handle req, rep ;

	req.setAttributeName(theName);
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
	M_Get_Attribute_Name req, rep ;

	req.setAttribute(theHandle);
	req.setObjectClass(whichClass);
	privateRefs->executeService(&req, &rep);
	return hla_strdup(rep.getAttributeName());
		}

// ----------------------------------------------------------------------------
// Get Interaction Class Handle
RTI::InteractionClassHandle
RTI::RTIambassador::getInteractionClassHandle(const char *theName)
throw (RTI::NameNotFound,
		RTI::FederateNotExecutionMember,
		RTI::ConcurrentAccessAttempted,
		RTI::RTIinternalError)
		{
	M_Get_Interaction_Class_Handle req, rep ;

	req.setClassName(theName);

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
	M_Get_Interaction_Class_Name req, rep ;

	req.setInteractionClass(theHandle);

	privateRefs->executeService(&req, &rep);

	return hla_strdup(rep.getClassName());
		}

// ----------------------------------------------------------------------------
// Get Parameter Handle
RTI::ParameterHandle
RTI::RTIambassador::getParameterHandle(const char *theName,
		InteractionClassHandle whichClass)
throw (RTI::InteractionClassNotDefined,
		RTI::NameNotFound,
		RTI::FederateNotExecutionMember,
		RTI::ConcurrentAccessAttempted,
		RTI::RTIinternalError)
		{
	M_Get_Parameter_Handle req, rep ;


	req.setParameterName(theName);
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
	M_Get_Parameter_Name req, rep ;

	req.setParameter(theHandle);
	req.setInteractionClass(whichClass);

	privateRefs->executeService(&req, &rep);

	return hla_strdup(rep.getParameterName());
		}

// ----------------------------------------------------------------------------
// Get Object Instance Handle
RTI::ObjectHandle
RTI::RTIambassador::getObjectInstanceHandle(const char *theName)
throw (RTI::RTIinternalError, RTI::ConcurrentAccessAttempted,
		RTI::FederateNotExecutionMember, RTI::ObjectNotKnown)
		{
	M_Get_Object_Instance_Handle req, rep ;

	req.setObjectInstanceName(theName);

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
	M_Get_Object_Instance_Name req, rep ;

	req.setObject(theHandle);

	privateRefs->executeService(&req, &rep);

	return hla_strdup(rep.getObjectInstanceName());
		}

// ----------------------------------------------------------------------------
/** Get routing space handle
    \param rs_name Name of the routing space
 */
RTI::SpaceHandle
RTI::RTIambassador::getRoutingSpaceHandle(const char *rs_name)
throw (RTI::NameNotFound,
		RTI::FederateNotExecutionMember,
		RTI::ConcurrentAccessAttempted,
		RTI::RTIinternalError)
		{
	Debug(D, pdDebug) << "Get routing space handle: " << rs_name << endl ;
	M_Get_Space_Handle req, rep ;

	req.setSpaceName(rs_name);
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
	M_Get_Space_Name req, rep ;

	req.setSpace(handle);
	privateRefs->executeService(&req, &rep);
	return hla_strdup(rep.getSpaceName());
		}

// ----------------------------------------------------------------------------
/** Get dimension handle
    \param dimension Name of the dimension
    \param space The dimension's routing SpaceHandle
 */
RTI::DimensionHandle
RTI::RTIambassador::getDimensionHandle(const char *dimension,
		SpaceHandle space)
throw (RTI::SpaceNotDefined,
		RTI::NameNotFound,
		RTI::FederateNotExecutionMember,
		RTI::ConcurrentAccessAttempted,
		RTI::RTIinternalError)
		{
	M_Get_Dimension_Handle req, rep ;

	req.setDimensionName(dimension);
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
	M_Get_Dimension_Name req, rep ;

	req.setDimension(dimension);
	req.setSpace(space);
	privateRefs->executeService(&req, &rep);
	return hla_strdup(rep.getDimensionName());
		}

// ----------------------------------------------------------------------------
/** Get attribute routing space handle
    \param attribute The attribute handle
    \param object_class The attribute's class handle
    \return The associated routing space handle
 */
RTI::SpaceHandle
RTI::RTIambassador::getAttributeRoutingSpaceHandle(AttributeHandle attribute,
		ObjectClassHandle object_class)
throw (RTI::ObjectClassNotDefined,
		RTI::AttributeNotDefined,
		RTI::FederateNotExecutionMember,
		RTI::ConcurrentAccessAttempted,
		RTI::RTIinternalError)
		{
	M_Get_Attribute_Space_Handle req, rep ;

	req.setAttribute(attribute);
	req.setObjectClass(object_class);
	privateRefs->executeService(&req, &rep);
	return rep.getSpace();
		}

// ----------------------------------------------------------------------------
// Get Object Class
RTI::ObjectClassHandle
RTI::RTIambassador::getObjectClass(ObjectHandle theObject)
throw (RTI::RTIinternalError, RTI::ConcurrentAccessAttempted,
		RTI::FederateNotExecutionMember, RTI::ObjectNotKnown)
		{
	M_Get_Object_Class req, rep ;

	req.setObject(theObject);
	privateRefs->executeService(&req, &rep);
	return rep.getObjectClass();
		}

// ----------------------------------------------------------------------------
/** Get interaction routing space handle
    \param inter The interaction handle
    \return The associated routing space
 */
RTI::SpaceHandle
RTI::RTIambassador::getInteractionRoutingSpaceHandle(InteractionClassHandle inter)
throw (RTI::InteractionClassNotDefined,
		RTI::FederateNotExecutionMember,
		RTI::ConcurrentAccessAttempted,
		RTI::RTIinternalError)
		{
	M_Get_Interaction_Space_Handle req, rep ;

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
	M_Get_Transportation_Handle req, rep ;

	req.setTransportationName(theName);
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
	M_Get_Transportation_Name req, rep ;

	req.setTransportation(theHandle);
	privateRefs->executeService(&req, &rep);
	return hla_strdup(rep.getTransportationName());
		}

// ----------------------------------------------------------------------------
// Get Ordering Handle
RTI::OrderingHandle
RTI::RTIambassador::getOrderingHandle(const char *theName)
throw (RTI::RTIinternalError, RTI::ConcurrentAccessAttempted,
		RTI::FederateNotExecutionMember, RTI::NameNotFound)
		{
	M_Get_Ordering_Handle req, rep ;

	req.setOrderingName(theName);
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
	M_Get_Ordering_Name req, rep ;

	req.setOrdering(theHandle);
	privateRefs->executeService(&req, &rep);
	return hla_strdup(rep.getOrderingName());
		}

void
RTI::RTIambassador::enableClassRelevanceAdvisorySwitch()
throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress,
		RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember)
		{
	M_Enable_Class_Relevance_Advisory_Switch req, rep ;

	privateRefs->executeService(&req, &rep);
		}
void
RTI::RTIambassador::disableClassRelevanceAdvisorySwitch()
throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress,
		RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember)
		{
	M_Disable_Class_Relevance_Advisory_Switch req, rep ;

	privateRefs->executeService(&req, &rep);
		}

void
RTI::RTIambassador::enableAttributeRelevanceAdvisorySwitch()
throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress,
		RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember)
		{
	M_Enable_Attribute_Relevance_Advisory_Switch req, rep ;

	privateRefs->executeService(&req, &rep);
		}

void
RTI::RTIambassador::disableAttributeRelevanceAdvisorySwitch()
throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress,
		RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember)
		{
	M_Disable_Attribute_Relevance_Advisory_Switch req, rep ;

	privateRefs->executeService(&req, &rep);
		}

void RTI::RTIambassador::enableAttributeScopeAdvisorySwitch()
throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress,
		RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember)
		{
	M_Enable_Attribute_Scope_Advisory_Switch req, rep ;

	privateRefs->executeService(&req, &rep);
		}

void
RTI::RTIambassador::disableAttributeScopeAdvisorySwitch()
throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress,
		RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember)
		{
	M_Disable_Attribute_Scope_Advisory_Switch req, rep ;

	privateRefs->executeService(&req, &rep);
		}

void
RTI::RTIambassador::enableInteractionRelevanceAdvisorySwitch()
throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress,
		RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember)
		{
	M_Enable_Interaction_Relevance_Advisory_Switch req, rep ;

	privateRefs->executeService(&req, &rep);
		}

void
RTI::RTIambassador::disableInteractionRelevanceAdvisorySwitch()
throw (RTI::RTIinternalError, RTI::RestoreInProgress, RTI::SaveInProgress,
		RTI::ConcurrentAccessAttempted, RTI::FederateNotExecutionMember)
		{
	M_Disable_Interaction_Relevance_Advisory_Switch req, rep ;

	privateRefs->executeService(&req, &rep);
		}

// $Id: RTIambassador.cc,v 1.2 2014/03/03 16:42:36 erk Exp $
