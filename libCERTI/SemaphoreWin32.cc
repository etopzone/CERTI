#include "SemaphoreWin32.hh"

// ************************************************
// Constructor
// ************************************************
SemaphoreWin32::SemaphoreWin32(){
_hSemaphore = NULL ;
}

// ************************************************
// Destructor
// ************************************************
SemaphoreWin32::~SemaphoreWin32() {}

// ************************************************
// Method : SemaphoreWin32::Create_Init(...)
// ************************************************
void SemaphoreWin32::Create_Init(const int initval, const std::string& New_Semname)
                                 throw(certi::SemaphoreNotCreated) {

_hSemaphore = CreateSemaphore(
                      (LPSECURITY_ATTRIBUTES)NULL,             // security attributes
                      (LONG)(initval),                         // initial count
                      (LONG)(1),                               // maximum count
                      (LPCTSTR)(New_Semname.c_str()));         // named semaphore

 if (_hSemaphore == NULL){
      throw(certi::SemaphoreNotCreated("CreateSemaphore() failed.")) ;
 }
 #ifdef DEBUG
 std::cout << "We create the semaphore identified by handle : " << _hSemaphore << " and name : " << New_Semname << std::endl ;
 #endif

} // End of method : Create_Init(...)

// ************************************************
// Method : SemaphoreWin32::Attach(...)
// ************************************************
void SemaphoreWin32::Attach(const std::string& New_Semname)
                            throw(certi::SemaphoreNotOpen){

// Open the semaphore

   _hSemaphore = OpenSemaphore(
                      SEMAPHORE_ALL_ACCESS,             // security attributes
                      FALSE,                            // Inherit Handle
                      (LPCTSTR)(New_Semname.c_str()));                        // named semaphore

#ifdef DEBUG
  std::cout << "We try to attach the semaphore identified by handle : " << _hSemaphore << " and name : " << New_Semname << std::endl ;
#endif

   if (_hSemaphore == NULL)
   {
        throw(certi::SemaphoreNotOpen("OpenSemaphore() failed.")) ;
   }

} // End of method : Attach(...)

// ************************************************
// Method : SemaphoreWin32::P
// ************************************************

void SemaphoreWin32::P()
                    throw(certi::SemaphoreHandlingError) {

#ifdef DEBUG
std::cout << "Begin of Operation P for the semaphore identified by handle : " << _hSemaphore << std::endl ;
#endif

DWORD  WINAPI dwRetCode;

dwRetCode = WaitForSingleObject(
                   (HANDLE)_hSemaphore,  // handle to semaphore
                   INFINITE);    // if we want to have P blocked

switch (dwRetCode)
      {
    // The semaphore object was signaled.
          case WAIT_OBJECT_0:
    // Semaphore is signaled
    // go ahead and continue the work
             break;

          default:
             // Handle errors
             throw(certi::SemaphoreHandlingError("WaitForSingleObject() failed.")) ;
      }

#ifdef DEBUG
std::cout << "End of Operation P for the semaphore identified by handle : " << _hSemaphore << std::endl  ;
#endif

} // End of P()


// ************************************************
// Method : SemaphoreWin32::V
// ************************************************

void SemaphoreWin32::V()
                    throw(certi::SemaphoreHandlingError) {

#ifdef DEBUG
std::cout << "Begin of Operation V for the semaphore identified by handle : " << _hSemaphore << std::endl ;
#endif

BOOL WINAPI retcode ;

  retcode = ReleaseSemaphore(
        _hSemaphore,  // handle to semaphore
        1,            // increase count by one
        NULL) ;       // not interested in previous count

  if (retcode == 0) {
      throw(certi::SemaphoreHandlingError("ReleaseSemaphore() failed.")) ;
    }

#ifdef DEBUG
std::cout << "End of Operation V for the semaphore identified by handle : " << _hSemaphore << std::endl  ;
#endif

} // End of V()

// ************************************************
// Method : SemaphoreWin32::Delete
// ************************************************
void SemaphoreWin32::Delete()
                    throw(certi::HandleNotClosed) {

BOOL WINAPI retcode ;

#ifdef DEBUG
std::cout << "Destroy the semaphore identified by handle : " << _hSemaphore << std::endl ;
#endif

retcode = CloseHandle(_hSemaphore);

if(retcode == 0)
   throw(certi::HandleNotClosed("CloseHandle() failed.")) ;
} // End of Delete()

