#ifndef SOCKET_SHM_H
#define SOCKET_SHM_H

#include <iostream>
// libhla includes
#include <libHLA/SHM.hh>
#include <libHLA/Semaphore.hh>

// Specifics includes
#include "Exception.hh"
#include <include/certi.hh>

class CERTI_EXPORT SocketSHM {
public:
    // Typedef Side
    typedef enum { SHM_SC, SHM_CS } SHM_SIDE_t;

    // Constructor
    SocketSHM(const std::string& Socket_Name, const SHM_SIDE_t& Socket_Side, const int Socket_Size)
    {
        _Name = Socket_Name;
        _Side = Socket_Side;
        _Size_Side = Socket_Size;
    }
    // Destructor
    virtual ~SocketSHM();

    virtual void Connect();

    virtual void Send(void* Buffer); // To send Data on a memory segment
    virtual void Receive(void* Buffer); // To receive Data on a memory segment

    virtual void Close(); // To Close the two SHMs

protected:
    std::string _Name;
    SHM_SIDE_t _Side;
    size_t _Size_Side;

    /***** Server -->>> Customer ******/
    libhla::ipc::SHM* _Shm_SC;
    libhla::ipc::Semaphore *_Sem_full_SC, *_Sem_empty_SC;

    /***** Customer -->>> Server ******/
    libhla::ipc::SHM* _Shm_CS;
    libhla::ipc::Semaphore *_Sem_full_CS, *_Sem_empty_CS;

}; // End of --> class SocketSHM

#endif
