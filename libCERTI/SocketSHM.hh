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
// $Id: SocketSHM.hh,v 1.1.2.1 2009/07/08 14:45:39 erk Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_SOCKET_SHM_HH
#define _CERTI_SOCKET_SHM_HH

#include <config.h>

#include <iostream>
using std::cout ;
using std::endl ;

#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <cstring>
#include <ulimit.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/tcp.h>

#include "Socket.hh"
#include "SocketTCP.hh"
#include "RTItypes.hh"
#include "PrettyDebug.hh"

#include "MessageBody.hh"
#include "NetworkMessage.hh"

// This is the read buffer of TCP sockets. It must be at least as long
// as the longest data ever received by a socket.
// If the next line is commented out, no buffer will be used at all.
#define SOCKSHM_BUFFER_LENGTH 4096

// Taille de la chaine de caractères du message
#define TAILLE_MESSAGE 256

// Constantes full and empty

#define FULL '1'
#define EMPTY '0'

// Nombre max de clients
#define MAX_CLIENTS 100

#define MAX_EXCEPTION_REASON_LENGTH 100

#define SOCKET_TYPE_SHM 4

namespace certi {
/*
//  Shared Memory de communication entre client et serveur

typedef struct {
    char qui ;       // char de synchronisation
    HeaderStruct Header ;
    MessageBody Body ;
} SharedMemory ;

typedef struct {
   SharedMemory S_C ;       // Zone serveur vers client
   SharedMemory C_S ;       // Zone client vers serveur
} SHM ;
*/

/*! IMPORTANT NOTE: This TCP socket implementation uses a Read Buffer to
  improve global read performances(by reducing Recv system calls). An
  important drawback of this improvement is that a socket can be marked as
  empty for the system, but in fact there is data waiting in the read
  buffer. This is especially a problem for processes using the 'select'
  system call: the socket won't be marked as ready for reading, because all
  data has already been read, and is waiting in the internal buffer.
  Therefore, before returning to a select loop, be sure to call the
  IsDataReady method to check whether any data is waiting for processing.
*/
class SocketSHM : public Socket
{
public :
    // ---------------------------------------------
    // -- Fonctions heritee de la classe Socket --
    // ---------------------------------------------

    void send(void *Buffer, unsigned long Size);

    void receive(void *Buffer, unsigned long Size);

    Boolean isDataReady(void) const ;

    int getClass(void) const { return SOCKET_TYPE_SHM ; };
    int returnSocket(void) const ;

    unsigned long returnAdress(void) const ;

    void close(void);
    
    // --------------------------
    // -- SHM Specific Methods --
    // --------------------------

    SocketSHM(void);
    ~SocketSHM(void);
    
    int accept(SocketTCP *serveur);
    
    void createSHMClient(unsigned int port, char *nom_serveur);
    /*
    int accept(SocketTCP *serveur);

    void createTCPClient(unsigned int port, char *nom_serveur);
    void createTCPClient(unsigned int port, unsigned long addr);
    void createTCPServer(unsigned int port = 0, unsigned long addr = INADDR_ANY);

    SocketTCP & operator= (SocketTCP &theSocket);
    */
    
private:
    
    // ------------------------
    // -- Private Attributes --
    // ------------------------

    SHM  * shm;
    int shmId;     //identifiant du segment
    
    bool isServer;
    
    
    /*
    long _socket_tcp ;

    struct sockaddr_in _sockIn ;
    Boolean _est_init_tcp ;

    unsigned long long SentBytesCount ;
    unsigned long long RcvdBytesCount ;

#ifdef SOCKTCP_BUFFER_LENGTH
    // This class can use a buffer to reduce the number of systems calls
    // when reading a lot of small amouts of data. Each time a Receive
    // is made, it will try to read SOCKTCP_BUFFER_LENGTH

    char ReadBuffer[SOCKTCP_BUFFER_LENGTH] ;
    unsigned long RBLength ;
#endif

    // ---------------------
    // -- Private Methods --
    // ---------------------

    unsigned int getPort(void) const ;
    unsigned long getAddr(void) const ;
    void setPort(unsigned int port);

    int bind(unsigned int port=0, unsigned long addr=INADDR_ANY);
    void changeReuseOption();
    int connect(unsigned int port, unsigned long addr);
    int listen(unsigned long howMuch=5);
    int open(void);
    int timeoutTCP(int, int);
*/
};
}

#endif // _CERTI_SOCKET_SHM_HH

// $Id: SocketSHM.hh,v 1.1.2.1 2009/07/08 14:45:39 erk Exp $
