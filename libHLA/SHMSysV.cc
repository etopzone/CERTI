#include "SHMSysV.hh"
#include "sha1.h"

namespace libhla {
namespace ipc {

key_t SHMSysV::ntokUser(const char* name, int32_t user_specific_value) {

key_t s_key ;
int32_t retcode ;

SHA1Context sha ;
 uint8_t Message_Digest[20] ;

/* We use the first byte of a SHA1 hash of the BBname
 * unless the algorithm fail.
 * If SHA1 fail we go back to poor key generation method
 * using the name length.
 * In both case we must Xored the key with user_specific in order
 * to isolate different user from using the same key
 */
	  
retcode  = SHA1Reset(&sha) ;
retcode &= SHA1Input(&sha, (const unsigned char *) name,strlen(name)) ;
retcode &= SHA1Result(&sha, Message_Digest);

/* SHA 1 NOK back to old poor method */
if (0 != retcode) {
   s_key = ((strlen(name) << 16) & 0xFFFF0000) ^ (user_specific_value & 0x0000FFFF);
   } 
else { s_key = (Message_Digest[0]        |
       (Message_Digest[1] << 8) |
       (Message_Digest[2] << 16)|
       (Message_Digest[3] << 24)) ^
        user_specific_value;
	}
	return s_key;
}

key_t
SHMSysV::ntok(const char* name) {
	return SHMSysV::ntokUser(name,getuid());
        // return SHMSysV::ntokUser(name,1);
}


// ************************************************
// Constructors with args
// ************************************************
SHMSysV::SHMSysV(const std::string& SHMName, const int SHMSize, const bool True)  : SHM(SHMName, SHMSize, True) { 
    _Key = ntok(SHMName.c_str());

#ifdef DEBUG
std::cout << " Cle produite : " << _Key << std::endl ;
#endif

    _Id = 0 ;  
}

SHMSysV::SHMSysV(const std::string& SHMName, const int SHMSize)  : SHM(SHMName, SHMSize) { 
     _Key = ntok(SHMName.c_str());

#ifdef DEBUG
std::cout << " Cle produite : " << _Key << std::endl ;
#endif

    _Id = 0 ;  
}


// ************************************************
// Destructor
// ************************************************
SHMSysV::~SHMSysV() {}

// ************************************************
// Method : SHMSysV::Open()
// ************************************************
void SHMSysV::Open() {

if ((_Id = shmget(_Key, (size_t)_Size, IPC_CREAT| 0666 )) < 0){  
    perror("Error with shmget in SHMSysV::Open()"); 
    exit(1);
    } // End of if

} // End of Open()

// ************************************************
// Method : SHMSysV::Attach()
// ************************************************
void SHMSysV::Attach() {

if ( ( _Shm = shmat(_Id, NULL, 0) ) == (void*) -1){
    perror("Error with shmat() in SHMSysV::Attach() "); 
    exit(1);
    } // End of if

} // End of Attach(...)

// ************************************************
// Method : Close()
// ************************************************
void SHMSysV::Close() {

// Close
if(shmdt(_Shm)){
        perror("Error with shmdt() in SHMSysV::Close()");
        exit(1);
      } // End of if(shmdt(_Shm))

if(_IsCreator){
// Destroy
if(shmctl(_Id, IPC_RMID,0)){
   perror("Error with shmctl() in SHMSysV::Close()");
   exit(1);
   } // End of if(shmctl(_Shm_Id, IPC_RMID,0))
} // End of (_IsServer)

} // End of Close()
} /* end namespace ipc  */
} /* end namespace libhla */
