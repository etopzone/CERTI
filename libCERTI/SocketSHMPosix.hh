#ifndef SOCKET_SHM_POSIX_H
#define SOCKET_SHM_POSIX_H

// Specifics includes
#include "Semaphore.hh"
#include "SemaphorePosix.hh"
#include "SHM.hh"
#include "SHMPosix.hh"

class SocketSHMPosix {
private:   
    /***** Server -->>> Customer ******/
    SHMPosix _Shm_SC ;
    SemaphorePosix _Sem_plein_SC, _Sem_vide_SC ;

    /***** Customer -->>> Server ******/
    SHMPosix _Shm_CS ; 
    SemaphorePosix _Sem_plein_CS, _Sem_vide_CS ;  

public :
    // Constructor
    SocketSHMPosix (const std::string& New_Shm_Name_SC, 
		    const std::string& New_Shm_Name_CS, 
                    const bool& IsServerSC,
                    const bool& IsServerCS,
                    const int& Size_Shm_SC,
                    const int& Size_Shm_CS,
                    const std::string& Nom_Sem_plein_SC, 
		    const std::string& Nom_Sem_vide_SC,
                    const std::string& Nom_Sem_plein_CS, 
		    const std::string& Nom_Sem_vide_CS);  
    // Destructor
    ~SocketSHMPosix (); 
     
    void Open();
    void Close(); // To Close the two SHMs

    void Send(void *Buffer) ; // To send Data on a memory segment
    void Receive(void *Buffer) ; // To receive Data on a memory segment

}; // End of --> class SocketSHM

#endif
