// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002  ONERA
//
// This file is part of CERTI-libcerti
//
// CERTI-libcerti is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libcerti is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: broadcast.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

// $Id: broadcast.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $

#ifndef _CERTI_BROADCAST_HH
#define _CERTI_BROADCAST_HH

namespace certi {

// States for a(Federate Handle, Attribute Handle) couple.
// bsNotSub means Not Subscribed.
enum BroadcastState {bsSent, bsWaiting, bsNotSub};
}

#endif // _CERTI_BROADCAST_HH

// $Id: broadcast.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
