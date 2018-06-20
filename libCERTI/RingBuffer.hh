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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
// ----------------------------------------------------------------------------

#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <iostream>

// Specifics includes
#include <include/certi.hh>
#include "Exception.hh"
#include <libHLA/libhla.hh>
#include <libHLA/SHM.hh>
#include <libHLA/Semaphore.hh>

class CERTI_EXPORT RingBuffer {
public:
    // Typedef Side
    typedef enum { BUFFER_SC, BUFFER_CS } BUFFER_SIDE_t;

    // Constructor
    RingBuffer(const std::string& name,
               const BUFFER_SIDE_t& side,
               const size_t size,
               const std::string& semaphore_type) // SHM_Sem_Type = Posix,SysV ou Win32
        ;
    // Destructor
    ~RingBuffer();

    void Attach();

    void Send(void* Buffer, size_t Size) // To send Data on a memory segment
        ;

    void Receive(void* Buffer, size_t Size) // To receive Data on a memory segment
        ;

    void Close(); // To Close the two SHMs

protected:
    std::string _Name;
    BUFFER_SIDE_t _Side;
    size_t _Size;

    /***** Server -->>> Customer ******/
    libhla::ipc::SHM* _Shm_SC;
    libhla::ipc::SHM* _Pw_Pr_SC;
    // _Count_SC, _Write_SC, _Read_SC
    // int _Tab_SC[3] ;
    int* _Tab_SC;
    libhla::ipc::Semaphore* _Sem_SC;

    /***** Customer -->>> Server ******/
    libhla::ipc::SHM* _Shm_CS;
    libhla::ipc::SHM* _Pw_Pr_CS;
    // _Count_CS, _Write_CS, _Read_CS
    // int _Tab_CS[3] ;
    int* _Tab_CS;
    libhla::ipc::Semaphore* _Sem_CS;

}; // End of --> class SocketSHM

#endif
