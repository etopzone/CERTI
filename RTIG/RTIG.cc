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
// ----------------------------------------------------------------------------

#include "RTIG.hh"
// #include <config.h>

#include <algorithm>
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
// #include <sys/select.h>

// #include "NM_Classes.hh"
#include "FedTimeD.hh"
#include "PrettyDebug.hh"
#include "Socket.hh"
#include <NetworkMessage.hh>

#ifdef _WIN32
#include <signal.h>
// #else
// #include <unistd.h>
#endif

// using std::cout;
// using std::endl;
// using std::cerr;

#ifdef LOG_MESSAGE_PROCESSING_TIMINGS

#include <chrono>
#include <numeric>

std::map<std::pair<int, std::string>, std::vector<std::chrono::nanoseconds>> the_timings;

#endif

namespace {
static constexpr auto defaultTcpPort = PORT_TCP_RTIG;
static constexpr auto tcpPortEnvironmentVariable = "CERTI_TCP_PORT";

static constexpr auto defaultUdpPort = PORT_UDP_RTIG;
static constexpr auto udpPortEnvironmentVariable = "CERTI_UDP_PORT";
}

namespace certi {
namespace rtig {

static PrettyDebug D("RTIG", __FILE__);
static PrettyDebug G("GENDOC", __FILE__);

bool RTIG::terminate = false;

RTIG::RTIG()
    : my_tcpPort(inferTcpPort())
    , my_udpPort(inferUdpPort())
    , my_verboseLevel(0)
    , my_listeningIPAddress(0)
    , my_federationHandles(1)
    , my_socketServer(&my_tcpSocketServer, &my_udpSocketServer)
    , my_auditServer(RTIG_AUDIT_FILENAME)
    , my_federations(my_socketServer, my_auditServer, my_verboseLevel)
    , my_processor{my_auditServer, my_socketServer, my_federationHandles, my_federations}
{
    my_NM_msgBufSend.reset();
    my_NM_msgBufReceive.reset();
}

RTIG::~RTIG()
{
    my_tcpSocketServer.close();
    my_udpSocketServer.close();

    std::cout << std::endl << "Stopping RTIG" << std::endl;
}

void RTIG::execute() throw(NetworkError)
{
    // create TCP and UDP connections for the RTIG server
    createSocketServers();

    if (my_verboseLevel > 0) {
        std::cout << "CERTI RTIG up and running ..." << std::endl;
    }
    terminate = false;

    int result;
    fd_set fd;
    Socket* link;
    while (!terminate) {
// Initialize fd_set structure with all opened sockets.
#if _WIN32

        result = 0; // Wait for an incoming message.
        while (!result) {
            FD_ZERO(&fd);
            FD_SET(my_tcpSocketServer.returnSocket(), &fd);

            int highest_fd = my_socketServer.addToFDSet(&fd);
            int server_socket = my_tcpSocketServer.returnSocket();

            // typedef struct timeval {  long tv_sec;  long tv_usec; }
            timeval watchDog;
            watchDog.tv_sec = 0;
            watchDog.tv_usec = 50000L;

            highest_fd = server_socket > highest_fd ? server_socket : highest_fd;

            result = select(highest_fd + 1, &fd, nullptr, nullptr, &watchDog);
            if (result < 0) {
                int test = WSAGetLastError();
                Debug(D, pdExcept) << "Catching Socket Error: " << test << std::endl;
            }

            if (terminate) {
                break;
            }
        }
        if (terminate) {
            break;
        }

        if ((result == -1) && (WSAGetLastError() == WSAEINTR)) {
            break;
        }
#else
        FD_ZERO(&fd);
        FD_SET(my_tcpSocketServer.returnSocket(), &fd);

        int fd_max = my_socketServer.addToFDSet(&fd);
        fd_max = std::max(my_tcpSocketServer.returnSocket(), fd_max);

        result = 0; // Wait for an incoming message.
        result = select(fd_max + 1, &fd, nullptr, nullptr, nullptr);

        if ((result == -1) && (errno == EINTR)) {
            break;
        }
#endif

        // Is it a message from an already opened connection?
        link = my_socketServer.getActiveSocket(&fd);
        if (link) {
            Debug(D, pdCom) << "Incoming message on socket " << link->returnSocket() << std::endl;

            try {
                do {
                    link = processIncomingMessage(link);
                    if (!link) {
                        break;
                    }
                } while (link->isDataReady());
            }
            catch (NetworkError& e) {
                if (!e.reason().empty()) {
                    Debug(D, pdExcept) << "Catching Network Error, reason: " << e.reason() << std::endl;
                }
                else {
                    Debug(D, pdExcept) << "Catching Network Error, no reason string" << std::endl;
                }
                std::cout << "RTIG dropping client connection " << link->returnSocket() << '.' << std::endl;
                closeConnection(link, true);
                link = nullptr;
            }
        }

        // Or on the server socket ?
        if (FD_ISSET(my_tcpSocketServer.returnSocket(), &fd)) {
            Debug(D, pdCom) << "New client" << std::endl;
            openConnection();
        }
    }
}

void RTIG::signalHandler(int sig)
{
#ifdef LOG_MESSAGE_PROCESSING_TIMINGS
    std::cerr << "//////////////////////" << std::endl;
    std::cerr << "/////  TIMINGS  //////" << std::endl;
    std::cerr << "//////////////////////" << std::endl;

    for (auto& kv : the_timings) {
        std::cerr << kv.first.first << " - " << kv.first.second << std::endl;

        auto& values = kv.second;
        std::sort(begin(values), end(values));

        std::cerr << "\tmin: " << values.front().count() << " ns" << std::endl;
        std::cerr << "\tmax: " << values.back().count() << " ns" << std::endl;
        std::cerr << "\taverage: "
                  << std::accumulate(begin(values), end(values), std::chrono::nanoseconds{}).count() / values.size()
                  << " ns" << std::endl;
        std::cerr << "\tmedian: " << values.at(values.size() / 2).count() << " ns" << std::endl;

        std::cerr << "\t\t";
        for (auto& time : values) {
            std::cerr << time.count() << " ";
        }
        std::cerr << std::endl;
    }

    std::cerr << "//////////////////////" << std::endl;
    std::cerr << "/////  TIMINGS  //////" << std::endl;
    std::cerr << "//////////////////////" << std::endl;
#endif

    Debug(D, pdError) << "Received Signal: " << sig << std::endl;

    if (sig == SIGINT) {
        terminate = true;
    }
#ifndef _WIN32
    if (sig == SIGPIPE) {
        std::cout << "Ignoring 'Broken pipe' signal" << std::endl;
    }
#endif
}

void RTIG::setVerboseLevel(const int level)
{
    my_verboseLevel = level;
    my_federations.setVerboseLevel(level);
}

void RTIG::setListeningIPAddress(const std::string& hostName) throw(NetworkError)
{
    Socket::host2addr(hostName, my_listeningIPAddress);
}

void RTIG::createSocketServers()
{
    if (my_listeningIPAddress == 0) {
        // default case on all network interfaces
        my_udpSocketServer.createServer(my_udpPort);
        my_tcpSocketServer.createServer(my_tcpPort);
    }
    else {
        //  1) listen on interface specified on the command line
        my_udpSocketServer.createServer(my_udpPort, my_listeningIPAddress);
        my_tcpSocketServer.createServer(my_tcpPort, my_listeningIPAddress);
    }
}

Socket* RTIG::chooseProcessingMethod(Socket* link, std::unique_ptr<NetworkMessage> msg)
{
    Debug(G, pdGendoc) << "enter RTIG::chooseProcessingMethod type (" << msg->getMessageName() << ")" << std::endl;

    // This may throw a security error.
    if (msg->getMessageType() != NetworkMessage::DESTROY_FEDERATION_EXECUTION) {
        my_socketServer.checkMessage(link->returnSocket(), msg.get());
    }
    
    if (msg->getMessageType() == NetworkMessage::CLOSE_CONNEXION) {
        Debug(D, pdTrace) << "Close connection: " << link->returnSocket() << std::endl;
        my_auditServer.setLevel(9);
        my_auditServer << "Socket " << int(link->returnSocket());
        closeConnection(link, false);
        link = NULL;
    }
    else {
        auto responses = my_processor.processEvent({link, std::move(msg)});
        
        for(auto& response: responses) {
            // send message
//             std::cout << "Sending response " << response.message()->getMessageType() << " to " << response.socket() << std::endl;
            response.message()->send(response.socket(), my_NM_msgBufSend); // send answer to RTIA
        }
    }

    return link; // It may have been set to NULL by closeConnection.
}

Socket* RTIG::processIncomingMessage(Socket* link) throw(NetworkError)
{
    Debug(G, pdGendoc) << "enter RTIG::processIncomingMessage" << std::endl;

#ifdef LOG_MESSAGE_PROCESSING_TIMINGS
    auto start = std::chrono::high_resolution_clock::now();
#endif

    if (!link) {
        Debug(D, pdError) << "No socket in processIncomingMessage" << std::endl;
        return nullptr;
    }

    /* virtual constructor call */
    auto msg = std::unique_ptr<NetworkMessage>(NM_Factory::receive(link));

    // Server Answer(only if an exception is raised)
    //         std::unique_ptr<NetworkMessage> rep();
    auto response = std::unique_ptr<NetworkMessage>(NM_Factory::create(msg->getMessageType()));
    response->setFederate(msg->getFederate());

    my_auditServer.startLine(msg->getFederation(), msg->getFederate(), msg->getMessageType());

// This macro is used to copy any non null exception reason
// string into our buffer(used for Audit purpose).

#define CPY_NOT_NULL(A)                                                                                                \
    {                                                                                                                  \
        if (!A.reason().empty()) {                                                                                     \
            exceptionReason = A.reason();                                                                              \
        }                                                                                                              \
    }

#define PRINT_DEBUG_MESSAGE(A) Debug(D, pdError) << "Caught exception " << A.name() << std::endl

#define BASIC_CATCH(ExceptionType)                                                                                     \
    catch (ExceptionType & e)                                                                                          \
    {                                                                                                                  \
        PRINT_DEBUG_MESSAGE(e);                                                                                        \
        CPY_NOT_NULL(e);                                                                                               \
        response->setException(e.type());                                                                              \
    }

    std::string exceptionReason;

    try {
        link = chooseProcessingMethod(link, std::move(msg));
    }
    BASIC_CATCH(ArrayIndexOutOfBounds)
    BASIC_CATCH(AttributeAlreadyOwned)
    BASIC_CATCH(AttributeAlreadyBeingAcquired)
    BASIC_CATCH(AttributeAlreadyBeingDivested)
    BASIC_CATCH(AttributeDivestitureWasNotRequested)
    BASIC_CATCH(AttributeAcquisitionWasNotRequested)
    BASIC_CATCH(AttributeNotDefined)
    BASIC_CATCH(AttributeNotKnown)
    BASIC_CATCH(AttributeNotOwned)
    BASIC_CATCH(AttributeNotPublished)
    BASIC_CATCH(AttributeNotSubscribed)
    BASIC_CATCH(ConcurrentAccessAttempted)
    BASIC_CATCH(CouldNotDiscover)
    BASIC_CATCH(CouldNotOpenRID)
    BASIC_CATCH(CouldNotOpenFED)
    BASIC_CATCH(CouldNotRestore)
    BASIC_CATCH(DeletePrivilegeNotHeld)
    BASIC_CATCH(ErrorReadingRID)
    BASIC_CATCH(EventNotKnown)
    BASIC_CATCH(FederateAlreadyPaused)
    BASIC_CATCH(FederateAlreadyExecutionMember)
    BASIC_CATCH(FederateDoesNotExist)
    BASIC_CATCH(FederateInternalError)
    BASIC_CATCH(FederateNameAlreadyInUse)
    BASIC_CATCH(FederateNotExecutionMember)
    BASIC_CATCH(FederateNotPaused)
    BASIC_CATCH(FederateNotPublishing)
    BASIC_CATCH(FederateNotSubscribing)
    BASIC_CATCH(FederateOwnsAttributes)
    BASIC_CATCH(FederatesCurrentlyJoined)
    BASIC_CATCH(FederateWasNotAskedToReleaseAttribute)
    BASIC_CATCH(FederationAlreadyPaused)
    BASIC_CATCH(FederationExecutionAlreadyExists)
    BASIC_CATCH(FederationExecutionDoesNotExist)
    BASIC_CATCH(FederationNotPaused)
    BASIC_CATCH(FederationTimeAlreadyPassed)
    BASIC_CATCH(IDsupplyExhausted)
    BASIC_CATCH(InteractionClassNotDefined)
    BASIC_CATCH(InteractionClassNotKnown)
    BASIC_CATCH(InteractionClassNotPublished)
    BASIC_CATCH(InteractionParameterNotDefined)
    BASIC_CATCH(InteractionParameterNotKnown)
    BASIC_CATCH(InvalidDivestitureCondition)
    BASIC_CATCH(InvalidExtents)
    BASIC_CATCH(InvalidFederationTime)
    BASIC_CATCH(InvalidFederationTimeDelta)
    BASIC_CATCH(InvalidObjectHandle)
    BASIC_CATCH(InvalidOrderingHandle)
    BASIC_CATCH(InvalidResignAction)
    BASIC_CATCH(InvalidRetractionHandle)
    BASIC_CATCH(InvalidRoutingSpace)
    BASIC_CATCH(InvalidTransportationHandle)
    BASIC_CATCH(MemoryExhausted)
    BASIC_CATCH(NameNotFound)
    BASIC_CATCH(NoPauseRequested)
    BASIC_CATCH(NoResumeRequested)
    BASIC_CATCH(ObjectClassNotDefined)
    BASIC_CATCH(ObjectClassNotKnown)
    BASIC_CATCH(ObjectClassNotPublished)
    BASIC_CATCH(ObjectClassNotSubscribed)
    BASIC_CATCH(ObjectNotKnown)
    BASIC_CATCH(ObjectAlreadyRegistered)
    BASIC_CATCH(RegionNotKnown)
    BASIC_CATCH(RestoreInProgress)
    BASIC_CATCH(RestoreNotRequested)
    BASIC_CATCH(SaveInProgress)
    BASIC_CATCH(SaveNotInitiated)
    BASIC_CATCH(SpaceNotDefined)
    BASIC_CATCH(SpecifiedSaveLabelDoesNotExist)
    BASIC_CATCH(TimeAdvanceAlreadyInProgress)
    BASIC_CATCH(TimeAdvanceWasNotInProgress)
    BASIC_CATCH(TooManyIDsRequested)
    BASIC_CATCH(UnableToPerformSave)
    BASIC_CATCH(UnimplementedService)
    BASIC_CATCH(UnknownLabel)
    BASIC_CATCH(ValueCountExceeded)
    BASIC_CATCH(ValueLengthExceeded)

    catch (RTIinternalError& e)
    {
        if (e.reason().empty()) {
            PRINT_DEBUG_MESSAGE(e);
        }
        else {
            Debug(D, pdExcept) << "Caught Exception: " << e.name() << ", " << e.reason() << std::endl;
        }
        CPY_NOT_NULL(e);
        response->setException(Exception::Type::RTIinternalError);
    }

    catch (SecurityError& e)
    {
        std::cout << std::endl << "Security Error : " << e.reason() << std::endl;
        CPY_NOT_NULL(e);
        response->setException(Exception::Type::SecurityError);
    }

    // Non RTI specific exception, Client connection problem(internal)
    catch (NetworkError& e)
    {
        exceptionReason = " - NetworkError";
        my_auditServer.endLine(static_cast<unsigned short>(response->getException()), exceptionReason);
        throw e;
    }

    // Default Handler
    catch (Exception& e)
    {
        Debug(D, pdExcept) << "Unknown Exception: " << e.name() << std::endl;
        CPY_NOT_NULL(e);
        response->setException(Exception::Type::RTIinternalError);
    }

    // buffer may contain an exception reason. If not, set it to OK
    // or Exception
    if (exceptionReason.empty()) {
        if (response->getException() == Exception::Type::NO_EXCEPTION) {
            exceptionReason = " - OK";
        }
        else {
            exceptionReason = " - Exception";
        }
    }

    my_auditServer.endLine(static_cast<unsigned short>(response->getException()), exceptionReason);
    if (link == nullptr) {
        return link;
    }

    /* FIXME ***/
    if (response->getException() != Exception::Type::NO_EXCEPTION) {
        Debug(G, pdGendoc) << "            processIncomingMessage ===> write on exception to RTIA" << std::endl;
        response->send(link, my_NM_msgBufSend);
        Debug(D, pdExcept) << "RTIG catched exception " << static_cast<long>(response->getException())
                           << " and sent it back to federate " << response->getFederate() << std::endl;
    }

#ifdef LOG_MESSAGE_PROCESSING_TIMINGS
    auto end = std::chrono::high_resolution_clock::now();

    the_timings[std::make_pair(msg->getMessageType(), msg->getMessageName())].push_back(end - start);
#endif

    Debug(G, pdGendoc) << "exit  RTIG::processIncomingMessage" << std::endl;
    return link;
}

void RTIG::openConnection()
{
    try {
        my_socketServer.open();
    }
    catch (RTIinternalError& e) {
        Debug(D, pdExcept) << "Error while accepting new connection: " << e.reason() << std::endl;
    }

    Debug(D, pdInit) << "Accepting new connection" << std::endl;
}

void RTIG::closeConnection(Socket* link, bool emergency)
{
    Handle federation;
    FederateHandle federate;

    Debug(G, pdGendoc) << "enter RTIG::closeConnection" << std::endl;
    try {
        my_socketServer.close(link->returnSocket(), federation, federate);
    }
    catch (RTIinternalError& e) {
        Debug(D, pdError) << "Connection not found while trying to close it" << std::endl;
    }

    if (emergency) {
        Debug(D, pdExcept) << "Killing Federate(" << federation << ", " << federate << ")..." << std::endl;
        my_federations.killFederate(federation, federate);
        Debug(D, pdExcept) << "Federate(" << federation << ", " << federate << ") killed" << std::endl;
    }

    Debug(G, pdGendoc) << "exit  RTIG::closeConnection" << std::endl;
}

int RTIG::inferTcpPort() const
{
    auto tcp_port_s = getenv(tcpPortEnvironmentVariable);
    if (tcp_port_s) {
        return std::stoi(tcp_port_s);
    }
    else {
        return std::stoi(defaultTcpPort);
    }
}

int RTIG::inferUdpPort() const
{
    auto udp_port_s = getenv(udpPortEnvironmentVariable);
    if (udp_port_s) {
        return std::stoi(udp_port_s);
    }
    else {
        return std::stoi(defaultUdpPort);
    }
}
}
} // namespace certi/rtig

// $Id: RTIG.cc,v 3.73 2014/04/16 12:24:01 erk Exp $
