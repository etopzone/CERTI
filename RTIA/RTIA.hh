// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
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
// $Id: RTIA.hh,v 3.5 2003/04/23 17:24:08 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_RTIA_HH
#define _CERTI_RTIA_HH

// Project
#include <config.h>
#include "XmlParser.hh"
#include "Communications.hh"
#include "TimeManagement.hh"
#include "FederationManagement.hh"
#include "ObjectManagement.hh"
#include "OwnershipManagement.hh"
#include "DeclarationManagement.hh"
#include "DataDistribution.hh"
#include "FedParser.hh"
#include "PrettyDebug.hh"

// Standard libraries
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <csignal>
#include <string>
#include <fstream>
using std::ifstream ;
using std::ios ;
using std::cout ;
using std::endl ;

namespace certi {
class XmlParser ;
namespace rtia {

class RTIA
{
public:
    RTIA(void);
    ~RTIA(void);

    void execute(void);

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

    int nb_messages[18] ;
    int nb_requetes[52] ;
    int TOTAL ;
    int nb_evenements ;

    void saveAndRestoreStatus(TypeService type)
        throw (SaveInProgress, RestoreInProgress);

    void processNetworkMessage(NetworkMessage*);
    void processFederateRequest(Message*);
    void chooseFederateProcessing(Message *req, Message &rep, TypeException &e);

    void count(void);
};

}} // namespace certi

#endif // _CERTI_RTIA_HH

// $Id: RTIA.hh,v 3.5 2003/04/23 17:24:08 breholee Exp $
