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
// $Id: SecureTCPSocket.cc,v 3.13 2009/10/21 18:56:28 erk Exp $
// ----------------------------------------------------------------------------


#include "SecureTCPSocket.hh"

#include "PrettyDebug.hh"

#include <sys/types.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#include <cstdlib>
//#include <pwd.h>

namespace certi {

static PrettyDebug D("STCPSOCK", __FILE__);

// ----------------------------------------------------------------------------
SecureTCPSocket::SecureTCPSocket()
    : SocketTCP()
{
#ifdef WITH_GSSAPI
    SessionInitialized = false ;
    DecryptedMessageReady = false ;

    GSSHandler = new GSSAPIHandler();

    IncomingBuffer.length = 0 ;
    IncomingBuffer.value = NULL ;

    CurrentOffset = 0 ;
#endif // WITH_GSSAPI
    PeerName = NULL ;
}

// ----------------------------------------------------------------------------
SecureTCPSocket::~SecureTCPSocket()
{
    if (PeerName != NULL) {
        free(PeerName);
        PeerName = NULL ;
    }

#ifdef WITH_GSSAPI
    delete GSSHandler ;
    GSSHandler = NULL ;
#endif // WITH_GSSAPI
}

// ----------------------------------------------------------------------------
void
SecureTCPSocket::send(const unsigned char *buffer, size_t size)
    throw (NetworkError, NetworkSignal)
{
    D.Out(pdTrace, "SSocket: Sending.");

#ifdef HLA_USES_GSSAPI
    // If the GSSAPI session is not Initialized, start the HandShake.
    if (!SessionInitialized)
        sendInitialToken();

    // Send Message
    sendMessage(buffer, size);
#else
    SocketTCP::send(buffer, size);
#endif
}

#ifdef WITH_GSSAPI
// ----------------------------------------------------------------------------
void
SecureTCPSocket::getMessage()
{
    if (DecryptedMessageReady) {
        D.Out(pdExcept, "Decrypted message already exists.");
        return ;
    }

    // Wait for an incoming message(GSS Handler will decrypt it).

    IncomingBuffer.length = 0 ;
    IncomingBuffer.value = NULL ;

    CurrentOffset = 0 ;

    GSSHandler->getMessage((SocketTCP *)this, &IncomingBuffer);

    DecryptedMessageReady = true ;
}
#endif // WITH_GSSAPI

#ifdef WITH_GSSAPI
// ----------------------------------------------------------------------------
void
SecureTCPSocket::getMessagePart(void *Buffer, unsigned long Size)
{
    if (!DecryptedMessageReady)
        getMessage();

    // Check available size
    if (Size > IncomingBuffer.length - CurrentOffset) {
        D.Out(pdExcept, "Size too big for data in buffer.");
        throw NetworkError("Not enough data in STCPSocket buffer.");
    }

    // Copy Message to Buffer
    memcpy((char *) Buffer,
           (char *) IncomingBuffer.value + CurrentOffset,
           Size);

    // Change current offset
    CurrentOffset += Size ;

    // Discard completely read buffers
    if (CurrentOffset >= IncomingBuffer.length) {
        GSSHandler->releaseBuffer(&IncomingBuffer);
        CurrentOffset = 0 ;
        DecryptedMessageReady = false ;
    }
}
#endif // WITH_GSSAPI

// ----------------------------------------------------------------------------
const char *
SecureTCPSocket::getPeerName()
{
    if (PeerName != NULL)
        return PeerName ;
#ifdef WITH_GSSAPI
    PeerName = GSSHandler->getRemoteName();
#endif // WITH_GSSAPI
    if (PeerName == NULL)
        throw RTIinternalError("Could not retrieve Peer's principal name.");
#ifdef WITH_GSSAPI
    return PeerName ;
#endif // WITH_GSSAPI
    throw RTIinternalError("No peer's principal name.");
}

// ----------------------------------------------------------------------------
void
SecureTCPSocket::receive(void *Buffer, unsigned long Size)
    throw (NetworkError, NetworkSignal)
{
    D.Out(pdTrace, "SSocket: Receiving.");

#ifdef HLA_USES_GSSAPI

    // If the GSSAPI session is not Initialized, start the HandShake.

    if (!SessionInitialized)
        receiveInitialToken();

    getMessagePart(Buffer, Size);

#else

    // Non secure communications
    SocketTCP::receive(Buffer, Size);

#endif // HLA_USES_GSSAPI

}

#ifdef WITH_GSSAPI
// ----------------------------------------------------------------------------
void
SecureTCPSocket::receiveInitialToken()
{
    D.Out(pdInit, "Receiving Initial Token.");

    // Initialize Local Principal Name with Server Default Name

    GSSHandler->setLocalName(HLA_SERVER_PRINCIPAL_NAME);

    // Accept Initial Handshake(started by client)

    GSSHandler->acceptSecContext((SocketTCP *) this);

    D.Out(pdInit, "GSSAPI session initialized.");

    SessionInitialized = true ;
}

// ----------------------------------------------------------------------------
void
SecureTCPSocket::sendInitialToken()
{
    uid_t CurrentUID ;
    struct passwd *PasswdEntry ;

    D.Out(pdInit, "Sending Initial Token.");

    // Initialize Local Principal Name with Login Name

    CurrentUID = getuid();
    PasswdEntry = getpwuid(CurrentUID);

    if (PasswdEntry == NULL)
        throw NetworkError("S/TCP: Unable to get Local Principal Name.");

    GSSHandler->setLocalName(PasswdEntry->pw_name);

    // Initialize Remote Principal Name with Constant

    GSSHandler->setRemoteName(HLA_SERVER_PRINCIPAL_NAME);

    // Start Initial Handshake

    GSSHandler->initSecContext((SocketTCP *) this);

    D.Out(pdInit, "GSSAPI session initialized.");

    SessionInitialized = true ;
}

// ----------------------------------------------------------------------------
void
SecureTCPSocket::sendMessage(void *Buffer, unsigned long Size)
{
    gss_buffer_desc OutputToken ;

    OutputToken.value = Buffer ;
    OutputToken.length = Size ;

    GSSHandler->sendMessage((SocketTCP *)this, &OutputToken);
}
#endif // WITH_GSSAPI

}

// $Id: SecureTCPSocket.cc,v 3.13 2009/10/21 18:56:28 erk Exp $
