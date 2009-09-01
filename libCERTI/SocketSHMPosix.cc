#include "SocketSHMPosix.hh"

// ************************************************
// Constructor with args
// ************************************************
SocketSHMPosix ::SocketSHMPosix (const std::string& New_Shm_Name_SC, 
		   	         const std::string& New_Shm_Name_CS, 
                                 const bool& IsServerSC,
                                 const bool& IsServerCS,
                                 const int& Size_Shm_SC,
                                 const int& Size_Shm_CS,
                                 const std::string& Nom_Sem_plein_SC, 
		                 const std::string& Nom_Sem_vide_SC,
                                 const std::string& Nom_Sem_plein_CS, 
		                 const std::string& Nom_Sem_vide_CS) 
                              : _Shm_SC(New_Shm_Name_SC, Size_Shm_SC, IsServerSC) ,
                                _Shm_CS(New_Shm_Name_CS, Size_Shm_CS, IsServerCS) {

// Semaphores
int init_plein = 0, init_vide = 1 ;

if(_Shm_SC.IsCreator()){
  _Sem_plein_SC.Create_Init(init_plein, Nom_Sem_plein_SC) ;
  _Sem_vide_SC.Create_Init(init_vide, Nom_Sem_vide_SC) ;
  }
if(_Shm_CS.IsCreator()){
  _Sem_plein_CS.Create_Init(init_plein, Nom_Sem_plein_CS) ;
  _Sem_vide_CS.Create_Init(init_vide, Nom_Sem_vide_CS) ;
  }
if(_Shm_SC.IsCreator()){
  _Sem_plein_CS.Attach(Nom_Sem_plein_CS) ;
  _Sem_vide_CS.Attach(Nom_Sem_vide_CS) ;
  }
if(_Shm_CS.IsCreator()){
  _Sem_plein_SC.Attach(Nom_Sem_plein_SC) ;
  _Sem_vide_SC.Attach(Nom_Sem_vide_SC) ;
  }

}
// ************************************************
// Destructor
// ************************************************
SocketSHMPosix ::~SocketSHMPosix() {

if(_Shm_SC.IsCreator()){
   _Sem_plein_SC.Delete() ;
   _Sem_vide_SC.Delete() ;
   }

if(_Shm_CS.IsCreator()){
   _Sem_plein_CS.Delete() ;
   _Sem_vide_CS.Delete() ;
   } 
} // End of ~SocketSHMPosix()

// ************************************************
// Method : SocketSHMPosix::Open()
// ************************************************
void SocketSHMPosix::Open() {


if(_Shm_SC.IsCreator()){
    _Sem_vide_SC.P() ; 
     _Shm_SC.Open() ;
     _Shm_SC.Attach() ;
    _Sem_vide_SC.V() ;
     } // End of --> if(_Shm_SC.GetIsServer())

if(_Shm_CS.IsCreator()){
     _Sem_vide_SC.P() ;  
     _Shm_SC.Open() ;
     _Shm_SC.Attach() ;
     _Sem_vide_SC.V() ;
     } // End of --> if(_Shm_CS.GetIsServer())

if(_Shm_CS.IsCreator()){
     _Sem_vide_CS.P() ;
     _Shm_CS.Open() ;
     _Shm_CS.Attach() ;
     _Sem_vide_CS.V() ;
     } // End of --> if(_Shm_CS.GetIsServer())

if(_Shm_SC.IsCreator()){
     _Sem_vide_CS.P() ;  
     _Shm_CS.Open() ;
     _Shm_CS.Attach() ;
     _Sem_vide_CS.V() ;
     } // End of --> if(_Shm_CS.GetIsServer())

} // End of Open()

// ************************************************
// Method : SocketSHMPosix::Send(...)
// ************************************************
void SocketSHMPosix::Send(void *Buffer) {
printf("On veut envoyer \n") ;

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

} // End of Send(...)

// ************************************************
// Method : SocketSHMPosix::Receive(...)
// ************************************************
void SocketSHMPosix::Receive(void *Buffer) {
printf("On veut recevoir \n") ;

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

} // End of --> SocketSHM::Receive(...)

// ************************************************
// Method : SocketSHMPosix::Close()
// ************************************************
void SocketSHMPosix::Close() {

_Shm_SC.Close() ;
_Shm_CS.Close() ;

} // End of --> SocketSHM::Close()

