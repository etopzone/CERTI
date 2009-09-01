#ifndef SEMAPHORESYSV_H
#define SEMAPHORESYSV_H

// Semaphores usefull systems includes
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/sem.h>

// Others systems includes 
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// Specifics includes
#include "Semaphore.hh"


class SemaphoreSysV : public Semaphore {
    private :
    int _Sem_Id ;

    public :
    SemaphoreSysV() ;
    ~SemaphoreSysV() ;
    void Create_Init(const int initval, const int cle) ;
    void P() ;
    void V() ;
    void Delete() ;

} ;

#endif
