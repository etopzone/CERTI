#ifndef SOCKET_SHM_SYSV_H
#define SOCKET_SHM_SYSV_H

// Specifics includes
#include "Semaphore.hh"
#include "SemaphoreSysV.hh"
#include "SHM.hh"
#include "SHMSysV.hh"

class SocketSHMSysV {
private:  
    /***** Server -->>> Customer ******/
    SHMSysV _Shm_SC ;
    SemaphoreSysV _Sem_plein_SC, _Sem_vide_SC ;

    /***** Customer -->>> Server ******/
    SHMSysV _Shm_CS ;
    SemaphoreSysV _Sem_plein_CS, _Sem_vide_CS ;      

public :
    // Constructor
    SocketSHMSysV (const std::string& New_Shm_Name_SC, 
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
		   const int Key_Sem_vide_CS);  
    // Destructor
    ~SocketSHMSysV (); 
     
    void Open();
    void Close(); // To Close the two SHMs

    void Send(void *Buffer) ; // To send Data on a memory segment
    void Receive(void *Buffer) ; // To receive Data on a memory segment
   
}; // End of --> class SocketSHM

#endif
