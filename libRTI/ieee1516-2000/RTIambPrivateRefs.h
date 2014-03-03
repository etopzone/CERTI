// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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
// $Id: RTIambPrivateRefs.h,v 1.1 2014/03/03 16:41:48 erk Exp $
// ----------------------------------------------------------------------------

#include <RTI/certiRTI1516.h>
#include "Message.hh"
#include "RootObject.hh"
#include "MessageBuffer.hh"

using namespace certi ;

class RTI1516ambPrivateRefs
{
public:
	RTI1516ambPrivateRefs();
    ~RTI1516ambPrivateRefs();

    void processException(Message *);
    void executeService(Message *requete, Message *reponse);
    void sendTickRequestStop();
    void callFederateAmbassador(Message *msg) throw (rti1516::RTIinternalError);
    void leave(const char *msg) throw (rti1516::RTIinternalError);

#ifdef _WIN32
	  HANDLE	handle_RTIA;
#else
    pid_t pid_RTIA ; //!< pid associated with rtia fork (private).
#endif

    //! Federate Ambassador reference for module calls.
    rti1516::FederateAmbassador *fed_amb ;

    //! used to prevent reentrant calls (see tick() and executeService()).
    bool is_reentrant ;

    RootObject *_theRootObj ;

    SocketUN *socketUn ;
    MessageBuffer msgBufSend,msgBufReceive ;
};

// $Id: RTIambPrivateRefs.h,v 1.1 2014/03/03 16:41:48 erk Exp $
