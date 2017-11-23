#ifndef SOCKET_SHM_WIN32_H
#define SOCKET_SHM_WIN32_H

// Specifics includes
#include <libHLA/SemaphoreWin32.hh>
#include <libHLA/SHMWin32.hh>
#include "SocketSHM.hh"
#include <include/certi.hh>

class CERTI_EXPORT SocketSHMWin32 : public SocketSHM {
private:
public:
    // Constructor
    SocketSHMWin32(const std::string& Socket_Name, const SocketSHM::SHM_SIDE_t& Socket_Side, const int Socket_size);
    // Destructor
    virtual ~SocketSHMWin32();

    void Open() throw(certi::SocketSHMNotOpen);

}; // End of --> class SocketSHM

#endif
