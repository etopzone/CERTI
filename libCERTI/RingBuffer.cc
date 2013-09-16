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

// Systems includes
#include <limits>
#include <string.h>
// Specifics includes
#ifdef _WIN32
   #include <windows.h>
   #include <process.h>
   #include "SocketSHMWin32.hh"
   #include "SemaphoreWin32.hh"
   #include "SHMWin32.hh"
using libhla::ipc::SHMWin32;
using libhla::ipc::SemaphoreWin32;
#else
   #include "SemaphorePosix.hh"
   #include "SHMPosix.hh"
   #include "SemaphoreSysV.hh"
   #include "SHMSysV.hh"
using libhla::ipc::SHMPosix;
using libhla::ipc::SemaphorePosix;
using libhla::ipc::SHMSysV;
using libhla::ipc::SemaphoreSysV;
#endif

using libhla::ipc::SHM;
using libhla::ipc::Semaphore;

#include "RingBuffer.hh"

RingBuffer::RingBuffer(const std::string& RingBuffer_Name,
           	           const BUFFER_SIDE_t& RingBuffer_Side,
                       const int RingBuffer_Size,
                       const std::string& Shm_Sem_Type )
                       throw (certi::RingBufferNotCreated) {

#ifdef DEBUG
std::cout << "-----------------------------------------------------------" << std::endl ;
std::cout << " ----------------------- RingBuffer -----------------------"<< std::endl ;
std::cout << "-----------------------------------------------------------" << std::endl ;
#endif

_Name = RingBuffer_Name ;
_Side = RingBuffer_Side ;
_Size = RingBuffer_Size ;

_Tab_SC = new int[3] ;
_Tab_SC[0] = RingBuffer_Size ;
_Tab_SC[1] = 0 ;
_Tab_SC[2] = 0 ;

_Tab_CS = new int[3] ;
_Tab_CS[0] = RingBuffer_Size ;
_Tab_CS[1] = 0 ;
_Tab_CS[2] = 0 ;

#ifndef _WIN32
if (Shm_Sem_Type == "SysV"){
   _Sem_CS = new SemaphoreSysV() ;
   _Sem_SC = new SemaphoreSysV() ;
   }
else if (Shm_Sem_Type == "Posix"){
   _Sem_CS = new SemaphorePosix() ;
   _Sem_SC = new SemaphorePosix() ;
   }
else{
    perror("Shm_Sem_Type Must be Posix or SysV") ;
    exit(1) ;
    }
#else
   _Sem_CS = new SemaphoreWin32() ;
   _Sem_SC = new SemaphoreWin32() ;
#endif


if(_Side == BUFFER_SC){
    try {
       _Sem_SC->Create_Init(1, Semaphore::buildSemName(RingBuffer_Name+"_BUFFER_SC")) ;
    }
    catch(Semaphore::SemaphoreNotCreated& e)
    {
        std::cout << "RingBuffer::RingBuffer() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::RingBufferNotCreated("RingBuffer() failed.")) ;;
    }
  }
else{
    try {
       _Sem_CS->Create_Init(1, Semaphore::buildSemName(RingBuffer_Name+"_BUFFER_CS")) ;
    }
    catch(Semaphore::SemaphoreNotCreated& e)
    {
        std::cout << "RingBuffer::RingBuffer() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::RingBufferNotCreated("RingBuffer() failed.")) ;;
    }
  }


if(_Side == BUFFER_SC){
#ifndef _WIN32
     if (Shm_Sem_Type == "SysV"){
         _Shm_SC = new SHMSysV(SHM::buildShmName(RingBuffer_Name+"_BUFFER_SC"), RingBuffer_Size,true) ;
         _Shm_CS = new SHMSysV(SHM::buildShmName(RingBuffer_Name+"_BUFFER_CS"), RingBuffer_Size) ;
         _Pw_Pr_SC = new SHMSysV(SHM::buildShmName(RingBuffer_Name+"_IND_SC"), 3 * sizeof(int) , true) ;
         _Pw_Pr_CS = new SHMSysV(SHM::buildShmName(RingBuffer_Name+"_IND_CS"), 3 * sizeof(int)) ;
         }
     if (Shm_Sem_Type == "Posix"){
         _Shm_SC = new SHMPosix(SHM::buildShmName(RingBuffer_Name+"_BUFFER_SC"), RingBuffer_Size,true) ;
         _Shm_CS = new SHMPosix(SHM::buildShmName(RingBuffer_Name+"_BUFFER_CS"), RingBuffer_Size) ;
         _Pw_Pr_SC = new SHMPosix(SHM::buildShmName(RingBuffer_Name+"_IND_SC"), 3 * sizeof(int) , true) ;
         _Pw_Pr_CS = new SHMPosix(SHM::buildShmName(RingBuffer_Name+"_IND_CS"), 3 * sizeof(int)) ;
         }
#else
         _Shm_SC = new SHMWin32(SHM::buildShmName(RingBuffer_Name+"_BUFFER_SC"), RingBuffer_Size,true) ;
         _Shm_CS = new SHMWin32(SHM::buildShmName(RingBuffer_Name+"_BUFFER_CS"), RingBuffer_Size) ;
         _Pw_Pr_SC = new SHMWin32(SHM::buildShmName(RingBuffer_Name+"_IND_SC"), 3 * sizeof(int) , true) ;
         _Pw_Pr_CS = new SHMWin32(SHM::buildShmName(RingBuffer_Name+"_IND_CS"), 3 * sizeof(int)) ;
#endif
  }
else {
#ifndef _WIN32
     if (Shm_Sem_Type == "SysV"){
       _Shm_CS = new SHMSysV(SHM::buildShmName(RingBuffer_Name+"_BUFFER_CS"), RingBuffer_Size, true) ;
       _Shm_SC = new SHMSysV(SHM::buildShmName(RingBuffer_Name+"_BUFFER_SC"), RingBuffer_Size) ;
       _Pw_Pr_CS = new SHMSysV(SHM::buildShmName(RingBuffer_Name+"_IND_CS"), 3 * sizeof(int), true) ;
       _Pw_Pr_SC = new SHMSysV(SHM::buildShmName(RingBuffer_Name+"_IND_SC"), 3 * sizeof(int)) ;
       }
     if (Shm_Sem_Type == "Posix"){
       _Shm_CS = new SHMPosix(SHM::buildShmName(RingBuffer_Name+"_BUFFER_CS"), RingBuffer_Size, true) ;
       _Shm_SC = new SHMPosix(SHM::buildShmName(RingBuffer_Name+"_BUFFER_SC"), RingBuffer_Size) ;
       _Pw_Pr_CS = new SHMPosix(SHM::buildShmName(RingBuffer_Name+"_IND_CS"), 3 * sizeof(int), true) ;
       _Pw_Pr_SC = new SHMPosix(SHM::buildShmName(RingBuffer_Name+"_IND_SC"), 3 * sizeof(int)) ;
       }
#else
     if (Shm_Sem_Type == "Win32"){
       _Shm_CS = new SHMWin32(SHM::buildShmName(RingBuffer_Name+"_BUFFER_CS"), RingBuffer_Size, true) ;
       _Shm_SC = new SHMWin32(SHM::buildShmName(RingBuffer_Name+"_BUFFER_SC"), RingBuffer_Size) ;
       _Pw_Pr_CS = new SHMWin32(SHM::buildShmName(RingBuffer_Name+"_IND_CS"), 3 * sizeof(int), true) ;
       _Pw_Pr_SC = new SHMWin32(SHM::buildShmName(RingBuffer_Name+"_IND_SC"), 3 * sizeof(int)) ;
       }
#endif
}

if(_Side == BUFFER_SC){
    try {
       _Shm_SC->Open() ;
    }
    catch(SHM::SharedMemoryNotOpen& e)
    {
        std::cout << "RingBuffer::RingBuffer() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::RingBufferNotCreated("RingBuffer() failed.")) ;;
    }
    try {
       _Shm_SC->Attach() ;
    }
    catch(SHM::SharedMemoryNotAttached& e)
    {
        std::cout << "RingBuffer::RingBuffer() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::RingBufferNotCreated("RingBuffer() failed.")) ;;
    }
    try {
       _Pw_Pr_SC->Open() ;
    }
    catch(SHM::SharedMemoryNotOpen& e)
    {
        std::cout << "RingBuffer::RingBuffer() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::RingBufferNotCreated("RingBuffer() failed.")) ;;
    }
    try {
       _Pw_Pr_SC->Attach() ;
    }
    catch(SHM::SharedMemoryNotAttached& e)
    {
        std::cout << "RingBuffer::RingBuffer() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::RingBufferNotCreated("RingBuffer() failed.")) ;;
    }

    #ifdef DEBUG
    std::cout << " The SHMs for RingBuffer from SERVER to CUSTOMER Exchange are Created and Attached " << std::endl ;
    std::cout << " -----> Adresse : _Shm_SC->GetShm() = " << _Shm_SC->GetShm() << std::endl ;
    std::cout << " -----> Adresse : _Pw_Pr_SC->GetShm() = " << _Pw_Pr_SC->GetShm() << std::endl ;
    #endif
     }
else{
    try {
       _Shm_CS->Open() ;
    }
    catch(SHM::SharedMemoryNotOpen& e)
    {
        std::cout << "RingBuffer::RingBuffer() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::RingBufferNotCreated("RingBuffer() failed.")) ;;
    }
    try {
       _Shm_CS->Attach() ;
    }
    catch(SHM::SharedMemoryNotAttached& e)
    {
        std::cout << "RingBuffer::RingBuffer() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::RingBufferNotCreated("RingBuffer() failed.")) ;;
    }
    try {
       _Pw_Pr_CS->Open() ;
    }
    catch(SHM::SharedMemoryNotOpen& e)
    {
        std::cout << "RingBuffer::RingBuffer() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::RingBufferNotCreated("RingBuffer() failed.")) ;;
    }
    try {
       _Pw_Pr_CS->Attach() ;
    }
    catch(SHM::SharedMemoryNotAttached& e)
    {
        std::cout << "RingBuffer::RingBuffer() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::RingBufferNotCreated("RingBuffer() failed.")) ;;
    }

     #ifdef DEBUG
     std::cout << " The SHMs for RingBuffer from CUSTOMER to SERVER Exchanges are Created and Attached" << std::endl ;
     std::cout << "  -----> Adresse : _Shm_CS->GetShm() = " << _Shm_CS->GetShm() << std::endl ;
     std::cout << "  -----> Adresse : _Pw_Pr_CS->GetShm() = " << _Pw_Pr_CS->GetShm() << std::endl ;
     #endif

     }
}  // End of RingBuffer Constructor
// ************************************************
// Method : RingBuffer::Attach()
// ************************************************
void RingBuffer::Attach() throw(certi::RingBufferNotAttached) {

if(_Side == BUFFER_CS){
    try {
       _Sem_SC->Attach(Semaphore::buildSemName(_Name+"_BUFFER_SC")) ;
    }
    catch(Semaphore::SemaphoreNotOpen& e)
    {
        std::cout << "RingBuffer::Attach() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::RingBufferNotAttached("Attach() failed.")) ;;
    }
  }
else{
    try {
       _Sem_CS->Attach(Semaphore::buildSemName(_Name+"_BUFFER_CS")) ;
    }
    catch(Semaphore::SemaphoreNotOpen& e)
    {
        std::cout << "RingBuffer::Attach() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::RingBufferNotAttached("Attach() failed.")) ;;
    }
  }

if(_Side == BUFFER_CS){
    try {
       _Shm_SC->Open() ;
    }
    catch(SHM::SharedMemoryNotOpen& e)
    {
        std::cout << "RingBuffer::Attach() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::RingBufferNotAttached("Attach() failed.")) ;;
    }
    try {
       _Shm_SC->Attach() ;
    }
    catch(SHM::SharedMemoryNotAttached& e)
    {
        std::cout << "RingBuffer::Attach() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::RingBufferNotAttached("Attach() failed.")) ;;
    }
    try {
       _Pw_Pr_SC->Open() ;
    }
    catch(SHM::SharedMemoryNotOpen& e)
    {
        std::cout << "RingBuffer::Attach() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::RingBufferNotAttached("Attach() failed.")) ;;
    }
    try {
       _Pw_Pr_SC->Attach() ;
    }
    catch(SHM::SharedMemoryNotAttached& e)
    {
        std::cout << "RingBuffer::Attach() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::RingBufferNotAttached("Attach() failed.")) ;;
    }

     #ifdef DEBUG
     std::cout << " The SHMs for RingBuffer from SERVER to CUSTOMER Exchanges are Identified and Attached " << std::endl ;
     std::cout << " Adresse : _Shm_SC->GetShm() = " << _Shm_SC->GetShm() << std::endl ;
     std::cout << " Adresse : _Pw_Pr_SC->GetShm() = " << _Pw_Pr_SC->GetShm() << std::endl ;
     #endif

     }
else{
    try {
       _Shm_CS->Open() ;
    }
    catch(SHM::SharedMemoryNotOpen& e)
    {
        std::cout << "RingBuffer::Attach() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::RingBufferNotAttached("Attach() failed.")) ;;
    }
    try {
       _Shm_CS->Attach() ;
    }
    catch(SHM::SharedMemoryNotAttached& e)
    {
        std::cout << "RingBuffer::Attach() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::RingBufferNotAttached("Attach() failed.")) ;;
    }
    try {
       _Pw_Pr_CS->Open() ;
    }
    catch(SHM::SharedMemoryNotOpen& e)
    {
        std::cout << "RingBuffer::Attach() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::RingBufferNotAttached("Attach() failed.")) ;;
    }
    try {
       _Pw_Pr_CS->Attach() ;
    }
    catch(SHM::SharedMemoryNotAttached& e)
    {
        std::cout << "RingBuffer::Attach() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::RingBufferNotAttached("Attach() failed.")) ;;
    }

     #ifdef DEBUG
     std::cout << " The SHMs for RingBuffer from CUSTOMER to SERVER Exchanges are Identified and Attached " << std::endl ;
     std::cout << " Adresse : _Shm_CS->GetShm() = " << _Shm_CS->GetShm() << std::endl ;
     std::cout << " Adresse : _Pw_Pr_CS->GetShm() = " << _Pw_Pr_CS->GetShm() << std::endl ;
     #endif

     }

if(_Side == BUFFER_CS){
    memcpy(_Pw_Pr_CS->GetShm(), _Tab_CS, 3 * sizeof(int) ) ;
     }
else{
    memcpy(_Pw_Pr_SC->GetShm(), _Tab_SC, 3 * sizeof(int) ) ;
     }

} // End of RingBuffer::Attach()

