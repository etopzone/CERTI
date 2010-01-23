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

// WARNING For SysV use :
// Before Use : --> #ipcs
// You can see all the semaphore and the shared memory in use
//  --> #ipcrm -m id_memory to erase a memory segment
//  --> #ipcrm -s id_semaphore to erase a semaphore

// Systems includes
#include <limits>

// Specifics includes
#ifdef _WIN32
  #ifndef NOMINMAX
  #define NOMINMAX
  #endif
  #include <windows.h>
  #include <process.h>
  #include "SocketSHMWin32.hh"
#else
  #include "SocketSHMPosix.hh"
  #include "SocketSHMSysV.hh"
#endif

#include "RingBuffer.hh"
#include "SharedStruct.hh"


#define NAME_AB "SocketAB"  // Socket Name
#define MAX_SIZE 50


/****************************************/
/*********** MAIN PROGRAM ***************/
/****************************************/

int main(){

// ************
// DECLARATIONS
// ************
int i = 1 ; // Count for while
shared_struct Data_Read ; // Data read in the Socket
shared_struct Data_Write ; // Data write in the Socket
std::string Shm_Sem_Type = "Win32" ;

#ifdef SIDE_SC
SocketSHM::SHM_SIDE_t Socket_Side = SocketSHM::SHM_SC ; // which side am I?
RingBuffer::BUFFER_SIDE_t Buffer_Side = RingBuffer::BUFFER_SC ; // which Buffer side am I?
#else
SocketSHM::SHM_SIDE_t Socket_Side = SocketSHM::SHM_CS ; // which side am I?
RingBuffer::BUFFER_SIDE_t Buffer_Side = RingBuffer::BUFFER_CS ; // which Buffer side am I?
#endif

std::string command;


int Size ;
Size = (int) sizeof(shared_struct) ;

// Initialize Datas
Data_Read.Header = 0 ; Data_Read.Body =0.0 ;
Data_Write.Header = 0 ; Data_Write.Body =0.0 ;

// ************
//    CODE
// ************
#ifdef _WIN32
SocketSHMWin32 Socket_Win32_AB(NAME_AB,Socket_Side,Size) ;
#else
// Posix Socket SHM
SocketSHMPosix Socket_Posix_AB(NAME_AB,
                               Socket_Side,
                               Size) ;

// SystemV Socket SHM
SocketSHMSysV Socket_SysV_AB(NAME_AB,
                             Socket_Side,
                             Size) ;
#endif
// RingBuffer Socket SHM

RingBuffer RingBuffer_AB(NAME_AB,
                         Buffer_Side,
                         MAX_SIZE,
                         Shm_Sem_Type ) ;

// ****************************
// Wainting for User Command n1
std::cout << "************************************************************" << std::endl ;
std::cout << "******* END OF INITIALIZATION PHASE 1 : ********************" << std::endl ;
#ifdef SIDE_SC
std::cout << "*************** Wait for Processus B ***********************" << std::endl ;
#else
std::cout << "*************** Wait for Processus A ***********************" << std::endl ;
#endif
std::cout << "************************************************************" << std::endl ;
std::cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );

#ifdef _WIN32
try {
   Socket_Win32_AB.Connect();
}
catch (certi::SocketNotConnected& e)
{
    std::cout << "Catch Exception SocketNotConnected" << std::endl ;
    std::cout << "SocketSHM::Connect() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
}
#else
Socket_Posix_AB.Connect() ; // Connect to created Semaphores
Socket_SysV_AB.Connect() ; // Connect to created Semaphores
#endif

try {
    RingBuffer_AB.Attach() ;
}
catch (certi::RingBufferNotAttached& e)
{
    std::cout << "Catch Exception RingBufferNotAttached" << std::endl ;
    std::cout << "RingBuffer::Attach() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
}

// ****************************
// Wainting for User Command n2
std::cout << "*******************************************************" << std::endl ;
std::cout << "*********** END OF INITIALIZATION PHASE 2 : ***********" << std::endl ;
#ifdef _WIN32
std::cout << "*** Click \"Win32\" to Open Win32 Socket SHM ************" << std::endl ;
#else
std::cout << "*** Click \"Posix\" to Open Posix Socket SHM ************" << std::endl ;
std::cout << "*** Click \"SysV\" to Open Systeme V Socket SHM *********" << std::endl ;
#endif
std::cout << "*** Click \"RB\" to Open Ring Buffer SHM **************" << std::endl ;
std::cout << "*******************************************************" << std::endl ;
std::cin >> command;
std::cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );

