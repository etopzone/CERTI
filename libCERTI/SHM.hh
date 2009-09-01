#ifndef SHM_H
#define SHM_H

// Systems includes
#include <math.h>
#include <iostream>
#include <string.h>

// Correctly define BOOL type
//#ifndef __bool_true_false_are_defined
//#  define TRUE 1
//#  define FALSE 0
//#  define bool int
//#  define __bool_true_false_are_defined
//#endif

class SHM {
    protected :
    std::string _Name ;
    int  _Size ;
    bool _IsCreator ;
    void *_Shm ;

    public :
    SHM(const std::string& SHMName, const int SHMSize,const bool isCreator) {_Name=SHMName; _Size=SHMSize; _IsCreator=isCreator; }
    virtual ~SHM() {};
    const std::string getName() const {return _Name ; } ; 
    inline void* GetShm(){return _Shm ; } ;
    inline bool IsCreator(){return _IsCreator ; } ;
    inline int GetSize(){return _Size ; } ;
    virtual void Open() = 0 ;
    virtual void Attach() = 0 ;
    virtual void Close() = 0 ;
} ;

#endif
