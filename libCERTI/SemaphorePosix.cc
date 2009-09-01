#include "SemaphorePosix.hh"

// ************************************************
// Constructor
// ************************************************
SemaphorePosix::SemaphorePosix(){}

// ************************************************
// Destructor
// ************************************************
SemaphorePosix::~SemaphorePosix() {}

// ************************************************
// Method : SemaphorePosix::Create_Init(...)
// ************************************************
void SemaphorePosix::Create_Init(const int initval, const std::string& New_Semname) {

 _Semname.assign(New_Semname) ;
sem_unlink(_Semname.c_str()) ;
_Sem = sem_open( _Semname.c_str(), O_CREAT | O_EXCL, S_IRUSR | S_IWUSR ,(int)initval);
if(_Sem == SEM_FAILED) {
     printf("Erreur de creation du semaphore ") ; 
     }

int sem_val ;
sem_getvalue(_Sem, &sem_val); 
printf("On a ouvert le semaphore %s avec une valeur %d\n", _Semname.c_str(), sem_val) ; 
} // End of Create_Init(...)

// ************************************************
// Method : SemaphorePosix::Attach(...)
// ************************************************
void SemaphorePosix::Attach(const std::string& New_Semname) {

_Semname.assign(New_Semname) ;
_Sem = sem_open( _Semname.c_str(), O_CREAT);  
if(_Sem == SEM_FAILED){
   printf("Erreur de creation du semaphore ") ; 
   } 

int sem_val ;
sem_getvalue(_Sem, &sem_val); 
printf("On a attaché le semaphore %s avec une valeur %d\n", _Semname.c_str(), sem_val) ; 
} // End of Attach(...)

// ************************************************
// Method : SemaphorePosix::P()
// ************************************************
// Decrement Semaphore Value SV=SV-1 

void SemaphorePosix::P() {

int sem_val ;
sem_getvalue(_Sem, &sem_val); 
printf("Debut P() Pour le Semaphore %s qui a une valeur %d \n", _Semname.c_str(), sem_val ) ;
sem_wait(_Sem) ;
sem_getvalue(_Sem, &sem_val);
printf("Fin P() Pour le Semaphore %s qui a une valeur %d \n", _Semname.c_str(), sem_val ) ; 
} // End of P()


// ************************************************
// Methode : SemaphorePosix::V()
// ************************************************
// Increment Semaphore Value SV=SV+1 

void SemaphorePosix::V() {

int sem_val ;
sem_getvalue(_Sem, &sem_val); 
printf("Debut V() Pour le Semaphore %s qui a une valeur %d \n", _Semname.c_str(), sem_val ) ;
sem_post(_Sem) ;
sem_getvalue(_Sem, &sem_val);
printf("Fin V() Pour le Semaphore %s qui a une valeur %d \n", _Semname.c_str(), sem_val ) ; 
printf("\n") ; 
} // End of V()

// ************************************************
// Methode : SemaphorePosix::Delete()
// ************************************************
void SemaphorePosix::Delete(){
sem_close(_Sem) ;
sem_destroy(_Sem);
} // End of Delete()