// Verify the command
while (command !="Win32" && command != "Posix" && command != "SysV" && command != "RB") {
    std::cout << "Wrong Command : Command must be : Win32 , Posix , SysV or RB" << std ::endl ;
    std::cin >> command;
    std::cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n' ); }

// Open the Socket
#ifdef _WIN32
if (command=="Win32")
{
    try {
       Socket_Win32_AB.Open() ;
    }
    catch (certi::SocketSHMNotOpen& e)
    {
        std::cout << "Catch Exception SocketSHMNotOpen" << std::endl ;
        std::cout << "SocketSHMWin32::Open() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
    }
}
#else
if (command=="Posix") Socket_Posix_AB.Open() ;
if (command=="SysV") Socket_SysV_AB.Open() ;
#endif

// ****************************
// Wainting for User Command n3
std::cout << "************************************************************" << std::endl ;
std::cout << "******* END OF INITIALIZATION PHASE 3 : ********************" << std::endl ;
std::cout << "****** Click \"ENTER\" to RunInter Process Exchange... *******" << std::endl ;
std::cout << "************************************************************" << std::endl ;
std::cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );

// If Ring Buffer USE
if (command=="RB"){
   shared_struct Data_Read_RB ; // Data read in the Socket
   Data_Read_RB.Header = 0 ; Data_Read_RB.Body =0.0 ;
   shared_struct Data_Write_RB ; // Data write in the Socket
   Data_Write_RB.Header = 0 ; Data_Write_RB.Body =0.0 ;
   std::string command_RB ;
   // *********************************************
   // Wainting for User Command n4 (For Ring Buffer)
   std::cout << "********************************************************" << std::endl ;
   std::cout << "*** Put \"S\" to Send Data on RingBuffer ***************" << std::endl ;
   std::cout << "*** Put \"R\" to Receive Data on RingBuffer ************" << std::endl ;
   std::cout << "*** Put \"END\" to Close RingBuffer Exchange ***********" << std::endl ;
   std::cout << "********************************************************" << std::endl ;

    // *************************
    // Computing RingBuffer SHM
    // *************************
   while(i<1000) {
     // Initialize Datas
     Data_Write_RB.Header = i ; Data_Write_RB.Body = i * 0.1 ;
     std::cin >> command_RB ;
     std::cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );
     if (command_RB=="S"){
         try {
            RingBuffer_AB.Send(&Data_Write_RB,Size) ;
            std::cout << " ******** SEND ********" << std ::endl ;
            #ifdef SIDE_SC
            std::cout << "Processus A :: DataWrite_RB.Header = " << Data_Write_RB.Header << std ::endl ;
            std::cout << "Processus A :: DataWrite_RB.Body = " << Data_Write_RB.Body << std ::endl ;
            #else
            std::cout << "Processus B :: DataWrite_RB.Header = " << Data_Write_RB.Header << std ::endl ;
            std::cout << "Processus B :: DataWrite_RB.Body = " << Data_Write_RB.Body << std ::endl ;
            #endif
            std::cout << " ************************ " <<  std ::endl ;
         }
         catch (certi::BufferFull& e)
         {
            std::cout << "Catch Exception BufferFull" << std::endl ;
            std::cout << "RingBuffer::Send() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
         }
         i++ ;
         }
     else if (command_RB=="R"){
         try {
            RingBuffer_AB.Receive(&Data_Read_RB,Size) ;
         }
         catch(certi::BufferEmpty& e)
         {
             std::cout << "Catch Exception BufferEmpty" << std::endl ;
             std::cout << "RingBuffer::Receive() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
         }

         std::cout << " ****** RECEIVE ******" << std ::endl ;
         #ifdef SIDE_SC
         std::cout << "Processus A :: DataRead_RB.Header = " << Data_Read_RB.Header << std ::endl ;
         std::cout << "Processus A :: DataRead_RB.Body = " << Data_Read_RB.Body << std ::endl ;
         #else
         std::cout << "Processus B :: DataRead_RB.Header = " << Data_Read_RB.Header << std ::endl ;
         std::cout << "Processus B :: DataRead_RB.Body = " << Data_Read_RB.Body << std ::endl ;
         #endif
         std::cout << " ************************** " <<  std::endl ;
         }
      else if (command_RB=="END"){ i=1000; }
      else{std::cout << "Wrong Command : Command must be : S , R or END" << std ::endl ;}
      } // End of while
    }
