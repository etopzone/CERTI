// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003, 2004  ONERA
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

#ifndef CERTI_RTIG_HH
#define CERTI_RTIG_HH

// #include <netinet/in.h>
#include <string>
#include <thread>

#include "AuditFile.hh"
#include "BasicMessage.hh"
#include "Exception.hh"
#include "FederationsList.hh"
#include "HandleManager.hh"
#include "MessageBuffer.hh"
#include "NM_Classes.hh" // IWYU pragma: export
// #include "NetworkMessage.hh"
// #include "SecureTCPSocket.hh"
#include "SocketServer.hh"
#include "SocketTCP.hh"
#include "SocketUDP.hh"
#include "certi.hh"

#include "ConcurentQueue.hh"
#include "MessageProcessor.hh"

namespace certi {

class NetworkMessage;
class Socket;

namespace rtig {

/**
     * This class manages the RTIG server, aka RTI Gateway server.
     * Each time a message is received, it is
     * processed by a RTI_processing.cc module.
     *
     * These modules called "processXXX" are divided into two parts :
     * <ul>
     *   <li> one part for treating the received message. </li>
     *   <li> another part generating and sending back a response. </li>
     * </ul>
     */
class RTIG {
public:
    RTIG();
    ~RTIG();

    void execute() throw(NetworkError);

    static void signalHandler(int sig);

    void setVerboseLevel(const int level);
    void setListeningIPAddress(const std::string& hostName) throw(NetworkError);

private:
    static int inferTcpPort();
    static int inferUdpPort();
    
    static bool terminate;

    void createSocketServers();

    /** Process incoming messages.
         *
         * This module works as follows:
         * Each processXXX module processes its own answer and any broadcast needed.
         * processXXX module calling is decided by the ChooseProcessingMethod module.
         * But if an exception occurs while processing a message, the exception is
         * caught by this module. Then a message, similar to the received one is sent
         * on the link. This message only holds the exception.
         * 
         * @return a boolean indicating if the socket is still active after the processing
         */
    bool processIncomingMessage(Socket*) throw(NetworkError);

    void openConnection();

    /** closeConnection
         * 
         * If a connection is closed in emergency, KillFederate will be called on
         * federations attribute to remove all references to this federate.
         */
    void closeConnection(Socket*, bool emergency);

    int my_tcpPort;
    int my_udpPort;
    int my_verboseLevel;
    in_addr_t my_listeningIPAddress;
    HandleManager<Handle> my_federationHandles;
    SocketTCP my_tcpSocketServer;
    SocketUDP my_udpSocketServer;
    SocketServer my_socketServer;
    AuditFile my_auditServer;
    FederationsList my_federations;
    /** The message buffer used to send Network messages */
    MessageBuffer my_NM_msgBufSend;
    /** The message buffer used to receive Network messages */
    MessageBuffer my_NM_msgBufReceive;
    
    MessageProcessor my_processor;
    
    ConcurentQueue<MessageProcessor::Responses> my_responses;
    ConcurentQueue<MessageEvent<NetworkMessage>> my_requests;
    
    std::thread my_response_processor;
    std::vector<std::thread> my_request_processors;
    
    void spawn_response_processor();
    void spawn_request_processor();
    
};
}
} // namespaces

#endif // CERTI_RTIG_HH

// $Id: RTIG.hh,v 3.39 2013/09/24 14:27:58 erk Exp $
