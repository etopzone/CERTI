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

// Systems includes
#include <limits>
#include <string.h>
// Specifics includes
#ifdef _WIN32
#include "SocketSHMWin32.hh"
#include <libHLA/SHMWin32.hh>
#include <libHLA/SemaphoreWin32.hh>
#include <process.h>
#include <windows.h>
using libhla::ipc::SHMWin32;
using libhla::ipc::SemaphoreWin32;
#else
#include <libHLA/SHMPosix.hh>
#include <libHLA/SHMSysV.hh>
#include <libHLA/SemaphorePosix.hh>
#include <libHLA/SemaphoreSysV.hh>
using libhla::ipc::SHMPosix;
using libhla::ipc::SemaphorePosix;
using libhla::ipc::SHMSysV;
using libhla::ipc::SemaphoreSysV;
#endif

using libhla::ipc::SHM;
using libhla::ipc::Semaphore;

#include "RingBuffer.hh"

#include "PrettyDebug.hh"

namespace {
static PrettyDebug D("RINGBUFFER", __FILE__);
}

RingBuffer::RingBuffer(const std::string& name,
                       const BUFFER_SIDE_t& side,
                       const size_t size,
                       const std::string& semaphore_type)
    : _Name{name}, _Side{side}, _Size{size}
{
    Debug(D, pdDebug) << "-----------------------------------------------------------" << std::endl;
    Debug(D, pdDebug) << " ----------------------- RingBuffer -----------------------" << std::endl;
    Debug(D, pdDebug) << "-----------------------------------------------------------" << std::endl;

    _Tab_SC = new int[3];
    _Tab_SC[0] = _Size;
    _Tab_SC[1] = 0;
    _Tab_SC[2] = 0;

    _Tab_CS = new int[3];
    _Tab_CS[0] = _Size;
    _Tab_CS[1] = 0;
    _Tab_CS[2] = 0;

#ifdef _WIN32
    _Sem_CS = new SemaphoreWin32();
    _Sem_SC = new SemaphoreWin32();
#else
    if (semaphore_type == "SysV") {
        _Sem_CS = new SemaphoreSysV();
        _Sem_SC = new SemaphoreSysV();
    }
    else if (semaphore_type == "Posix") {
        _Sem_CS = new SemaphorePosix();
        _Sem_SC = new SemaphorePosix();
    }
    else {
        perror("Shm_Sem_Type Must be Posix or SysV");
        exit(1);
    }
#endif

    try {
        if (_Side == BUFFER_SC) {
            _Sem_SC->Create_Init(1, Semaphore::buildSemName(_Name + "_BUFFER_SC"));

#ifdef _WIN32
            _Shm_SC = new SHMWin32(SHM::buildShmName(_Name + "_BUFFER_SC"), _Size, true);
            _Shm_CS = new SHMWin32(SHM::buildShmName(_Name + "_BUFFER_CS"), _Size);
            _Pw_Pr_SC = new SHMWin32(SHM::buildShmName(_Name + "_IND_SC"), 3 * sizeof(int), true);
            _Pw_Pr_CS = new SHMWin32(SHM::buildShmName(_Name + "_IND_CS"), 3 * sizeof(int));
#else
            if (semaphore_type == "SysV") {
                _Shm_SC = new SHMSysV(SHM::buildShmName(_Name + "_BUFFER_SC"), _Size, true);
                _Shm_CS = new SHMSysV(SHM::buildShmName(_Name + "_BUFFER_CS"), _Size);
                _Pw_Pr_SC = new SHMSysV(SHM::buildShmName(_Name + "_IND_SC"), 3 * sizeof(int), true);
                _Pw_Pr_CS = new SHMSysV(SHM::buildShmName(_Name + "_IND_CS"), 3 * sizeof(int));
            }
            if (semaphore_type == "Posix") {
                _Shm_SC = new SHMPosix(SHM::buildShmName(_Name + "_BUFFER_SC"), _Size, true);
                _Shm_CS = new SHMPosix(SHM::buildShmName(_Name + "_BUFFER_CS"), _Size);
                _Pw_Pr_SC = new SHMPosix(SHM::buildShmName(_Name + "_IND_SC"), 3 * sizeof(int), true);
                _Pw_Pr_CS = new SHMPosix(SHM::buildShmName(_Name + "_IND_CS"), 3 * sizeof(int));
            }
#endif

            _Shm_SC->Open();
            _Shm_SC->Attach();
            _Pw_Pr_SC->Open();
            _Pw_Pr_SC->Attach();

            Debug(D, pdDebug) << " The SHMs for RingBuffer from SERVER to CUSTOMER Exchange are Created and Attached "
                              << std::endl;
            Debug(D, pdDebug) << " -----> Adresse : _Shm_SC->GetShm() = " << _Shm_SC->GetShm() << std::endl;
            Debug(D, pdDebug) << " -----> Adresse : _Pw_Pr_SC->GetShm() = " << _Pw_Pr_SC->GetShm() << std::endl;
        }
        else {
            _Sem_CS->Create_Init(1, Semaphore::buildSemName(_Name + "_BUFFER_CS"));

#ifdef _WIN32
            if (semaphore_type == "Win32") {
                _Shm_CS = new SHMWin32(SHM::buildShmName(_Name + "_BUFFER_CS"), _Size, true);
                _Shm_SC = new SHMWin32(SHM::buildShmName(_Name + "_BUFFER_SC"), _Size);
                _Pw_Pr_CS = new SHMWin32(SHM::buildShmName(_Name + "_IND_CS"), 3 * sizeof(int), true);
                _Pw_Pr_SC = new SHMWin32(SHM::buildShmName(_Name + "_IND_SC"), 3 * sizeof(int));
            }
#else
            if (semaphore_type == "SysV") {
                _Shm_CS = new SHMSysV(SHM::buildShmName(_Name + "_BUFFER_CS"), _Size, true);
                _Shm_SC = new SHMSysV(SHM::buildShmName(_Name + "_BUFFER_SC"), _Size);
                _Pw_Pr_CS = new SHMSysV(SHM::buildShmName(_Name + "_IND_CS"), 3 * sizeof(int), true);
                _Pw_Pr_SC = new SHMSysV(SHM::buildShmName(_Name + "_IND_SC"), 3 * sizeof(int));
            }
            if (semaphore_type == "Posix") {
                _Shm_CS = new SHMPosix(SHM::buildShmName(_Name + "_BUFFER_CS"), _Size, true);
                _Shm_SC = new SHMPosix(SHM::buildShmName(_Name + "_BUFFER_SC"), _Size);
                _Pw_Pr_CS = new SHMPosix(SHM::buildShmName(_Name + "_IND_CS"), 3 * sizeof(int), true);
                _Pw_Pr_SC = new SHMPosix(SHM::buildShmName(_Name + "_IND_SC"), 3 * sizeof(int));
            }
#endif

            _Shm_CS->Open();
            _Shm_CS->Attach();
            _Pw_Pr_CS->Open();
            _Pw_Pr_CS->Attach();

            Debug(D, pdDebug) << " The SHMs for RingBuffer from CUSTOMER to SERVER Exchanges are Created and Attached"
                              << std::endl;
            Debug(D, pdDebug) << "  -----> Adresse : _Shm_CS->GetShm() = " << _Shm_CS->GetShm() << std::endl;
            Debug(D, pdDebug) << "  -----> Adresse : _Pw_Pr_CS->GetShm() = " << _Pw_Pr_CS->GetShm() << std::endl;
        }
    }
    catch (Semaphore::SemaphoreNotCreated& e) {
        DEBUG_EXCEPTION(D, e);
        throw(certi::RingBufferNotCreated("RingBuffer() failed."));
    }
    catch (SHM::SharedMemoryNotOpen& e) {
        DEBUG_EXCEPTION(D, e);
        throw(certi::RingBufferNotCreated("RingBuffer() failed."));
    }
    catch (SHM::SharedMemoryNotAttached& e) {
        DEBUG_EXCEPTION(D, e);
        throw(certi::RingBufferNotCreated("RingBuffer() failed."));
    }
}

