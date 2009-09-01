#include "SHMPosix.hh"

// ************************************************
// Constructor with args
// ************************************************
SHMPosix::SHMPosix(const std::string& New_Shm_Name, const int New_Shm_Size, const bool It_IsCreator) 
  : SHM(New_Shm_Name, New_Shm_Size, It_IsCreator) { 
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
        perror("In shm_open()");
        exit(1);
        }
    fprintf(stderr, "Created shared memory object %s\n", _Name.c_str());

    ret = ftruncate(_Id, _Size);
     if (ret < 0) {
        perror("In ftruncate()");
        exit(1);
        }

 } // End of if IsCreator()
 else{
    _Id = shm_open(_Name.c_str(), O_RDONLY, S_IRWXU | S_IRWXG);
    if (_Id < 0) {
        perror("In shm_open()");
        exit(1);
        }
       fprintf(stderr, "Attach shared memory object %s\n", _Name.c_str());
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
        perror("In mmap()");
        exit(1);}
    if (_Shm == MAP_FAILED) {
        perror("In mmap()");
        exit(1);}
    close(_Id) ;
} // End of IsServer
else{
 /* requesting the shared segment for write    --  mmap() */    
    _Shm = mmap(NULL, _Size, PROT_READ , MAP_SHARED, _Id, 0);
    if (_Shm == NULL) {
        perror("In mmap()");
        exit(1);}
    if (_Shm == MAP_FAILED) {
        perror("In mmap()");
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
    perror("In shm_unlink()");
    exit(1);
    }

// Destroy
if (IsCreator()){
if(munmap(_Shm, _Size)){
   perror("munmap(_Shm, size) : ");
   exit(1);
   } // End of if
}

} // End of Close()

