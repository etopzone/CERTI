// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
//
// This program is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
// ----------------------------------------------------------------------------

#include "SocketSHMWin32.hh"

#include <libHLA/SHMWin32.hh>
using libhla::ipc::SHM;
using libhla::ipc::SHMWin32;

#include <libHLA/SemaphoreWin32.hh>
using libhla::ipc::Semaphore;
using libhla::ipc::SemaphoreWin32;

#include "PrettyDebug.hh"

namespace {
static PrettyDebug D("SOCKET", __FILE__);
}

// ************************************************
// Constructor with args
// ************************************************
SocketSHMWin32::SocketSHMWin32(const std::string& Socket_Name, const SHM_SIDE_t& Socket_Side, const int Socket_Size)
    : SocketSHM(Socket_Name, Socket_Side, Socket_Size)
{
    if (_Side == SHM_SC) {
        _Shm_SC = new SHMWin32(SHM::buildShmName(Socket_Name + "_SC"), Socket_Size, true);
        _Shm_CS = new SHMWin32(SHM::buildShmName(Socket_Name + "_CS"), Socket_Size);
    }
    else {
        _Shm_CS = new SHMWin32(SHM::buildShmName(Socket_Name + "_CS"), Socket_Size, true);
        _Shm_SC = new SHMWin32(SHM::buildShmName(Socket_Name + "_SC"), Socket_Size);
    }

    // Semaphores
    _Sem_full_SC = new SemaphoreWin32();
    _Sem_empty_SC = new SemaphoreWin32();
    _Sem_full_CS = new SemaphoreWin32();
    _Sem_empty_CS = new SemaphoreWin32();

    int init_full = 0, init_empty = 1; // Initialisation values

    try {
        if (_Side == SHM_SC) {
            _Sem_full_SC->Create_Init(init_full, Semaphore::buildSemName(Socket_Name + "_FULL_SC"));
            _Sem_empty_SC->Create_Init(init_empty, Semaphore::buildSemName(Socket_Name + "_EMPTY_SC"));
        }
        else {
            _Sem_full_CS->Create_Init(init_full, Semaphore::buildSemName(Socket_Name + "_FULL_CS"));
            _Sem_empty_CS->Create_Init(init_empty, Semaphore::buildSemName(Socket_Name + "_EMPTY_CS"));
        }
    }
    catch (Semaphore::SemaphoreNotCreated& e) {
        DEBUG_EXCEPTION(D, e);
        throw(certi::SocketSHMNotCreated("SocketSHMWin32() failed."));
    }
}

// ************************************************
// Destructor
// ************************************************
SocketSHMWin32::~SocketSHMWin32()
{
    try {
        if (_Side == SHM_SC) {
            _Sem_full_SC->Delete();
            _Sem_empty_SC->Delete();
        }
        else {
            _Sem_full_CS->Delete();
            _Sem_empty_CS->Delete();
        }
    }
    catch (Semaphore::HandleNotClosed& e) {
        DEBUG_EXCEPTION(D, e);
    }

    delete _Sem_full_SC;
    delete _Sem_empty_SC;
    delete _Sem_full_CS;
    delete _Sem_empty_CS;

    delete _Shm_SC;
    delete _Shm_CS;

} // End of ~SocketSHMWin32()

// ************************************************
// Method : SocketSHMWin32::Open()
// ************************************************
void SocketSHMWin32::Open() throw(certi::SocketSHMNotOpen)
{
    try {
        if (_Side == SHM_SC) {
            _Sem_empty_SC->P();
            _Shm_SC->Open();
            _Shm_SC->Attach();
            _Sem_full_SC->V();

            Debug(D, pdDebug) << " The SHM from Server to Customer is Open " << std::endl;
        }
        else {
            _Sem_empty_CS->P();
            _Shm_CS->Open();
            _Shm_CS->Attach();
            _Sem_full_CS->V();

            Debug(D, pdDebug) << " The SHM from Customer to Server is Create and Attach" << std::endl;
        }
        if (_Side == SHM_CS) {
            _Sem_full_SC->P();
            _Shm_SC->Open();
            _Shm_SC->Attach();
            _Sem_empty_SC->V();

            Debug(D, pdDebug) << " The SHM from Server to Customer is identified and attached " << std::endl;
        }
        else {
            _Sem_full_CS->P();
            _Shm_CS->Open();
            _Shm_CS->Attach();
            _Sem_empty_CS->V();

            Debug(D, pdDebug) << " The SHM from Customer to Server is identified and attached " << std::endl;
        }
    }
    catch (Semaphore::SemaphoreHandlingError& e) {
        DEBUG_EXCEPTION(D, e);
        throw(certi::SocketSHMNotOpen("SocketSHMWin32::Open() failed."));
    }
    catch (SHM::SharedMemoryNotOpen& e) {
        DEBUG_EXCEPTION(D, e);
        throw(certi::SocketSHMNotOpen("SocketSHMWin32::Open() failed."));
    }
    catch (SHM::SharedMemoryNotAttached& e) {
        DEBUG_EXCEPTION(D, e);
        throw(certi::SocketSHMNotOpen("SocketSHMWin32::Open() failed."));
    }

} // End of Open()
