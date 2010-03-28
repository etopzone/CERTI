#ifndef SOCKET_SHM_H
#define SOCKET_SHM_H

#include <iostream>
// libhla includes
#include "Semaphore.hh"
#include "SHM.hh"

// Specifics includes
#include "certi.hh"
#include "Exception.hh"

class CERTI_EXPORT SocketSHM {
public :
    // Typedef Side
    typedef enum{SHM_SC,SHM_CS} SHM_SIDE_t ;

    // Constructor
    SocketSHM (const std::string& Socket_Name,
               const SHM_SIDE_t& Socket_Side,
               const int Socket_Size ) { _Name = Socket_Name ; _Side = Socket_Side ; _Size_Side = Socket_Size ; }
    // Destructor
    virtual ~SocketSHM () ;

    virtual void Connect() throw (certi::SocketNotConnected) ;

    virtual void Send(void *Buffer)
                      throw (certi::MessageNotSent) ; // To send Data on a memory segment
    virtual void Receive(void *Buffer)
                      throw (certi::MessageNotReceived) ; // To receive Data on a memory segment

    virtual void Close()
                throw(certi::SocketNotClosed) ; // To Close the two SHMs

protected :
    std::string _Name ;
    SHM_SIDE_t _Side ;
    size_t _Size_Side ;

    /***** Server -->>> Customer ******/
    libhla::ipc::SHM *_Shm_SC ;
    libhla::ipc::Semaphore *_Sem_full_SC, *_Sem_empty_SC ;

    /***** Customer -->>> Server ******/
    libhla::ipc::SHM *_Shm_CS ;
    libhla::ipc::Semaphore *_Sem_full_CS, *_Sem_empty_CS ;

}; // End of --> class SocketSHM

#endif