else{ // else SocketSHM (SysV and Posix) Use
    #ifdef SIDE_SC
    // Send to B For the First Time (INITIALIZE)
    // Send to B
        #ifdef _WIN32
        if (command=="Win32")
        {
            try {
                Socket_Win32_AB.Send(&Data_Write) ;
            }
            catch (certi::MessageNotSent& e)
            {
               std::cout << "Catch Exception MessageNotSent" << std::endl ;
               std::cout << "SocketSHM::Send() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
            }
        }
        #else
        if (command=="Posix") Socket_Posix_AB.Send(&Data_Write) ;
        if (command=="SysV") Socket_SysV_AB.Send(&Data_Write) ;
        #endif
    #endif
    // *********************
    // Computing Socket SHM
    // *********************
    while(i<1000) {
       std::cout << " ***********  COMPUTING PHASE n°= " << i << "*************" << std ::endl ;

       /**************************************/
       /************* RECEIVING **************/
       /**************************************/
       // Read from B
       #ifdef _WIN32
       if (command=="Win32")
       {
           try {
              Socket_Win32_AB.Receive(&Data_Read) ;
           }
           catch (certi::MessageNotReceived& e)
           {
               std::cout << "Catch Exception MessageNotReceived" << std::endl ;
               std::cout << "SocketSHM::Receive() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
           }
       }
       #else
       if (command=="Posix") Socket_Posix_AB.Receive(&Data_Read) ;
       if (command=="SysV") Socket_SysV_AB.Receive(&Data_Read) ;
       #endif

       /**************************************/
       /************* COMPUTE ****************/
       /**************************************/
      // Print Receiving Data
       std::cout << " ****** RECEIVE ******" << std ::endl ;
       #ifdef SIDE_SC
       std::cout << "Processus A :: DataRead.Header = " << Data_Read.Header << std ::endl ;
       std::cout << "Processus A :: DataRead.Body = " << Data_Read.Body << std ::endl ;
       #else
       std::cout << "Processus B :: DataRead.Header = " << Data_Read.Header << std ::endl ;
       std::cout << "Processus B :: DataRead.Body = " << Data_Read.Body << std ::endl ;
       #endif
       std::cout << " ************************** " <<  std::endl ;

       // Product a new Data
       Data_Write.Header = Data_Read.Header + 1 ;
       Data_Write.Body = Data_Read.Body + 0.1 ;

       // Print Sending Data
       std::cout << " ******** SEND ********" << std ::endl ;
       #ifdef SIDE_SC
       std::cout << "Processus A :: DataWrite.Header = " << Data_Write.Header << std ::endl ;
       std::cout << "Processus A :: DataWrite.Body = " << Data_Write.Body << std ::endl ;
       #else
       std::cout << "Processus B :: DataWrite.Header = " << Data_Write.Header << std ::endl ;
       std::cout << "Processus B :: DataWrite.Body = " << Data_Write.Body << std ::endl ;
       #endif
       std::cout << " ************************ " <<  std ::endl ;

       /**************************************/
       /************* SENDING ****************/
       /**************************************/
       // Send to B
       #ifdef _WIN32
       if (command=="Win32")
       {
           try {
               Socket_Win32_AB.Send(&Data_Write) ;
           }
           catch (certi::MessageNotSent& e)
           {
               std::cout << "Catch Exception MessageNotSent" << std::endl ;
               std::cout << "SocketSHM::Send() Exception. " <<  "Name is : " << e._name << " Reason is : " << e._reason << std::endl ;
           }
       }
       #else
       if (command=="Posix") Socket_Posix_AB.Send(&Data_Write) ;
       if (command=="SysV") Socket_SysV_AB.Send(&Data_Write) ;
       #endif

       // On incremente le compteur
       i++ ;

       } // End of while (i<1000)

   } // end of else for if (commande=="RB")

   // Waiting the two process are out of while boucle
   #ifdef _WIN32
     Sleep(1000) ;
   #else
     sleep(1) ;
   #endif

   // Close the Ring Buffer
   RingBuffer_AB.Close() ;
   // Close the socket
   #ifdef _WIN32
      if (command=="Win32") Socket_Win32_AB.Close() ;
    #else
       if (command=="Posix") Socket_Posix_AB.Close() ;;
       if (command=="SysV") Socket_SysV_AB.Close() ;
    #endif
      std::cout << "END OF THE PROGRAM : SEE YOU SOON :) !!!!!!!!!!!"  << std ::endl ;
return 0 ;

} // End of Main Program
