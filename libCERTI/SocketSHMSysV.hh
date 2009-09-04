#ifndef SOCKET_SHM_SYSV_H
#define SOCKET_SHM_SYSV_H

// Specifics includes
#include "SemaphoreSysV.hh"
#include "SHMSysV.hh"
#include "SocketSHM.hh"

class SocketSHMSysV : public SocketSHM {
private:    

public :
    // Constructor
    SocketSHMSysV (const std::string& Socket_Name, 
                   const SHM_SIDE_t& Socket_Side,
                   const int Socket_size );  
    // Destructor
    ~SocketSHMSysV (); 
     
    void Open();
   
}; // End of --> class SocketSHM

#endif
