#include "SemaphoreSysV.hh"

// ************************************************
// Constructor
// ************************************************
SemaphoreSysV::SemaphoreSysV(){
_Sem_Id = 0 ;
}

// ************************************************
// Destructor
// ************************************************
SemaphoreSysV::~SemaphoreSysV() {}

// ************************************************
// Method : SemaphoreSysV::Create_Init(...)
// ************************************************
void SemaphoreSysV::Create_Init(const int initval, const int cle) {
int flag ;
 // On commence par cr\'eer le s\'emaphore 
 _Sem_Id = semget((key_t)cle,1,IPC_CREAT | IPC_EXCL |0666);
 flag = 1 ;
 if (_Sem_Id == -1){
   _Sem_Id = semget(cle,1,0666) ;
   flag = 2 ;
   if (_Sem_Id == -1) {
      perror("Error with semget()") ;
      exit(1) ;
   }
 }
 /* dans le code ci-dessus, deux processus sans liens
  * de parent\'e peuvent utiliser le m\^eme s\'emaphore \`a condition
  * de conna\^\i{}tre la cl\'e et la cha\^\i{}ne de caract\`eres */
 if (flag == 1) printf("We create the semaphore identified by number %d\n",_Sem_Id) ;
 if (flag == 2) printf("We attach the semaphore identified by number %d\n",_Sem_Id) ;

 union semun{
     int val ;  
   struct semid_ds *buf ; // Not use here
   ushort *array; // Not use here       
 } arg_ctl ;

 // Maintenant on affecte une valeur initiale au s\'emaphore
 arg_ctl.val = initval ;
 if (semctl(_Sem_Id,0,SETVAL,arg_ctl) == -1){
    perror("Error during semaphore initialization") ;
    exit(1) ;
        }
printf("We init the semaphore with value %d\n",initval) ;

} // End of method : sem_create

// ************************************************
// Method : SemaphoreSysV::P
// ************************************************
// Decrement Semaphore Value SV=SV-1 

void SemaphoreSysV::P() {
printf("Begin of Operation P for the semaphore identified by number %d\n",_Sem_Id) ;
struct sembuf semparP ;
  semparP.sem_num = 0 ;
  semparP.sem_op = -1 ;
  semparP.sem_flg = SEM_UNDO ;

 if (semop(_Sem_Id,&semparP,1) == -1) {
    perror("Error in P() : semop()") ;
    exit(1);
    } 
printf("End of Operation P for the semaphore identified by number %d\n",_Sem_Id) ;
} // End of P()


// ************************************************
// Method : SemaphoreSysV::V
// ************************************************
// Increment Semaphore Value SV=SV+1 

void SemaphoreSysV::V() {
printf("Begin of Operation V for the semaphore identified by number %d\n",_Sem_Id) ;
struct sembuf semparV ;
  semparV.sem_num = 0 ;
  semparV.sem_op = 1 ;
  semparV.sem_flg = SEM_UNDO ;

  if (semop(_Sem_Id,&semparV,1) ==-1) {
    perror("Error in V() : semop()") ;
    exit(1);
    } 
printf("End of Operation V for the semaphore identified by number %d\n",_Sem_Id) ;
} // End of V()

// ************************************************
// Method : SemaphoreSysV::Delete
// ************************************************
void SemaphoreSysV::Delete(){

printf("Destroy the semaphore identified by number %d\n",_Sem_Id) ;

if(semctl(_Sem_Id,0,IPC_RMID,0) == -1)
    perror("Erreur in Destroy Semaphore") ;

} // End of Delete()

