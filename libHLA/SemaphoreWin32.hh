#ifndef SEMAPHOREWIN32_H
#define SEMAPHOREWIN32_H

// Semaphores usefull systems includes
#include <windows.h>
#include <conio.h>
#include <tchar.h>

// Others systems includes
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// Specifics includes
#include "Semaphore.hh"

namespace libhla {
namespace ipc {

class HLA_EXPORT SemaphoreWin32 : public Semaphore {
    private :
        HANDLE _hSemaphore;

    public :
    SemaphoreWin32() ;
    virtual ~SemaphoreWin32() ;
    void Create_Init(const int initval, const std::string& New_Semname)
                     throw(SemaphoreNotCreated) ;
    void Attach(const std::string& New_Semname )
                throw(SemaphoreNotOpen) ;
    void P()
         throw(SemaphoreHandlingError) ;
    void V()
         throw(SemaphoreHandlingError) ;
    void Delete()
         throw(HandleNotClosed) ;

} ;
} /* end namespace ipc  */
} /* end namespace libhla */
#endif
