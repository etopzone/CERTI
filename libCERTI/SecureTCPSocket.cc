// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free softwarE ; you can redistribute it and/or
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
// $Id: SecureTCPSocket.cc,v 3.6 2005/04/30 17:32:27 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "SecureTCPSocket.hh"

#include "PrettyDebug.hh"

#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>

namespace certi {

static pdCDebug D("STCPSOCK", "(s-TCPsok) - ");

// ----------------------
// -- SecureTCPSocket --
// ----------------------

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


// ----------------------
// -- SecureTCPSocket --
// ----------------------

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


// -------------
// -- Emettre --
// -------------

void
SecureTCPSocket::send(void *Buffer, unsigned long Size)
    throw (NetworkError, NetworkSignal)
{
    D.Out(pdTrace, "SSocket: Sending.");

#ifdef HLA_USES_GSSAPI

    // If the GSSAPI session is not Initialized, start the HandShake.

    if (!SessionInitialized)
        sendInitialToken();

    // Send Message

    sendMessage(Buffer, Size);

#else

    SocketTCP::send(Buffer, Size);

#endif
}

#ifdef WITH_GSSAPI

// ----------------
// -- GetMessage --
// ----------------

void SecureTCPSocket::getMessage()
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

// --------------
// -- GetClass --
// --------------

int
SecureTCPSocket::getClass() const
{
#ifdef HLA_USES_GSSAPI
    return SOCKET_TYPE_S_TCP ;
#else
    return SOCKET_TYPE_TCP ;
#endif
}

#ifdef WITH_GSSAPI

// --------------------
// -- GetMessagePart --
// --------------------

void SecureTCPSocket::getMessagePart(void *Buffer, unsigned long Size)
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

// -----------------
// -- GetPeerName --
// -----------------

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


// --------------
// -- Recevoir --
// --------------

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

// ----------------------
// -- RecvInitialToken --
// ----------------------

void SecureTCPSocket::receiveInitialToken()
{
    D.Out(pdInit, "Receiving Initial Token.");

    // Initialize Local Principal Name with Server Default Name

    GSSHandler->setLocalName(HLA_SERVER_PRINCIPAL_NAME);

    // Accept Initial Handshake(started by client)

    GSSHandler->acceptSecContext((SocketTCP *) this);

    D.Out(pdInit, "GSSAPI session initialized.");

    SessionInitialized = true ;
}


// ----------------------
// -- SendInitialToken --
// ----------------------

void SecureTCPSocket::sendInitialToken()
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


// -----------------
// -- SendMessage --
// -----------------

void SecureTCPSocket::sendMessage(void *Buffer, unsigned long Size)
{
    gss_buffer_desc OutputToken ;

    OutputToken.value = Buffer ;
    OutputToken.length = Size ;

    GSSHandler->sendMessage((SocketTCP *)this, &OutputToken);
}

#endif // WITH_GSSAPI

}

// $Id: SecureTCPSocket.cc,v 3.6 2005/04/30 17:32:27 breholee Exp $