void RingBuffer::Attach()
{
    try {
        if (_Side == BUFFER_CS) {
            _Sem_SC->Attach(Semaphore::buildSemName(_Name + "_BUFFER_SC"));

            _Shm_SC->Open();
            _Shm_SC->Attach();
            _Pw_Pr_SC->Open();
            _Pw_Pr_SC->Attach();

            Debug(D, pdDebug)
                << " The SHMs for RingBuffer from SERVER to CUSTOMER Exchanges are Identified and Attached "
                << std::endl;
            Debug(D, pdDebug) << " Adresse : _Shm_SC->GetShm() = " << _Shm_SC->GetShm() << std::endl;
            Debug(D, pdDebug) << " Adresse : _Pw_Pr_SC->GetShm() = " << _Pw_Pr_SC->GetShm() << std::endl;

            memcpy(_Pw_Pr_CS->GetShm(), _Tab_CS, 3 * sizeof(int));
        }
        else {
            _Sem_CS->Attach(Semaphore::buildSemName(_Name + "_BUFFER_CS"));

            _Shm_CS->Open();
            _Shm_CS->Attach();
            _Pw_Pr_CS->Open();
            _Pw_Pr_CS->Attach();

            Debug(D, pdDebug)
                << " The SHMs for RingBuffer from CUSTOMER to SERVER Exchanges are Identified and Attached "
                << std::endl;
            Debug(D, pdDebug) << " Adresse : _Shm_CS->GetShm() = " << _Shm_CS->GetShm() << std::endl;
            Debug(D, pdDebug) << " Adresse : _Pw_Pr_CS->GetShm() = " << _Pw_Pr_CS->GetShm() << std::endl;

            memcpy(_Pw_Pr_SC->GetShm(), _Tab_SC, 3 * sizeof(int));
        }
    }
    catch (Semaphore::SemaphoreNotOpen& e) {
        DEBUG_EXCEPTION(D, e);
        throw(certi::RingBufferNotAttached("Attach() failed."));
    }
    catch (SHM::SharedMemoryNotOpen& e) {
        DEBUG_EXCEPTION(D, e);
        throw(certi::RingBufferNotAttached("Attach() failed."));
    }
    catch (SHM::SharedMemoryNotAttached& e) {
        DEBUG_EXCEPTION(D, e);
        throw(certi::RingBufferNotAttached("Attach() failed."));
    }
}

