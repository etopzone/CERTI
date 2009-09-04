#ifndef SHM_SYSV_H
#define SHM_SYSV_H

// SHMs useful systems includes
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h>

// Others systems includes
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

// Specifics includes
#include "SHM.hh"

class SHMSysV : public SHM {

    private :
    int _Id ;  
    key_t _Key ; 
   
    public :
    SHMSysV(const std::string& SHMName, const int SHMSize, const bool True) ;
    SHMSysV(const std::string& SHMName, const int SHMSize) ;  
    ~SHMSysV() ;
    void Open() ;
    void Attach() ;
    void Close() ;

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
     * L'objectif de cette fonction est de generer des
     * clefs differentes pour des noms differents, a la maniere
     * d'une fonction de hachage ou checksum parfaite.
     * Cette fonction vise les meme objectifs que ftok(3)
     * avec les noms de fichiers mais avec une chaine
     * de caractere quelconque.
     * @param name un nom representant l'element IPC pour lequel on
     *                veut generer une clef.
     * @return SysV IPC key corresponding to the specified name.
     */
    key_t
    static ntok(const char* name);


} ;

#endif
