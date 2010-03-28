// SHM useful systems includes
#include <windows.h>
#include <conio.h>
#include <tchar.h>

// Others Systems includes
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#include "SHMWin32.hh"

namespace libhla {
namespace ipc {
// ************************************************
// Constructor with args
// ************************************************
SHMWin32::SHMWin32(const std::string& SHMName, const int SHMSize, const bool True)  : SHM(SHMName, SHMSize, True) {
        _hMapFile = NULL ;
        _pBuf = NULL ;
}

SHMWin32::SHMWin32(const std::string& SHMName, const int SHMSize)  : SHM(SHMName,SHMSize) {
        _hMapFile = NULL ;
        _pBuf = NULL ;
}

// ************************************************
// Destructor
SHMWin32::~SHMWin32() {}

// ************************************************
// Method : SHMWin32::Open()
// ************************************************
void SHMWin32::Open() throw(certi::SharedMemoryNotOpen) {

int ret ;

   _hMapFile = CreateFileMapping(
                      INVALID_HANDLE_VALUE,                // use paging file
                      NULL,                                // default security
                      PAGE_READWRITE,                      // read/write access
                      (DWORD)0,                                   // max. object size
                      (DWORD)_Size,                        // buffer size
                      (LPCTSTR)(_Name.c_str()));           // name of mapping object

    if (_hMapFile == NULL) {
        throw(certi::SharedMemoryNotOpen("CreateFileMapping() failed.")) ;
        }

#ifdef DEBUG
std::cout <<  "Created shared memory object : " << _Name.c_str() << std::endl ;
#endif

} // End of Open()

// ************************************************
// Method : SHMWin32::Attach()
// ************************************************
void SHMWin32::Attach() throw(certi::SharedMemoryNotAttached) {

BOOL WINAPI retcode ;

_pBuf = (LPTSTR) MapViewOfFile(_hMapFile,
                   FILE_MAP_ALL_ACCESS,      // read/write permission
                   0,
                   0,
                   (SIZE_T)(GetSize()));

    if (_pBuf == NULL) {
        retcode=UnmapViewOfFile((PVOID)_pBuf);
        CloseHandle(_hMapFile);
        throw(certi::SharedMemoryNotAttached("MapViewOfFile() failed.")) ;
    }

    _Shm = (void *) _pBuf ;

} // End of Attach()

// ************************************************
// Method : SHMWin32::Close()
// ************************************************
void SHMWin32::Close() throw(certi::SharedMemoryNotClosed,
                             certi::HandleNotClosed) {

BOOL WINAPI retcode ;

// Unmap
   retcode=UnmapViewOfFile((PVOID)_pBuf);

if (retcode == 0) {
    throw(certi::SharedMemoryNotClosed("UnMapViewOfFile() failed.")) ;
    }

#ifdef DEBUG
std::cout <<  "Close SHM (UnmapViewOfFile) : " << _Name.c_str() << std::endl ;
#endif

// Close handle
   retcode = CloseHandle(_hMapFile);

if(retcode == 0){
   throw(certi::HandleNotClosed("CloseHandle() failed.")) ;
   } // End of if

} // End of Close()
} /* end namespace ipc  */
} /* end namespace libhla */
