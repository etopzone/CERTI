#include "SHMSysV.hh"

// ************************************************
// Constructor with args
// ************************************************
SHMSysV::SHMSysV(const std::string& New_Name, const int New_Key, const int New_Size, const bool It_IsCreator) : SHM(New_Name, New_Size, It_IsCreator) {
    _Id = 0 ;  
    _Key = New_Key ;
}


// ************************************************
// Destructor
// ************************************************
SHMSysV::~SHMSysV() {}

// ************************************************
// Method : SHMSysV::Open()
// ************************************************
void SHMSysV::Open() {

if ((_Id = shmget((key_t)_Key, (size_t) _Size, IPC_CREAT| 0666 )) < 0){  
    perror("Error in shmget "); 
    exit(1);
    } // End of if

} // End of Open()

// ************************************************
// Method : SHMSysV::Attach()
// ************************************************
void SHMSysV::Attach() {

if ( ( _Shm = shmat(_Id, NULL, 0) ) == (void*) -1){
    perror("Error in shmat "); 
    exit(1);
    } // End of if

} // End of Attach(...)

// ************************************************
// Method : Close()
// ************************************************
void SHMSysV::Close() {

// Close
if(shmdt(_Shm)){
        perror("Error in shmdt ");
        exit(1);
      } // End of if(shmdt(_Shm))

if(_IsCreator){
// Destroy
if(shmctl(_Id, IPC_RMID,0)){
   perror("Error in shmctl(IPC_RMID) ");
   exit(1);
   } // End of if(shmctl(_Shm_Id, IPC_RMID,0))
} // End of (_IsServer)

} // End of Close()

