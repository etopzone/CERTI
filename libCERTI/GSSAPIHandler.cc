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
// ----------------------------------------------------------------------------

#ifdef WITH_GSSAPI

#include "GSSAPIHandler.hh"
#include "PrettyDebug.hh"
#include "SocketTCP.hh"

namespace certi {

static PrettyDebug D("HLAGSSAPI", "(H-GSSAPI) - ");

// ----------------------------------------------------------------------------
/*! Retrieve credential for Local principal.
  Parameter can be GSS_C_INITIATE(client) or GSS_C_ACCEPT(server).
*/
void GSSAPIHandler::acquireCred(int InitOrAccept)
{
    D.Out(pdInit, "GSSAPI: Acquire Credential.");

    Code = gss_acquire_cred(&Minor,
                            LocalName, // Principal Name
                            HLA_GSS_SESSION_DURATION, // Expiration delay
                            GSS_C_NULL_OID_SET,
                            InitOrAccept, // Cred. used to init.
                            &Credential, // Returned Credential
                            NULL,
                            NULL);

    detectError("GSSAPI: Acquire Credential");

}

// ----------------------------------------------------------------------------
/*! Session Initiator must call SetRemoteName, and then InitSecContext. On the
  remote side, AcceptSecContext will be directly called. In both case, Local
  Name must have been previously set.
*/
void GSSAPIHandler::acceptSecContext(SocketTCP *Socket)
{
    // The Input and Output Buffers used by the initial Handshake.

    gss_buffer_desc InputToken = GSS_C_EMPTY_BUFFER ;
    gss_buffer_desc OutputToken = GSS_C_EMPTY_BUFFER ;

    int ReturnedFlags = 0 ;
    OM_uint32 TimeRec = 0 ;

    gss_cred_id_t DelegCredential ;

    // Prevent Re-entrant calls

    if (InitSecContext_Started == RTI_TRUE) {
        D.Out(pdExcept, "GSSAPI: Reentrant call of GSSHandler::InitSecContext.");
        printf("GSSAPI Fatal Error : Reentrant call in InitSecContext.\n");
        throw NetworkError("Reentrant call of InitSecContext.");
    }

    InitSecContext_Started = RTI_TRUE ;
    IsClient = RTI_FALSE ;

    // Check Local Principal Name

    if (LocalName == GSS_C_NO_NAME) {
        printf("GSSAPI: Local Name must be set before accept_sec_ctxt.\n");
        throw NetworkError("GSSAPI: Accept_Sec_Context: No name set.");
    }


    // Retrieve Credential

    acquireCred(GSS_C_ACCEPT);


    // Accept Security Context(Server Side)

    do {

        // Wait for peer's response if needed(InputToken may be allocated)

        getToken(Socket, InputToken);

        D.Out(pdInit, "GSSAPI: Received Security Context Token(%d bytes)",
              InputToken.length);

        // Accept Token

        Code = gss_accept_sec_context(&Minor,
                                      &Context, // Initially no context
                                      Credential,
                                      &InputToken, // Client Token
                                      GSS_C_NO_CHANNEL_BINDINGS,
                                      &RemoteName, // Client Name or NULL
                                      &MechType,
                                      &OutputToken,
                                      &ReturnedFlags,
                                      &TimeRec,
                                      &DelegCredential);

        detectError("GSSAPI: Accept Security Context");

        // If the Credential was delegated, print a warning.

        if (DelegCredential != GSS_C_NO_CREDENTIAL) {
            printf("GSSAPI: Warning : Client has delegated his credential.\n");
            gss_release_cred(&Minor, &DelegCredential);
        }

        // If a service was requested but is not available, print a warning.

        if (HLA_GSS_FLAGS & !ReturnedFlags) {
            printf("GSSAPI: Some unavailable service was requested.\n");
            printf("GSSAPI: Returned service flags are : 0x%X\n", ReturnedFlags);
        }

        D.Out(pdInit, "GSSAPI: Security Context accepted.");

        // Free Input Token

        if (InputToken.value != NULL)
            free(InputToken.value);

        InputToken.length = 0 ;
        InputToken.value = NULL ;

        // Send Output token to peer and then release it.

        if (OutputToken.length > 0) {

            D.Out(pdInit,
                  "GSSAPI: Accept Sec Context sending answer Token(%d bytes).",
                  OutputToken.length);

            sendToken(Socket, OutputToken);

            gss_release_buffer(&Minor, &OutputToken);

            OutputToken.length = 0 ;
            OutputToken.value = NULL ;
        }

    } while (Code != GSS_S_COMPLETE); // Loop until no response is needed

}

// ----------------------------------------------------------------------------
//! GSSAPIHandler.
GSSAPIHandler::GSSAPIHandler()
{
    InitSecContext_Started = RTI_FALSE ;

    LocalName = GSS_C_NO_NAME ;
    RemoteName = GSS_C_NO_NAME ;

    Credential = GSS_C_NO_CREDENTIAL ;
    Context = GSS_C_NO_CONTEXT ;

    MechType = GSS_C_NULL_OID ;

    IsClient = RTI_FALSE ; // Reset by Init and Accept Sec. Context
}

// ----------------------------------------------------------------------------
//! Destructor.
GSSAPIHandler::~GSSAPIHandler()
{
    D.Out(pdTerm, "GSSAPI: Releasing GSSAPI objects.");

    // Release Names

    if (LocalName != GSS_C_NO_NAME) {
        gss_release_name(&Minor, &LocalName);
        LocalName = GSS_C_NO_NAME ;
    }

    if (RemoteName != GSS_C_NO_NAME) {
        gss_release_name(&Minor, &RemoteName);
        RemoteName = GSS_C_NO_NAME ;
    }

    // Release Credential

    if (Credential != GSS_C_NO_CREDENTIAL) {
        gss_release_cred(&Minor, &Credential);
        Credential = GSS_C_NO_CREDENTIAL ;
    }

    // Release Context(Client only)

    if ((IsClient == RTI_TRUE) && (Context != GSS_C_NO_CONTEXT)) {

        gss_delete_sec_context(&Minor, &Context, GSS_C_NO_BUFFER);

        D.Out(pdTerm, "GSSAPI: Released Context.");

        Context = GSS_C_NO_CONTEXT ;
    }
}

// ----------------------------------------------------------------------------
/*! Throw NetworkError exception if 'Code' is different from GSS_S_COMPLETE.
  An Error message is displayed, starting with ContextString if not NULL.
*/
void GSSAPIHandler::detectError(char *ContextString)
{
    gss_buffer_desc ErrorMessage ;

    if (GSS_ERROR(Code)) {

        gss_display_status(&Minor, Code,
                           GSS_C_GSS_CODE, GSS_MECH_DES,
                           NULL, &ErrorMessage);

        if (ContextString != NULL)
            printf("%s: %s\n", ContextString, (char *)ErrorMessage.value);
        else
            printf("GSSAPI: %s\n", (char *)ErrorMessage.value);

        gss_release_buffer(&Minor, &ErrorMessage);

        throw NetworkError("Error in GSSAPI(see console output)");
    }

}

// ----------------------------------------------------------------------------
/*! All buffers contain clear-text messages. Incoming buffer value array is
  allocated by getMessage, and must be freed by calling ReleaseBuffer.
*/
void GSSAPIHandler::getMessage(SocketTCP *Socket,
                               gss_buffer_t IncomingBuffer)
{
    gss_buffer_desc SealedToken ;
    gss_buffer_desc Signature ;

    int ConfWasUsed ;
    int QOPState ;

    switch(RTI_GSSAPI_USAGE) {

      case RTI_GSS_NOTHING:
        // Get non-encrypted token.
        getToken(Socket, SealedToken);

        // 'Copy' non encrypted Token into Incoming Buffer(just change pointers)
        IncomingBuffer->value = SealedToken.value ;
        IncomingBuffer->length = SealedToken.length ;

        // Don't free SealedToken ! IncomingBuffer will be freed by a call
        // to GSSAPIHandler::ReleaseBuffer.
        break ;

      case RTI_GSS_SIGN_ONLY:
        // Get Token(ie "Buffer Length" + "Buffer" + "Signature Token")
        getToken(Socket, SealedToken);

        // Get Buffer length
        memcpy(& (IncomingBuffer->length), SealedToken.value, sizeof(OM_uint32));

        // Get Buffer
        IncomingBuffer->value
            = (void *) calloc(IncomingBuffer->length, sizeof(char));

        memcpy(IncomingBuffer->value,
               (char *) SealedToken.value + sizeof(OM_uint32),
               IncomingBuffer->length);

        // Check Signature
        Signature.value
            = (char *)SealedToken.value + sizeof(OM_uint32) + IncomingBuffer->length ;
        Signature.length
            = SealedToken.length - sizeof(OM_uint32) - IncomingBuffer->length ;

        Code = gss_verify(&Minor,
                          Context,
                          IncomingBuffer,
                          &Signature,
                          &QOPState);

        detectError("GSSAPI: Get Message[verify]");

        // Free memory
        free(SealedToken.value);

        break ;

      case RTI_GSS_ENCRYPT:

        // Get encrypted token.
        getToken(Socket, SealedToken);

        // Decrypt Token
        Code = gss_unseal(&Minor,
                          Context,
                          &SealedToken,
                          IncomingBuffer,
                          &ConfWasUsed,
                          &QOPState);

        free(SealedToken.value);
        break ;

      default:
        printf("BAD VALUE FOR RTI_GSSAPI_USAGE\n");
        throw RTIinternalError();
    }

    detectError("GSSAPI: Get Message[unseal]");
}

// ----------------------------------------------------------------------------
/*! After the security context has been accepted, the server can retrieve the
  client principal name. The string is allocated with malloc and must be
  freed by the caller. The returned name is stripped from any network
  address part(starting with a '@').
*/
char *GSSAPIHandler::getRemoteName()
{
    gss_buffer_desc OutputName ;
    gss_OID OutputNameType ;
    char *Buffer ;

    gss_display_name(&Minor,
                     RemoteName,
                     &OutputName,
                     &OutputNameType);

    detectError("GSSAPI: GetRemoteName");

    // Copy OutputName into char buffer.

    Buffer = (char *) malloc((OutputName.length + 1) * sizeof(char));
    strncpy(Buffer, (char *) OutputName.value, OutputName.length);
    Buffer[OutputName.length] = '\0' ;

    releaseBuffer(&OutputName);

    // Remove any '@'(network address part)

    if (strchr(Buffer, '@') != NULL)
        strchr(Buffer, '@')[0] = '\0' ;

    D.Out(pdInit, "GSSAPI: Peer is %s.", Buffer);

    return Buffer ;
}

// ----------------------------------------------------------------------------
/*! Send and receive Token. Network formet is :
  (unsigned long) Length(4 bytes = sizeof(long))
  (char *) Value(Length bytes)
  The getToken method takes an empty buffer has a second parameter, and
  allocates(with calloc) enough space for the incoming token. Memory must be
  freed with free().
*/
void GSSAPIHandler::getToken(SocketTCP *Socket, gss_buffer_desc &Buffer)
{
    // Read Token size.
    Socket->SocketTCP::receive((void *) &Buffer.length,
                               sizeof(Buffer.length));

    // Allocate memory for content.
    Buffer.value = (void *) calloc(Buffer.length, sizeof(char));

    // Read content.
    Socket->SocketTCP::receive((void *) Buffer.value,
                               Buffer.length);
}

// ----------------------------------------------------------------------------
/*! Session Initiator must call SetRemoteName, and then InitSecContext.
  On the remote side, AcceptSecContext will be directly called. In both
  case, Local Name must have been previously set.
*/
void GSSAPIHandler::initSecContext(SocketTCP *Socket)
{
    // The Input and Output Buffers used by the initial Handshake.

    gss_buffer_desc InputToken = GSS_C_EMPTY_BUFFER ;
    gss_buffer_desc OutputToken = GSS_C_EMPTY_BUFFER ;

    gss_buffer_t InputTokenPtr = GSS_C_NO_BUFFER ;

    int ReturnedFlags = 0 ;
    OM_uint32 ContextValidity = 0 ;

    // Prevent Re-entrant calls

    if (InitSecContext_Started == RTI_TRUE) {
        D.Out(pdExcept, "GSSAPI: Reentrant call of GSSHandler::InitSecContext.");
        printf("GSSAPI Fatal Error : Reentrant call in InitSecContext.\n");
        throw NetworkError("Reentrant call of InitSecContext.");
    }

    InitSecContext_Started = RTI_TRUE ;
    IsClient = RTI_TRUE ;

    // Check Local and Remote Names

    if ((LocalName == GSS_C_NO_NAME) || (RemoteName == GSS_C_NO_NAME)) {
        printf("GSSAPI: Local/Remote Name must be set before init_sec_ctxt.\n");
        throw NetworkError("GSSAPI: Init_Sec_Context: No name set.");
    }


    // Retrieve Credential

    acquireCred(GSS_C_INITIATE);

    D.Out(pdInit, "GSSAPI: Init Sec Context.");

    // Get Security Context(Client Side)

    do {

        if (InputToken.length != 0)
            InputTokenPtr = &InputToken ;
        else
            InputTokenPtr = GSS_C_NO_BUFFER ;

        Code = gss_init_sec_context(&Minor,
                                    Credential,
                                    &Context,
                                    RemoteName,
                                    GSS_C_NULL_OID,
                                    HLA_GSS_FLAGS, // See RTIconfig.hh
                                    HLA_GSS_SESSION_DURATION, // See RTIconfig.hh
                                    GSS_C_NO_CHANNEL_BINDINGS,
                                    InputTokenPtr,
                                    &MechType,
                                    &OutputToken,
                                    &ReturnedFlags,
                                    &ContextValidity);

        detectError("GSSAPI: Init Security Context");

        // If a service was requested but is not available, print a warning.

        if (HLA_GSS_FLAGS & !ReturnedFlags) {
            printf("GSSAPI: Some unavailable service was requested.\n");
            printf("GSSAPI: Returned service flags are : 0x%X\n", ReturnedFlags);
        }

        D.Out(pdInit, "GSSAPI: Security Context initiated(Validity: %d).",
              ContextValidity);

        // Free Input Token(allocated in a previous loop)

        if (InputToken.value != NULL)
            free(InputToken.value);

        InputToken.length = 0 ;
        InputToken.value = NULL ;

        // Send Output token to peer and then release it.

        if (OutputToken.length > 0) {

            D.Out(pdInit, "GSSAPI: Init Sec Context sending Token(%d bytes).",
                  OutputToken.length);

            sendToken(Socket, OutputToken);

            gss_release_buffer(&Minor, &OutputToken);

            OutputToken.length = 0 ;
            OutputToken.value = NULL ;
        }

        // Wait for peer's response if needed(InputToken may be allocated)

        if (Code==GSS_S_CONTINUE_NEEDED) {

            D.Out(pdInit, "GSSAPI: Init Sec Context waiting for server answer...");

            getToken(Socket, InputToken);
        }

    } while (Code != GSS_S_COMPLETE); // Loop until no response is needed

}

// ----------------------------------------------------------------------------
/*! All buffers contain clear-text messages. Incoming buffer value array is
  allocated by GetMessage, and must be freed by calling releaseBuffer.
*/
void GSSAPIHandler::releaseBuffer(gss_buffer_t IncomingBuffer)
{
    switch(RTI_GSSAPI_USAGE) {

      case RTI_GSS_NOTHING:
      case RTI_GSS_SIGN_ONLY:
        // Buffer was allocated by GetToken
        free(IncomingBuffer->value);
        IncomingBuffer->value = NULL ;
        IncomingBuffer->length = 0 ;
        break ;

      case RTI_GSS_ENCRYPT:
        gss_release_buffer(&Minor, IncomingBuffer);
    }
}

// ----------------------------------------------------------------------------
//! sendMessage.
void
GSSAPIHandler::sendMessage(SocketTCP *Socket,
                           gss_buffer_t OutcomingBuffer)
{
    gss_buffer_desc SealedToken ;
    gss_buffer_desc SignatureToken ;

    OM_uint32 OutLength = OutcomingBuffer->length ;
    void *OutValue = OutcomingBuffer->value ;
    int ConfState ;

    detectError("GSSAPI: Seal Message");

    switch(RTI_GSSAPI_USAGE) {

      case RTI_GSS_NOTHING:
        // Send clear-text message.
        sendToken(Socket, *OutcomingBuffer);
        break ;

      case RTI_GSS_SIGN_ONLY:
        // Compute Signature Token
        Code = gss_sign(&Minor,
                        Context,
                        GSS_C_QOP_DEFAULT,
                        OutcomingBuffer,
                        &SignatureToken);

        detectError("GSSAPI: Send Message[sign]");

        // Compute SealedToken : "Buffer Length" + "Buffer" + "Signature Token"
        SealedToken.length = sizeof(OM_uint32);
        SealedToken.length += OutLength ;
        SealedToken.length += SignatureToken.length ;

        SealedToken.value = (void *) calloc(SealedToken.length, sizeof(char));

        // Copy OutcomingBuffer length
        memcpy(SealedToken.value, & (OutLength), sizeof(OM_uint32));

        // Copy OutcomingBuffer
        memcpy((char *) SealedToken.value + sizeof(OM_uint32),
               OutValue, OutLength);

        // Copy SignatureToken
        memcpy((char *) SealedToken.value + sizeof(OM_uint32) + OutLength,
               SignatureToken.value, SignatureToken.length);

        // Send resulting Token
        sendToken(Socket, SealedToken);

        // Free memory
        free(SealedToken.value);
        gss_release_buffer(&Minor, &SignatureToken);

        break ;

      case RTI_GSS_ENCRYPT:

        Code = gss_seal(&Minor,
                        Context,
                        RTI_TRUE,
                        GSS_C_QOP_DEFAULT,
                        OutcomingBuffer,
                        &ConfState,
                        &SealedToken);

        detectError("GSSAPI: Send Message[seal]");

        sendToken(Socket, SealedToken);
        gss_release_buffer(&Minor, &SealedToken);
    }

}

// ----------------------------------------------------------------------------
/*! Send and receive Token. Network formet is :
  (unsigned long) Length(4 bytes = sizeof(long))
  (char *) Value(Length bytes)
  The GetToken method takes an empty buffer has a second parameter, and
  allocates(with calloc) enough space for the incoming token. Memory must be
  freed with free().
*/
void GSSAPIHandler::sendToken(SocketTCP *Socket, gss_buffer_desc Buffer)
{
    // Write Token Length
    Socket->SocketTCP::send((void *) &Buffer.length,
                            sizeof(Buffer.length));

    // Write Token Content
    Socket->SocketTCP::send((void *) Buffer.value,
                            Buffer.length);
}

// ----------------------------------------------------------------------------
//! setLocalName.
void GSSAPIHandler::setLocalName(char *PrincipalName)
{
    char *p ;
    char bidon[80] ;

    gss_buffer_desc LocalBuffer = GSS_C_EMPTY_BUFFER ;

    strcpy(bidon, PrincipalName);

    // Put Principal Name in GSSAPI buffer

    if ((PrincipalName == NULL) || (strlen(PrincipalName) == 0))
        throw NetworkError("Bad Local Principal Name.");

    // Remove any machine name(like in rtip@jocaste)
    p = strchr(bidon, '@');
    if (p != NULL) {
        *p = '\0' ;
    }

    LocalBuffer.value = bidon ;
    LocalBuffer.length = strlen(bidon) + 1 ;

    // Give Name to GSSAPI

    Code = gss_import_name(&Minor,
                           (gss_buffer_t) &LocalBuffer,
                           GSS_C_NULL_OID,
                           &LocalName);

    D.Out(pdInit, "GSSAPI: Local Name set to %s.", bidon);

    detectError("GSSAPI/setLocalName");
}

// ----------------------------------------------------------------------------
//! setRemoveName.
/*! For client, before InitSecCntxt.
 */
void GSSAPIHandler::setRemoteName(char *PrincipalName)
{
    gss_buffer_desc LocalBuffer = GSS_C_EMPTY_BUFFER ;

    // Put Principal Name in GSSAPI buffer

    if ((PrincipalName == NULL) || (strlen(PrincipalName) == 0))
        throw NetworkError("Bad Local Principal Name.");

    LocalBuffer.value = PrincipalName ;
    LocalBuffer.length = strlen(PrincipalName) + 1 ;

    // Give Name to GSSAPI

    Code = gss_import_name(&Minor,
                           (gss_buffer_t) &LocalBuffer,
                           GSS_C_NULL_OID,
                           &RemoteName);

    D.Out(pdInit, "GSSAPI: Remote Name set to %s.", PrincipalName);

    detectError("GSSAPI/setRemoteName");
}

}

#endif // WITH_GSSAPI
