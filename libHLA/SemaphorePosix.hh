#ifndef SEMAPHORE_POSIX_H
#define SEMAPHORE_POSIX_H

// Others Systems includes
#include <string>
#include <iostream>

// Specifics includes
#include "Semaphore.hh"
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L
#endif
#include <semaphore.h>

namespace libhla {
namespace ipc {

class HLA_EXPORT SemaphorePosix : public Semaphore {
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
} /* end namespace ipc  */
} /* end namespace libhla */
#endif
