#include "SemaphoreSysV.hh"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

#include "sha1.h"

namespace libhla {
namespace ipc {

key_t SemaphoreSysV::ntokUser(const char* name, int32_t user_specific_value)
{
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

    retcode = SHA1Reset(&sha);
    retcode &= SHA1Input(&sha, (const unsigned char*) name, strlen(name));
    retcode &= SHA1Result(&sha, Message_Digest);

    /* SHA 1 NOK back to old poor method */
    if (0 != retcode) {
        s_key = ((strlen(name) << 16) & 0xFFFF0000) ^ (user_specific_value & 0x0000FFFF);
    }
    else {
        s_key = (Message_Digest[0] | (Message_Digest[1] << 8) | (Message_Digest[2] << 16) | (Message_Digest[3] << 24))
            ^ user_specific_value;
    }
    return s_key;
}

key_t SemaphoreSysV::ntok(const char* name)
{
    return SemaphoreSysV::ntokUser(name, getuid());
    // return SemaphoreSysV::ntokUser(name,1);
}
// ************************************************
// Constructor
// ************************************************
SemaphoreSysV::SemaphoreSysV()
{
    _Sem_Id = 0;
    _Sem_Key = 0;
}

// ************************************************
// Destructor
// ************************************************
SemaphoreSysV::~SemaphoreSysV()
{
}

// ************************************************
// Method : SemaphoreSysV::Create_Init(...)
// ************************************************
void SemaphoreSysV::Create_Init(const int initval, const std::string& New_Semname)
{
    _Sem_Key = ntok(New_Semname.c_str());
    _Sem_Id = semget(_Sem_Key, 1, IPC_CREAT | 0666);
    if (_Sem_Id == -1) {
        perror("Error with semget() in SemaphoreSysV::Create_Init(...)");
        exit(1);
    }
#ifdef DEBUG
    std::cout << "We create the semaphore identified by number : " << _Sem_Id << std::endl;
#endif

    union semun {
        int val;
        struct semid_ds* buf; // Not use here
        ushort* array; // Not use here
    } arg_ctl;

    // Initialize
    arg_ctl.val = initval;
    if (semctl(_Sem_Id, 0, SETVAL, arg_ctl) == -1) {
        perror("Error during semaphore initialization");
        exit(1);
    }
#ifdef DEBUG
    std::cout << "We init the semaphore with value : " << initval << std::endl;
#endif

} // End of method : Create_Init(...)

// ************************************************
// Method : SemaphoreSysV::Attach(...)
// ************************************************
void SemaphoreSysV::Attach(const std::string& New_Semname)
{
    _Sem_Key = ntok(New_Semname.c_str());
    _Sem_Id = semget(_Sem_Key, 1, 0666);
    if (_Sem_Id == -1) {
        perror("Error with semget() in SemaphoreSysV::Attach(...)");
        exit(1);
    }
#ifdef DEBUG
    std::cout << "We attach the semaphore identified by number : " << _Sem_Id << std::endl;
#endif

} // End of method : Attach(...)

// ************************************************
// Method : SemaphoreSysV::P
// ************************************************
// Decrement Semaphore Value SV=SV-1

void SemaphoreSysV::P()
{
#ifdef DEBUG
    std::cout << "Begin of Operation P for the semaphore identified by number : " << _Sem_Id << std::endl;
#endif

    struct sembuf semparP;
    semparP.sem_num = 0;
    semparP.sem_op = -1;
    semparP.sem_flg = SEM_UNDO;

    if (semop(_Sem_Id, &semparP, 1) == -1) {
        perror("Error with semop() in SemaphoreSysV::P()");
        exit(1);
    }

#ifdef DEBUG
    std::cout << "End of Operation P for the semaphore identified by number : " << _Sem_Id << std::endl;
#endif

} // End of P()

// ************************************************
// Method : SemaphoreSysV::V
// ************************************************
// Increment Semaphore Value SV=SV+1

void SemaphoreSysV::V()
{
#ifdef DEBUG
    std::cout << "Begin of Operation V for the semaphore identified by number : " << _Sem_Id << std::endl;
#endif

    struct sembuf semparV;
    semparV.sem_num = 0;
    semparV.sem_op = 1;
    semparV.sem_flg = SEM_UNDO;

    if (semop(_Sem_Id, &semparV, 1) == -1) {
        perror("Error with semop() in SemaphoreSysV::V() ");
        exit(1);
    }

#ifdef DEBUG
    std::cout << "End of Operation V for the semaphore identified by number : " << _Sem_Id << std::endl;
#endif

} // End of V()

// ************************************************
// Method : SemaphoreSysV::Delete
// ************************************************
void SemaphoreSysV::Delete()
{
#ifdef DEBUG
    std::cout << "Destroy the semaphore identified by number : " << _Sem_Id << std::endl;
#endif

    if (semctl(_Sem_Id, 0, IPC_RMID, 0) == -1)
        perror("Error with semctl() in SemaphoreSysV::Delete()");

} // End of Delete()

} /* end namespace ipc  */
} /* end namespace libhla */
