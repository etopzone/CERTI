#ifndef SOCKET_SHM_SYSV_H
#define SOCKET_SHM_SYSV_H

// Specifics includes
#include <libHLA/SHMSysV.hh>
#include <libHLA/SemaphoreSysV.hh>
#include "SocketSHM.hh"

class CERTI_EXPORT SocketSHMSysV : public SocketSHM {
private:
public:
    // Constructor
    SocketSHMSysV(const std::string& Socket_Name, const SHM_SIDE_t& Socket_Side, const int Socket_Size);
    // Destructor
    virtual ~SocketSHMSysV();

    void Open();

}; // End of --> class SocketSHM

#endif
