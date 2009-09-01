#ifndef SOCKET_SHM_SYSV_H
#define SOCKET_SHM_SYSV_H

// Specifics includes
#include "SemaphoreSysV.hh"
#include "SHMSysV.hh"

class SocketSHMSysV {
private:  
    /***** Server -->>> Customer ******/
    SHMSysV _Shm_SC ;
    SemaphoreSysV _Sem_plein_SC, _Sem_vide_SC ;

    /***** Customer -->>> Server ******/
    SHMSysV _Shm_CS ;
    SemaphoreSysV _Sem_plein_CS, _Sem_vide_CS ;      

    /**
     * Build a SysV IPC key from a name and user specific value.
     * The purpose of this function is to build a (quasi) unique
     * key from unique entry as ftok(3) do with existing file name.
     * We use SHA1 hash function Xored with the user_specific
     * value supplied.
     * @param[in] name, the name representing the IPC element for which
     *                 we want a key.
     * @param[in] user_specific_value, any user specific value
     *                               (for example uid).
     * @return The generated SysV IPC key corresponding to the specified entry
     */
    key_t
    static ntokUser(const char* name, int32_t user_specific_value);

    /**
     * Build a SysV IPC key from a name.
     * L'objectif de cette fonction est de g�n�rer des
     * clefs diff�rentes pour des noms diff�rents, � la mani�re
     * d'une fonction de hachage ou checksum parfaite.
     * Cette fonction vise les m�me objectifs que ftok(3)
     * avec les noms de fichiers mais avec une chaine
     * de caractere quelconque.
     * @param name un nom repr�sentant l'�l�ment IPC pour lequel on
     *                veut g�n�rer une clef.
     * @return SysV IPC key corresponding to the specified name.
     */
    key_t
    static ntok(const char* name);

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
