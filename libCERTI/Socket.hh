// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
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
// $Id: Socket.hh,v 3.1 2002/12/11 00:47:33 breholee Exp $
// ---------------------------------------------------------------------------

// $Id: Socket.hh,v 3.1 2002/12/11 00:47:33 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_SOCKET_HH
#define _CERTI_SOCKET_HH

#include <stdio.h>
#include "baseTypes.hh"

// Those values are returned by the GetClass method.

#define SOCKET_TYPE_TCP 0 // TCP Socket class ID
#define SOCKET_TYPE_MC 1 // Multicast Socket class ID
#define SOCKET_TYPE_UDP 2
#define SOCKET_TYPE_S_TCP 3 // Secure TCP Socket class ID

namespace certi {
// Those Exceptions will be used by all Socket classes.
RTI_EXCEPT(NetworkSignal) // Used when received a signal during system call.
  RTI_EXCEPT(NetworkError) // Used for all other errors(connection closed...)

  class Socket
  {
  public:

    // -----------------------
    // -- Read/Write Method --
    // -----------------------

    virtual void send(void *Buffer, unsigned long Size) = 0;
    virtual void receive(void *Buffer, unsigned long Size) = 0;
    virtual void close() = 0;
 
    // This method may be used for implementation using Read Buffers,
    // because in that case 'select' system calls are not trustworthy.
    // See Important Note in SocketTCP.hh
    virtual Boolean isDataReady() = 0;

    // ---------------------
    // -- Get Descriptors --
    // ---------------------

    virtual int getClass() = 0;
    virtual int returnSocket() = 0;
    virtual unsigned long returnAdress() = 0;

    // ----------------
    // -- Descructor --
    // ----------------

    virtual ~Socket() {};
  };

}

#endif // _CERTI_SOCKET_HH

// $Id: Socket.hh,v 3.1 2002/12/11 00:47:33 breholee Exp $
