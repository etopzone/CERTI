// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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
#include "SHMWin32.hh"
using libhla::ipc::SHM;
using libhla::ipc::SHMWin32;

#include "SemaphoreWin32.hh"
using libhla::ipc::Semaphore;
using libhla::ipc::SemaphoreWin32;


// ************************************************
// Constructor with args
// ************************************************
SocketSHMWin32::SocketSHMWin32(const std::string& Socket_Name,
                             const SHM_SIDE_t& Socket_Side,
                             const int Socket_Size )
                             throw (certi::SocketSHMNotCreated): SocketSHM(Socket_Name,Socket_Side,Socket_Size)
                             {

if(_Side == SHM_SC){
_Shm_SC = new SHMWin32(SHM::buildShmName(Socket_Name+"_SC"), Socket_Size,true) ;
_Shm_CS = new SHMWin32(SHM::buildShmName(Socket_Name+"_CS"), Socket_Size) ;
  }
else {
_Shm_CS = new SHMWin32(SHM::buildShmName(Socket_Name+"_CS"), Socket_Size,true) ;
_Shm_SC = new SHMWin32(SHM::buildShmName(Socket_Name+"_SC"), Socket_Size) ;
}

// Semaphores
_Sem_full_SC = new SemaphoreWin32() ;
_Sem_empty_SC = new SemaphoreWin32() ;
_Sem_full_CS = new SemaphoreWin32() ;
_Sem_empty_CS = new SemaphoreWin32() ;

int init_full = 0, init_empty = 1 ; // Initialisation values

if(_Side == SHM_SC){
    try {
       _Sem_full_SC->Create_Init(init_full, Semaphore::buildSemName(Socket_Name+"_FULL_SC")) ;
    }
    catch(Semaphore::SemaphoreNotCreated& e)
    {
        std::cout << "SocketSHMWin32() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketSHMNotCreated("SocketSHMWin32() failed.")) ;
    }
    try {
       _Sem_empty_SC->Create_Init(init_empty, Semaphore::buildSemName(Socket_Name+"_EMPTY_SC")) ;
    }
    catch(Semaphore::SemaphoreNotCreated& e)
    {
        std::cout << "SocketSHMWin32() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketSHMNotCreated("SocketSHMWin32() failed.")) ;
    }
  }
else{
    try {
       _Sem_full_CS->Create_Init(init_full, Semaphore::buildSemName(Socket_Name+"_FULL_CS")) ;
    }
    catch(Semaphore::SemaphoreNotCreated& e)
    {
        std::cout << "SocketSHMWin32() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketSHMNotCreated("SocketSHMWin32() failed.")) ;
    }
    try {
       _Sem_empty_CS->Create_Init(init_empty, Semaphore::buildSemName(Socket_Name+"_EMPTY_CS")) ;
    }
    catch(Semaphore::SemaphoreNotCreated& e)
    {
        std::cout << "SocketSHMWin32() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketSHMNotCreated("SocketSHMWin32() failed.")) ;
    }
  }

}

// ************************************************
// Destructor
// ************************************************
SocketSHMWin32::~SocketSHMWin32()
                                throw(certi::SocketSHMNotDeleted) {

if(_Side == SHM_SC){
    try {
       _Sem_full_SC->Delete() ;
    }
    catch(Semaphore::HandleNotClosed& e)
    {
        std::cout << "~SocketSHMWin32() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketSHMNotDeleted("~SocketSHMWin32() failed.")) ;
    }
    try {
       _Sem_empty_SC->Delete() ;
    }
    catch(Semaphore::HandleNotClosed& e)
    {
        std::cout << "~SocketSHMWin32() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketSHMNotDeleted("~SocketSHMWin32() failed.")) ;
    }
   }
else{
    try {
       _Sem_full_CS->Delete() ;
    }
    catch(Semaphore::HandleNotClosed& e)
    {
        std::cout << "~SocketSHMWin32() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketSHMNotDeleted("~SocketSHMWin32() failed.")) ;
    }
    try {
       _Sem_empty_CS->Delete() ;
    }
    catch(Semaphore::HandleNotClosed& e)
    {
        std::cout << "~SocketSHMWin32() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketSHMNotDeleted("~SocketSHMWin32() failed.")) ;
    }
   }
delete _Sem_full_SC  ;
delete _Sem_empty_SC  ;
delete _Sem_full_CS  ;
delete _Sem_empty_CS ;

delete _Shm_SC ;
delete _Shm_CS ;

} // End of ~SocketSHMWin32()


