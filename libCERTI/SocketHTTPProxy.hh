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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
// ----------------------------------------------------------------------------

#ifndef CERTI_SOCKET_HTTP_PROXY_HH
#define CERTI_SOCKET_HTTP_PROXY_HH

#include "SocketTCP.hh"

namespace certi {

class CERTI_EXPORT SocketHTTPProxy : public SocketTCP
{
public :
    SocketHTTPProxy();
    virtual ~SocketHTTPProxy();

    virtual void createConnection(const char *server_name, unsigned int port)
        throw (NetworkError);

protected:
    int sendHTTPConnect(const char* addr, in_port_t port);
    size_t receiveLine(char *Buffer, size_t maxSize) throw (NetworkError, NetworkSignal);
};

} // namespace certi

#endif // CERTI_SOCKET_HTTP_PROXY_HH

