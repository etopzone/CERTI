// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
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

#include <libCERTI/FedTimeD.hh>
#include <libCERTI/NetworkMessage.hh>
#include <libCERTI/PrettyDebug.hh>
#include <libCERTI/Socket.hh>

#ifdef _WIN32
#include <signal.h>
#endif

//#define LOG_MESSAGE_PROCESSING_TIMINGS
#ifdef LOG_MESSAGE_PROCESSING_TIMINGS

#include <chrono>
#include <numeric>

std::map<certi::NetworkMessage::Type, std::vector<std::chrono::nanoseconds>> the_timings;

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

void RTIG::execute()
{
    // create TCP and UDP connections for the RTIG server
    createSocketServers();

    if (my_verboseLevel > 0) {
        std::cout << "CERTI RTIG up and running ..." << std::endl;
    }
    terminate = false;

    fd_set fd;
    Socket* link{nullptr};
    int result{0};
    
#ifdef CERTI_RTIG_USE_EPOLL    
int Epollfd;
my_socketServer.createEpollFd();
int fdtcp = my_tcpSocketServer.returnSocket();
my_socketServer.addElementEpoll(fdtcp);
Epollfd = my_socketServer.getEpollDescriptor();
#endif

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
        
		link = my_socketServer.getActiveSocket(&fd);

        // Is it a message from an already opened connection?
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
                    Debug(D, pdExcept) << "Catching Network Error, unknown reason" << std::endl;
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
#else

#ifdef CERTI_RTIG_USE_SELECT
        FD_ZERO(&fd);
        FD_SET(my_tcpSocketServer.returnSocket(), &fd);

        int fd_max = my_socketServer.addToFDSet(&fd);
        fd_max = std::max(my_tcpSocketServer.returnSocket(), fd_max);

        // Wait for an incoming message.
        result = select(fd_max + 1, &fd, nullptr, nullptr, nullptr);

        if ((result == -1) && (errno == EINTR)) {
            break;
        }
		link = my_socketServer.getActiveSocket(&fd);

        // Is it a message from an already opened connection?
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
                    Debug(D, pdExcept) << "Catching Network Error, unknown reason" << std::endl;
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
#endif
#ifdef CERTI_RTIG_USE_POLL
        std::vector<struct pollfd> SocketVector;
        my_socketServer.constructPollList();
        struct pollfd tcp_server;
        tcp_server.fd = my_tcpSocketServer.returnSocket();
        tcp_server.events = POLLIN;
        my_socketServer.addElementPollList(tcp_server);
        SocketVector = my_socketServer.getSocketVector();
        // blocking call (SHOULD IT BE THIS WAY ??)
        result = ::poll(&SocketVector[0], SocketVector.size(), -1);
        if ((result == -1) && (errno == EINTR)) {
            break;
        }
        for (std::vector<struct pollfd>::iterator it = SocketVector.begin() ; it != SocketVector.end(); ++it)
		{
			short revents = it->revents;
			if (revents == POLLIN)
			{
				link = my_socketServer.getSocketFromFileDescriptor(it->fd);
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
							Debug(D, pdExcept) << "Catching Network Error, unknown reason" << std::endl;
						}
						std::cout << "RTIG dropping client connection " << link->returnSocket() << '.' << std::endl;
						closeConnection(link, true);
						link = nullptr;
					}
				}
				if (my_tcpSocketServer.returnSocket() == it->fd) {
					Debug(D, pdCom) << "New client" << std::endl;
					openConnection();
				}
			}
		}
        my_socketServer.resetSocketVector();
#endif
#ifdef CERTI_RTIG_USE_EPOLL
		struct epoll_event pevents[ 200 ];
		result = epoll_wait( Epollfd, pevents, 200, -1 );
		if ((result == -1) && (errno == EINTR)) 
		{
				break;
		}
		for ( int i = 0; i < result; i++ )
		{
			if (pevents[i].events == EPOLLIN)
			{
				link = my_socketServer.getSocketFromFileDescriptor(pevents[i].data.fd);
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
								Debug(D, pdExcept) << "Catching Network Error, unknown reason" << std::endl;
							}
							std::cout << "RTIG dropping client connection " << link->returnSocket() << '.' << std::endl;
							closeConnection(link, true);
							link = nullptr;
						}
					}
					if (my_tcpSocketServer.returnSocket() == pevents[i].data.fd) {
						Debug(D, pdCom) << "New client" << std::endl;
						openConnection();
					}
			}		
		}

#endif

#endif // #if _WIN32
    }
}