RingBuffer::~RingBuffer()
{
    try {
        if (_Side == BUFFER_SC) {
            _Sem_SC->Delete();
        }
        else {
            _Sem_CS->Delete();
        }
    }
    catch (SHM::HandleNotClosed& e) {
        DEBUG_EXCEPTION(D, e);
        //throw(certi::RingBufferNotDeleted("~RingBuffer() failed."));
        // Do not throw in dtor, delete anyway
    }

    delete _Sem_SC;
    delete _Sem_CS;
    delete _Shm_SC;
    delete _Shm_CS;
    delete _Pw_Pr_SC;
    delete _Pw_Pr_CS;
}

void RingBuffer::Send(void* Buffer, size_t Size)
{
    Debug(D, pdDebug) << "RingBuffer --> Try to Send..." << std::endl;

    if (Size > _Size) {
        throw(certi::MessageTooLong("RingBuffer::Send() failed."));
    }

    try {
        if (_Side == BUFFER_SC) {
            _Sem_SC->P();

            memcpy(_Tab_SC, _Pw_Pr_SC->GetShm(), 3 * sizeof(int));

            Debug(D, pdDebug) << "RingBuffer::Send(...) --> BEGIN Algorithm : Count_SC = " << _Tab_SC[0]
                              << " | Write_SC = " << _Tab_SC[1] << "| Read_SC = " << _Tab_SC[2] << std::endl;

            if ((int) Size
                > _Tab_SC[0]) { // Test si il y a assez de place disponible dans le buffer (Exeption a envoyer)
                _Sem_SC->V();
                throw(certi::BufferFull("RingBuffer::Send() failed."));
            }

            if ((_Tab_SC[2] > _Tab_SC[1]) || ((_Tab_SC[1] >= _Tab_SC[2]) && (Size + _Tab_SC[1] <= _Size))) {
                //  Plecture >= Pecriture || Pecriture >= Plecture && Taille a ecrire + Pecriture <=TailleTotaleSegment

                Debug(D, pdDebug) << "RingBuffer::Send(...) --> Utilisation memcpy Simple " << std::endl;
                Debug(D, pdDebug) << "RingBuffer::Send(...) --> Adresse Utilisee  _Shm_SC->GetShm() = "
                                  << _Shm_SC->GetShm() << "| _Shm_SC->GetShm() + _Tab_SC[1] = "
                                  << (void*) ((char*) (_Shm_SC->GetShm()) + (sizeof(void*) * _Tab_SC[1])) << std::endl;

                memcpy((void*) ((char*) (_Shm_SC->GetShm()) + (sizeof(void*) * _Tab_SC[1])), Buffer, Size);

                _Tab_SC[0] -= Size;
                _Tab_SC[1] = (_Tab_SC[1] + Size) % _Size;
            }
            else {
                Debug(D, pdDebug) << "RingBuffer::Send(...) --> Utilisation memcpy Double " << std::endl;

                int Rest_byte_in_SHM = _Size - _Tab_SC[1];

                memcpy((void*) ((char*) (_Shm_SC->GetShm()) + (sizeof(void*) * _Tab_SC[1])), Buffer, Rest_byte_in_SHM);

                memcpy(_Shm_SC->GetShm(),
                       (void*) ((char*) (Buffer) + (sizeof(void*) * Rest_byte_in_SHM)),
                       Size - Rest_byte_in_SHM);

                _Tab_SC[1] = (_Tab_SC[1] + Size) % _Size;
                _Tab_SC[0] -= Size;

            } // Fin du if ((_Tab_SC[2] >= _Tab_SC[1]) ...) /else

            Debug(D, pdDebug) << "RingBuffer::Send(...) --> END of Algorithm : Count_SC = " << _Tab_SC[0]
                              << " | Write_SC = " << _Tab_SC[1] << "| Read_SC = " << _Tab_SC[2] << std::endl;

            memcpy(_Pw_Pr_SC->GetShm(), _Tab_SC, 3 * sizeof(int));

            _Sem_SC->V();
        }
        else {
            _Sem_CS->P();

            memcpy(_Tab_CS, _Pw_Pr_CS->GetShm(), 3 * sizeof(int));

            Debug(D, pdDebug) << "RingBuffer::Send(...) --> BEGIN Algorithm : Count_CS = " << _Tab_CS[0]
                              << " | Write_CS = " << _Tab_CS[1] << "| Read_CS = " << _Tab_CS[2] << std::endl;

            if ((int) Size > _Tab_CS[0]) { // Test si il y a assez de place disponible dans le buffer
                _Sem_CS->V();
                throw(certi::BufferFull("RingBuffer::Send() failed."));
            }

            if ((_Tab_CS[2] > _Tab_CS[1]) || ((_Tab_CS[1] >= _Tab_CS[2]) && (Size + _Tab_CS[1] <= _Size))) {
                //  Plecture >= Pecriture || Pecriture >= Plecture && Taille a ecrire + Pecriture <=TailleTotaleSegment

                Debug(D, pdDebug) << "RingBuffer::Send(...) --> Utilisation memcpy Simple " << std::endl;
                Debug(D, pdDebug) << "RingBuffer::Send(...) --> Adresse Utilisee  _Shm_CS->GetShm() = "
                                  << _Shm_CS->GetShm() << "| _Shm_CS->GetShm() + _Tab_CS[1] = "
                                  << (void*) ((char*) (_Shm_CS->GetShm()) + (sizeof(void*) * _Tab_CS[1])) << std::endl;

                memcpy((void*) ((char*) (_Shm_CS->GetShm()) + (sizeof(void*) * _Tab_CS[1])), Buffer, Size);

                _Tab_CS[0] -= Size;
                _Tab_CS[1] = (_Tab_CS[1] + Size) % _Size;
            }
            else {
                Debug(D, pdDebug) << "RingBuffer::Send(...) --> Utilisation memcpy Double " << std::endl;

                int Rest_byte_in_SHM = _Size - _Tab_CS[1];

                memcpy((void*) ((char*) (_Shm_CS->GetShm()) + (sizeof(void*) * _Tab_CS[1])), Buffer, Rest_byte_in_SHM);

                memcpy(_Shm_CS->GetShm(),
                       (void*) ((char*) (Buffer) + (sizeof(void*) * Rest_byte_in_SHM)),
                       Size - Rest_byte_in_SHM);

                _Tab_CS[1] = (_Tab_CS[1] + Size) % _Size;

                _Tab_CS[0] -= Size;
            }

            Debug(D, pdDebug) << "RingBuffer::Send(...) --> END of Algorithm : Count_CS = " << _Tab_CS[0]
                              << " | Write_CS = " << _Tab_CS[1] << "| Read_CS = " << _Tab_CS[2] << std::endl;

            memcpy(_Pw_Pr_CS->GetShm(), _Tab_CS, 3 * sizeof(int));

            _Sem_CS->V();
        }
    }
    catch (Semaphore::SemaphoreHandlingError& e) {
        DEBUG_EXCEPTION(D, e);
        throw(certi::MessageNotSent("RingBuffer::Send() failed."));
    }

    Debug(D, pdDebug) << "RingBuffer --> Send Complete !!" << std::endl;
}