// ************************************************
// Destructor
// ************************************************
RingBuffer ::~RingBuffer () throw(certi::RingBufferNotDeleted) {
if(_Side == BUFFER_SC){
    try {
       _Sem_SC->Delete() ;
    }
    catch(SHM::HandleNotClosed& e)
    {
        std::cout << "RingBuffer::~RingBuffer() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::RingBufferNotDeleted("~RingBuffer() failed.")) ;;
    }
   }
else{
    try {
       _Sem_CS->Delete() ;
    }
    catch(SHM::HandleNotClosed& e)
    {
        std::cout << "RingBuffer::~RingBuffer() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::RingBufferNotDeleted("~RingBuffer() failed.")) ;;
    }
   }

delete _Sem_SC  ;
delete _Sem_CS  ;
delete _Shm_SC ;
delete _Shm_CS ;
delete _Pw_Pr_SC ;
delete _Pw_Pr_CS ;
} // End of RingBuffer Destructor

// ************************************************
// Method : RingBuffer::Send(...)
// ************************************************
void RingBuffer::Send(void *Buffer, size_t Size)
                      throw (certi::MessageNotSent,
                             certi::MessageTooLong,
                             certi::BufferFull) {

#ifdef DEBUG
std::cout << "RingBuffer --> Try to Send..." << std::endl ;
#endif

if (Size > _Size) {
   throw(certi::MessageTooLong("RingBuffer::Send() failed.")) ;
   }

if(_Side == BUFFER_SC){
   try {
      _Sem_SC->P() ;
   }
   catch(Semaphore::SemaphoreHandlingError& e)
   {
        std::cout << "RingBuffer::Send() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::MessageNotSent("RingBuffer::Send() failed.")) ;;
   }

   memcpy(_Tab_SC, _Pw_Pr_SC->GetShm(), 3 * sizeof(int) ) ;

    #ifdef DEBUG
    std::cout << "RingBuffer::Send(...) --> BEGIN Algorithm : Count_SC = " << _Tab_SC[0] << " | Write_SC = " << _Tab_SC[1] << "| Read_SC = " << _Tab_SC[2] << std::endl ;
    #endif

    if ((int)Size > _Tab_SC[0]) { // Test si il y a assez de place disponible dans le buffer (Exeption a envoyer)
       try {
        _Sem_SC->V() ;
       }
       catch(Semaphore::SemaphoreHandlingError& e)
       {
          std::cout << "RingBuffer::Send() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
          throw (certi::MessageNotSent("RingBuffer::Send() failed.")) ;;
       }
       throw(certi::BufferFull("RingBuffer::Send() failed.")) ;
    }

    if ( (_Tab_SC[2] > _Tab_SC[1]) || ((_Tab_SC[1] >= _Tab_SC[2]) && (Size + _Tab_SC[1] <= _Size)) ) {
    //  Plecture >= Pecriture || Pecriture >= Plecture && Taille a ecrire + Pecriture <=TailleTotaleSegment

        #ifdef DEBUG
        std::cout << "RingBuffer::Send(...) --> Utilisation memcpy Simple " << std::endl ;
        std::cout << "RingBuffer::Send(...) --> Adresse Utilisee  _Shm_SC->GetShm() = " << _Shm_SC->GetShm() << "| _Shm_SC->GetShm() + _Tab_SC[1] = " << (void *)((char *)(_Shm_SC->GetShm()) + (sizeof(void *) * _Tab_SC[1])) << std::endl ;
        #endif

        memcpy((void *)((char *)(_Shm_SC->GetShm())+ (sizeof(void *) * _Tab_SC[1])) , Buffer, Size) ;

        _Tab_SC[0] -= Size ;
        _Tab_SC[1] = (_Tab_SC[1] + Size) % _Size ;
        }
   else {
        #ifdef DEBUG
        std::cout << "RingBuffer::Send(...) --> Utilisation memcpy Double " << std::endl ;
        #endif

        int Rest_byte_in_SHM =  _Size - _Tab_SC[1] ;

        memcpy((void *)((char *)(_Shm_SC->GetShm())+ (sizeof(void *) * _Tab_SC[1])), Buffer, Rest_byte_in_SHM ) ;

        memcpy(_Shm_SC->GetShm(), (void *)((char *)(Buffer) + (sizeof(void *) * Rest_byte_in_SHM)), Size - Rest_byte_in_SHM ) ;

        _Tab_SC[1] = (_Tab_SC[1] + Size) % _Size ;
        _Tab_SC[0] -= Size ;

        } // Fin du if ((_Tab_SC[2] >= _Tab_SC[1]) ...) /else

     #ifdef DEBUG
     std::cout << "RingBuffer::Send(...) --> END of Algorithm : Count_SC = "  << _Tab_SC[0] << " | Write_SC = " << _Tab_SC[1] << "| Read_SC = " << _Tab_SC[2] << std::endl ;
     #endif

     memcpy(_Pw_Pr_SC->GetShm(), _Tab_SC, 3 * sizeof(int) ) ;

     try {
        _Sem_SC->V() ;
     }
     catch(Semaphore::SemaphoreHandlingError& e)
     {
        std::cout << "RingBuffer::Send() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::MessageNotSent("RingBuffer::Send() failed.")) ;;
     }
     }
else{
    try {
       _Sem_CS->P() ;
    }
    catch(Semaphore::SemaphoreHandlingError& e)
    {
       std::cout << "RingBuffer::Send() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
       throw (certi::MessageNotSent("RingBuffer::Send() failed.")) ;;
    }

    memcpy(_Tab_CS, _Pw_Pr_CS->GetShm(), 3 * sizeof(int) ) ;

    #ifdef DEBUG
    std::cout << "RingBuffer::Send(...) --> BEGIN Algorithm : Count_CS = " << _Tab_CS[0] << " | Write_CS = " << _Tab_CS[1] << "| Read_CS = " << _Tab_CS[2] << std::endl ;
    #endif

    if ((int)Size > _Tab_CS[0]) { // Test si il y a assez de place disponible dans le buffer
       try {
          _Sem_CS->V() ;
       }
       catch(Semaphore::SemaphoreHandlingError& e)
       {
          std::cout << "RingBuffer::Send() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
          throw (certi::MessageNotSent("RingBuffer::Send() failed.")) ;;
       }
       throw(certi::BufferFull("RingBuffer::Send() failed.")) ;
    }

    if ( (_Tab_CS[2] > _Tab_CS[1]) || ((_Tab_CS[1] >= _Tab_CS[2]) && (Size + _Tab_CS[1] <= _Size)) ) {
    //  Plecture >= Pecriture || Pecriture >= Plecture && Taille a ecrire + Pecriture <=TailleTotaleSegment

        #ifdef DEBUG
        std::cout << "RingBuffer::Send(...) --> Utilisation memcpy Simple " << std::endl ;
        std::cout << "RingBuffer::Send(...) --> Adresse Utilisee  _Shm_CS->GetShm() = " <<  _Shm_CS->GetShm() << "| _Shm_CS->GetShm() + _Tab_CS[1] = " << (void *)((char *)(_Shm_CS->GetShm())+ (sizeof(void *) * _Tab_CS[1])) << std::endl ;
        #endif

        memcpy((void *)((char *)(_Shm_CS->GetShm())+ (sizeof(void *) * _Tab_CS[1])) , Buffer, Size) ;

        _Tab_CS[0] -= Size ;
        _Tab_CS[1] = (_Tab_CS[1] + Size) % _Size ;
        }
   else {
        #ifdef DEBUG
        std::cout << "RingBuffer::Send(...) --> Utilisation memcpy Double " << std::endl ;
        #endif

        int Rest_byte_in_SHM =  _Size - _Tab_CS[1] ;

        memcpy((void *)((char *)(_Shm_CS->GetShm())+ (sizeof(void *) * _Tab_CS[1])), Buffer, Rest_byte_in_SHM ) ;

        memcpy(_Shm_CS->GetShm(), (void *)((char *)(Buffer) + (sizeof(void *) * Rest_byte_in_SHM)), Size - Rest_byte_in_SHM ) ;

         _Tab_CS[1] = (_Tab_CS[1] + Size) % _Size ;

         _Tab_CS[0] -= Size ;

       }

      #ifdef DEBUG
      std::cout << "RingBuffer::Send(...) --> END of Algorithm : Count_CS = " << _Tab_CS[0] << " | Write_CS = " << _Tab_CS[1] << "| Read_CS = " << _Tab_CS[2] << std::endl ;
      #endif

     memcpy(_Pw_Pr_CS->GetShm(), _Tab_CS, 3 * sizeof(int) ) ;

     try {
        _Sem_CS->V() ;
     }
     catch(Semaphore::SemaphoreHandlingError& e)
     {
        std::cout << "RingBuffer::Send() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
        throw (certi::MessageNotSent("RingBuffer::Send() failed.")) ;;
     }
     }

#ifdef DEBUG
std::cout << "RingBuffer --> Send Complete !!" << std::endl ;
#endif

} // End of RingBuffer::Send(...)

