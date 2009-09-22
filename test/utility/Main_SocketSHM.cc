// *****************************************************************************
// To Compile without cmake :
// g++ Main_SocketSHM_A.cc SocketSHMPosix.cc SemaphorePosix.cc SemaphoreWin32.cc SocketSHMSysV.cc SocketSHMWin32.cc SHMSysV.cc SemaphoreSysV.cc SemaphoreWin32.cc -o Processus_A -lrt
// *****************************************************************************
// For SysV use :
// Before Use : --> #ipcs
// You can see all the semaphore and the shared memory in use
//  --> #ipcrm -m id_memory to erase a memory segment
//  --> #ipcrm -s id_semaphore to erase a semaphore

// Systems includes
#include <limits>
#include "SocketSHM.hh"
// Specifics includes

#ifdef _WIN32
  #include <windows.h>
  #include <process.h>
  #include "SocketSHMWin32.hh"
#else
  #include "SocketSHMPosix.hh"
  #include "SocketSHMSysV.hh"
#endif

#include "SharedStruct.hh"


#define NAME_AB "SocketAB"  // Socket Name


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

#ifdef SIDE_SC
SocketSHM::SHM_SIDE_t Socket_Side = SocketSHM::SHM_SC ; // which side am I?
std::cout << "I am side : " << Socket_Side << std::endl ;
#else
SocketSHM::SHM_SIDE_t Socket_Side = SocketSHM::SHM_CS ; // which side am I?
std::cout << "I am side : " << Socket_Side << std::endl ;
#endif

std::string command;


int size ;
size = (int) sizeof(shared_struct) ;

// Initialize Datas
Data_Read.Header = 0 ; Data_Read.Body =0.0 ;
Data_Write.Header = 0 ; Data_Write.Body =0.0 ;

// ************
//    CODE
// ************

#ifdef _WIN32
SocketSHMWin32 Socket_Win32_AB(NAME_AB,Socket_Side,size) ;
#else
// Posix Socket SHM
SocketSHMPosix Socket_Posix_AB(NAME_AB,Socket_Side,size) ;
// SystemV Socket SHM
SocketSHMSysV Socket_SysV_AB(NAME_AB,Socket_Side,size) ;
#endif

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
Socket_Win32_AB.Connect();
#else
Socket_Posix_AB.Connect() ; // Connect to created Semaphores
Socket_SysV_AB.Connect() ;  // Connect to created Semaphores
#endif
#ifndef _WIN32
// Wainting for User Command n2
std::cout << "*******************************************************" << std::endl ;
std::cout << "*********** END OF INITIALIZATION PHASE 2 : ***********" << std::endl ;
std::cout << "*** Click \"Posix\" to Open Posix Socket SHM **********" << std::endl ;
std::cout << "*** Click \"SysV\" to Open Systeme V Socket SHM *******" << std::endl ;
std::cout << "*******************************************************" << std::endl ;
std::cin >> command;
std::cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );
#endif

#ifdef _WIN32
Socket_Win32_AB.Open() ;
#else
// Open the Socket
if (command=="Posix") Socket_Posix_AB.Open() ;
if (command=="SysV") Socket_SysV_AB.Open() ;
#endif

// Wainting for User Command n3
std::cout << "************************************************************" << std::endl ;
#ifdef _WIN32
std::cout << "******* END OF INITIALIZATION PHASE 2 : ********************" << std::endl ;
#else
std::cout << "******* END OF INITIALIZATION PHASE 3 : ********************" << std::endl ;
#endif
std::cout << "****** Click \"ENTER\" to RunInter Process Exchange... *****" << std::endl ;
std::cout << "************************************************************" << std::endl ;
std::cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );

#ifdef SIDE_SC
// Send to B For the First Time (INITIALIZE)
// Send to B
#ifdef _WIN32
Socket_Win32_AB.Send(&Data_Write);
#else
if (command=="Posix") Socket_Posix_AB.Send(&Data_Write) ;
if (command=="SysV") Socket_SysV_AB.Send(&Data_Write) ;
#endif
#endif

 while(i<1000) {
    std::cout << " ***********  COMPUTING PHASE n°= " << i << "*************" << std ::endl ;

    /**************************************/
    /************* RECEIVING **************/
    /**************************************/
    #ifdef _WIN32
     Socket_Win32_AB.Receive(&Data_Read) ;
    #else
    // Read from B
    if (command=="Posix") Socket_Posix_AB.Receive(&Data_Read) ;
    if (command=="SysV") Socket_SysV_AB.Receive(&Data_Read) ;
    #endif

    /**************************************/
    /************* COMPUTE ****************/
    /**************************************/
   // Print Receiving Data
    std::cout << " ****** RECEIVE ******" << std ::endl ;
    std::cout << "Processus A || DataRead.Header = " << Data_Read.Header << std ::endl ;
    std::cout << " Processus A || DataRead.Body = " << Data_Read.Body << std ::endl ;
    std::cout << " ************************** " <<  std::endl ;

    // Product a new Data
    Data_Write.Header = Data_Read.Header + 1 ;
    Data_Write.Body = Data_Read.Body + 0.1 ;

    // Print Sending Data
    std::cout << " ******** SEND ********" << std ::endl ;
    std::cout << "Processus A || DataWrite.Header = " << Data_Write.Header << std ::endl ;
    std::cout << " Processus A || DataWrite.Body = " << Data_Write.Body << std ::endl ;
    std::cout << " ************************ " <<  std ::endl ;

    /**************************************/
    /************* SENDING ****************/
    /**************************************/
    // Send to B
    #ifdef _WIN32
     Socket_Win32_AB.Send(&Data_Write) ;
    #else
    if (command=="Posix") Socket_Posix_AB.Send(&Data_Write) ;
    if (command=="SysV") Socket_SysV_AB.Send(&Data_Write) ;
    #endif

    // On incremente le compteur
    i++ ;

    } // End of while (i<1000)

// Waiting the two process are out of while boucle
#ifdef _WIN32
Sleep(1000);
#else
sleep(1) ;
#endif

// Close the socket
#ifdef _WIN32
Socket_Win32_AB.Close();
#else
if (command=="Posix") Socket_Posix_AB.Close() ;
if (command=="SysV") Socket_SysV_AB.Close() ;
#endif
} // End of Main Program
