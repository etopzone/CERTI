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
#else
   #include "SemaphorePosix.hh"
   #include "SHMPosix.hh"
   #include "SemaphoreSysV.hh"
   #include "SHMSysV.hh"
#endif

#include "RingBuffer.hh"

RingBuffer::RingBuffer(const std::string& RingBuffer_Name,
           	       const BUFFER_SIDE_t& RingBuffer_Side,
                       const int RingBuffer_Size,
                       const std::string& Shm_Sem_Type ) {

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
  _Sem_SC->Create_Init(1, Semaphore::buildSemName(RingBuffer_Name+"_BUFFER_SC")) ;
  }
else{
  _Sem_CS->Create_Init(1, Semaphore::buildSemName(RingBuffer_Name+"_BUFFER_CS")) ;
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
     _Shm_SC->Open() ;
     _Shm_SC->Attach() ;
     _Pw_Pr_SC->Open() ;
     _Pw_Pr_SC->Attach() ;
    #ifdef DEBUG
    std::cout << " The SHMs for RingBuffer from SERVER to CUSTOMER Exchange are Created and Attached " << std::endl ;
    std::cout << " -----> Adresse : _Shm_SC->GetShm() = " << _Shm_SC->GetShm() << std::endl ;
    std::cout << " -----> Adresse : _Pw_Pr_SC->GetShm() = " << _Pw_Pr_SC->GetShm() << std::endl ;
    #endif
     }
