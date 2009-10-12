#ifndef SEMAPHORE_POSIX_H
#define SEMAPHORE_POSIX_H

// Posix Semaphores systems includes
// Don't forget to add "-lrt" to compilation line
#include <sys/types.h>
#include <Semaphore.hh>
#include <fcntl.h>

// Others Systems includes
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

// Specifics includes
#include "Semaphore.hh"

class SemaphorePosix : public Semaphore {
    private :
    std::string _Semname ;
    sem_t *_Sem ;

    public :
    SemaphorePosix() ;
    virtual ~SemaphorePosix() ;
    void Create_Init(const int initval, const std::string& New_Semname ) ;
    void Attach(const std::string& New_Semname ) ;
    void P() ;
    void V() ;
    void Delete() ;

} ;

#endif
