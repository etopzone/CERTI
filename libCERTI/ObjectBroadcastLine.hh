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
// $Id: ObjectBroadcastLine.hh,v 3.2 2003/02/17 09:17:03 breholee Exp $
// ----------------------------------------------------------------------------

// Classe privee pour les listes de diffusion. Les *BroadcastLine
// decrivent une ligne des listes de diffusion.

#ifndef _CERTI_OBJECT_BROADCAST_LINE_HH
#define _CERTI_OBJECT_BROADCAST_LINE_HH

namespace certi {

class ObjectBroadcastLine {
public:
    enum State {
        sent, waiting, notSub
    };

    FederateHandle Federate ;
    State state[MAX_ATTRIBUTES_PER_CLASS + 1] ;

    ObjectBroadcastLine(FederateHandle theFederate,
                        State theInitialState = notSub);
};

}

#endif // _CERTI_OBJECT_BROADCAST_LINE_HH
