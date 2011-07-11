#ifndef SOCKET_SHM_POSIX_H
#define SOCKET_SHM_POSIX_H

// Specifics includes
#include "SocketSHM.hh"

class CERTI_EXPORT SocketSHMPosix : public SocketSHM {
private:   

public :
    // Constructor
    SocketSHMPosix (const std::string& Socket_Name, 
                    const SHM_SIDE_t& Socket_Side,
                    const int Socket_Size ); 
    // Destructor
    virtual ~SocketSHMPosix ();
     
    void Open();
    

}; // End of --> class SocketSHM

#endif
