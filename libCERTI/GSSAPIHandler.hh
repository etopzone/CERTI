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
// $Id: GSSAPIHandler.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_GSSAPI_HANDLER_HH
#define _CERTI_GSSAPI_HANDLER_HH
#ifdef WITH_GSSAPI

#include "RTItypes.hh" // For constants' values
#include <stdio.h>

#include "PrettyDebug.hh"
#include "SocketTCP.hh"
#include "gssapi.h"

namespace certi {

class GSSAPIHandler 
{
public:

  typedef enum GSSRole {GSS_Client, GSS_Server};

  // --------------------------------
  // -- Constructor and Destructor --
  // --------------------------------

  GSSAPIHandler();
  ~GSSAPIHandler();

  // -----------
  // -- Names --
  // -----------

  void setLocalName(char *PrincipalName);
  void setRemoteName(char *PrincipalName); //For client,before InitSecCntxt

  // After the security context has been accepted, the server can retrieve
  // the client principal name. The string is allocated with malloc and must
  // be freed by the caller. The returned name is stripped from any network
  // address part(starting with a '@').

  char *getRemoteName();

  // ------------------
  // -- Init Session --
  // ------------------

  // Session Initiator must call SetRemoteName, and then InitSecContext.
  // On the remote side, AcceptSecContext will be directly called.
  // In both case, Local Name must have been previously set.

  void initSecContext(SocketTCP *Socket);
  void acceptSecContext(SocketTCP *Socket);

  // ------------------------------
  // -- Message Exchange Methods --
  // ------------------------------

  // All buffers contain clear-text messages. Incoming buffer value array
  // is allocated by GetMessage, and must be freed by calling ReleaseBuffer.

  void getMessage(SocketTCP *Socket, gss_buffer_t IncomingBuffer);
  void releaseBuffer(gss_buffer_t IncomingBuffer);
  void sendMessage(SocketTCP *Socket, gss_buffer_t OutcomingBuffer);

private:

  // ---------------------
  // -- Private Methods --
  // ---------------------

  // Retrieve credential for Local principal.
  // Parameter can be GSS_C_INITIATE(client) or GSS_C_ACCEPT(server)

  void acquireCred(int InitOrAccept);

  // Throw NetworkError exception if 'Code' is different from GSS_S_COMPLETE.
  // An Error message is displayed, starting with ContextString if not NULL.

  void detectError(char *ContextString = NULL);

  // Send and receive Token. Network formet is :
  //(unsigned long) Length(4 bytes = sizeof(long))
  //(char *) Value(Length bytes)
  // The GetToken method takes an empty buffer has a second parameter,
  // and allocates(with calloc) enough space for the incoming token. Memory
  // must be freed with free().

  void getToken(SocketTCP *Socket, gss_buffer_desc &Buffer);
  void sendToken(SocketTCP *Socket, gss_buffer_desc Buffer);

  // ------------------------
  // -- Private Attributes --
  // ------------------------

  Boolean InitSecContext_Started;

  // Principal Names
  gss_name_t LocalName; // Internal form
  gss_name_t RemoteName; // Internal form

  // Return Codes
  OM_uint32 Code;
  OM_uint32 Minor;

  // Local credential
  gss_cred_id_t Credential;

  // Local context(Only initiator(client) may delete the context)
  gss_ctx_id_t Context;

  // Actual Mechanism
  gss_OID MechType;

  // Role
  Boolean IsClient;
};
}

#endif // WITH_GSSAPI
#endif // _CERTI_GSSAPI_HANDLER_HH

// $Id: GSSAPIHandler.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
