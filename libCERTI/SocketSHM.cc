#include "SocketSHM.hh"
#include <cstring>

// ************************************************
// Destructor
// ************************************************
SocketSHM ::~SocketSHM (){}

// ************************************************
// Method : SocketSHM::Connect()
// ************************************************
void SocketSHM::Connect() {

if(_Side == SHM_CS){
  _Sem_full_SC->Attach(Semaphore::buildSemName(_Name+"_FULL_SC")) ;
  _Sem_empty_SC->Attach(Semaphore::buildSemName(_Name+"_EMPTY_SC")) ;
  }
else{
  _Sem_full_CS->Attach(Semaphore::buildSemName(_Name+"_FULL_CS")) ;
  _Sem_empty_CS->Attach(Semaphore::buildSemName(_Name+"_EMPTY_CS")) ;
  }

}

// ************************************************
// Method : SocketSHM::Send(...)
// ************************************************
void SocketSHM::Send(void *Buffer) {

#ifdef DEBUG
std::cout << "Try to Send..." << std::endl ;
#endif

if(_Side == SHM_SC){
     _Sem_empty_SC->P() ;     
    memcpy(_Shm_SC->GetShm(), Buffer, _Shm_SC->GetSize());
     _Sem_full_SC->V() ;
     } 
else{
     _Sem_empty_CS->P() ;  
     memcpy(_Shm_CS->GetShm(), Buffer, _Shm_CS->GetSize());
     _Sem_full_CS->V() ;
     } 

#ifdef DEBUG
std::cout << "Send Complete !!" << std::endl ;
#endif

} // End of Send(...)

// ************************************************
// Method : SocketSHMS::Receive(...)
// ************************************************
void SocketSHM::Receive(void *Buffer) {

#ifdef DEBUG
std::cout << "Try to Receive..." << std::endl ;
#endif

if(_Side == SHM_SC){
     _Sem_full_CS->P() ; 
    memcpy(Buffer, _Shm_CS->GetShm(), _Shm_CS->GetSize());
     _Sem_empty_CS->V() ;
     } 
else{
     _Sem_full_SC->P() ;  
     memcpy( Buffer, _Shm_SC->GetShm(), _Shm_SC->GetSize());
    _Sem_empty_SC->V() ;
     } 

#ifdef DEBUG
std::cout << "Receive complete!!!" << std::endl ;
#endif

} // End of Receive(...)

// ************************************************
// Method : SocketSHM::Close()
// ************************************************
void SocketSHM::Close() {

_Shm_SC->Close() ;
_Shm_CS->Close() ;

} // End of --> SocketSHM::Close()