else{
     _Shm_CS->Open() ;
     _Shm_CS->Attach() ;
     _Pw_Pr_CS->Open() ;
     _Pw_Pr_CS->Attach() ;
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
void RingBuffer::Attach() {

if(_Side == BUFFER_CS){
  _Sem_SC->Attach(Semaphore::buildSemName(_Name+"_BUFFER_SC")) ;
  }
else{
  _Sem_CS->Attach(Semaphore::buildSemName(_Name+"_BUFFER_CS")) ;
  }

if(_Side == BUFFER_CS){
     _Shm_SC->Open() ;
     _Shm_SC->Attach() ;
     _Pw_Pr_SC->Open() ;
     _Pw_Pr_SC->Attach() ;
     #ifdef DEBUG
     std::cout << " The SHMs for RingBuffer from SERVER to CUSTOMER Exchanges are Identified and Attached " << std::endl ;
     std::cout << " Adresse : _Shm_SC->GetShm() = " << _Shm_SC->GetShm() << std::endl ;
     std::cout << " Adresse : _Pw_Pr_SC->GetShm() = " << _Pw_Pr_SC->GetShm() << std::endl ;
     #endif
     }
else{
     _Shm_CS->Open() ;
     _Shm_CS->Attach() ;
     _Pw_Pr_CS->Open() ;
     _Pw_Pr_CS->Attach() ;
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
RingBuffer ::~RingBuffer (){
if(_Side == BUFFER_SC){
   _Sem_SC->Delete() ;
   }
else{
   _Sem_CS->Delete() ;
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
void RingBuffer::Send(void *Buffer, size_t Size) {

#ifdef DEBUG
std::cout << "RingBuffer --> Try to Send..." << std::endl ;
#endif

if (Size > _Size) {
   perror("RingBuffer::Send(...) : Size too big !! ") ;
   exit(1);
   }

if(_Side == BUFFER_SC){

    _Sem_SC->P() ;
    memcpy(_Tab_SC, _Pw_Pr_SC->GetShm(), 3 * sizeof(int) ) ;

    #ifdef DEBUG
    std::cout << "RingBuffer::Send(...) --> BEGIN Algorithm : Count_SC = " << _Tab_SC[0] << " | Write_SC = " << _Tab_SC[1] << "| Read_SC = " << _Tab_SC[2] << std::endl ;
    #endif

    if (Size > _Tab_SC[0]) { // Test si il y a assez de place disponible dans le buffer (Exeption Ã  envoyer)
    perror("Probleme !! ") ;
    exit(1);
    }

    if ( (_Tab_SC[2] > _Tab_SC[1]) || ((_Tab_SC[1] >= _Tab_SC[2]) && (Size + _Tab_SC[1] <= _Size)) ) {
    //  Plecture >= Pecriture || Pecriture >= Plecture && Taille a ecrire + Pecriture <=TailleTotaleSegment

        #ifdef DEBUG
        std::cout << "RingBuffer::Send(...) --> Utilisation memcpy Simple " << std::endl ;
        std::cout << "RingBuffer::Send(...) --> Adresse Utilisee  _Shm_SC->GetShm() = " << _Shm_SC->GetShm() << "| _Shm_SC->GetShm() + _Tab_SC[1] = " << (void *)((char *)(_Shm_SC->GetShm()) + (sizeof(void *) * _Tab_SC[1])) << std::endl ;
        #endif

        memcpy((void *)((char *)(_Shm_SC->GetShm())+ (sizeof(void *) * _Tab_SC[1])) , Buffer, Size) ;

        _Tab_SC[0] -= Size ;
        // _Tab_SC[1] += Size ;
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
        // _Tab_SC[0] = _Tab_SC[2] - _Tab_SC[1]  ;

        } // Fin du if ((_Tab_SC[2] >= _Tab_SC[1]) ...) /else

     #ifdef DEBUG
     std::cout << "RingBuffer::Send(...) --> END of Algorithm : Count_SC = "  << _Tab_SC[0] << " | Write_SC = " << _Tab_SC[1] << "| Read_SC = " << _Tab_SC[2] << std::endl ;
     #endif

     memcpy(_Pw_Pr_SC->GetShm(), _Tab_SC, 3 * sizeof(int) ) ;

     _Sem_SC->V() ;
     }
else{

    _Sem_CS->P() ;
    memcpy(_Tab_CS, _Pw_Pr_CS->GetShm(), 3 * sizeof(int) ) ;

    #ifdef DEBUG
    std::cout << "RingBuffer::Send(...) --> BEGIN Algorithm : Count_CS = " << _Tab_CS[0] << " | Write_CS = " << _Tab_CS[1] << "| Read_CS = " << _Tab_CS[2] << std::endl ;
    #endif

    if (Size > _Tab_CS[0]) { // Test si il y a assez de place disponible dans le buffer
    perror("Probleme !! ") ;
    exit(1);
    }

    if ( (_Tab_CS[2] > _Tab_CS[1]) || ((_Tab_CS[1] >= _Tab_CS[2]) && (Size + _Tab_CS[1] <= _Size)) ) {
    //  Plecture >= Pecriture || Pecriture >= Plecture && Taille a ecrire + Pecriture <=TailleTotaleSegment

        #ifdef DEBUG
        std::cout << "RingBuffer::Send(...) --> Utilisation memcpy Simple " << std::endl ;
        std::cout << "RingBuffer::Send(...) --> Adresse Utilisee  _Shm_CS->GetShm() = " <<  _Shm_CS->GetShm() << "| _Shm_CS->GetShm() + _Tab_CS[1] = " << (void *)((char *)(_Shm_CS->GetShm())+ (sizeof(void *) * _Tab_CS[1])) << std::endl ;
        #endif

        memcpy((void *)((char *)(_Shm_CS->GetShm())+ (sizeof(void *) * _Tab_CS[1])) , Buffer, Size) ;
        _Tab_CS[0] -= Size ;
       // _Tab_CS[1] += Size ;
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
         // _Tab_CS[0] = _Tab_CS[2] - _Tab_CS[1]  ;
         _Tab_CS[0] -= Size ;

       }

      #ifdef DEBUG
      std::cout << "RingBuffer::Send(...) --> END of Algorithm : Count_CS = " << _Tab_CS[0] << " | Write_CS = " << _Tab_CS[1] << "| Read_CS = " << _Tab_CS[2] << std::endl ;
      #endif

     memcpy(_Pw_Pr_CS->GetShm(), _Tab_CS, 3 * sizeof(int) ) ;

     _Sem_CS->V() ;
     }

#ifdef DEBUG
std::cout << "RingBuffer --> Send Complete !!" << std::endl ;
#endif
} // End of RingBuffer::Send(...)

// ************************************************
// Method : RingBuffer::Receive(...)
// ************************************************
void RingBuffer::Receive(void *Buffer, size_t Size) {

#ifdef DEBUG
std::cout << "RingBuffer -->  Try to Receive..." << std::endl ;
#endif

if (Size > _Size) {
    perror("RingBuffer::Receive(...) : Size too big !! ") ;
    exit(1);
    }

if(_Side == BUFFER_SC){

    _Sem_CS->P() ;

    memcpy(_Tab_CS, _Pw_Pr_CS->GetShm(), 3 * sizeof(int) ) ;

    if (_Tab_CS[0] == _Size ) { // Test si il y a assez de place disponible dans le buffer (Exeption Ã  envoyer)
       #ifdef DEBUG
       std::cout << "RingBuffer::Receive(...) --> Nothing to Read on _Shm_SC !!"<< std::endl ;
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
            // _Tab_CS[0] = (_Tab_CS[0] + Size) % _Size ;
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
        #endif
        std::cout << "AVANT  memcpy(_Pw_Pr_CS->GetShm(), _Tab_CS, 3 * sizeof(int) ) ; !! " << std::endl ;
        memcpy(_Pw_Pr_CS->GetShm(), _Tab_CS, 3 * sizeof(int) ) ;
        std::cout << "APRES  memcpy(_Pw_Pr_CS->GetShm(), _Tab_CS, 3 * sizeof(int) ) ; !! " << std::endl ;
        } // Fin du else for if (_Tab_SC[0] == _Size )

     _Sem_CS->V() ;
     }
else{

    _Sem_SC->P() ;
       memcpy(_Tab_SC, _Pw_Pr_SC->GetShm(), 3 * sizeof(int) ) ;

    if (_Tab_SC[0] == _Size ) { // Test si il y a assez de place disponible dans le buffer (Exeption Ã  envoyer)
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
           //  _Tab_SC[0] = (_Tab_SC[0] + Size) % _Size ;
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
            // _Tab_CS[0] = _Tab_CS[2] - _Tab_CS[1]  ;
            _Tab_SC[0] += Size ;
            }

         #ifdef DEBUG
         std::cout << "RingBuffer::Receive(...) --> End of Algorithm : Count_SC = " << _Tab_SC[0] << " | Write_SC = " << _Tab_SC[1] << "| Read_SC = " << _Tab_SC[2] << std::endl ;
         #endif

         memcpy(_Pw_Pr_SC->GetShm(), _Tab_SC, 3 * sizeof(int) ) ;
         }  // Fin du else for if (_Tab_CS[0] == _Size )

     _Sem_SC->V() ;
     }

#ifdef DEBUG
std::cout << "RingBuffer --> Receive complete!!!" << std::endl ;
#endif
} // End of RingBuffer::Receive(...)

// ************************************************
// Method : RingBuffer::Close()
// ************************************************
void RingBuffer::Close() {

_Shm_SC->Close() ;
_Shm_CS->Close() ;
_Pw_Pr_SC->Close() ;
_Pw_Pr_CS->Close() ;

} // End of RingBuffer::Close()

