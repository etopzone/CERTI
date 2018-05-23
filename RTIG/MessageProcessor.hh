// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (MessageEvent& request) 2002, 2003, 2004  ONERA
//
// This file is part of CERTI
//
// CERTI is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (MessageEvent& request) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// ----------------------------------------------------------------------------

#ifndef CERTI_MESSAGEPROCESSOR_HH
#define CERTI_MESSAGEPROCESSOR_HH

#include <vector>

#include <libHLA/MessageBuffer.hh>

#include <libCERTI/AuditFile.hh>
#include <libCERTI/HandleManager.hh>
#include <libCERTI/MessageEvent.hh>
#include <libCERTI/NM_Classes.hh>
#include <libCERTI/SocketServer.hh>

#include "FederationsList.hh"

namespace certi {

namespace rtig {

class MessageProcessor {
public:
    MessageProcessor(AuditFile& audit_server,
                     SocketServer& socket_server,
                     HandleManager<Handle>& handle_generator,
                     FederationsList& federations);

    /** Choose the right process subfunction and calls it.
     *  
     * This module chooses the right processing module to call.
     * This process is done by examinating the message type.
     * 
     * @param request the MessageEvent to process
     * 
     * @return the MessageEvents to send back
     */
    Responses processEvent(MessageEvent<NetworkMessage> request);

private:
    // Event handlers
    Responses process(MessageEvent<NM_Create_Federation_Execution>&& request);
    Responses process(MessageEvent<NM_Join_Federation_Execution>&& request);
    Responses process(MessageEvent<NM_Resign_Federation_Execution>&& request);
    Responses process(MessageEvent<NM_Destroy_Federation_Execution>&& request);
    Responses process(MessageEvent<NM_Set_Class_Relevance_Advisory_Switch>&& request);
    Responses process(MessageEvent<NM_Set_Interaction_Relevance_Advisory_Switch>&& request);
    Responses process(MessageEvent<NM_Set_Attribute_Relevance_Advisory_Switch>&& request);
    Responses process(MessageEvent<NM_Set_Attribute_Scope_Advisory_Switch>&& request);
    Responses process(MessageEvent<NM_Set_Time_Regulating>&& request);
    Responses process(MessageEvent<NM_Set_Time_Constrained>&& request);
    Responses process(MessageEvent<NM_Message_Null>&& request, bool anonymous = false);
    Responses process(MessageEvent<NM_Message_Null_Prime>&& request);
    Responses process(MessageEvent<NM_Register_Federation_Synchronization_Point>&& request);
    Responses process(MessageEvent<NM_Synchronization_Point_Achieved>&& request);
    Responses process(MessageEvent<NM_Request_Federation_Save>&& request);
    Responses process(MessageEvent<NM_Federate_Save_Begun>&& request);
    Responses process(MessageEvent<NM_Federate_Save_Complete>&& request);
    Responses process(MessageEvent<NM_Federate_Save_Not_Complete>&& request);
    Responses process(MessageEvent<NM_Request_Federation_Restore>&& request);
    Responses process(MessageEvent<NM_Federate_Restore_Complete>&& request);
    Responses process(MessageEvent<NM_Federate_Restore_Not_Complete>&& request);
    Responses process(MessageEvent<NM_Publish_Object_Class>&& request);
    Responses process(MessageEvent<NM_Unpublish_Object_Class>&& request);
    Responses process(MessageEvent<NM_Subscribe_Object_Class>&& request);
    Responses process(MessageEvent<NM_Unsubscribe_Object_Class>&& request);
    Responses process(MessageEvent<NM_Publish_Interaction_Class>&& request);
    Responses process(MessageEvent<NM_Unpublish_Interaction_Class>&& request);
    Responses process(MessageEvent<NM_Subscribe_Interaction_Class>&& request);
    Responses process(MessageEvent<NM_Unsubscribe_Interaction_Class>&& request);
    Responses process(MessageEvent<NM_Reserve_Object_Instance_Name>&& request);
    Responses process(MessageEvent<NM_Register_Object>&& request);
    Responses process(MessageEvent<NM_Update_Attribute_Values>&& request);
    Responses process(MessageEvent<NM_Send_Interaction>&& request);
    Responses process(MessageEvent<NM_Delete_Object>&& request);
    Responses process(MessageEvent<NM_Query_Attribute_Ownership>&& request);
    Responses process(MessageEvent<NM_Negotiated_Attribute_Ownership_Divestiture>&& request);
    Responses process(MessageEvent<NM_Attribute_Ownership_Acquisition_If_Available>&& request);
    Responses process(MessageEvent<NM_Unconditional_Attribute_Ownership_Divestiture>&& request);
    Responses process(MessageEvent<NM_Attribute_Ownership_Acquisition>&& request);
    Responses process(MessageEvent<NM_Cancel_Negotiated_Attribute_Ownership_Divestiture>&& request);
    Responses process(MessageEvent<NM_Is_Attribute_Owned_By_Federate>&& request);
    Responses process(MessageEvent<NM_Attribute_Ownership_Release_Response>&& request);
    Responses process(MessageEvent<NM_Cancel_Attribute_Ownership_Acquisition>&& request);
    Responses process(MessageEvent<NM_DDM_Create_Region>&& request);
    Responses process(MessageEvent<NM_DDM_Modify_Region>&& request);
    Responses process(MessageEvent<NM_DDM_Delete_Region>&& request);
    Responses process(MessageEvent<NM_DDM_Associate_Region>&& request);
    Responses process(MessageEvent<NM_DDM_Unassociate_Region>&& request);
    Responses process(MessageEvent<NM_DDM_Subscribe_Attributes>&& request);
    Responses process(MessageEvent<NM_DDM_Unsubscribe_Attributes>&& request);
    Responses process(MessageEvent<NM_DDM_Subscribe_Interaction>&& request);
    Responses process(MessageEvent<NM_DDM_Unsubscribe_Interaction>&& request);
    Responses process(MessageEvent<NM_DDM_Register_Object>&& request);
    Responses process(MessageEvent<NM_Request_Object_Attribute_Value_Update>&& request);
    Responses process(MessageEvent<NM_Request_Class_Attribute_Value_Update>&& request);
    Responses process(MessageEvent<NM_Enable_Asynchronous_Delivery>&& request);
    Responses process(MessageEvent<NM_Disable_Asynchronous_Delivery>&& request);
    Responses process(MessageEvent<NM_Time_State_Update>&& request);

    AuditFile& my_auditServer;
    SocketServer& my_socketServer;
    HandleManager<Handle>& my_federationHandleGenerator;
    FederationsList& my_federations;

    MessageBuffer my_messageBuffer;
};
}
}

#endif // CERTI_MESSAGEPROCESSOR_HH