void RTIG::signalHandler(int sig)
{
#ifdef LOG_MESSAGE_PROCESSING_TIMINGS
    std::cerr << "//////////////////////" << std::endl;
    std::cerr << "/////  TIMINGS  //////" << std::endl;
    std::cerr << "//////////////////////" << std::endl;

    for (auto& kv : the_timings) {
        auto message = NM_Factory::create(static_cast<NetworkMessage::Type>(kv.first));
        std::cerr << static_cast<std::underlying_type<NetworkMessage::Type>::type>(kv.first) << " - "
                  << message->getMessageName() << std::endl;
        delete message;

        auto& values = kv.second;
        std::sort(begin(values), end(values));

        std::cerr << "\tcount: " << values.size() << std::endl;
        std::cerr << "\tmin: " << values.front().count() << " ns" << std::endl;
        std::cerr << "\tmax: " << values.back().count() << " ns" << std::endl;
        std::cerr << "\taverage: "
                  << std::accumulate(begin(values), end(values), std::chrono::nanoseconds{}).count() / values.size()
                  << " ns" << std::endl;
        std::cerr << "\tmedian: " << values.at(values.size() / 2).count() << " ns" << std::endl;

#if 0
        std::cerr << "\t\t";
        for (auto& time : values) {
            std::cerr << time.count() << " ";
        }
        std::cerr << std::endl;
#endif
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

void RTIG::setListeningIPAddress(const std::string& hostName)
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

Socket* RTIG::processIncomingMessage(Socket* link)
{
    Debug(G, pdGendoc) << "enter RTIG::processIncomingMessage" << std::endl;

#ifdef LOG_MESSAGE_PROCESSING_TIMINGS
    auto start = std::chrono::high_resolution_clock::now();
#endif

    if (!link) {
        Debug(D, pdError) << "No socket in processIncomingMessage" << std::endl;
        return nullptr;
    }

    auto msg = MessageEvent<NetworkMessage>(link, std::unique_ptr<NetworkMessage>(NM_Factory::receive(link)));

    auto federate = msg.message()->getFederate();
    auto messageType = msg.message()->getMessageType();

    my_auditServer.startLine(
        msg.message()->getFederation(),
        federate,
        AuditLine::Type(static_cast<std::underlying_type<NetworkMessage::Type>::type>(messageType)));

    try {
        // This may throw a security error.
        if (messageType != NetworkMessage::Type::DESTROY_FEDERATION_EXECUTION) {
            my_socketServer.checkMessage(link->returnSocket(), msg.message());
        }

        if (messageType == NetworkMessage::Type::CLOSE_CONNEXION) {
            Debug(D, pdTrace) << "Close connection: " << link->returnSocket() << std::endl;
            my_auditServer.setLevel(AuditLine::Level(9));
            my_auditServer << "Socket " << int(link->returnSocket());
            closeConnection(link, false);
            link = nullptr;
        }
        else {
            auto responses = my_processor.processEvent(std::move(msg));

            Debug(D, pdDebug) << responses.size() << " responses" << std::endl;
            for (auto& response : responses) {
                Debug(D, pdDebug) << "Send back " << response.message()->getMessageName() << " to " << response.sockets().size() << " federates" << std::endl;
                for (const auto& socket: response.sockets()) {
                    if(socket) {
                        Debug(D, pdDebug) << "to " << socket->returnSocket() << std::endl;
                    }
                    else {
                        Debug(D, pdDebug) << "to nullptr" << std::endl;
                    }
                }
                response.message()->send(response.sockets(), my_NM_msgBufSend); // send answer to RTIA
            }
        }

        my_auditServer.endLine(AuditLine::Status(Exception::Type::NO_EXCEPTION), " - OK");

#ifdef LOG_MESSAGE_PROCESSING_TIMINGS
        auto end = std::chrono::high_resolution_clock::now();

        the_timings[messageType].push_back(end - start);
#endif

        Debug(G, pdGendoc) << "exit  RTIG::processIncomingMessage" << std::endl;
        return link;
    }

    // Non RTI specific exception, Client connection problem(internal)
    catch (NetworkError& e) {
        my_auditServer.setLevel(AuditLine::Level(10));
        my_auditServer.endLine(AuditLine::Status(e.type()), e.reason() + " - NetworkError");
        throw;
    }

    // Default Handler
    catch (Exception& e) {
        Debug(D, pdExcept) << "Caught Exception: " << e.name() << " - " << e.reason() << std::endl;
        Debug(G, pdGendoc) << "Caught Exception: " << e.name() << " - " << e.reason() << std::endl;

        // Server Answer(only if an exception is raised)
        auto response = std::unique_ptr<NetworkMessage>(NM_Factory::create(messageType));
        response->setFederate(federate);
        response->setException(e.type(), e.reason());

        my_auditServer.setLevel(AuditLine::Level(10));
        my_auditServer.endLine(AuditLine::Status(e.type()), e.reason() + " - Exception");

        if (link) {
            Debug(G, pdGendoc) << "            processIncomingMessage ===> send exception back to RTIA" << std::endl;
            response->send(link, my_NM_msgBufSend);
            Debug(D, pdExcept) << "RTIG caught exception " << static_cast<long>(e.type())
                               << " and sent it back to federate " << federate << std::endl;
        }

#ifdef LOG_MESSAGE_PROCESSING_TIMINGS
        auto end = std::chrono::high_resolution_clock::now();

        the_timings[messageType].push_back(end - start);
#endif

        Debug(G, pdGendoc) << "exit  RTIG::processIncomingMessage" << std::endl;
        return link;
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
        for (auto& response : my_federations.killFederate(federation, federate)) {
            response.message()->send(response.sockets(), my_NM_msgBufSend);
        }
        Debug(D, pdExcept) << "Federate(" << federation << ", " << federate << ") killed" << std::endl;
    }

    Debug(G, pdGendoc) << "exit  RTIG::closeConnection" << std::endl;
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
}
} // namespace certi/rtig

// $Id: RTIG.cc,v 3.73 2014/04/16 12:24:01 erk Exp $
