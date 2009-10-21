#ifndef SEMAPHOREWIN32_H
#define SEMAPHOREWIN32_H

// Semaphores usefull systems includes
#include <windows.h>
#include <conio.h>
#include <tchar.h>

// Others systems includes
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>

// Specifics includes
#include "certi.hh"
#include "Semaphore.hh"


class CERTI_EXPORT SemaphoreWin32 : public Semaphore {
    private :
        HANDLE WINAPI _hSemaphore;

    public :
    SemaphoreWin32() ;
    virtual ~SemaphoreWin32() ;
    void Create_Init(const int initval, const std::string& New_Semname)
                     throw(certi::SemaphoreNotCreated) ;
    void Attach(const std::string& New_Semname )
                throw(certi::SemaphoreNotOpen) ;
    void P()
         throw(certi::SemaphoreHandlingError) ;
    void V()
         throw(certi::SemaphoreHandlingError) ;
    void Delete()
         throw(certi::HandleNotClosed) ;

} ;

#endif
