#include "SocketSHMSysV.hh"
#include "sha1.h"


key_t
SocketSHMSysV::ntokUser(const char* name, int32_t user_specific_value) {
	  key_t s_key;
	  int32_t retcode;

	  SHA1Context sha;
	  uint8_t Message_Digest[20];

	  /* We use the first byte of a SHA1 hash of the BBname
	   * unless the algorithm fail.
	   * If SHA1 fail we go back to poor key generation method
	   * using the name length.
	   * In both case we must Xored the key with user_specific in order
	   * to isolate different user from using the same key
	   */
	  retcode  = SHA1Reset(&sha);
	  retcode &= SHA1Input(&sha, (const unsigned char *) name,strlen(name));
	  retcode &= SHA1Result(&sha, Message_Digest);

	  /* SHA 1 NOK back to old poor method */
	  if (0 != retcode) {
	    s_key = ((strlen(name) << 16) & 0xFFFF0000) ^ (user_specific_value & 0x0000FFFF);
	  } else {
	    s_key = (Message_Digest[0]        |
		     (Message_Digest[1] << 8) |
		     (Message_Digest[2] << 16)|
		     (Message_Digest[3] << 24)) ^
	      user_specific_value;
	  }
	  return s_key;
}

key_t
SocketSHMSysV::ntok(const char* name) {
	return SocketSHMSysV::ntokUser(name,getuid());
}

// ************************************************
// Constructor with args
// ************************************************
SocketSHMSysV::SocketSHMSysV(const std::string& New_Shm_Name_SC, 
		             const std::string& New_Shm_Name_CS, 
                             const int New_Shm_Key_SC, 
		             const int New_Shm_Key_CS, 
                             const bool IsServerSC,
                             const bool IsServerCS,
                             const int Size_Shm_SC,
                             const int Size_Shm_CS,
                             const int Key_Sem_plein_SC, 
		             const int Key_Sem_vide_SC,
                             const int Key_Sem_plein_CS, 
		             const int Key_Sem_vide_CS) 
                            : _Shm_SC(New_Shm_Name_SC, New_Shm_Key_SC, Size_Shm_SC, IsServerSC) ,
                              _Shm_CS(New_Shm_Name_CS, New_Shm_Key_CS, Size_Shm_CS, IsServerCS) {

// Semaphores
int init_plein = 0, init_vide = 1 ; // Initialisation values

_Sem_plein_SC.Create_Init(init_plein, Key_Sem_plein_SC) ;
_Sem_vide_SC.Create_Init(init_vide, Key_Sem_vide_SC) ;

_Sem_plein_CS.Create_Init(init_plein, Key_Sem_plein_CS) ;
_Sem_vide_CS.Create_Init(init_vide, Key_Sem_vide_CS) ;

}

// ************************************************
// Destructor
// ************************************************
SocketSHMSysV::~SocketSHMSysV() {

if(_Shm_SC.IsCreator()){
   _Sem_plein_SC.Delete() ;
   _Sem_vide_SC.Delete() ;
   }
if(_Shm_CS.IsCreator()){
   _Sem_plein_CS.Delete() ;
   _Sem_vide_CS.Delete() ;
   } 

}

// ************************************************
// Method : SocketSHMSysV::Open()
// ************************************************
void SocketSHMSysV::Open() {

std::cout << " Open the socket from Server to Customer " << std::endl ;
_Shm_SC.Open() ;
std::cout << " Attach the socket from Server to Customer  " << std::endl ;
_Shm_SC.Attach() ;
std::cout << " Open the socket from Customer to Server " << std::endl ;
_Shm_CS.Open() ;
std::cout << " Attach the socket from Customer to Server " << std::endl ;
_Shm_CS.Attach() ;
} // End of Open()


// ************************************************
// Method : SocketSHMSysV::Send(...)
// ************************************************
void SocketSHMSysV::Send(void *Buffer) {

printf("Try to Send \n") ;

if(_Shm_SC.IsCreator()){
     _Sem_vide_SC.P() ;     
    memcpy(_Shm_SC.GetShm(), Buffer, _Shm_SC.GetSize());
     _Sem_plein_SC.V() ;
     } // End of --> if(_Shm_SC.GetIsServer())

if(_Shm_CS.IsCreator()){
     _Sem_vide_CS.P() ;  
     memcpy(_Shm_CS.GetShm(), Buffer, _Shm_CS.GetSize());
     _Sem_plein_CS.V() ;
     } // End of --> if(_Shm_CS.GetIsServer())

printf("Send Complete !!\n") ;

} // End of Send(...)

// ************************************************
// Method : SocketSHMSysV::Receive(...)
// ************************************************
void SocketSHMSysV::Receive(void *Buffer) {

printf("Try to Receive\n") ;

if(_Shm_SC.IsCreator()){
     _Sem_plein_CS.P() ; 
    memcpy(Buffer, _Shm_CS.GetShm(), _Shm_CS.GetSize());
     _Sem_vide_CS.V() ;
     } // End of --> if(_Shm_SC.GetIsServer())

if(_Shm_CS.IsCreator()){
     _Sem_plein_SC.P() ;  
     memcpy( Buffer, _Shm_SC.GetShm(), _Shm_SC.GetSize());
    _Sem_vide_SC.V() ;
     } // End of --> if(_Shm_CS.GetIsServer())

printf("Receive complete\n") ;

} // End of Receive(...)

// ************************************************
// Method : SocketSHMSysV::Close()
// ************************************************
void SocketSHMSysV::Close() {

_Shm_SC.Close() ;
_Shm_CS.Close() ;

} // End of Close()

