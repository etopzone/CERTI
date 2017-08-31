#include <PrettyDebug.hh>
#include <PrettyDebug.hh>
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

#include <algorithm>
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>

#include "FedTimeD.hh"
#include "Socket.hh"
#include <NetworkMessage.hh>
#include <PrettyDebug.hh>

#ifdef _WIN32
#include <signal.h>
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
    , my_federations(my_verboseLevel)
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
    
    // Create sub processes
    for(auto i{0}; i<1; ++i) {
        my_request_processors.push_back(std::thread([=] { spawn_request_processor(); } ));
        auto name = "rtig_proc_" + std::to_string(i);
        pthread_setname_np(my_request_processors.back().native_handle(), name.c_str());
        std::cout << name << " == " << my_request_processors.back().get_id() << std::endl;
    }

    fd_set fd;
    Socket* link{nullptr};
    int result{0};

    while (!terminate) {
#if _WIN32
        result = 0;

        while (!result) {
            // Initialize fd_set structure with all opened sockets.
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

        // Wait for an incoming message.
        result = select(fd_max + 1, &fd, nullptr, nullptr, nullptr);

        if ((result == -1) && (errno == EINTR)) {
            break;
        }
#endif

        link = my_socketServer.getActiveSocket(&fd);

        // Is it a message from an already opened connection?
        if (link) {
            Debug(D, pdCom) << "Incoming message on socket " << link->returnSocket() << std::endl;

            do {
                auto linkStillActive = processIncomingMessage(link);

                if (!linkStillActive || !link) {
                    break;
                }
            } while (link->isDataReady());
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

int RTIG::inferTcpPort()
{
    auto tcp_port_s = getenv(tcpPortEnvironmentVariable);
    if (tcp_port_s) {
        return std::stoi(tcp_port_s);
    }
    else {
        return std::stoi(defaultTcpPort);
    }
}

int RTIG::inferUdpPort()
{
    auto udp_port_s = getenv(udpPortEnvironmentVariable);
    if (udp_port_s) {
        return std::stoi(udp_port_s);
    }
    else {
        return std::stoi(defaultUdpPort);
    }
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

bool RTIG::processIncomingMessage(Socket* link) throw(NetworkError)
{
    Debug(G, pdGendoc) << "enter RTIG::processIncomingMessage" << std::endl;

    if (!link) {
        Debug(D, pdError) << "No socket in processIncomingMessage" << std::endl;
        return false;
    }

    auto msg = MessageEvent<NetworkMessage>(link, std::unique_ptr<NetworkMessage>(NM_Factory::receive(link)));

    auto federate = msg.message()->getFederate();
    auto messageType = msg.message()->getMessageType();

    try {
        // This may throw a security error.
        if (messageType != NetworkMessage::Type::DESTROY_FEDERATION_EXECUTION) {
            my_socketServer.checkMessage(link->returnSocket(), msg.message());
        }

        if (messageType == NetworkMessage::Type::CLOSE_CONNEXION) {
            Debug(D, pdTrace) << "Close connection: " << link->returnSocket() << std::endl;
            my_auditServer.startLine(msg.message()->getFederation(),
                                     msg.message()->getFederate(), 
                                     AuditLine::Type(static_cast<std::underlying_type<NetworkMessage::Type>::type>(messageType)));
            my_auditServer.setLevel(AuditLine::Level(9));
            my_auditServer << "Close socket " << static_cast<int>(link->returnSocket());
            
            closeConnection(link, false);
            
            my_auditServer.endLine(AuditLine::Status(Exception::Type::NO_EXCEPTION), " - OK");
            
            Debug(G, pdGendoc) << "exit  RTIG::processIncomingMessage with closed socket" << std::endl;
            return false;
        }

        my_requests.push(std::move(msg));

        Debug(G, pdGendoc) << "exit  RTIG::processIncomingMessage" << std::endl;
        return true;
    }

    // Non RTI specific exception, Client connection problem(internal)
    catch (NetworkError& e) {
        my_auditServer.startLine(msg.message()->getFederation(),
                                 msg.message()->getFederate(), 
                                 AuditLine::Type(static_cast<std::underlying_type<NetworkMessage::Type>::type>(messageType)));
        my_auditServer.setLevel(AuditLine::Level(10));
        my_auditServer.endLine(AuditLine::Status(e.type()), e.reason() + " - NetworkError");
        
        if (!e.reason().empty()) {
            Debug(D, pdExcept) << "Catching Network Error, reason: " << e.reason() << std::endl;
        }
        else {
            Debug(D, pdExcept) << "Catching Network Error, unknown reason" << std::endl;
        }
        
        std::cout << "RTIG dropping client connection " << link->returnSocket() << std::endl;
        closeConnection(link, true);
        return false;
    }

    // Default Handler
    catch (Exception& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << " - " << e.reason() << std::endl;

        // Server Answer(only if an exception is raised)
        auto response = std::unique_ptr<NetworkMessage>(NM_Factory::create(messageType));
        response->setFederate(federate);
        response->setException(e.type(), e.reason());
        
        my_auditServer.startLine(msg.message()->getFederation(),
                                 msg.message()->getFederate(), 
                                 AuditLine::Type(static_cast<std::underlying_type<NetworkMessage::Type>::type>(messageType)));
        my_auditServer.setLevel(AuditLine::Level(10));
        my_auditServer.endLine(AuditLine::Status(e.type()), e.reason() + " - Exception");

        if (link) {
            Debug(G, pdGendoc) << "            processIncomingMessage ===> send exception back to RTIA" << std::endl;
            response->send(link, my_NM_msgBufSend);
            Debug(D, pdExcept) << "RTIG caught exception " << static_cast<long>(e.type())
                               << " and sent it back to federate " << federate << std::endl;
        }

        Debug(G, pdGendoc) << "exit  RTIG::processIncomingMessage" << std::endl;
        return true;
    }
}

void RTIG::openConnection()
{
    try {
        my_socketServer.open();
        Debug(D, pdInit) << "Accepting new connection" << std::endl;
    }
    catch (RTIinternalError& e) {
        Debug(D, pdExcept) << "Error while accepting new connection: " << e.reason() << std::endl;
    }
}

void RTIG::closeConnection(Socket* link, bool emergency)
{
    FederationHandle federation(0);
    FederateHandle federate(0);

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

void RTIG::spawn_request_processor()
{
    Debug(G, pdGendoc) << "Enter " << __PRETTY_FUNCTION__ << std::endl;
    
    MessageProcessor mp{my_auditServer, my_socketServer, my_federationHandles, my_federations};
    
    while (!terminate) {
        try {
//             Debug(D, pdDebug) << "Fetch next request" << std::endl;
            
            auto request = my_requests.popTimeout();
            
            for (auto& response : mp.processEvent(std::move(request))) {
                MessageBuffer responseMessageBuffer;
                // std::cout << "Sending response " << response.message()->getMessageType() << " to " << response.socket() << std::endl;
                response.message()->send(response.socket(), responseMessageBuffer); // send answer to RTIA
            }
        }
        catch (Exception& e) {
            Debug(D, pdExcept) << "Caught Exception: " << e.name() << " - " << e.reason() << std::endl;
        }
        catch (decltype(my_requests)::pop_timeout_exception& e) {
            // Do nothing, this timeout is just here to avoid infinitely blocking at program's termination
//             Debug(D, pdDebug) << "timeout on pop, going on" << std::endl;
        }
    }
    
    Debug(G, pdGendoc) << "Exit " << __PRETTY_FUNCTION__ << std::endl;
}
}
} // namespace certi/rtig

// $Id: RTIG.cc,v 3.73 2014/04/16 12:24:01 erk Exp $