void RingBuffer::Receive(void* Buffer, size_t Size)
{
    Debug(D, pdDebug) << "RingBuffer -->  Try to Receive..." << std::endl;

    if (Size > _Size) {
        throw(certi::MessageTooLong("RingBuffer::Receive() failed."));
    }

    try {
        if (_Side == BUFFER_SC) {
            _Sem_CS->P();

            memcpy(_Tab_CS, _Pw_Pr_CS->GetShm(), 3 * sizeof(int));

            // Is there enough space in the buffer?
            if (_Tab_CS[0] == (int) _Size) {
                _Sem_CS->V();
                Debug(D, pdDebug) << "lllll RingBuffer::Receive(...) --> Nothing to Read on _Shm_SC !!" << std::endl;

                throw(certi::BufferEmpty("RingBuffer::Receive() failed."));
            }
            else {
                Debug(D, pdDebug) << "RingBuffer::Receive(...) --> BEGIN Algorithm : Count_CS = " << _Tab_CS[0]
                                  << " | Write_CS = " << _Tab_CS[1] << "| Read_CS = " << _Tab_CS[2] << std::endl;

                if ((_Tab_CS[1] > _Tab_CS[2]) || ((_Tab_CS[2] >= _Tab_CS[1]) && (Size + _Tab_CS[2] <= _Size))) {
                    //  Plecture >= Pecriture || Pecriture >= Plecture && Taille a ecrire + Pecriture <=TailleTotaleSegment

                    Debug(D, pdDebug) << "RingBuffer::Receive(...) --> Utilisation memcpy Simple " << std::endl;
                    Debug(D, pdDebug) << "RingBuffer::Receive(...) --> Adresse Utilisee  _Shm_CS->GetShm() = "
                                      << _Shm_CS->GetShm() << "| _Shm_CS->GetShm() + _Tab_CS[2] = "
                                      << (void*) ((char*) (_Shm_CS->GetShm()) + (sizeof(void*) * _Tab_CS[2]))
                                      << std::endl;

                    memcpy(Buffer, (void*) ((char*) (_Shm_CS->GetShm()) + (sizeof(void*) * _Tab_CS[2])), Size);

                    _Tab_CS[0] += Size;
                    _Tab_CS[2] += Size;
                }
                else {
                    Debug(D, pdDebug) << "RingBuffer::Receive(...) --> Utilisation memcpy Double " << std::endl;

                    int Rest_byte_in_SHM = _Size - _Tab_CS[2];

                    memcpy(
                        Buffer, (void*) ((char*) (_Shm_CS->GetShm()) + (sizeof(void*) * _Tab_CS[2])), Rest_byte_in_SHM);

                    memcpy((void*) ((char*) (Buffer) + (sizeof(void*) * Rest_byte_in_SHM)),
                           _Shm_CS->GetShm(),
                           Size - Rest_byte_in_SHM);

                    _Tab_CS[2] = (_Tab_CS[2] + Size) % _Size;
                    _Tab_CS[0] += Size;
                }

                Debug(D, pdDebug) << "RingBuffer::Receive(...) --> END Algorithm : Count_CS = " << _Tab_CS[0]
                                  << " | Write_CS = " << _Tab_CS[1] << "| Read_CS = " << _Tab_CS[2] << std::endl;
                Debug(D, pdDebug) << "AVANT  memcpy(_Pw_Pr_CS->GetShm(), _Tab_CS, 3 * sizeof(int) ) ; !! " << std::endl;

                memcpy(_Pw_Pr_CS->GetShm(), _Tab_CS, 3 * sizeof(int));

                Debug(D, pdDebug) << "APRES  memcpy(_Pw_Pr_CS->GetShm(), _Tab_CS, 3 * sizeof(int) ) ; !! " << std::endl;
            }

            _Sem_CS->V();
        }
        else {
            _Sem_SC->P();

            memcpy(_Tab_SC, _Pw_Pr_SC->GetShm(), 3 * sizeof(int));

            // Is there enough space in the buffer?
            if (_Tab_SC[0] == (int) _Size) {
                _Sem_SC->V();
                Debug(D, pdDebug) << "RingBuffer::Receive(...) --> Nothing to Read on _Shm_CS !!" << std::endl;

                throw(certi::BufferEmpty("RingBuffer::Receive() failed."));
            }
            else {
                Debug(D, pdDebug) << "RingBuffer::Receive(...) --> Adresse _Tab_SC = " << _Tab_SC
                                  << " Adresse _Pw_Pr_SC = " << _Pw_Pr_SC << std::endl;
                Debug(D, pdDebug) << "RingBuffer::Receive(...) --> Begin of Algorithm : Count_SC = " << _Tab_SC[0]
                                  << " | Write_SC = " << _Tab_SC[1] << "| Read_SC = " << _Tab_SC[2] << std::endl;

                if ((_Tab_SC[1] > _Tab_SC[2]) || ((_Tab_SC[2] >= _Tab_SC[1]) && (Size + _Tab_SC[2] <= _Size))) {
                    //  Plecture >= Pecriture || Pecriture >= Plecture && Taille a ecrire + Pecriture <=TailleTotaleSegment

                    Debug(D, pdDebug) << "RingBuffer::Receive(...) --> Utilisation memcpy Simple " << std::endl;
                    Debug(D, pdDebug) << "RingBuffer::Receive(...) --> Adresse Utilisee  _Shm_SC->GetShm() = "
                                      << _Shm_SC->GetShm() << "| _Shm_SC->GetShm() + _Tab_SC[2] = "
                                      << (void*) ((char*) (_Shm_SC->GetShm()) + (sizeof(void*) * _Tab_SC[2]))
                                      << std::endl;

                    memcpy(Buffer, (void*) ((char*) (_Shm_SC->GetShm()) + (sizeof(void*) * _Tab_SC[2])), Size);

                    _Tab_SC[0] += Size;
                    _Tab_SC[2] += Size;
                }
                else {
                    Debug(D, pdDebug) << "RingBuffer::Receive(...) --> Utilisation memcpy Double " << std::endl;

                    int Rest_byte_in_SHM = _Size - _Tab_SC[2];

                    memcpy(
                        Buffer, (void*) ((char*) (_Shm_SC->GetShm()) + (sizeof(void*) * _Tab_SC[2])), Rest_byte_in_SHM);

                    memcpy((void*) ((char*) (Buffer) + (sizeof(void*) * Rest_byte_in_SHM)),
                           _Shm_SC->GetShm(),
                           Size - Rest_byte_in_SHM);

                    _Tab_SC[2] = (_Tab_SC[2] + Size) % _Size;
                    _Tab_SC[0] += Size;
                }

                Debug(D, pdDebug) << "RingBuffer::Receive(...) --> End of Algorithm : Count_SC = " << _Tab_SC[0]
                                  << " | Write_SC = " << _Tab_SC[1] << "| Read_SC = " << _Tab_SC[2] << std::endl;

                memcpy(_Pw_Pr_SC->GetShm(), _Tab_SC, 3 * sizeof(int));
            }
            _Sem_SC->V();
        }
    }
    catch (Semaphore::SemaphoreHandlingError& e) {
        DEBUG_EXCEPTION(D, e);
        throw(certi::MessageNotReceived("RingBuffer::Receive() failed."));
    }

    Debug(D, pdDebug) << "RingBuffer --> Receive complete!!!" << std::endl;
}

void RingBuffer::Close()
{
    try {
        _Shm_SC->Close();
        _Shm_CS->Close();
        _Pw_Pr_SC->Close();
        _Pw_Pr_CS->Close();
    }
    catch (SHM::SharedMemoryNotClosed& e) {
        DEBUG_EXCEPTION(D, e);
        throw(certi::RingBufferNotClosed("RingBuffer::Close() failed."));
    }
    catch (SHM::HandleNotClosed& e) {
        DEBUG_EXCEPTION(D, e);
        throw(certi::RingBufferNotClosed("RingBuffer::Close() failed."));
    }
}