// ************************************************
// Method : RingBuffer::Receive(...)
// ************************************************
void RingBuffer::Receive(void *Buffer, size_t Size)
                         throw (certi::MessageNotReceived,
                                certi::MessageTooLong,
                                certi::BufferEmpty) {

#ifdef DEBUG
std::cout << "RingBuffer -->  Try to Receive..." << std::endl ;
#endif

if (Size > _Size) {
   throw(certi::MessageTooLong("RingBuffer::Receive() failed.")) ;
   }

if(_Side == BUFFER_SC){
    try {
       _Sem_CS->P() ;
    }
    catch(Semaphore::SemaphoreHandlingError& e)
    {
       std::cout << "RingBuffer::Receive() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
       throw (certi::MessageNotReceived("RingBuffer::Receive() failed.")) ;;
    }

    memcpy(_Tab_CS, _Pw_Pr_CS->GetShm(), 3 * sizeof(int) ) ;

    if (_Tab_CS[0] == (int) _Size ) { // Test si il y a assez de place disponible dans le buffer (Exeption Ã  envoyer)
       try {
          _Sem_CS->V() ;
       }
       catch(Semaphore::SemaphoreHandlingError& e)
       {
          std::cout << "RingBuffer::Send() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
          throw (certi::MessageNotReceived("RingBuffer::Receive() failed.")) ;;
       }
       throw(certi::BufferEmpty("RingBuffer::Receive() failed.")) ;

       #ifdef DEBUG
       std::cout << "lllll RingBuffer::Receive(...) --> Nothing to Read on _Shm_SC !!"<< std::endl ;
       #endif
    }
    else {

         #ifdef DEBUG
         std::cout << "RingBuffer::Receive(...) --> BEGIN Algorithm : Count_CS = " << _Tab_CS[0] << " | Write_CS = " << _Tab_CS[1] << "| Read_CS = " << _Tab_CS[2] << std::endl ;
         #endif

        if ( (_Tab_CS[1] > _Tab_CS[2]) || ((_Tab_CS[2] >= _Tab_CS[1]) && (Size + _Tab_CS[2] <= _Size)) ) {
       //  Plecture >= Pecriture || Pecriture >= Plecture && Taille a ecrire + Pecriture <=TailleTotaleSegment

            #ifdef DEBUG
            std::cout << "RingBuffer::Receive(...) --> Utilisation memcpy Simple " << std::endl ;
            std::cout << "RingBuffer::Receive(...) --> Adresse Utilisee  _Shm_CS->GetShm() = " <<  _Shm_CS->GetShm() << "| _Shm_CS->GetShm() + _Tab_CS[2] = " << (void *)((char *)(_Shm_CS->GetShm())+ (sizeof(void *) * _Tab_CS[2])) << std::endl ;
            #endif

            memcpy(Buffer, (void *)((char *)(_Shm_CS->GetShm())+ (sizeof(void *) * _Tab_CS[2])) ,Size) ;

            _Tab_CS[0] += Size ;
            _Tab_CS[2] += Size ;
            }
       else {

            #ifdef DEBUG
            std::cout << "RingBuffer::Receive(...) --> Utilisation memcpy Double " << std::endl ;
            #endif

            int Rest_byte_in_SHM =  _Size - _Tab_CS[2] ;

            memcpy( Buffer, (void *)((char *)(_Shm_CS->GetShm())+ (sizeof(void *) * _Tab_CS[2])), Rest_byte_in_SHM ) ;

            memcpy( (void *)((char *)(Buffer) + (sizeof(void *) * Rest_byte_in_SHM)), _Shm_CS->GetShm(), Size - Rest_byte_in_SHM ) ;

            _Tab_CS[2] = (_Tab_CS[2] + Size) % _Size ;
            _Tab_CS[0] += Size ;
           } // Fin du if ((_Tab_SC[2] >= _Tab_SC[1]) ...) /else

        #ifdef DEBUG
        std::cout << "RingBuffer::Receive(...) --> END Algorithm : Count_CS = " << _Tab_CS[0] << " | Write_CS = " << _Tab_CS[1] << "| Read_CS = " << _Tab_CS[2] << std::endl ;
        std::cout << "AVANT  memcpy(_Pw_Pr_CS->GetShm(), _Tab_CS, 3 * sizeof(int) ) ; !! " << std::endl ;
        #endif

        memcpy(_Pw_Pr_CS->GetShm(), _Tab_CS, 3 * sizeof(int) ) ;

        #ifdef DEBUG
        std::cout << "APRES  memcpy(_Pw_Pr_CS->GetShm(), _Tab_CS, 3 * sizeof(int) ) ; !! " << std::endl ;
        #endif

        } // Fin du else for if (_Tab_SC[0] == _Size )

     try {
     _Sem_CS->V() ;
     }
     catch(Semaphore::SemaphoreHandlingError& e)
     {
       std::cout << "RingBuffer::Receive() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
       throw (certi::MessageNotReceived("RingBuffer::Receive() failed.")) ;;
     }
     }
else{
    try {
       _Sem_SC->P() ;
    }
    catch(Semaphore::SemaphoreHandlingError& e)
    {
       std::cout << "RingBuffer::Receive() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
       throw (certi::MessageNotReceived("RingBuffer::Receive() failed.")) ;;
    }

    memcpy(_Tab_SC, _Pw_Pr_SC->GetShm(), 3 * sizeof(int) ) ;

    if (_Tab_SC[0] == (int)_Size ) { // Test si il y a assez de place disponible dans le buffer (Exeption Ã  envoyer)
       try {
         _Sem_SC->V() ;
       }
       catch(Semaphore::SemaphoreHandlingError& e)
       {
          std::cout << "RingBuffer::Receive() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
          throw (certi::MessageNotReceived("RingBuffer::Receive() failed.")) ;
       }
       throw(certi::BufferEmpty("RingBuffer::Receive() failed.")) ;

         #ifdef DEBUG
         std::cout << "RingBuffer::Receive(...) --> Nothing to Read on _Shm_CS !!"<< std::endl ;
         #endif

         }
    else {

         #ifdef DEBUG
         std::cout << "RingBuffer::Receive(...) --> Adresse _Tab_SC = " << _Tab_SC << " Adresse _Pw_Pr_SC = " << _Pw_Pr_SC << std::endl ;
         std::cout << "RingBuffer::Receive(...) --> Begin of Algorithm : Count_SC = " << _Tab_SC[0] << " | Write_SC = " << _Tab_SC[1] << "| Read_SC = " << _Tab_SC[2] << std::endl ;
         #endif

        if ( (_Tab_SC[1] > _Tab_SC[2]) || ((_Tab_SC[2] >= _Tab_SC[1]) && (Size + _Tab_SC[2] <= _Size)) ) {
        //  Plecture >= Pecriture || Pecriture >= Plecture && Taille a ecrire + Pecriture <=TailleTotaleSegment

            #ifdef DEBUG
            std::cout << "RingBuffer::Receive(...) --> Utilisation memcpy Simple " << std::endl ;
            std::cout << "RingBuffer::Receive(...) --> Adresse Utilisee  _Shm_SC->GetShm() = " <<  _Shm_SC->GetShm() << "| _Shm_SC->GetShm() + _Tab_SC[2] = " << (void *)((char *)(_Shm_SC->GetShm())+ (sizeof(void *) * _Tab_SC[2])) << std::endl ;
            #endif

            memcpy( Buffer, (void *)((char *)(_Shm_SC->GetShm())+ (sizeof(void *) * _Tab_SC[2])) , Size) ;

            _Tab_SC[0] += Size ;
            _Tab_SC[2] += Size ;
            }
       else {

            #ifdef DEBUG
            std::cout << "RingBuffer::Receive(...) --> Utilisation memcpy Double " << std::endl ;
            #endif

            int Rest_byte_in_SHM =  _Size - _Tab_SC[2] ;

            memcpy(Buffer, (void *)((char *)(_Shm_SC->GetShm())+ (sizeof(void *) * _Tab_SC[2])), Rest_byte_in_SHM ) ;

            memcpy((void *)((char *)(Buffer) + (sizeof(void *) * Rest_byte_in_SHM)), _Shm_SC->GetShm(), Size - Rest_byte_in_SHM ) ;

            _Tab_SC[2] = (_Tab_SC[2] + Size) % _Size ;
            _Tab_SC[0] += Size ;
            }

         #ifdef DEBUG
         std::cout << "RingBuffer::Receive(...) --> End of Algorithm : Count_SC = " << _Tab_SC[0] << " | Write_SC = " << _Tab_SC[1] << "| Read_SC = " << _Tab_SC[2] << std::endl ;
         #endif

         memcpy(_Pw_Pr_SC->GetShm(), _Tab_SC, 3 * sizeof(int) ) ;

         }  // Fin du else for if (_Tab_CS[0] == _Size )
     try {
        _Sem_SC->V() ;
     }
     catch(Semaphore::SemaphoreHandlingError& e)
     {
       std::cout << "RingBuffer::Receive() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
       throw (certi::MessageNotReceived("RingBuffer::Receive() failed.")) ;;
     }
     }

#ifdef DEBUG
std::cout << "RingBuffer --> Receive complete!!!" << std::endl ;
#endif

} // End of RingBuffer::Receive(...)

