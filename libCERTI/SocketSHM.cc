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
// $Id: SocketSHM.cc,v 1.1.2.1 2009/07/08 14:45:39 erk Exp $
// ----------------------------------------------------------------------------

#include "SocketSHM.hh"

namespace certi {

static pdCDebug D("SOCKSHM", "(SocketSHM) - ");


// ----------------------------------------------------------------------------
//! accept.
int SocketSHM::accept(SocketTCP *serveur)
{
  long _socket_tcp ;

  struct sockaddr_in _sockIn ;
    struct protoent *TCPent ;
    int optval = 1 ;
    socklen_t l ;

    assert(serveur != NULL);

    l = sizeof(_sockIn);

    _socket_tcp = ::accept(serveur->returnSocket(), (sockaddr*)&_sockIn, &l);
    //HPUX: _socket_tcp = accept(serveur->_socket_tcp, (sockaddr*)&_sockIn,
    //(int*) &l);
    if (_socket_tcp < 0) {
        perror("SocketTCP: Accept");
        exit(-1);
    }

    // Set the TCP_NODELAY option(Server Side)
    TCPent = getprotobyname("tcp");
    if (TCPent == NULL) {
        cout << "Unable to retrieve TCP protocol number." << endl ;
        return 0 ;
    }

    if (setsockopt(_socket_tcp,
                   TCPent->p_proto,
                   TCP_NODELAY,
                   (char *) &optval,
                   sizeof(optval))) {
        cout << "Error while calling setsockopt." << endl ;
        return 0 ;
    }
  D.Out(pdTrace, "Acceptation d'une connexion cote serveur") ;
  isServer = true;
  if ((shmId = shmget (IPC_PRIVATE, sizeof(SHM),IPC_CREAT|0666)) == -1)
       { perror ("shmget"); exit (EXIT_FAILURE);}
  D.Out(pdTrace, "Creating shared memory with id = %d", shmId) ;

       /* attachement a une adresse non fixee */
  
  if ((shm = (SHM *) shmat(shmId, 0 ,0)) == (SHM *) -1)
  { perror ("shmat"); exit (EXIT_FAILURE);}
  D.Out(pdTrace, "Shared memory attached") ;
  
   
  shm -> S_C.qui = EMPTY ;                 // le segment est libre en écriture
  shm -> C_S.qui = EMPTY ; 
  
  int ctrl;
  write(_socket_tcp,&shmId,(unsigned) sizeof(int));
  read(_socket_tcp,&ctrl,(unsigned) sizeof(int));
  if(ctrl != shmId) cout<< "erreur de transmission de l'identifiant shm";
  ::close(_socket_tcp);
  
  return shmId;
}

// ----------------------------------------------------------------------------
//! createSHMClient
void
SocketSHM::createSHMClient(unsigned int port, char *nom_serveur){
  long _socket_tcp ;

  struct sockaddr_in _sockIn ;
  
  
  D.Out(pdTrace, "Creation socket cote client") ;
  
  _socket_tcp=socket(AF_INET, SOCK_STREAM, 0);
  
  D.Out(pdTrace, "Recuperation des infos sur le serveur") ;
  // recuperer les infos sur le serveur a partir du nom
  struct hostent *hptr = gethostbyname(nom_serveur);
  if (hptr == 0) {
      perror("SocketTCP: gethostbyname");
      exit(-1);
  }

  unsigned long addr = 0 ;
  memcpy((void *) &addr, (void *) hptr->h_addr, hptr->h_length);
  
    
  int Result ;
  struct protoent *TCPent ;
  int optval = 1 ;
  
  _sockIn.sin_family=AF_INET ;
  _sockIn.sin_port=htons(port);
  _sockIn.sin_addr.s_addr=addr ;

  D.Out(pdTrace, "Tentative de connexion") ;
  Result = ::connect(_socket_tcp, (sockaddr*)&_sockIn, sizeof(_sockIn));

  if (Result < 0)
      return  ;

  // Set the TCP_NODELAY option(Client Side)

  TCPent = getprotobyname("tcp");
  if (TCPent == NULL) {
      cout << "Unable to retrieve TCP protocol number." << endl ;
      return  ;
  }

  if (setsockopt(_socket_tcp,
                 TCPent->p_proto,
                 TCP_NODELAY,
                 (char *) &optval,
                 sizeof(optval))) {
      cout << "Error while calling setsockopt." << endl ;
      return  ;
  }
  D.Out(pdTrace, "Lecture des informations de connexion") ;
  read(_socket_tcp,&shmId,(unsigned) sizeof(int));
  write(_socket_tcp,&shmId,(unsigned) sizeof(int));
  ::close(_socket_tcp);
  
  D.Out(pdTrace, "Attachement de la memoire") ;
  isServer = false;
  //shmId = id_shm;
  if ((shm = (SHM *) shmat(shmId, 0 ,0)) == (SHM *) -1)
  { perror ("shmat"); exit (EXIT_FAILURE);}
  D.Out(pdTrace, "Shared memory attached with id = %d", shmId) ;
}


// ----------------------------------------------------------------------------
//! Constructor.
SocketSHM::SocketSHM(void)
{
}
// ----------------------------------------------------------------------------
//! Destructor.
SocketSHM::~SocketSHM(void)
{
    // Fermeture
    //if (_est_init_tcp)
        close();
/*
#ifdef RTI_PRINTS_STATISTICS
    cout << endl ;
    cout << "TCP Socket(" << _socket_tcp << "): Total Sent Bytes : "
         << SentBytesCount << '.' << endl ;
    cout << "TCP Socket(" << _socket_tcp << "): Total Received Bytes : "
         << RcvdBytesCount << '.' << endl ;
    cout << endl ;
#endif
*/
}

// ----------------------------------------------------------------------------
//! send.
void SocketSHM::send(void *Buffer, unsigned long Size)
{
  if(isServer){
    D.Out(pdTrace, "Ecriture sur SHM %d",shmId) ;
    D.Out(pdTrace, "Socket bloque en ecriture cote serveur") ;
    while(shm->S_C.qui==FULL)D.Out(pdTrace, "S_C.qui : %c",shm->S_C.qui);
    D.Out(pdTrace, "Ecriture debloque cote serveur : writing Header") ;
    shm->S_C.Header = ((SharedMemory *)Buffer)->Header;
    D.Out(pdTrace, "writing body") ;
    shm->S_C.Body = ((SharedMemory *)Buffer)->Body;
    shm->S_C.qui=FULL;
    D.Out(pdTrace, "S_C.qui positionne a : %c",shm->S_C.qui) ;
  }
  else{
    D.Out(pdTrace, "Ecriture sur SHM %d",shmId) ;
    D.Out(pdTrace, "Socket bloque en Ecriture cote client") ;
    int result=shm->C_S.qui;
    while(result==FULL)result=shm->C_S.qui;
    D.Out(pdTrace, "Ecriture debloque cote client : writing Header") ;
    shm->C_S.Header = ((SharedMemory *)Buffer)->Header;
    D.Out(pdTrace, "writing body") ;
    shm->C_S.Body = ((SharedMemory *)Buffer)->Body;
    shm->C_S.qui=FULL;
    D.Out(pdTrace, "C_S.qui positionne a : %c",shm->C_S.qui) ;
  }
    /*long nSent = 0 ;
    long total_sent = 0 ;
    long expected_size = Size ;

    assert(_est_init_tcp);

    D.Out(pdDebug, "Beginning to send SHM message...");

    while (total_sent < expected_size) { 
        nSent = ::send(_socket_tcp,
                       (char *) Buffer + total_sent,
                       expected_size - total_sent,
                       0);

        if (nSent < 0) {
            D.Out(pdExcept, "Error while sending on TCP socket.");
            if (errno == EINTR)
                throw NetworkSignal();
            else {
                perror("TCP Socket(EmettreTCP) ");
                throw NetworkError("Error while sending TCP message.");
            }
        }

        if (nSent == 0) {
            D.Out(pdExcept, "No data could be sent, connection closed?.");
            throw NetworkError("Could not send any data on TCP socket.");
        }

        total_sent += nSent ;
        D.Out(pdTrace, "Sent %ld bytes out of %ld.", total_sent, expected_size);
    }

    SentBytesCount += total_sent ;*/
}

// ----------------------------------------------------------------------------
//! close.
void SocketSHM::close(void)
{
    if(isServer){
      /* detachement des segments memoire*/
      if(shmdt(shm)){
        perror("Echec du shmdt car : ");
        exit(1);
      }
      D.Out(pdTrace, "Memoire detachee cotee serveur") ;
  
  
      /*Destruction des segments memoire */
      if(shmctl(shmId, IPC_RMID,0)){
        perror("Echec de shmctl(IPC_RMID) car : ");
        exit(1);
      }
      D.Out(pdTrace, "Memoire detruite") ;
    }
    else{
      /* detachement des segments memoire*/
      if(shmdt(shm)){
        perror("Echec du shmdt car : ");
        exit(1);
      }
      D.Out(pdTrace, "Memoire detachee cotee client") ;
    }
}


// ----------------------------------------------------------------------------
/*! Return RTI_TRUE if any data as already been read from the system socket
  and is waiting in the internal buffer, else RTI_FALSE.
*/
Boolean
SocketSHM::isDataReady(void) const
{
  if(isServer){
    if (shm->C_S.qui == FULL) return RTI_TRUE;
    else return RTI_FALSE;
  }
  else{
    if (shm->S_C.qui == FULL) return RTI_TRUE;
    else return RTI_FALSE;
  }
}


// ----------------------------------------------------------------------------
//! receive.
void SocketSHM::receive(void *Buffer, unsigned long Size)
{
  if(isServer){
    D.Out(pdTrace, "Lecture sur SHM %d",shmId) ;
    D.Out(pdTrace, "Socket bloque en lecture cote serveur") ;
    while(shm->C_S.qui==EMPTY) {
        D.Out(pdTrace, "C_S.qui : %c",shm->C_S.qui);
        if (sched_yield ())  {
            D.Out(pdError, " SocketSHM::receive : sched_yield problem"); exit (-1) ;
        }

    }
    D.Out(pdTrace, "Lecture debloque cote serveur : reading Header") ;
    ((SharedMemory *)Buffer)->Header = shm->C_S.Header;
    D.Out(pdTrace, "reading body") ;
    ((SharedMemory *)Buffer)->Body = shm->C_S.Body;
    shm->C_S.qui=EMPTY;
    D.Out(pdTrace, "C_S.qui positionne a : %c",shm->C_S.qui) ;
  }
  else{
    D.Out(pdTrace, "Lecture sur SHM %d",shmId) ;
    D.Out(pdTrace, "Socket bloque en lecture cote client") ;
    int result=shm->S_C.qui;
    while(result==EMPTY) {  
        if (sched_yield ())  {
            D.Out(pdError, " SocketSHM::receive : sched_yield problem"); exit (-1) ;
        }
        
        result=shm->S_C.qui;
    }
    D.Out(pdTrace, "Lecture debloque cote client : reading Header") ;
    ((SharedMemory *)Buffer)->Header = shm->S_C.Header;
    D.Out(pdTrace, "reading body") ;
    ((SharedMemory *)Buffer)->Body = shm->S_C.Body;
    shm->S_C.qui=EMPTY;
    D.Out(pdTrace, "S_C.qui positionne a : %c",shm->S_C.qui) ;
  }
    
    /*
    assert(_est_init_tcp);

    long nReceived = 0 ;
#ifndef SOCKTCP_BUFFER_LENGTH
    long RBLength = 0 ;
#endif

    D.Out(pdDebug, "Beginning to receive TCP message...");

    while (RBLength < Size)
        {

#ifdef SOCKTCP_BUFFER_LENGTH
            nReceived = recv(_socket_tcp,
                             ReadBuffer + RBLength,
                             SOCKTCP_BUFFER_LENGTH - RBLength,
                             0);
#else
            nReceived = recv(_socket_tcp,
                             (char *) Buffer + RBLength,
                             Size - RBLength,
                             0);
#endif

            if (nReceived < 0) {
                D.Out(pdExcept, "Error while receiving on TCP socket.");
                if (errno == EINTR)
                    throw NetworkSignal();
                else {
                    perror("TCP Socket(RecevoirTCP) ");
                    throw NetworkError("Error while receiving TCP message.");
                }
            }

            if (nReceived == 0) {
                D.Out(pdExcept, "TCP connection has been closed by peer.");
                throw NetworkError("Connection closed by client.");
            }

            RBLength += nReceived ;
            RcvdBytesCount += nReceived ;

            D.Out(pdTrace, "Received %ld bytes out of %ld.", RBLength, Size);
        }

#ifdef SOCKTCP_BUFFER_LENGTH
    memcpy(Buffer, (void *) ReadBuffer, Size);
    memmove((void *) ReadBuffer,
            (void *)(ReadBuffer + Size),
            RBLength - Size);
    RBLength -= Size ;
#endif
*/
}

// ----------------------------------------------------------------------------
//! Returns the address.
unsigned long
SocketSHM::returnAdress(void) const
{
    return (unsigned long) shmId;
}

// ----------------------------------------------------------------------------
//! Returns the socket.
int
SocketSHM::returnSocket(void) const
{
    return  shmId;
}

}

// $Id: SocketSHM.cc,v 1.1.2.1 2009/07/08 14:45:39 erk Exp $
