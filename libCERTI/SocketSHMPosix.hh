#ifndef SOCKET_SHM_POSIX_H
#define SOCKET_SHM_POSIX_H

// Specifics includes
#include "SemaphorePosix.hh"
#include "SHMPosix.hh"
#include "SocketSHM.hh"
#include "SHM.hh"
#include "Semaphore.hh"

class SocketSHMPosix : public SocketSHM {
private:   

public :
    // Constructor
    SocketSHMPosix (const std::string& Socket_Name, 
                    const SHM_SIDE_t& Socket_Side,
                    const int Socket_Size ); 
    // Destructor
    ~SocketSHMPosix (); 
     
    void Open();
    

}; // End of --> class SocketSHM

#endif