// ************************************************
// Method : SocketSHMWin32::Open()
// ************************************************
void SocketSHMWin32::Open()
                        throw(certi::SocketSHMNotOpen){

if(_Side == SHM_SC) {
    try {
       _Sem_empty_SC->P() ;
    }
    catch(Semaphore::SemaphoreHandlingError& e)
    {
        std::cout << "SocketSHMWin32::Open() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketSHMNotOpen("SocketSHMWin32::Open() failed.")) ;
    }
    try {
       _Shm_SC->Open() ;
    }
    catch(SHM::SharedMemoryNotOpen& e)
    {
        std::cout << "SocketSHMWin32::Open() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketSHMNotOpen("SocketSHMWin32::Open() failed.")) ;
    }
    try {
       _Shm_SC->Attach() ;
    }
    catch(SHM::SharedMemoryNotAttached& e)
    {
        std::cout << "SocketSHMWin32::Open() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketSHMNotOpen("SocketSHMWin32::Open() failed.")) ;
    }
    try {
       _Sem_full_SC->V() ;
    }
    catch(Semaphore::SemaphoreHandlingError& e)
    {
        std::cout << "SocketSHMWin32::Open() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketSHMNotOpen("SocketSHMWin32::Open() failed.")) ;
    }

    #ifdef DEBUG
    std::cout << " The SHM from Server to Customer is Open " << std::endl ;
    #endif

     }
else{
    try {
       _Sem_empty_CS->P() ;
    }
    catch(Semaphore::SemaphoreHandlingError& e)
    {
        std::cout << "SocketSHMWin32::Open() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketSHMNotOpen("SocketSHMWin32::Open() failed.")) ;
    }
    try {
       _Shm_CS->Open() ;
    }
    catch(SHM::SharedMemoryNotOpen& e)
    {
        std::cout << "SocketSHMWin32::Open() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketSHMNotOpen("SocketSHMWin32::Open() failed.")) ;
    }
    try {
       _Shm_CS->Attach() ;
    }
    catch(SHM::SharedMemoryNotAttached& e)
    {
        std::cout << "SocketSHMWin32::Open() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketSHMNotOpen("SocketSHMWin32::Open() failed.")) ;
    }
    try {
       _Sem_full_CS->V() ;
    }
    catch(Semaphore::SemaphoreHandlingError& e)
    {
        std::cout << "SocketSHMWin32::Open() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketSHMNotOpen("SocketSHMWin32::Open() failed.")) ;
    }

     #ifdef DEBUG
     std::cout << " The SHM from Customer to Server is Create and Attach" << std::endl ;
     #endif

     }
if(_Side == SHM_CS){
    try {
       _Sem_full_SC->P() ;
    }
    catch(Semaphore::SemaphoreHandlingError& e)
    {
        std::cout << "SocketSHMWin32::Open() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketSHMNotOpen("SocketSHMWin32::Open() failed.")) ;
    }
    try {
       _Shm_SC->Open() ;
    }
    catch(SHM::SharedMemoryNotOpen& e)
    {
        std::cout << "SocketSHMWin32::Open() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketSHMNotOpen("SocketSHMWin32::Open() failed.")) ;
    }
    try {
       _Shm_SC->Attach() ;
    }
    catch(SHM::SharedMemoryNotAttached& e)
    {
        std::cout << "SocketSHMWin32::Open() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketSHMNotOpen("SocketSHMWin32::Open() failed.")) ;
    }
    try {
       _Sem_empty_SC->V() ;
    }
    catch(Semaphore::SemaphoreHandlingError& e)
    {
        std::cout << "SocketSHMWin32::Open() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketSHMNotOpen("SocketSHMWin32::Open() failed.")) ;
    }

     #ifdef DEBUG
     std::cout << " The SHM from Server to Customer is identified and attached " << std::endl ;
     #endif

     }
else{
    try {
       _Sem_full_CS->P() ;
    }
    catch(Semaphore::SemaphoreHandlingError& e)
    {
        std::cout << "SocketSHMWin32::Open() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketSHMNotOpen("SocketSHMWin32::Open() failed.")) ;
    }
    try {
       _Shm_CS->Open() ;
    }
    catch(SHM::SharedMemoryNotOpen& e)
    {
        std::cout << "SocketSHMWin32::Open() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketSHMNotOpen("SocketSHMWin32::Open() failed.")) ;
    }
    try {
       _Shm_CS->Attach() ;
    }
    catch(SHM::SharedMemoryNotAttached& e)
    {
        std::cout << "SocketSHMWin32::Open() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketSHMNotOpen("SocketSHMWin32::Open() failed.")) ;
    }
    try {
       _Sem_empty_CS->V() ;
    }
    catch(Semaphore::SemaphoreHandlingError& e)
    {
        std::cout << "SocketSHMWin32::Open() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::SocketSHMNotOpen("SocketSHMWin32::Open() failed.")) ;
    }

     #ifdef DEBUG
     std::cout << " The SHM from Customer to Server is identified and attached " << std::endl ;
     #endif

     }

} // End of Open()



