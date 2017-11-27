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
void GSSAPIHandler::acquireCred(int initOrAccept)
{
    D.Out(pdInit, "GSSAPI: Acquire Credential.");

    Code = gss_acquire_cred(&Minor,
                            LocalName, // Principal Name
                            HLA_GSS_SESSION_DURATION, // Expiration delay
                            GSS_C_NULL_OID_SET,
                            initOrAccept, // Cred. used to init.
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
void GSSAPIHandler::acceptSecContext(SocketTCP* socket)
{
    // The Input and Output Buffers used by the initial Handshake.

    gss_buffer_desc InputToken = GSS_C_EMPTY_BUFFER;
    gss_buffer_desc OutputToken = GSS_C_EMPTY_BUFFER;

    int ReturnedFlags = 0;
    OM_uint32 TimeRec = 0;

    gss_cred_id_t DelegCredential;

    // Prevent Re-entrant calls

    if (InitSecContext_Started == RTI_TRUE) {
        D.Out(pdExcept, "GSSAPI: Reentrant call of GSSHandler::InitSecContext.");
        printf("GSSAPI Fatal Error : Reentrant call in InitSecContext.\n");
        throw NetworkError("Reentrant call of InitSecContext.");
    }

    InitSecContext_Started = RTI_TRUE;
    IsClient = RTI_FALSE;

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

        getToken(socket, InputToken);

        D.Out(pdInit, "GSSAPI: Received Security Context Token(%d bytes)", InputToken.length);

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

        InputToken.length = 0;
        InputToken.value = NULL;

        // Send Output token to peer and then release it.

        if (OutputToken.length > 0) {
            D.Out(pdInit, "GSSAPI: Accept Sec Context sending answer Token(%d bytes).", OutputToken.length);

            sendToken(socket, OutputToken);

            gss_release_buffer(&Minor, &OutputToken);

            OutputToken.length = 0;
            OutputToken.value = NULL;
        }

    } while (Code != GSS_S_COMPLETE); // Loop until no response is needed
}

// ----------------------------------------------------------------------------
//! GSSAPIHandler.
GSSAPIHandler::GSSAPIHandler()
{
    InitSecContext_Started = RTI_FALSE;

    LocalName = GSS_C_NO_NAME;
    RemoteName = GSS_C_NO_NAME;

    Credential = GSS_C_NO_CREDENTIAL;
    Context = GSS_C_NO_CONTEXT;

    MechType = GSS_C_NULL_OID;

    IsClient = RTI_FALSE; // Reset by Init and Accept Sec. Context
}

// ----------------------------------------------------------------------------
//! Destructor.
GSSAPIHandler::~GSSAPIHandler()
{
    D.Out(pdTerm, "GSSAPI: Releasing GSSAPI objects.");

    // Release Names

    if (LocalName != GSS_C_NO_NAME) {
        gss_release_name(&Minor, &LocalName);
        LocalName = GSS_C_NO_NAME;
    }

    if (RemoteName != GSS_C_NO_NAME) {
        gss_release_name(&Minor, &RemoteName);
        RemoteName = GSS_C_NO_NAME;
    }

    // Release Credential

    if (Credential != GSS_C_NO_CREDENTIAL) {
        gss_release_cred(&Minor, &Credential);
        Credential = GSS_C_NO_CREDENTIAL;
    }

    // Release Context(Client only)

    if ((IsClient == RTI_TRUE) && (Context != GSS_C_NO_CONTEXT)) {
        gss_delete_sec_context(&Minor, &Context, GSS_C_NO_BUFFER);

        D.Out(pdTerm, "GSSAPI: Released Context.");

        Context = GSS_C_NO_CONTEXT;
    }
}

// ----------------------------------------------------------------------------
/*! Throw NetworkError exception if 'Code' is different from GSS_S_COMPLETE.
  An Error message is displayed, starting with contextString if not NULL.
*/
void GSSAPIHandler::detectError(char* contextString)
{
    gss_buffer_desc ErrorMessage;

    if (GSS_ERROR(Code)) {
        gss_display_status(&Minor, Code, GSS_C_GSS_CODE, GSS_MECH_DES, NULL, &ErrorMessage);

        if (contextString != NULL)
            printf("%s: %s\n", contextString, (char*) ErrorMessage.value);
        else
            printf("GSSAPI: %s\n", (char*) ErrorMessage.value);

        gss_release_buffer(&Minor, &ErrorMessage);

        throw NetworkError("Error in GSSAPI(see console output)");
    }
}

// ----------------------------------------------------------------------------
/*! All buffers contain clear-text messages. Incoming buffer value array is
  allocated by getMessage, and must be freed by calling ReleaseBuffer.
*/
void GSSAPIHandler::getMessage(SocketTCP* socket, gss_buffer_t incoming_buffer)
{
    gss_buffer_desc SealedToken;
    gss_buffer_desc Signature;

    int ConfWasUsed;
    int QOPState;

    switch (RTI_GSSAPI_USAGE) {
    case RTI_GSS_NOTHING:
        // Get non-encrypted token.
        getToken(socket, SealedToken);

        // 'Copy' non encrypted Token into Incoming Buffer(just change pointers)
        incoming_buffer->value = SealedToken.value;
        incoming_buffer->length = SealedToken.length;

        // Don't free SealedToken ! incoming_buffer will be freed by a call
        // to GSSAPIHandler::ReleaseBuffer.
        break;

    case RTI_GSS_SIGN_ONLY:
        // Get Token(ie "Buffer Length" + "Buffer" + "Signature Token")
        getToken(socket, SealedToken);

        // Get Buffer length
        memcpy(&(incoming_buffer->length), SealedToken.value, sizeof(OM_uint32));

        // Get Buffer
        incoming_buffer->value = (void*) calloc(incoming_buffer->length, sizeof(char));

        memcpy(incoming_buffer->value, (char*) SealedToken.value + sizeof(OM_uint32), incoming_buffer->length);

        // Check Signature
        Signature.value = (char*) SealedToken.value + sizeof(OM_uint32) + incoming_buffer->length;
        Signature.length = SealedToken.length - sizeof(OM_uint32) - incoming_buffer->length;

        Code = gss_verify(&Minor, Context, incoming_buffer, &Signature, &QOPState);

        detectError("GSSAPI: Get Message[verify]");

        // Free memory
        free(SealedToken.value);

        break;

    case RTI_GSS_ENCRYPT:

        // Get encrypted token.
        getToken(socket, SealedToken);

        // Decrypt Token
        Code = gss_unseal(&Minor, Context, &SealedToken, incoming_buffer, &ConfWasUsed, &QOPState);

        free(SealedToken.value);
        break;

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
char* GSSAPIHandler::getRemoteName()
{
    gss_buffer_desc OutputName;
    gss_OID OutputNameType;
    char* Buffer;

    gss_display_name(&Minor, RemoteName, &OutputName, &OutputNameType);

    detectError("GSSAPI: GetRemoteName");

    // Copy OutputName into char buffer.

    Buffer = (char*) malloc((OutputName.length + 1) * sizeof(char));
    strncpy(Buffer, (char*) OutputName.value, OutputName.length);
    Buffer[OutputName.length] = '\0';

    releaseBuffer(&OutputName);

    // Remove any '@'(network address part)

    if (strchr(Buffer, '@') != NULL)
        strchr(Buffer, '@')[0] = '\0';

    D.Out(pdInit, "GSSAPI: Peer is %s.", Buffer);

    return Buffer;
}

// ----------------------------------------------------------------------------
/*! Send and receive Token. Network formet is :
  (unsigned long) Length(4 bytes = sizeof(long))
  (char *) Value(Length bytes)
  The getToken method takes an empty buffer has a second parameter, and
  allocates(with calloc) enough space for the incoming token. Memory must be
  freed with free().
*/
void GSSAPIHandler::getToken(SocketTCP* socket, gss_buffer_desc& buffer)
{
    // Read Token size.
    socket->SocketTCP::receive((void*) &buffer.length, sizeof(buffer.length));

    // Allocate memory for content.
    buffer.value = (void*) calloc(buffer.length, sizeof(char));

    // Read content.
    socket->SocketTCP::receive((void*) buffer.value, buffer.length);
}

// ----------------------------------------------------------------------------
/*! Session Initiator must call SetRemoteName, and then InitSecContext.
  On the remote side, AcceptSecContext will be directly called. In both
  case, Local Name must have been previously set.
*/
void GSSAPIHandler::initSecContext(SocketTCP* socket)
{
    // The Input and Output Buffers used by the initial Handshake.

    gss_buffer_desc InputToken = GSS_C_EMPTY_BUFFER;
    gss_buffer_desc OutputToken = GSS_C_EMPTY_BUFFER;

    gss_buffer_t InputTokenPtr = GSS_C_NO_BUFFER;

    int ReturnedFlags = 0;
    OM_uint32 ContextValidity = 0;

    // Prevent Re-entrant calls

    if (InitSecContext_Started == RTI_TRUE) {
        D.Out(pdExcept, "GSSAPI: Reentrant call of GSSHandler::InitSecContext.");
        printf("GSSAPI Fatal Error : Reentrant call in InitSecContext.\n");
        throw NetworkError("Reentrant call of InitSecContext.");
    }

    InitSecContext_Started = RTI_TRUE;
    IsClient = RTI_TRUE;

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
            InputTokenPtr = &InputToken;
        else
            InputTokenPtr = GSS_C_NO_BUFFER;

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

        D.Out(pdInit, "GSSAPI: Security Context initiated(Validity: %d).", ContextValidity);

        // Free Input Token(allocated in a previous loop)

        if (InputToken.value != NULL)
            free(InputToken.value);

        InputToken.length = 0;
        InputToken.value = NULL;

        // Send Output token to peer and then release it.

        if (OutputToken.length > 0) {
            D.Out(pdInit, "GSSAPI: Init Sec Context sending Token(%d bytes).", OutputToken.length);

            sendToken(socket, OutputToken);

            gss_release_buffer(&Minor, &OutputToken);

            OutputToken.length = 0;
            OutputToken.value = NULL;
        }

        // Wait for peer's response if needed(InputToken may be allocated)

        if (Code == GSS_S_CONTINUE_NEEDED) {
            D.Out(pdInit, "GSSAPI: Init Sec Context waiting for server answer...");

            getToken(socket, InputToken);
        }

    } while (Code != GSS_S_COMPLETE); // Loop until no response is needed
}

// ----------------------------------------------------------------------------
/*! All buffers contain clear-text messages. Incoming buffer value array is
  allocated by GetMessage, and must be freed by calling releaseBuffer.
*/
void GSSAPIHandler::releaseBuffer(gss_buffer_t incoming_buffer)
{
    switch (RTI_GSSAPI_USAGE) {
    case RTI_GSS_NOTHING:
    case RTI_GSS_SIGN_ONLY:
        // Buffer was allocated by GetToken
        free(incoming_buffer->value);
        incoming_buffer->value = NULL;
        incoming_buffer->length = 0;
        break;

    case RTI_GSS_ENCRYPT:
        gss_release_buffer(&Minor, incoming_buffer);
    }
}

// ----------------------------------------------------------------------------
//! sendMessage.
void GSSAPIHandler::sendMessage(SocketTCP* socket, gss_buffer_t outcoming_buffer)
{
    gss_buffer_desc SealedToken;
    gss_buffer_desc SignatureToken;

    OM_uint32 OutLength = outcoming_buffer->length;
    void* OutValue = outcoming_buffer->value;
    int ConfState;

    detectError("GSSAPI: Seal Message");

    switch (RTI_GSSAPI_USAGE) {
    case RTI_GSS_NOTHING:
        // Send clear-text message.
        sendToken(socket, *outcoming_buffer);
        break;

    case RTI_GSS_SIGN_ONLY:
        // Compute Signature Token
        Code = gss_sign(&Minor, Context, GSS_C_QOP_DEFAULT, outcoming_buffer, &SignatureToken);

        detectError("GSSAPI: Send Message[sign]");

        // Compute SealedToken : "Buffer Length" + "Buffer" + "Signature Token"
        SealedToken.length = sizeof(OM_uint32);
        SealedToken.length += OutLength;
        SealedToken.length += SignatureToken.length;

        SealedToken.value = (void*) calloc(SealedToken.length, sizeof(char));

        // Copy outcoming_buffer length
        memcpy(SealedToken.value, &(OutLength), sizeof(OM_uint32));

        // Copy outcoming_buffer
        memcpy((char*) SealedToken.value + sizeof(OM_uint32), OutValue, OutLength);

        // Copy SignatureToken
        memcpy((char*) SealedToken.value + sizeof(OM_uint32) + OutLength, SignatureToken.value, SignatureToken.length);

        // Send resulting Token
        sendToken(socket, SealedToken);

        // Free memory
        free(SealedToken.value);
        gss_release_buffer(&Minor, &SignatureToken);

        break;

    case RTI_GSS_ENCRYPT:

        Code = gss_seal(&Minor, Context, RTI_TRUE, GSS_C_QOP_DEFAULT, outcoming_buffer, &ConfState, &SealedToken);

        detectError("GSSAPI: Send Message[seal]");

        sendToken(socket, SealedToken);
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
void GSSAPIHandler::sendToken(SocketTCP* socket, gss_buffer_desc buffer)
{
    // Write Token Length
    socket->SocketTCP::send((void*) &buffer.length, sizeof(buffer.length));

    // Write Token Content
    socket->SocketTCP::send((void*) buffer.value, buffer.length);
}

// ----------------------------------------------------------------------------
//! setLocalName.
void GSSAPIHandler::setLocalName(char* principal_name)
{
    char* p;
    char bidon[80];

    gss_buffer_desc LocalBuffer = GSS_C_EMPTY_BUFFER;

    strcpy(bidon, principal_name);

    // Put Principal Name in GSSAPI buffer

    if ((principal_name == NULL) || (strlen(principal_name) == 0))
        throw NetworkError("Bad Local Principal Name.");

    // Remove any machine name(like in rtip@jocaste)
    p = strchr(bidon, '@');
    if (p != NULL) {
        *p = '\0';
    }

    LocalBuffer.value = bidon;
    LocalBuffer.length = strlen(bidon) + 1;

    // Give Name to GSSAPI

    Code = gss_import_name(&Minor, (gss_buffer_t) &LocalBuffer, GSS_C_NULL_OID, &LocalName);

    D.Out(pdInit, "GSSAPI: Local Name set to %s.", bidon);

    detectError("GSSAPI/setLocalName");
}

// ----------------------------------------------------------------------------
//! setRemoveName.
/*! For client, before InitSecCntxt.
 */
void GSSAPIHandler::setRemoteName(char* principal_name)
{
    gss_buffer_desc LocalBuffer = GSS_C_EMPTY_BUFFER;

    // Put Principal Name in GSSAPI buffer

    if ((principal_name == NULL) || (strlen(principal_name) == 0))
        throw NetworkError("Bad Local Principal Name.");

    LocalBuffer.value = principal_name;
    LocalBuffer.length = strlen(principal_name) + 1;

    // Give Name to GSSAPI

    Code = gss_import_name(&Minor, (gss_buffer_t) &LocalBuffer, GSS_C_NULL_OID, &RemoteName);

    D.Out(pdInit, "GSSAPI: Remote Name set to %s.", principal_name);

    detectError("GSSAPI/setRemoteName");
}
}

#endif // WITH_GSSAPI
