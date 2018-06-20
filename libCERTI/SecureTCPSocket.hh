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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: SecureTCPSocket.hh,v 3.10 2009/11/24 21:44:47 erk Exp $
// ----------------------------------------------------------------------------

#ifndef CERTI_SECURE_TCP_SOCKET_HH
#define CERTI_SECURE_TCP_SOCKET_HH

#include "GSSAPIHandler.hh"
#include "SocketTCP.hh"
#include <include/certi.hh>

namespace certi {

// ============================================================================
/** Security layer using GSSAPI over a TCP socket
 */
class CERTI_EXPORT SecureTCPSocket : public SocketTCP {
public:
    SecureTCPSocket();
    virtual ~SecureTCPSocket();

    virtual void send(const unsigned char*, size_t);
    virtual void receive(void* Buffer, unsigned long Size);

    // FIXME: Peut-etre devrait-on regarder si un message est pret en interne,
    // et balancer une exception dans ce cas la.
    virtual bool isDataReady() const
    {
        return SocketTCP::isDataReady();
    }

    // Return Peer's principal name. Must not be freed ! Principal name is
    // without any network address part(starting with a '@').
    const char* getPeerName();

private:
#ifdef WITH_GSSAPI
    // Initial Token exchange to open the GSS API session. The client send the
    // initial token to a server principal whose name is fixed.
    void sendInitialToken();
    void receiveInitialToken();

    // Input/Output methods. Parameters of SendMessage are unscrambled
    // messages. GetMessage read an incoming message from the TCP
    // socket, decrypt and verify it, and then store it in
    // InternalBuffer. Call GetMessagePart to retrieve it.
    void sendMessage(void* Buffer, unsigned long Size);
    void getMessage();

    // Copy a part of an already received and decrypted message to buffer.
    // The copied part is(Size) bytes long. If(Size) bytes are not available,
    // an exception is raised.
    void getMessagePart(void* Buffer, unsigned long Size);

    bool SessionInitialized;
    bool DecryptedMessageReady;

    GSSAPIHandler* GSSHandler;

    // GSSAPI buffer for incming messages. Memory is allocated by GSSHandler
    // but deleted locally after use. Base offset is zero.
    gss_buffer_desc IncomingBuffer;
    unsigned long CurrentOffset; // Current offset in buffer.

#endif // WITH_GSSAPI

    char* PeerName;
};

} // namespace certi

#endif // CERTI_SECURE_TCP_SOCKET_HH

// $Id: SecureTCPSocket.hh,v 3.10 2009/11/24 21:44:47 erk Exp $
