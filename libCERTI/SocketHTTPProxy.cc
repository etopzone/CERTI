// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This program is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: SocketHTTPProxy.cc,v 3.7 2010/03/14 14:38:27 gotthardp Exp $
// ----------------------------------------------------------------------------

#include "SocketHTTPProxy.hh"
#include "PrettyDebug.hh"

#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cstdio>

namespace certi {

static PrettyDebug D("SOCKHTTP", "(SocketHTTPProxy) - ");
static PrettyDebug G("GENDOC",__FILE__);

/**
 * @page certi_HTTP_proxy Connecting to RTIG via a HTTP tunnel
 *
 * To pass the RTIA--RTIG connection through firewalls, you may use
 * the HTTP tunnel.
 *
 * Federates behind a firewall may be unnable to connect to the RTIG.
 * To connect via a HTTP tunnel
 * -# Set the \p CERTI_HOST and \p CERTI_TCP_PORT environment variables
 *    to RTIG address and port.
 * -# Set the \p CERTI_HTTP_PROXY environment variable to HTTP proxy address
 *    in the form http://host:port.
 * -# Run the federate.
 *
 * If \p CERTI_HTTP_PROXY is not defined, the system-wide \p http_proxy is used.
 * To disable HTTP tunneling, you must unset both environment variables, or set
 * \p CERTI_HTTP_PROXY to an empty string.
 *
 * If the HTTP proxy is directly accessible for the federate (RTIA), you can set
 * the \p CERTI_HTTP_PROXY environment variable to address of the HTTP proxy,
 * e.g. http://proxy.example.com. The default port is 3128.
 *
 * Note: In the HTTP proxy configuration you may need to enable the HTTP CONNECT
 * method for the port number defined in \p CERTI_TCP_PORT. For example, in
 * the /etc/squid/squid.conf you may need to configure
\verbatim
 acl CERTI_ports port 60400   # the value of CERTI_TCP_PORT
 acl CONNECT method CONNECT
 http_access allow CONNECT CERTI_ports
\endverbatim
 *
 * If you cannot access the HTTP proxy directly, you may use SSH port forwarding.
 * The SSH client will listen to a local port and will ask the remote SSH server
 * to open an outgoing connection to the HTTP proxy. It will then forward all
 * traffic between the local port and the HTTP proxy inside the SSH connection.
 *
 * To use SSH port forwarding
 * -# Set the \p CERTI_HTTP_PROXY environment variable to an arbitrary local port
 * number, e.g. http://localhost:8808.
 * -# Establish an SSH connection as follows.
 *
 * On Windows you may use the PuTTY client
 * http://www.chiark.greenend.org.uk/~sgtatham/putty
 *
 * Create a SSH session and select the SSH protocol.
 * Open the Connection -- SSH -- Tunnels configuration. Select "Local", enter
 * chosen arbitrary "Source port" number (e.g. 8808) and the HTTP proxy
 * address as "Destination". Make sure you then click "Add".
 *
 * \image html  "putty-portforwarding.png"
 * \image latex "putty-portforwarding.png" "Putty Portforwarding" width=12cm
 *
 * Most Linux systems have a SSH client installed. Use the \p ssh command.
\verbatim
 ssh -L8808:proxy.example.com:3128 user@hostname
\endverbatim
 *
 */

// ----------------------------------------------------------------------------
SocketHTTPProxy::SocketHTTPProxy() : SocketTCP()
{
}

// ----------------------------------------------------------------------------
SocketHTTPProxy::~SocketHTTPProxy()
{
}

// ----------------------------------------------------------------------------
void
SocketHTTPProxy::createConnection(const char *server_name, unsigned int port)
    throw (NetworkError)
{
    const char *http_proxy;
    // check if proxy is requested
    http_proxy = getenv("CERTI_HTTP_PROXY");
    if (http_proxy == NULL)
        http_proxy = getenv("http_proxy"); // global settings

    if (http_proxy != NULL && *http_proxy != '\0') {
        std::string proxy_address;
        in_port_t proxy_port;

        // skip http://
        const char *protoend = strchr(http_proxy, ':');
        if (protoend == NULL ||
          protoend[1] != '/' || protoend[2] != '/' || protoend[3] == '\0')
        {
            D.Out(pdDebug, "Invalid HTTP proxy URL.");
            throw NetworkError("Invalid HTTP proxy URL.");
        }

        const char *strpaddress = protoend+3;
        const char *strpport = strchr(strpaddress, ':');
        if (strpport) {
            proxy_address.assign(strpaddress, strpport-strpaddress);
            proxy_port = atoi(strpport+1);
        }
        else {
            proxy_address.assign(strpaddress);
            proxy_port = 3128;
        }

        D.Out(pdTrace, "Connect to '%s:%d' via 'http://%s:%d'.",
            server_name, port, proxy_address.c_str(), proxy_port);
        SocketTCP::createConnection(proxy_address.c_str(), proxy_port);
        sendHTTPConnect(server_name, port);
    }
    else {
        D.Out(pdTrace, "Connect to '%s:%d'.", server_name, port);
        SocketTCP::createConnection(server_name, port);
    }
}

// ----------------------------------------------------------------------------
int SocketHTTPProxy::sendHTTPConnect(const char* addr, in_port_t port)
{
    std::stringstream request;
    // build the HTTP request
    request
        << "CONNECT " << addr << ":" << port << " HTTP/1.0\x0D\x0A"
        << "Host: " << addr << ":" << port << "\x0D\x0A"
        << "\x0D\x0A";

    D.Out(pdTrace, "HTTP proxy connection request\n%s<",
        request.str().c_str());
    // send the HTTP reuqest
    send((const unsigned char*)request.str().c_str(), request.str().length());

    size_t received;
    static const size_t maxLineLength = 1024;
    char response[maxLineLength];

    // wait for the HTTP response
    timeoutTCP(5,0);
    // receive the HTTP response line
    received = receiveLine(response, maxLineLength);
    D.Out(pdTrace, "<HTTP PROXY> %s", response);

    char* sCode;
    // the response line must be "HTTP/<version> <code> <reason>"
    if (received < 5 || strncmp(response,"HTTP/",5) != 0 ||
        (sCode = strchr(response, ' ')) == NULL) {
        D.Out(pdDebug, "Unexpected HTTP response.");
        throw NetworkError("Unexpected HTTP response.");
    }

    int iCode = atoi(sCode);
    // the response code must be 2xx
    if (iCode < 200 || iCode >= 300) {
        throw NetworkError(stringize() <<
            "Proxy connection refused: " << response);
    }

    // receive the rest of the HTTP response
    // we wait for an empty line
    do {
        timeoutTCP(5,0);
        // receive the HTTP header
        received = receiveLine(response, maxLineLength);
        D.Out(pdTrace, "<HTTP PROXY> %s", response);
    }
    while (received > 0);

    return 1;
}

// ----------------------------------------------------------------------------
size_t
SocketHTTPProxy::receiveLine(char *buffer, size_t max_size)
    throw (NetworkError, NetworkSignal)
{
    size_t nReceived = 0;
    while (nReceived < max_size-1)
    {
        int result = ::recv(SocketTCP::returnSocket(), buffer+nReceived, 1, 0);
        if (result < 0)
        {
            D.Out(pdExcept, "Error while receiving on TCP socket.");
#ifdef _WIN32
            if (WSAGetLastError() == WSAEINTR)
#else
            if (errno == EINTR)
#endif
                throw NetworkSignal("");
            else {
                perror("TCP Socket(RecevoirTCP) ");
                throw NetworkError("Error while receiving TCP message.");
            }
        }
        else if (result == 0)	{
            D.Out(pdExcept, "TCP connection has been closed by peer.");
            throw NetworkError("Connection closed by client.");
        }

        nReceived += result;
        // update statistics
        RcvdBytesCount += result;

        if (buffer[nReceived-1] == '\x0A') {
            // strip trailing CRLF and terminate the received string
            if(nReceived > 1 && buffer[nReceived-2] == '\x0D') {
                buffer[nReceived-2] = 0;
                return nReceived-2;
            }
            else {
                buffer[nReceived-1] = 0;
                return nReceived-1;
            }
        }
    }

    throw NetworkError("Too large HTTP response received.");
}

} // namespace

// $Id: SocketHTTPProxy.cc,v 3.7 2010/03/14 14:38:27 gotthardp Exp $
