#include "SocketSHM.hh"
#include <cstring>

#include "PrettyDebug.hh"

namespace {
static PrettyDebug D("SOCKET", __FILE__);
}

SocketSHM::~SocketSHM()
{
}

void SocketSHM::Connect()
{
    Debug(D, pdDebug) << "Try to Connect..." << std::endl;

    try {
        if (_Side == SHM_CS) {
            _Sem_full_SC->Attach(libhla::ipc::Semaphore::buildSemName(_Name + "_FULL_SC"));
            _Sem_empty_SC->Attach(libhla::ipc::Semaphore::buildSemName(_Name + "_EMPTY_SC"));
        }
        else {
            _Sem_full_CS->Attach(libhla::ipc::Semaphore::buildSemName(_Name + "_FULL_CS"));
            _Sem_empty_CS->Attach(libhla::ipc::Semaphore::buildSemName(_Name + "_EMPTY_CS"));
        }
    }
    catch (libhla::ipc::SHM::SharedMemoryNotAttached& e) {
        DEBUG_EXCEPTION(D, e);
        throw(certi::SocketNotConnected("Connect() failed."));
    }
}

void SocketSHM::Send(void* Buffer)
{
    Debug(D, pdDebug) << "Try to Send..." << std::endl;

    try {
        if (_Side == SHM_SC) {
            _Sem_empty_SC->P();
            memcpy(_Shm_SC->GetShm(), Buffer, _Shm_SC->GetSize());
            _Sem_full_SC->V();
        }
        else {
            _Sem_empty_CS->P();
            memcpy(_Shm_CS->GetShm(), Buffer, _Shm_CS->GetSize());
            _Sem_full_CS->V();
        }
    }
    catch (libhla::ipc::Semaphore::SemaphoreHandlingError& e) {
        DEBUG_EXCEPTION(D, e);
        throw(certi::MessageNotSent("Send() failed."));
    }

    Debug(D, pdDebug) << "Send Complete !!" << std::endl;
}

void SocketSHM::Receive(void* Buffer)
{
    Debug(D, pdDebug) << "Try to Receive..." << std::endl;

    try {
        if (_Side == SHM_SC) {
            _Sem_full_CS->P();
            memcpy(Buffer, _Shm_CS->GetShm(), _Shm_CS->GetSize());
            _Sem_empty_CS->V();
        }
        else {
            _Sem_full_SC->P();
            memcpy(Buffer, _Shm_SC->GetShm(), _Shm_SC->GetSize());
            _Sem_empty_SC->V();
        }
    }
    catch (libhla::ipc::Semaphore::SemaphoreHandlingError& e) {
        DEBUG_EXCEPTION(D, e);
        throw(certi::MessageNotReceived("Receive() failed."));
    }

    Debug(D, pdDebug) << "Receive complete!!!" << std::endl;
}

void SocketSHM::Close()
{
    Debug(D, pdDebug) << "Try to Close..." << std::endl;

    try {
        _Shm_SC->Close();
        _Shm_CS->Close();
    }
    catch (libhla::ipc::SHM::SharedMemoryNotClosed& e) {
        DEBUG_EXCEPTION(D, e);
        throw(certi::SocketNotClosed("Close() failed."));
    }
    catch (libhla::ipc::SHM::HandleNotClosed& e) {
        DEBUG_EXCEPTION(D, e);
        throw(certi::SocketNotClosed("Close() failed."));
    }
}
