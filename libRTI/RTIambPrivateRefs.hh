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
// $Id: RTIambPrivateRefs.hh,v 3.7.4.1 2009/01/05 13:34:52 gotthardp Exp $
// ----------------------------------------------------------------------------

#include "RTI.hh"
#include "fedtime.hh"
#include "Message.hh"
#include "RootObject.hh"
#include "MessageBuffer.hh"

using namespace certi ;

class RTIambPrivateRefs
{
public:
	RTIambPrivateRefs();
    ~RTIambPrivateRefs();

    void processException(Message *);
    void executeService(Message *requete, Message *reponse);
    void sendTickRequestStop();
    void callFederateAmbassador(Message *msg) throw (RTI::RTIinternalError);
    void leave(const char *msg) throw (RTI::RTIinternalError);

	#ifdef _WIN32
	  HANDLE	handle_RTIA;
	#endif

    pid_t pid_RTIA ; //!< pid associated with rtia fork (private).

    //! Federate Ambassador reference for module calls.
    RTI::FederateAmbassador *fed_amb ;

    //! used to prevent reentrant calls (see tick() and executeService()).
    bool is_reentrant ;

    RootObject *_theRootObj ;

    SocketUN *socketUn ;
    MessageBuffer msgBufSend,msgBufReceive ;
};

// $Id: RTIambPrivateRefs.hh,v 3.7.4.1 2009/01/05 13:34:52 gotthardp Exp $
