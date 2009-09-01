#ifndef SHM_POSIX_H
#define SHM_POSIX_H

// SHM useful systems includes
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

// Others Systems includes
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

// Specifics includes
#include "SHM.hh"


class SHMPosix : public SHM {

    private :
    int _Id ;  // identifiant du segment memoire affecté dans la methode : Open()
   
    public :
    SHMPosix(const std::string& New_Shm_Name, const int New_Shm_Size, const bool It_IsServer) ;
    ~SHMPosix() ;

    void Open() ;
    void Attach() ;
    void Close() ;

} ;

#endif
