// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
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
// $Id: GSSAPIHandler.hh,v 3.3 2003/02/19 18:07:29 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_GSSAPI_HANDLER_HH
#define _CERTI_GSSAPI_HANDLER_HH
#ifdef WITH_GSSAPI

#include <config.h>

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
    GSSAPIHandler(void);
    ~GSSAPIHandler(void);

    // -----------
    // -- Names --
    // -----------
    void setLocalName(char *PrincipalName);
    void setRemoteName(char *PrincipalName);
    char *getRemoteName(void);

    // ------------------
    // -- Init Session --
    // ------------------
    void initSecContext(SocketTCP *Socket);
    void acceptSecContext(SocketTCP *Socket);

    // ------------------------------
    // -- Message Exchange Methods --
    // ------------------------------
    void getMessage(SocketTCP *Socket, gss_buffer_t IncomingBuffer);
    void releaseBuffer(gss_buffer_t IncomingBuffer);
    void sendMessage(SocketTCP *Socket, gss_buffer_t OutcomingBuffer);

private:

    // ---------------------
    // -- Private Methods --
    // ---------------------
    void acquireCred(int initOrAccept);
    void detectError(char *contextString = NULL);

    void getToken(SocketTCP *socket, gss_buffer_desc &buffer);
    void sendToken(SocketTCP *socket, gss_buffer_desc buffer);

    // ------------------------
    // -- Private Attributes --
    // ------------------------

    Boolean InitSecContext_Started ;

    // Principal Names
    gss_name_t LocalName ; // Internal form
    gss_name_t RemoteName ; // Internal form

    // Return Codes
    OM_uint32 Code ;
    OM_uint32 Minor ;

    // Local credential
    gss_cred_id_t Credential ;

    // Local context(Only initiator(client) may delete the context)
    gss_ctx_id_t Context ;

    // Actual Mechanism
    gss_OID MechType ;

    // Role
    Boolean IsClient ;
};
}

#endif // WITH_GSSAPI
#endif // _CERTI_GSSAPI_HANDLER_HH

// $Id: GSSAPIHandler.hh,v 3.3 2003/02/19 18:07:29 breholee Exp $
