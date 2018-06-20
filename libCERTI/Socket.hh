// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
// ----------------------------------------------------------------------------

#ifndef CERTI_SOCKET_HH
#define CERTI_SOCKET_HH

#include "Exception.hh"
#include <include/certi.hh>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
typedef u_long in_addr_t;
typedef unsigned short in_port_t;
#else
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
typedef int SOCKET;
#include <unistd.h>
#endif

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 4096
#endif

#include <cerrno>
#include <cstring>
#include <sstream>
#include <string>

namespace certi {

class CERTI_EXPORT Socket {
public:
    typedef unsigned long ByteCount_t;
    virtual ~Socket(){};

    virtual void createConnection(const char* server_name, unsigned int port) = 0;
    virtual void send(const unsigned char*, size_t) = 0;
    virtual void receive(void* Buffer, unsigned long Size) = 0;
    virtual void close() = 0;

    // This method may be used for implementation using Read Buffers,
    // because in that case 'select' system calls are not trustworthy.
    // See Important Note in SocketTCP.hh
    virtual bool isDataReady() const = 0;

    virtual unsigned long returnAdress() const = 0;

    virtual SOCKET returnSocket() = 0;

    /**
     * This function builds a string which represents
     * the provided IPv4 address as a "w.x.y.z".
     * @param[in] addr, the IPv4 address
     * @return the string "w.x.y.z"
     */
    static const std::string addr2string(in_addr_t addr)
    {
        typedef union {
            uint32_t addr;
            uint8_t parts[4];
        } addr_union_t;
        std::stringstream msg;

        addr_union_t uaddr;
        uaddr.addr = (uint32_t) ntohl((uint32_t)(addr));
        msg << "" << static_cast<int>(uaddr.parts[3]) << "." << static_cast<int>(uaddr.parts[2]) << "."
            << static_cast<int>(uaddr.parts[1]) << "." << static_cast<int>(uaddr.parts[0]);
        return msg.str();
    }

    /**
     * This function builds an IP address out of an hostname.
     */
    static void host2addr(const std::string& hostName, in_addr_t& addr)
    {
        addr = 0;
        // get host information from server name
        // this may perform DNS query
        struct hostent* hptr = gethostbyname(hostName.c_str());
        // FIXME we should probably use getaddrinfo instead
        if (NULL == hptr) {
            throw NetworkError("gethostbyname gave NULL answer for hostname <" + hostName + "> with error <"
                               + std::string(strerror(errno))
                               + ">");
        }
        memcpy((void*) &addr, (void*) hptr->h_addr, hptr->h_length);
        ;
    }

    /**
     * Get the IP address corresponding to the first interface of the host.
     */
    static void getMyFirstIPaddr(in_addr_t& addr)
    {
        char name[MAXHOSTNAMELEN + 1];
        /* FIXME gethostname is deprecated
         * we should use getnameinfo and getaddrinfo
         */
        if (0 != gethostname(name, 1024)) {
            throw NetworkError("gethostname FAILED with error <" + std::string(strerror(errno)) + ">");
        }
        Socket::host2addr(name, addr);
    }
};

} // namespace certi

#endif // CERTI_SOCKET_HH
