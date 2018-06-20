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

#ifndef CERTI_SOCKET_MC_HH
#define CERTI_SOCKET_MC_HH

#include <libHLA/MessageBuffer.hh>
#include "NetworkMessage.hh"
#include "Socket.hh"

namespace certi {

class SocketMC : public Socket {
public:
    int _socket_mc;
    bool _est_init_mc;

    SocketMC();
    virtual ~SocketMC();

    void CreerSocketMC(char* addr, unsigned long port);
    void CreerSocketMC(unsigned long addr, unsigned long port);

    virtual void send(const unsigned char*, size_t);
    virtual void receive(void* Buffer, unsigned long Size);

    SOCKET returnSocket();

    virtual unsigned long returnAdress() const;
    virtual void close();

    void sendMC(NetworkMessage* msg);
    char* receiveMC(NetworkMessage* msg);

private:
    struct sockaddr_in _sin;
    struct sockaddr_in _sin_e;
#ifdef _WIN32
    int _sinlen;
#else
    socklen_t _sinlen;
#endif
    SOCKET _socket_emetteur;
    int _sinlen_e;

    int timeoutMC(int, int);

    int _num_msg;
    MessageBuffer MCMessageBuffer;
};

} // namespace certi

#endif // CERTI_SOCKET_MC_HH
