// Posix Semaphores systems includes
// Don't forget to add "-lrt" to compilation line
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <Semaphore.hh>
#include <fcntl.h>

// Others Systems includes
#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <iostream>

// Specifics includes
#include <semaphore.h>
#include "SemaphorePosix.hh"

namespace libhla {
namespace ipc {

// ************************************************
// Constructor
// ************************************************
SemaphorePosix::SemaphorePosix(){
_Sem = NULL ;
}

// ************************************************
// Destructor
// ************************************************
SemaphorePosix::~SemaphorePosix() {
if (_Sem!=NULL) {
  Delete() ;
}
}

// ************************************************
// Method : SemaphorePosix::Create_Init(...)
// ************************************************
void SemaphorePosix::Create_Init(const int initval, const std::string& New_Semname) {

 _Semname.assign(New_Semname) ;
sem_unlink(_Semname.c_str()) ;
_Sem = sem_open( _Semname.c_str(), O_CREAT | O_EXCL , S_IRUSR | S_IWUSR ,(int)initval) ;
if(_Sem == SEM_FAILED) {
     perror("Error with sem_open() in SemaphorePosix::Create_Init(...)") ; 
     }

#ifdef DEBUG
int sem_val ;
sem_getvalue(_Sem, &sem_val) ; 
std::cout << "We create the semaphore identified by name : " << _Semname.c_str() << " with an init value : " <<  sem_val << std ::endl ; 
#endif

} // End of Create_Init(...)

// ************************************************
// Method : SemaphorePosix::Attach(...)
// ************************************************
void SemaphorePosix::Attach(const std::string& New_Semname) {

 _Semname.assign(New_Semname) ;
_Sem = sem_open( New_Semname.c_str(), O_CREAT ) ;  
if(_Sem == SEM_FAILED){
   perror("Error with sem_open() in SemaphorePosix::Attach(...)") ; 
   } 

#ifdef DEBUG
int sem_val ;
sem_getvalue(_Sem, &sem_val) ; 
std::cout << "We attach the semaphore identified by name : " << _Semname.c_str() << " which have the value : " <<  sem_val << std ::endl ; 
#endif
} // End of Attach(...)

// ************************************************
// Method : SemaphorePosix::P()
// ************************************************
// Decrement Semaphore Value SV=SV-1 

void SemaphorePosix::P() {

#ifdef DEBUG
int sem_val ;
sem_getvalue(_Sem, &sem_val) ; 
std::cout << "Begin of Operation P for the semaphore identified by name : " << _Semname << " which have the value : " << sem_val << std::endl ;
#endif

sem_wait(_Sem) ;

#ifdef DEBUG
sem_getvalue(_Sem, &sem_val) ;
std::cout << "End of Operation P for the semaphore identified by name : " << _Semname << " which have the value : " << sem_val << std::endl ; 
#endif

} // End of P()


// ************************************************
// Methode : SemaphorePosix::V()
// ************************************************
// Increment Semaphore Value SV=SV+1 

void SemaphorePosix::V() {

#ifdef DEBUG
int sem_val ;
sem_getvalue(_Sem, &sem_val) ; 
std::cout << "Begin of Operation V for the semaphore identified by name <" << _Semname << "> which have the value : " << sem_val << std::endl ;
#endif

sem_post(_Sem) ;

#ifdef DEBUG
sem_getvalue(_Sem, &sem_val) ;
std::cout << "End of Operation V for the semaphore identified by name <" << _Semname << "> which have the value : " << sem_val << std::endl ; 
#endif

} // End of V()

// ************************************************
// Methode : SemaphorePosix::Delete()
// ************************************************
void SemaphorePosix::Delete(){

sem_close(_Sem) ;
sem_destroy(_Sem) ;

} // End of Delete()
} /* end namespace ipc  */
} /* end namespace libhla */
