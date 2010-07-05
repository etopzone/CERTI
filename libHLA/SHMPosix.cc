// SHM useful systems includes
#define _POSIX_C_SOURCE 200112L
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

// Others Systems includes
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

#include "SHMPosix.hh"
namespace libhla {
namespace ipc {
// ************************************************
// Constructor with args
// ************************************************
SHMPosix::SHMPosix(const std::string& SHMName, const int SHMSize, const bool True)  : SHM(SHMName, SHMSize, True) { 
    _Id = 0 ;  
}

SHMPosix::SHMPosix(const std::string& SHMName, const int SHMSize)  : SHM(SHMName,SHMSize) { 
    _Id = 0 ;  
}

// ************************************************
// Destructor
SHMPosix::~SHMPosix() {}

// ************************************************
// Method : SHMPosix::Open()
// ************************************************
void SHMPosix::Open() {

int ret ;

 if (IsCreator()){
    ret = shm_unlink(_Name.c_str());
    _Id = shm_open(_Name.c_str(), O_CREAT | O_EXCL | O_RDWR , S_IRWXU | S_IRWXG);
    if (_Id < 0) {
        perror("Error with shm_open() in SHMPosix::Open()");
        exit(1);
        }
#ifdef DEBUG
std::cout <<  "Created shared memory object : " << _Name.c_str() << std::endl ;
#endif

    ret = ftruncate(_Id, _Size);
     if (ret < 0) {
        perror("In ftruncate()");
        exit(1);
        }

 } // End of if IsCreator()
 else{
    _Id = shm_open(_Name.c_str(), O_RDONLY, S_IRWXU | S_IRWXG);
    if (_Id < 0) {
        perror("Error with shm_open() in SHMPosix::Open()");
        exit(1);
        }
#ifdef DEBUG
std::cout <<  "Attach shared memory object : " << _Name.c_str() << std::endl ;
#endif
 } // End of else IsCreator()

} // End of Open()

// ************************************************
// Method : SHMPosix::Attach()
// ************************************************
void SHMPosix::Attach() {

if (IsCreator()){
    /* requesting the shared segment for write    --  mmap() */    
    _Shm = mmap(NULL, _Size, PROT_READ | PROT_WRITE , MAP_SHARED, _Id, 0);
    if (_Shm == NULL) {
        perror("Error with mmap() in SHMPosix::Attach() ");
        exit(1);}
    if (_Shm == MAP_FAILED) {
        perror("Error with mmap() in SHMPosix::Attach() ");
        exit(1);}
    close(_Id) ;
} // End of IsServer
else{
 /* requesting the shared segment for write    --  mmap() */    
    _Shm = mmap(NULL, _Size, PROT_READ , MAP_SHARED, _Id, 0);
    if (_Shm == NULL) {
        perror("Error with mmap() in SHMPosix::Attach() ");
        exit(1);}
    if (_Shm == MAP_FAILED) {
        perror("Error with mmap() in SHMPosix::Attach() ");
        exit(1);}
    close(_Id) ;
} // End of else (IsServer)
} // End of Attach()

// ************************************************
// Method : SHMPosix::Close()
// ************************************************
void SHMPosix::Close() {

// Close
if (shm_unlink(_Name.c_str()) != 0) {
    perror("Error with shm_unlink() in SHMPosix::Close() ");
    exit(1);
    }

// Destroy
if (IsCreator()){
if(munmap(_Shm, _Size)){
   perror("Error with munmap() in SHMPosix::Close() ");
   exit(1);
   } // End of if
}

} // End of Close()
} /* end namespace ipc  */
} /* end namespace libhla */
