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
// $Id: SocketUN.hh,v 3.1 2002/12/11 00:47:34 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_SOCKET_UN_HH
#define _CERTI_SOCKET_UN_HH

#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <assert.h>
#include <errno.h>

#include "Socket.hh"
#include "PrettyDebug.hh"
#include "RTItypes.hh"

#define NOM_FICHIER_SOCKET "ComFedRTI"

namespace certi {
// Signal Handler Types for a UNIX socket : - stSignalInterrupt :
// return when read/write operation is interrupted by a signal. The
// RW operation may not be complete.  - stSignalIgnore : Ignore
// signal interruption and continue to perform the RW operation till
// its done.  LibRTI will use the stSignalIgnore, RTIA will use
// stSignalInterrupt.
typedef enum {stSignalInterrupt, stIgnoreSignal} SignalHandlerType;

// This is the read buffer of UNIX sockets. It must be at least as
// long as the longest data ever received by a socket.  If the next
// line is commented out, no buffer will be used at all.
#define SOCKUN_BUFFER_LENGTH 4096

// IMPORTANT NOTE: This UNIX socket implementation uses a Read
// Buffer to improve global read performances(by reducing Read
// system calls).  An important drawback of this improvement is that
// a socket can be marked as empty for the system, but in fact there
// is data waiting in the read buffer. This is especially a problem
// for processes using the 'select' system call: the socket won't be
// marked as ready for reading, because all data has already been
// read, and is waiting in the internal buffer. Therefore, before
// returning to a select loop, be sure to call the IsDataReady
// method to check whether any data is waiting for processing.

class SocketUN
{
public:
 
  // ------------------------------
  // -- Constructor / Destructor --
  // ------------------------------

  // Does not open the socket. See Init methods below.
  SocketUN(SignalHandlerType theType = stSignalInterrupt);

  ~SocketUN(); // Close the socket

  // ----------------------------
  // -- Initialization Methods --
  // ----------------------------

  // Called by client to connect
  void connectUN(pid_t Server_pid);

  // Called by server to open the socket and wait for the connection.
  void acceptUN();


  // -----------------
  // -- R/W Methods --
  // -----------------

  // Return RTI_TRUE if any data as already been read from the system
  // socket and is waiting in the internal buffer, else RTI_FALSE.
  Boolean isDataReady();

  void send(void *Buffer, unsigned long Size)
    throw(NetworkError,
	  NetworkSignal);
 
  void receive(void *Buffer, unsigned long Size)
    throw(NetworkError,
	  NetworkSignal);

protected:
  void error(const char*);

  int _socket_un;

  Boolean _est_serveur;
  Boolean _est_init_un;

  SignalHandlerType HandlerType;

  char SocketName[108];

private:

  // ------------------------
  // -- Private Attributes --
  // ------------------------

  unsigned long long SentBytesCount;
  unsigned long long RcvdBytesCount;
 
  // la socket du serveur RTIA qui a ete cree par le federe-client
  int sock_connect;

  pdCDebug *pD;

 
#ifdef SOCKUN_BUFFER_LENGTH
  // This class can use a buffer to reduce the number of systems
  // calls when reading a lot of small amouts of data. Each time a
  // Receive is made, it will try to read SOCKUN_BUFFER_LENGTH

  char ReadBuffer[SOCKUN_BUFFER_LENGTH];
  unsigned long RBLength;
#endif
};
}

#endif // _CERTI_SOCKET_UN_HH

// $Id: SocketUN.hh,v 3.1 2002/12/11 00:47:34 breholee Exp $
