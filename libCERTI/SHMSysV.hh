#ifndef SHM_SYSV_H
#define SHM_SYSV_H

// SHMs useful systems includes
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h>

// Others systems includes
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

// Specifics includes
#include "SHM.hh"

class SHMSysV : public SHM {

    private :
    int _Id ;  
    int _Key ; 
   
    public :
    SHMSysV(const std::string& New_Name, const int New_Key, const int New_Size, const bool It_IsCreator) ;
    ~SHMSysV() ;
    void Open() ;
    void Attach() ;
    void Close() ;

} ;






#endif
