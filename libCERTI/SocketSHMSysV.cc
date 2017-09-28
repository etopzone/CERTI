#include "SocketSHMSysV.hh"
using libhla::ipc::Semaphore;
using libhla::ipc::SHM;
using libhla::ipc::SemaphoreSysV;
using libhla::ipc::SHMSysV;

// ************************************************
// Constructor with args
// ************************************************
SocketSHMSysV::SocketSHMSysV(const std::string& Socket_Name, const SHM_SIDE_t& Socket_Side, const int Socket_Size)
    : SocketSHM(Socket_Name, Socket_Side, Socket_Size)
{
    if (_Side == SHM_SC) {
        _Shm_SC = new SHMSysV(SHM::buildShmName(Socket_Name + "_SC"), Socket_Size, true);
        _Shm_CS = new SHMSysV(SHM::buildShmName(Socket_Name + "_CS"), Socket_Size);
    }
    else {
        _Shm_CS = new SHMSysV(SHM::buildShmName(Socket_Name + "_CS"), Socket_Size, true);
        _Shm_SC = new SHMSysV(SHM::buildShmName(Socket_Name + "_SC"), Socket_Size);
    }

    // Semaphores
    _Sem_full_SC = new SemaphoreSysV();
    _Sem_empty_SC = new SemaphoreSysV();
    _Sem_full_CS = new SemaphoreSysV();
    _Sem_empty_CS = new SemaphoreSysV();

    int init_full = 0, init_empty = 1; // Initialisation values

    if (_Side == SHM_SC) {
        _Sem_full_SC->Create_Init(init_full, Semaphore::buildSemName(Socket_Name + "_FULL_SC"));
        _Sem_empty_SC->Create_Init(init_empty, Semaphore::buildSemName(Socket_Name + "_EMPTY_SC"));
    }
    else {
        _Sem_full_CS->Create_Init(init_full, Semaphore::buildSemName(Socket_Name + "_FULL_CS"));
        _Sem_empty_CS->Create_Init(init_empty, Semaphore::buildSemName(Socket_Name + "_EMPTY_CS"));
    }
}

// ************************************************
// Destructor
// ************************************************
SocketSHMSysV::~SocketSHMSysV()
{
    if (_Side == SHM_SC) {
        _Sem_full_SC->Delete();
        _Sem_empty_SC->Delete();
    }
    else {
        _Sem_full_CS->Delete();
        _Sem_empty_CS->Delete();
    }
}

// ************************************************
// Method : SocketSHMSysV::Open()
// ************************************************
void SocketSHMSysV::Open()
{
    if (_Side == SHM_SC) {
        _Sem_empty_SC->P();
        _Shm_SC->Open();
        _Shm_SC->Attach();
        _Sem_empty_SC->V();
#ifdef DEBUG
        std::cout << " The SHM from Server to Customer is Create and attach " << std::endl;
#endif
    }
    else {
        _Sem_empty_CS->P();
        _Shm_CS->Open();
        _Shm_CS->Attach();
        _Sem_empty_CS->V();
#ifdef DEBUG
        std::cout << " The SHM from Customer to Server is Create and Attach" << std::endl;
#endif
    }
    if (_Side == SHM_CS) {
        _Sem_empty_SC->P();
        _Shm_SC->Open();
        _Shm_SC->Attach();
        _Sem_empty_SC->V();
#ifdef DEBUG
        std::cout << " The SHM from Server to Customer is identified and attached " << std::endl;
#endif
    }
    else {
        _Sem_empty_CS->P();
        _Shm_CS->Open();
        _Shm_CS->Attach();
        _Sem_empty_CS->V();
#ifdef DEBUG
        std::cout << " The SHM from Customer to Server is identified and attached " << std::endl;
#endif
    }

} // End of Open()
