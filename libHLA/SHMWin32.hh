#ifndef SHM_WIN32_H
#define SHM_WIN32_H

// Specifics includes
#include "SHM.hh"

namespace libhla {
namespace ipc {

class HLA_EXPORT SHMWin32 : public SHM {

    private :
        HANDLE _hMapFile ;
        LPCTSTR _pBuf;

    public :
    SHMWin32(const std::string& SHMName, const int SHMSize, const bool True) ;
    SHMWin32(const std::string& SHMName, const int SHMSize) ;
    virtual ~SHMWin32() ;

    void Open() throw(SharedMemoryNotOpen) ;
    void Attach() throw(SharedMemoryNotAttached) ;
    void Close() throw(SharedMemoryNotClosed,
                       HandleNotClosed);

} ;
} /* end namespace ipc  */
} /* end namespace libhla */
#endif
