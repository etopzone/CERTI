#include "SocketSHM.hh"
#include <cstring>

// ************************************************
// Destructor
// ************************************************
SocketSHM ::~SocketSHM (){}

// ************************************************
// Method : SocketSHM::Connect()
// ************************************************
void SocketSHM::Connect() throw (certi::SocketNotConnected) {

#ifdef DEBUG
std::cout << "Try to Connect..." << std::endl ;
#endif

if(_Side == SHM_CS){
    try {
       _Sem_full_SC->Attach(Semaphore::buildSemName(_Name+"_FULL_SC")) ;
    }
    catch(certi::SharedMemoryNotAttached& e)
    {
        std::cout << "SocketSHM::Connect() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketNotConnected("Connect() failed."));
    }
    try {
       _Sem_empty_SC->Attach(Semaphore::buildSemName(_Name+"_EMPTY_SC")) ;
    }
    catch(certi::SharedMemoryNotAttached& e)
    {
        std::cout << "SocketSHM::Connect() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketNotConnected("Connect() failed."));
    }
  }
else{
    try {
       _Sem_full_CS->Attach(Semaphore::buildSemName(_Name+"_FULL_CS")) ;
    }
    catch(certi::SharedMemoryNotAttached& e)
    {
        std::cout << "SocketSHM::Connect() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketNotConnected("Connect() failed."));
    }
    try {
       _Sem_empty_CS->Attach(Semaphore::buildSemName(_Name+"_EMPTY_CS")) ;
    }
    catch(certi::SharedMemoryNotAttached& e)
    {
        std::cout << "SocketSHM::Connect() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketNotConnected("Connect() failed."));
    }
  }

}

// ************************************************
// Method : SocketSHM::Send(...)
// ************************************************
void SocketSHM::Send(void *Buffer) throw (certi::MessageNotSent) {

#ifdef DEBUG
std::cout << "Try to Send..." << std::endl ;
#endif

if(_Side == SHM_SC){
    try {
       _Sem_empty_SC->P() ;
    }
    catch(certi::SemaphoreHandlingError& e)
    {
        std::cout << "SocketSHM::Send() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::MessageNotSent("Send() failed.")) ;
    }

    memcpy(_Shm_SC->GetShm(), Buffer, _Shm_SC->GetSize());

    try {
       _Sem_full_SC->V() ;
    }
    catch(certi::SemaphoreHandlingError& e)
    {
        std::cout << "SocketSHM::Send() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::MessageNotSent("Send() failed.")) ;
    }
    }
else{
    try {
       _Sem_empty_CS->P() ;
    }
    catch(certi::SemaphoreHandlingError& e)
    {
        std::cout << "SocketSHM::Send() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::MessageNotSent("Send() failed.")) ;
    }

     memcpy(_Shm_CS->GetShm(), Buffer, _Shm_CS->GetSize());

     try {
        _Sem_full_CS->V() ;
     }
     catch(certi::SemaphoreHandlingError& e)
     {
        std::cout << "SocketSHM::Send() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::MessageNotSent("Send() failed.")) ;
     }
    }

#ifdef DEBUG
std::cout << "Send Complete !!" << std::endl ;
#endif

} // End of Send(...)

// ************************************************
// Method : SocketSHMS::Receive(...)
// ************************************************
void SocketSHM::Receive(void *Buffer) throw (certi::MessageNotReceived) {

#ifdef DEBUG
std::cout << "Try to Receive..." << std::endl ;
#endif

if(_Side == SHM_SC){
    try {
       _Sem_full_CS->P() ;
    }
    catch(certi::SemaphoreHandlingError& e)
    {
       std::cout << "SocketSHM::Receive() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
       throw (certi::MessageNotReceived("Receive() failed.")) ;
    }

    memcpy(Buffer, _Shm_CS->GetShm(), _Shm_CS->GetSize());

    try {
       _Sem_empty_CS->V() ;
    }
    catch(certi::SemaphoreHandlingError& e)
    {
       std::cout << "SocketSHM::Receive() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
       throw (certi::MessageNotReceived("Receive() failed.")) ;
    }
    }
else{
    try {
       _Sem_full_SC->P() ;
    }
    catch(certi::SemaphoreHandlingError& e)
    {
       std::cout << "SocketSHM::Receive() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
       throw (certi::MessageNotReceived("Receive() failed.")) ;
    }

    memcpy( Buffer, _Shm_SC->GetShm(), _Shm_SC->GetSize());

    try {
       _Sem_empty_SC->V() ;
    }
    catch(certi::SemaphoreHandlingError& e)
    {
       std::cout << "SocketSHM::Receive() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
       throw (certi::MessageNotReceived("Receive() failed.")) ;
    }
    }

#ifdef DEBUG
std::cout << "Receive complete!!!" << std::endl ;
#endif

} // End of Receive(...)

// ************************************************
// Method : SocketSHM::Close()
// ************************************************
void SocketSHM::Close() throw(certi::SocketNotClosed) {

#ifdef DEBUG
std::cout << "Try to Close..." << std::endl ;
#endif

try {
   _Shm_SC->Close() ;
}
catch (certi::SharedMemoryNotClosed& e)
{
    std::cout << "SocketSHM::Close() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
    throw (certi::SocketNotClosed("Close() failed.")) ;
}
catch (certi::HandleNotClosed& e)
{
    std::cout << "SocketSHM::Close() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
    throw (certi::SocketNotClosed("Close() failed.")) ;
}
try {
   _Shm_CS->Close() ;
}
catch (certi::SharedMemoryNotClosed& e)
{
    std::cout << "SocketSHM::Close() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
    throw (certi::SocketNotClosed("Close() failed.")) ;
}
catch (certi::HandleNotClosed& e)
{
    std::cout << "SocketSHM::Close() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
    throw (certi::SocketNotClosed("Close() failed.")) ;
}

} // End of --> SocketSHM::Close()

