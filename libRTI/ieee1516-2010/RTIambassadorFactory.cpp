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

#include <RTI/RTIambassadorFactory.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#ifndef _WIN32
#include <csignal>
#include <unistd.h>
#endif

#include "PrettyDebug.hh"
#include "RTIambPrivateRefs.h"
#include "RTIambassadorImplementation.h"

#include "M_Classes.hh"

#include "config.h"

rti1516e::RTIambassadorFactory::RTIambassadorFactory() = default;

rti1516e::RTIambassadorFactory::~RTIambassadorFactory() noexcept = default;

namespace {
static PrettyDebug D1516("LIBRTI1516e", __FILE__);
static PrettyDebug G1516("GENDOC1516e", __FILE__);
}

std::auto_ptr<rti1516e::RTIambassador>
rti1516e::RTIambassadorFactory::createRTIambassador() throw(rti1516e::RTIinternalError)
{
    certi::RTI1516ambassador* p_ambassador(new certi::RTI1516ambassador());

    Debug(G1516, pdGendoc) << "enter RTIambassador::RTIambassador" << std::endl;
    PrettyDebug::setFederateName("LibRTI::UnjoinedFederate");
    std::wstringstream msg;

    p_ambassador->p = std::auto_ptr<certi::RTI1516ambassador::Private>(new certi::RTI1516ambassador::Private);

#ifdef _WIN32
    p_ambassador->p->handle_RTIA = (HANDLE) -1;
#endif

    p_ambassador->p->socket_un = std::auto_ptr<certi::SocketUN>(new certi::SocketUN(certi::stIgnoreSignal));

    std::vector<std::string> rtiaList;

    const char* env = getenv("CERTI_RTIA");
    if (env && strlen(env)) {
        rtiaList.push_back(std::string(env));
    }

    env = getenv("CERTI_HOME");
    if (env && strlen(env)) {
        rtiaList.push_back(std::string(env) + "/bin/rtia");
    }

    rtiaList.push_back(PACKAGE_INSTALL_PREFIX "/bin/rtia");
    rtiaList.push_back("rtia");

#if defined(RTIA_USE_TCP)
    int port = p_ambassador->p->socketUn->listenUN();
    if (port == -1) {
        Debug(D1516, pdError) << "Cannot listen to RTIA connection. Abort." << std::endl;
        throw rti1516e::RTIinternalError(L"Cannot listen to RTIA connection");
    }
#else
    int pipeFd = p_ambassador->p->socket_un->socketpair();
    if (pipeFd == -1) {
        Debug(D1516, pdError) << "Cannot get socketpair to RTIA connection. Abort." << std::endl;
        throw rti1516e::RTIinternalError(L"Cannot get socketpair to RTIA connection");
    }
#endif

#ifdef _WIN32
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

#ifndef RTIA_CONSOLE_SHOW
    // Avoid displaying console window when running RTIA.
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
#endif

#if !defined(RTIA_USE_TCP)
    SOCKET newPipeFd;
    if (!DuplicateHandle(GetCurrentProcess(),
                         (HANDLE) pipeFd,
                         GetCurrentProcess(),
                         (HANDLE*) &newPipeFd,
                         0,
                         TRUE, // Inheritable
                         DUPLICATE_SAME_ACCESS)) {
        Debug(D1516, pdError) << "Cannot duplicate socket for RTIA connection. Abort." << std::endl;
        throw rti1516e::RTIinternalError(L"Cannot duplicate socket for RTIA connection. Abort.");
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
        if (CreateProcess(nullptr, // No module name (use command line).
                          (char*) stream.str().c_str(), // Command line.
                          nullptr, // Process handle not inheritable.
                          nullptr, // Thread handle not inheritable.
                          TRUE, // Set handle inheritance to TRUE.
                          0, // No creation flags.
                          nullptr, // Use parent's environment block.
                          nullptr, // Use parent's starting directory.
                          &si, // Pointer to STARTUPINFO structure.
                          &pi)) // Pointer to PROCESS_INFORMATION structure.
        {
            success = true;
            break;
        }
    }
    if (!success) {
        msg << "CreateProcess - GetLastError()=<" << GetLastError() << "> "
            << "Cannot connect to RTIA.exe";
        throw rti1516e::RTIinternalError(msg.str());
    }

    p_ambassador->p->handle_RTIA = pi.hProcess;

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

    switch ((p_ambassador->p->pid_RTIA = fork())) {
    case -1: // fork failed.
        perror("fork");
        // unbock the above blocked signals
        sigprocmask(SIG_SETMASK, &oset, nullptr);
#if !defined(RTIA_USE_TCP)
        close(pipeFd);
#endif
        throw rti1516e::RTIinternalError(L"fork failed in RTIambassador constructor");
        break;

    case 0: // child process (RTIA).
        // close all open filedescriptors except the pipe one
        for (int fdmax = sysconf(_SC_OPEN_MAX), fd = 3; fd < fdmax; ++fd) {
#if !defined(RTIA_USE_TCP)
            if (fd == pipeFd)
                continue;
#endif
            close(fd);
        }
        for (unsigned i = 0; i < rtiaList.size(); ++i) {
            std::stringstream stream;
#if defined(RTIA_USE_TCP)
            stream << port;
            execlp(rtiaList[i].c_str(), rtiaList[i].c_str(), "-p", stream.str().c_str(), nullptr);
#else
            stream << pipeFd;
            execlp(rtiaList[i].c_str(), rtiaList[i].c_str(), "-f", stream.str().c_str(), nullptr);
#endif
        }
        // unbock the above blocked signals
        sigprocmask(SIG_SETMASK, &oset, nullptr);
        msg << "Could not launch RTIA process (execlp): " << strerror(errno) << std::endl
            << "Maybe RTIA is not in search PATH environment.";
        throw rti1516e::RTIinternalError(msg.str().c_str());

    default: // father process (Federe).
        // unbock the above blocked signals
        sigprocmask(SIG_SETMASK, &oset, nullptr);
#if !defined(RTIA_USE_TCP)
        close(pipeFd);
#endif
        break;
    }
#endif

#if defined(RTIA_USE_TCP)
    if (p_ambassador->p->socketUn->acceptUN(10 * 1000) == -1) {
#ifdef _WIN32
        TerminateProcess(p_ambassador->p->handle_RTIA, 0);
#else
        kill(p_ambassador->p->pid_RTIA, SIGINT);
#endif
        throw rti1516e::RTIinternalError(wstringize() << "Cannot connect to RTIA");
    }
#endif

    certi::M_Open_Connexion req, rep;
    req.setVersionMajor(certi::CERTI_Message::versionMajor);
    req.setVersionMinor(certi::CERTI_Message::versionMinor);

    Debug(G1516, pdGendoc) << "        ====>executeService OPEN_CONNEXION" << std::endl;
    p_ambassador->p->executeService(&req, &rep);

    Debug(G1516, pdGendoc) << "exit  RTIambassador::RTIambassador" << std::endl;

    return std::auto_ptr<rti1516e::RTIambassador>(p_ambassador);
}

//} // end namespace rti1516
