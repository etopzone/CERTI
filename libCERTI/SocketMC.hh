// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: SocketMC.hh,v 3.6 2003/11/12 14:40:57 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_SOCKET_MC_HH
#define _CERTI_SOCKET_MC_HH

#include "Socket.hh"
#include "NetworkMessage.hh"

#include <sys/socket.h>
#include <netinet/in.h>

namespace certi {

class SocketMC : public Socket
{
public:
    int _socket_mc ;
    Boolean _est_init_mc ;

    SocketMC();
    virtual ~SocketMC();

    void CreerSocketMC(char *addr, unsigned long port);
    void CreerSocketMC(unsigned long addr, unsigned long port);

    void send(void *Buffer, unsigned long Size)
        throw (NetworkError, NetworkSignal);
    void receive(void *Buffer, unsigned long Size)
        throw (NetworkError, NetworkSignal);

    int returnSocket() const ;
    unsigned long returnAdress() const ;

    int getClass() const { return SOCKET_TYPE_MC ; };

    void close();

    void sendMC(NetworkMessage *msg);
    char *receiveMC(NetworkMessage *msg);

private:
    int portableSelect(fd_set *, struct timeval *);

    struct sockaddr_in _sin ;
    socklen_t _sinlen ;

    int _socket_emetteur ;
    struct sockaddr_in _sin_e ;
    int _sinlen_e ;

    int timeoutMC(int, int);

    int _num_msg ;
};

}

#endif // _CERTI_SOCKET_MC_HH

// $Id: SocketMC.hh,v 3.6 2003/11/12 14:40:57 breholee Exp $
