#ifndef SHM_WIN32_H
#define SHM_WIN32_H

// Specifics includes
#include "SHM.hh"
#include "certi.hh"
#include "Exception.hh"

class CERTI_EXPORT SHMWin32 : public SHM {

    private :
        HANDLE _hMapFile ;
        LPCTSTR _pBuf;

    public :
    SHMWin32(const std::string& SHMName, const int SHMSize, const bool True) ;
    SHMWin32(const std::string& SHMName, const int SHMSize) ;
    ~SHMWin32() ;

    void Open() throw(certi::SharedMemoryNotOpen) ;
    void Attach() throw(certi::SharedMemoryNotAttached) ;
    void Close() throw(certi::SharedMemoryNotClosed,
                       certi::HandleNotClosed);

} ;

#endif