// ************************************************
// Method : RingBuffer::Close()
// ************************************************
void RingBuffer::Close()
                throw (certi::RingBufferNotClosed) {
try {
   _Shm_SC->Close() ;
}
catch(SHM::SharedMemoryNotClosed& e)
{
    std::cout << "RingBuffer::Close() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
    throw (certi::RingBufferNotClosed("RingBuffer::Close() failed.")) ;;
}
catch(SHM::HandleNotClosed& e)
{
    std::cout << "RingBuffer::Close() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
    throw (certi::RingBufferNotClosed("RingBuffer::Close() failed.")) ;;
}
try {
   _Shm_CS->Close() ;
}
catch(SHM::SharedMemoryNotClosed& e)
{
    std::cout << "RingBuffer::Close() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
    throw (certi::RingBufferNotClosed("RingBuffer::Close() failed.")) ;;
}
catch(SHM::HandleNotClosed& e)
{
    std::cout << "RingBuffer::Close() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
    throw (certi::RingBufferNotClosed("RingBuffer::Close() failed.")) ;;
}
try {
   _Pw_Pr_SC->Close() ;
}
catch(SHM::SharedMemoryNotClosed& e)
{
    std::cout << "RingBuffer::Close() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
    throw (certi::RingBufferNotClosed("RingBuffer::Close() failed.")) ;;
}
catch(SHM::HandleNotClosed& e)
{
    std::cout << "RingBuffer::Close() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
    throw (certi::RingBufferNotClosed("RingBuffer::Close() failed.")) ;;
}
try {
   _Pw_Pr_CS->Close() ;
}
catch(SHM::SharedMemoryNotClosed& e)
{
    std::cout << "RingBuffer::Close() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
    throw (certi::RingBufferNotClosed("RingBuffer::Close() failed.")) ;;
}
catch(SHM::HandleNotClosed& e)
{
    std::cout << "RingBuffer::Close() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
    throw (certi::RingBufferNotClosed("RingBuffer::Close() failed.")) ;;
}

} // End of RingBuffer::Close()

