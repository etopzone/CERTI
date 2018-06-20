// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
//
// This file is part of CERTI-libRTI
//
// CERTI-libRTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libRTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: RTIambPrivateRefs.h,v 1.2 2014/03/07 18:00:49 erk Exp $
// ----------------------------------------------------------------------------

#include "RTIambassadorImplementation.h"

#include <RTI/Typedefs.h>

#include "Message.hh"
#include "MessageBuffer.hh"
#include "RootObject.hh"
#include <RTI/certiRTI1516.h>

namespace certi {

struct RTI1516ambassador::Private {
    /** Process exception from received message.
     * When a message is received from RTIA, it can contains an exception.
     * This exception is processed by this module and a new exception is thrown.
     */
    void processException(Message* msg);
    void executeService(Message* requete, Message* reponse);
    void sendTickRequestStop();
    void callFederateAmbassador(Message* msg);
    void leave(const char* msg);

#ifdef _WIN32
    HANDLE handle_RTIA;
#else
    /// pid associated with rtia fork (private).
    pid_t pid_RTIA{-1};
#endif

    /// Federate Ambassador reference for module calls.
    rti1516e::FederateAmbassador* fed_amb{nullptr};

    /// used to prevent reentrant calls (see tick() and executeService()).
    bool is_reentrant{false};

    RootObject* root_object{nullptr};

    std::unique_ptr<SocketUN> socket_un{nullptr};
    MessageBuffer msgBufSend, msgBufReceive;
};
}
