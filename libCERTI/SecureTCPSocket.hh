// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002  ONERA
//
// This file is part of CERTI-libcerti
//
// CERTI-libcerti is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libcerti is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: SecureTCPSocket.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

// $Id: SecureTCPSocket.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------
// Couche de securite utilisant GSSAPI au-dessus d'un socket TCP.

#ifndef _CERTI_SECURE_TCP_SOCKET_HH
#define _CERTI_SECURE_TCP_SOCKET_HH

#include "SocketTCP.hh"
#include "GSSAPIHandler.hh"
#include "RTItypes.hh" 
#include "PrettyDebug.hh"

#include <sys/types.h>
#include <unistd.h>    
#include <pwd.h>       

namespace certi {

class SecureTCPSocket : public certi::SocketTCP
{
public:

  // --------------------------------
  // -- Constructor and Destructor --
  // --------------------------------

  SecureTCPSocket();
  ~SecureTCPSocket();

  // --------------------------
  // -- Read & Write Methods --
  // --------------------------

  void send(void *Buffer, unsigned long Size)
    throw(NetworkError, NetworkSignal) ;
  void receive(void *Buffer, unsigned long Size)
    throw(NetworkError, NetworkSignal) ;

  // ---------------------------
  // -- Others Socket Methods --(Most are handled directly by SocketTCP)
  // ---------------------------

  // FIXME: Peut-etre devrait-on regarder si un message est pret en interne,
  // et balancer une exception dans ce cas la.
  Boolean isDataReady() { return SocketTCP::isDataReady(); }
  int getClass();


  // -------------------------------------
  // -- Secure Socket Specific services --
  // -------------------------------------

  // Return Peer's principal name. Must not be freed ! Principal name is
  // without any network address part(starting with a '@').
  const char   *getPeerName();

private:
  char            *PeerName;
#ifdef WITH_GSSAPI
  // --------------------------------
  // -- Private General Attributes --
  // --------------------------------

  Boolean SessionInitialized;
  Boolean DecryptedMessageReady;

  GSSAPIHandler *GSSHandler;

  // GSSAPI buffer for incming messages. Memory is allocated by GSSHandler
  // but deleted locally after use. Base offset is zero.

  gss_buffer_desc IncomingBuffer;
  unsigned long CurrentOffset;   // Current offset in buffer.

  //char *PeerName;

  // ---------------------
  // -- Private Methods --
  // ---------------------

  // Initial Token exchange to open the GSS API session. The client send the
  // initial token to a server principal whose name is fixed.

  void sendInitialToken();
  void receiveInitialToken();

  // Input/Output methods. Parameters of SendMessage are unscrambled
  // messages.  GetMessage read an incoming message from the TCP
  // socket, decrypt and verify it, and then store it in
  // InternalBuffer. Call GetMessagePart to retrieve it.
  
  void sendMessage(void *Buffer,  unsigned long Size);
  void getMessage();

  // Copy a part of an already received and decrypted message to buffer.
  // The copied part is(Size) bytes long. If(Size) bytes are not available,
  // an exception is raised.

  void getMessagePart(void *Buffer,  unsigned long Size);
#endif // WITH_GSSAPI
};
}

#endif // _CERTI_SECURE_TCP_SOCKET_HH

// $Id: SecureTCPSocket.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $

