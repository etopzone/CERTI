//#include "RTI/RTI1516.h"

#include <RTI/RTIambassadorFactory.h>
#include <memory>
#include <iostream>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#ifndef _WIN32
#include <csignal>
#endif

#include "PrettyDebug.hh"
#include "RTI1516ambassador.h"

#include "M_Classes.hh"

#include "config.h"

rti1516::RTIambassadorFactory::RTIambassadorFactory()
{
}

rti1516::RTIambassadorFactory::~RTIambassadorFactory()
throw ()
{
}

namespace {
	static PrettyDebug D1516("LIBRTI1516", __FILE__);
	static PrettyDebug G1516("GENDOC1516",__FILE__) ;
}

std::auto_ptr< rti1516::RTIambassador >
rti1516::RTIambassadorFactory::createRTIambassador(std::vector< std::wstring > & args)
throw (BadInitializationParameter,
	   RTIinternalError)
{
	rti1516::RTI1516ambassador* p_ambassador(new rti1516::RTI1516ambassador());

	std::auto_ptr< rti1516::RTIambassador > ap_ambassador(p_ambassador);

	G1516.Out(pdGendoc,"enter RTIambassador::RTIambassador");
	PrettyDebug::setFederateName( "LibRTI::UnjoinedFederate" );
	std::wstringstream msg;

	p_ambassador->privateRefs = new RTI1516ambPrivateRefs();

	p_ambassador->privateRefs->socketUn = new SocketUN(stIgnoreSignal);

	p_ambassador->privateRefs->is_reentrant = false ;

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
	int port = p_ambassador->privateRefs->socketUn->listenUN();
	if (port == -1) {
		D1516.Out( pdError, "Cannot listen to RTIA connection. Abort." );
		throw rti1516::RTIinternalError(L"Cannot listen to RTIA connection" );
	}
#else
	int pipeFd = p_ambassador->privateRefs->socketUn->socketpair();
	if (pipeFd == -1) {
		D1516.Out( pdError, "Cannot get socketpair to RTIA connection. Abort." );
		throw rti1516::RTIinternalError( L"Cannot get socketpair to RTIA connection" );
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
			D1516.Out( pdError, "Cannot duplicate socket for RTIA connection. Abort." );
			throw rti1516::RTIinternalError( L"Cannot duplicate socket for RTIA connection. Abort." );
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
		throw rti1516::RTIinternalError(msg.str());
	}

	p_ambassador->privateRefs->handle_RTIA = pi.hProcess;

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

	switch((p_ambassador->privateRefs->pid_RTIA = fork())) {
	case -1: // fork failed.
		perror("fork");
		// unbock the above blocked signals
		sigprocmask(SIG_SETMASK, &oset, NULL);
#if !defined(RTIA_USE_TCP)
		close(pipeFd);
#endif
		throw rti1516::RTIinternalError(wstringize() << "fork failed in RTIambassador constructor");
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
			<< std::endl
			<< "Maybe RTIA is not in search PATH environment.";
		throw rti1516::RTIinternalError(msg.str().c_str());

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
	if (p_ambassador->privateRefs->socketUn->acceptUN(10*1000) == -1) {
#ifdef _WIN32
		TerminateProcess(p_ambassador->privateRefs->handle_RTIA, 0);
#else
		kill(p_ambassador->privateRefs->pid_RTIA, SIGINT );
#endif
		throw rti1516::RTIinternalError( wstringize() << "Cannot connect to RTIA" );
	}
#endif

	certi::M_Open_Connexion req, rep ;
	req.setVersionMajor(CERTI_Message::versionMajor);
	req.setVersionMinor(CERTI_Message::versionMinor);

	G1516.Out(pdGendoc,"        ====>executeService OPEN_CONNEXION");
	p_ambassador->privateRefs->executeService(&req, &rep);

	G1516.Out(pdGendoc,"exit  RTIambassador::RTIambassador");

	return ap_ambassador;
}

//} // end namespace rti1516
