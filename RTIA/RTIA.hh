// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI
//
// CERTI is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
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
// $Id: RTIA.hh,v 3.13 2008/05/07 09:55:01 rousse Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_RTIA_HH
#define _CERTI_RTIA_HH

// Project
#include "Communications.hh"
#include "TimeManagement.hh"
#include "FederationManagement.hh"
#include "ObjectManagement.hh"
#include "OwnershipManagement.hh"
#include "DeclarationManagement.hh"
#include "DataDistribution.hh"
#include "Statistics.hh"

namespace certi {
namespace rtia {

class RTIA
{
public:
    RTIA();
    ~RTIA();

    void execute();
    void displayStatistics();

private:
    RootObject *rootObject ;

    FederationManagement *fm ;
    TimeManagement *tm ;
    Communications *comm ;
    Queues *queues ;
    ObjectManagement *om ;
    OwnershipManagement *owm ;
    DeclarationManagement *dm ;
    DataDistribution *ddm ;
    Statistics stat ;

    void saveAndRestoreStatus(Message::Type type)
        throw (SaveInProgress, RestoreInProgress);

    void processNetworkMessage(NetworkMessage *);
    void processFederateRequest(Message *);
    void chooseFederateProcessing(Message *, Message &, TypeException &)
       throw (CouldNotOpenFED,FederationExecutionAlreadyExists,ErrorReadingFED,
              FederateAlreadyExecutionMember,SaveInProgress,RestoreInProgress);
    void processOngoingTick();
};

}} // namespace certi

#endif // _CERTI_RTIA_HH

// $Id: RTIA.hh,v 3.13 2008/05/07 09:55:01 rousse Exp $
