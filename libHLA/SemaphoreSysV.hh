#ifndef SEMAPHORESYSV_H
#define SEMAPHORESYSV_H

// Semaphores usefull systems includes
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/sem.h>

// Others systems includes 
#include <unistd.h>
#include <cstdlib>
#include <iostream>

// Specifics includes
#include "Semaphore.hh"

namespace libhla {
namespace ipc {

class HLA_EXPORT SemaphoreSysV : public Semaphore {
    private :
    int _Sem_Id ;
    key_t _Sem_Key ;

    public :
    SemaphoreSysV() ;
    virtual ~SemaphoreSysV() ;
    void Create_Init(const int initval, const std::string& New_Semname) ;
    void Attach(const std::string& New_Semname ) ;
    void P() ;
    void V() ;
    void Delete() ;

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
} /* end namespace ipc  */
} /* end namespace libhla */
#endif
