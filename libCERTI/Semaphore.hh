#ifndef SEMAPHORE_H
#define SEMAPHORE_H

// Systems includes
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


class Semaphore {
    private :
    // int _Sem_Id ;

    public :
    // Semaphore() ;
    virtual ~Semaphore() {};
    // virtual void Sem_create_init(int initval, char * semname) = 0 ;
    // virtual void Sem_create(key_t cle) = 0 ;
    // virtual void Sem_init(int initval) = 0 ;
    virtual void P() = 0 ;
    virtual void V() = 0 ;
    virtual void Delete() = 0 ;

} ;

#endif
