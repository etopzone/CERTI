// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002  ONERA
//
// This file is part of CERTI-libRTI
//
// CERTI-libRTI is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libRTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: RTIambassador.cc,v 3.3 2002/11/27 23:07:19 breholee Exp $
// ---------------------------------------------------------------------------

// classe RTIambassador

#include "baseTypes.hh"
#include "RTItypes.hh"
#include "FederateAmbassador.hh"
#include "Message.hh"
#include "SocketUN.hh"
#include "RootObject.hh"
#include "fedtime.hh"
#include "PrettyDebug.hh"

//#include "privateRTIambassador.hh"
#include "RTIambassador.hh"

namespace certi {

static pdCDebug D("LIBRTI", "(libRTI  ) - ");

// Data integrity variable

const char *new_char1 = "\?";
const char *new_char2 = "\\";

//-----------------------------------------------------------------
void Sortir(const char *msg)              // FIXME: prototype?
{
  printf("libRTI: %s\n", msg);
  exit(-1);
}

//-----------------------------------------------------------------
void 
RTIambassador::executeService(Message *req,Message *rep)
{
  // lever une exception si appel reentrant
  if(en_service)
    throw ConcurrentAccessAttempted();

  D.Out(pdDebug,"envoi de la requete au RTIA");

  en_service = RTI_TRUE;
  try {
    req->write((SocketUN *) this);
   }
  catch(NetworkError) {
    printf("LibRTI: Catched NetworkError, throw RTIinternalError.\n");
    throw RTIinternalError();
  }
   
  D.Out(pdDebug,"attente reponse du RTIA");

  // attendre la reponse du RTI 
  try {
    rep->read((SocketUN *) this);
  }
  catch(NetworkError) {
    printf("LibRTI: Catched NetworkError, throw RTIinternalError.\n");
    throw RTIinternalError();
  }

  D.Out(pdDebug,"reponse du RTIA recue");

  if(rep->Type != req->Type) {
    printf("LibRTI: Assertion failed: rep->Type != req->Type\n");
    throw RTIinternalError("LibRTI:ExecuterService: rep->Type != req->Type");
  }

  en_service = RTI_FALSE;
  
  D.Out(pdDebug,"traitement de l'exception(réponse) retournee");

  processException(rep);

  D.Out(pdDebug,"traitement realise");
}


//-----------------------------------------------------------------
// Constructeur

RTIambassador::RTIambassador()
  throw(MemoryExhausted, RTIinternalError) : SocketUN(stIgnoreSignal)
{
  en_service = RTI_FALSE;
  const char *rtiaexec = "rtia" ;
  const char *rtiaenv = getenv("CERTI_RTIA");
  const char *rtiacall ;
  if(rtiaenv) rtiacall = rtiaenv ;
  else rtiacall = rtiaexec ;

  // creation du processus RTIA
  switch((pid_RTIA = fork()))
  {
    case -1: // erreur du fork
      perror("fork");
      throw RTIinternalError("Sortir sur instruction fork");
      break;

    case 0: // processus fils(RTIA)
      execlp(rtiacall , NULL);
      perror("execlp");
      exit(-1);

    default:  // processus pere(Federe)
      sleep(1);
      connectUN(pid_RTIA);
      break;
  }
}


//-----------------------------------------------------------------
// Destructeur

RTIambassador::~RTIambassador()
  throw(RTIinternalError)
{
  kill(pid_RTIA, SIGINT);
}

//*******************************************************************
//              Convertion des types
//*******************************************************************

CAttributeHandleValuePairSet* RTIambassador::AHStoCAHVPS(
  const AttributeHandleSet *ahs)
{
  ULong taille;
  CAttributeHandleValuePairSet* cahvps;
  CAttributeHandleValuePair *cahvp;

  cahvps = new CAttributeHandleValuePairSet();
  taille = ahs->size();

  for(unsigned int i = 0;i<taille;i++)
  {
    cahvp = new CAttributeHandleValuePair();
    cahvp->_attrib = ahs->getHandle(i);
    cahvps->add(cahvp);
  }

  return cahvps;
}

CAttributeHandleValuePairSet* RTIambassador::AHVPStoCAHVPS(
  const AttributeHandleValuePairSet *ahvps)
{
  ULong taille;
  ULong valueLenght;
  char buff[1024];
  CAttributeHandleValuePairSet *cahvps;
  CAttributeHandleValuePair *cahvp;

  cahvps = new CAttributeHandleValuePairSet();
  taille = ahvps->size();

  for(unsigned int i = 0;i<taille;i++)
  {
    cahvp = new CAttributeHandleValuePair();
    cahvp->_attrib = ahvps->getHandle(i);
    ahvps->getValue(i,(char*)buff,valueLenght);
    memcpy(cahvp->_value.value, buff, valueLenght);
    cahvp->_value.lenght = valueLenght;
    cahvps->add(cahvp);
  }
  return cahvps;
}

CParameterHandleValuePairSet* RTIambassador::PHVPStoCPHVPS(
  const ParameterHandleValuePairSet *phvps)
{
  ULong taille;
  ULong valueLenght;
  char buff[1024];
  CParameterHandleValuePairSet *cphvps;
  CParameterHandleValuePair *cphvp;

  cphvps = new CParameterHandleValuePairSet();
  taille = phvps->size();

  for(unsigned int i = 0;i<taille;i++)
  {
    cphvp = new CParameterHandleValuePair();
    cphvp->_param = phvps->getHandle(i);
    phvps->getValue(i,(char*)buff,valueLenght);
    memcpy(cphvp->_value.value, buff, valueLenght);
    cphvp->_value.lenght = valueLenght;
    cphvps->add(cphvp);
  }

  return cphvps;
}

AttributeHandleValuePairSet* 
RTIambassador::CAHVPStoAHVPS(CAttributeHandleValuePairSet *cahvps)
{
  ULong taille;
  ULong longueur;
  CAttributeHandleValuePair *cahvp;
  AttributeHandleValuePairSet *ahvps = NULL;

  taille = cahvps->_size;
  ahvps = AttributeSetFactory::create( taille );

  for(unsigned int i = 0;i<taille;i++)
  {
    //cahvp = cahvps->getWithHandle( i );
    cahvp = cahvps->getIeme( i );

    if(cahvp != NULL) {
      if(&cahvp->_value != NULL) {
	// decodage
        getStringToObjectLength(cahvp->_value.value,longueur);
        char *valeur = new char[longueur];
	valeur[0]='\0' ;
        stringToObject(cahvp->_value.value,valeur,longueur);
	memcpy(cahvp->_value.value, valeur, longueur);
	/*  ahvps->add(cahvp->_attrib,
	             cahvp->_value.value,
	(strlen(cahvp->_value.value)*sizeof(char)) ); */

	ahvps->add(cahvp->_attrib,
	             cahvp->_value.value,
	             longueur );
      }
      else {
        ahvps->add(cahvp->_attrib, '\0', 0);
      }
    }
  }

  return ahvps;
}

ParameterHandleValuePairSet* 
RTIambassador::CPHVPStoPHVPS(CParameterHandleValuePairSet *cphvps)
{
  ULong taille;
  ULong longueur;
  CParameterHandleValuePair *cphvp;
  ParameterHandleValuePairSet *phvps = NULL;

  taille = cphvps->_size;
  phvps = ParameterSetFactory::create( taille );

  for(unsigned int i = 0;i<taille;i++)
  {
    //cphvp = cphvps->getWithHandle( i );
    cphvp = cphvps->getIeme( i );

    if(cphvp != NULL) {
      if(&cphvp->_value != NULL) {
	// decodage
        getStringToObjectLength(cphvp->_value.value,longueur);
        char *valeur = new char[longueur];
	valeur[0] = '\0' ;
        stringToObject(cphvp->_value.value,valeur,longueur);
	memcpy(cphvp->_value.value, valeur, longueur);
	/*  phvps->add(cphvp->_param,
	    cphvp->_value.value,
	    (strlen(cphvp->_value.value)*sizeof(char)) );*/

	phvps->add(cphvp->_param,
  		     cphvp->_value.value,
		     longueur );	
      }
      else {
        phvps->add(cphvp->_param,'\0', 0);
      }
    }

  }

  return phvps;
}

//*******************************************************************
//              Data Integrity
//*******************************************************************

void RTIambassador::objectToString(
  const char *char_initial,
  ULong taille,
  char *char_final)
{
  ULong i =0;
  while(i<taille) {
    if(!strncmp(&char_initial[i],"\0",1)) {
      strcat(char_final,new_char1);
      i++;
    }
    else {
      if(!strncmp(&char_initial[i],new_char1,1)) {
        strcat(char_final,new_char2);
        strcat(char_final,new_char1);
        i++;
        while(!strncmp(&char_initial[i],new_char1,1) &&(i<taille)) {
          strcat(char_final,new_char1);
          strcat(char_final,new_char1);
          i++;
        }
        strcat(char_final,new_char2);
      }
      else {
        if(!strncmp(&char_initial[i],new_char2,1)) {
          strcat(char_final,new_char2);
          strcat(char_final,new_char1);
          strcat(char_final,new_char1);
          i++;
          while(!strncmp(&char_initial[i],new_char2,1) &&(i<taille)) {
            strcat(char_final,new_char1);
            strcat(char_final,new_char1);
            i++;
          }
          strcat(char_final,new_char2);
        }
        else {
          strncat(char_final,&char_initial[i],1);
          i++;
        }
      }
    }
  }
}

void RTIambassador::stringToObject(
  char *char_initial,
  char *char_final,
  ULong taille)
{
  ULong compteur = 0;
  ULong i = 0;
  ULong j = 0;
  ULong indice =0;
  ULong longueur = strlen(char_initial);

  for( j=0; j<taille; j++) {
    strcpy(&char_final[j],"\0");
  }

  while(i<longueur) {
    if(!strncmp(&char_initial[i],"\0",1)) {
      i++;
      indice++;
    }
    else {
      if(!strncmp(&char_initial[i],new_char1,1)) {
        i++;
        indice++;
      }
      else {
        if(!strncmp(&char_initial[i],new_char2,1)) {
          i++;
          while(!strncmp(&char_initial[i],new_char1,1) &&(i<longueur)) {
            compteur++;
            i++;
          }
          i++;
          if((compteur%2)==0) {
            for(j=0; j<(compteur/2); j++) {
              strncpy(&char_final[indice],new_char2,1);
              indice++;
            }
            compteur = 0;
          }
          else {
            for(j=0; j<(1+((compteur-1)/2)); j++) {
              strncpy(&char_final[indice],new_char1,1);
              indice++;
            }
            compteur = 0;
          }
        }
        else {
            strncpy(&char_final[indice],&char_initial[i],1);
            i++;
            indice++;
        }
      }
    }
  }
}

void RTIambassador::getObjectToStringLength(
  char *char_initial,
  ULong taille_initiale,
  ULong& taille)
{
  ULong compteur = 0;
  ULong i = 0;
  taille = 0;
  while(i<taille_initiale) {
    if(!strncmp(&char_initial[i],"\0",1)) {
      taille++;
      i++;
    }
    else {
      if(!strncmp(&char_initial[i],new_char1,1)) {
        i++;
        while(!strncmp(&char_initial[i],new_char1,1) &&(i<taille_initiale)) {
          compteur++;
          i++;
        }
        taille+=3+2*compteur;
        compteur = 0;
      }
      else {
        if(!strncmp(&char_initial[i],new_char2,1)) {
          i++;
          while(!strncmp(&char_initial[i],new_char2,1)&&(i<taille_initiale)) {
            compteur++;
            i++;
          }
          taille+=4+2*compteur;
          compteur = 0;
        }
        else {
          taille++;
          i++;
        }
      }
    }
  }
}

void 
RTIambassador::getStringToObjectLength(
  char *char_initial,
  ULong& taille)
{
  ULong compteur = 0;
  ULong longueur = strlen(char_initial);
  ULong i = 0;
  taille = 0;
  while(i<longueur) {
    if(!strncmp(&char_initial[i],"\0",1)) {
      taille++;
      i++;
    }
    else {
      if(!strncmp(&char_initial[i],new_char1,1)) {
        taille++;
        i++;
      }
      else {
        if(!strncmp(&char_initial[i],new_char2,1)) {
          i++;
          while(!strncmp(&char_initial[i],new_char1,1) &&(i<longueur)) {
            compteur++;
            i++;
          }
          if((compteur%2)==0) {
            taille+=(compteur/2);
            compteur = 0;
          }
          else {
            taille+=1+((compteur-1)/2);
            compteur = 0;
          }
        }
        else {
          taille++;
          i++;
        }
      }
    }
  }
}

//*******************************************************************
//              Gestion de la federation
//*******************************************************************


//-----------------------------------------------------------------
// 2.1 Create Federation Execution

void RTIambassador::createFederationExecution(
  const char *executionName,
  const char *FED)
throw(
  FederationExecutionAlreadyExists,
  ConcurrentAccessAttempted,
  CouldNotOpenFED,//not implemented
  ErrorReadingFED,//not implemented
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError)
{
  Message req, rep;
  char *exeName = new char[20];
  strcpy(exeName,executionName);
  strcat(exeName,"\56");
  strcat(exeName,"fed");

  req.Type             = CREATE_FEDERATION_EXECUTION;
  req.setFederationName((FederationExecutionName)executionName);

   if(!strcasecmp(FED,exeName)) {
    executeService(&req, &rep);
  }
  else {
    throw RTIinternalError();
  }
}

//-----------------------------------------------------------------
// 2.2 Destroy Federation Execution

void RTIambassador::destroyFederationExecution(
  const char *executionName)
throw(
  FederatesCurrentlyJoined,
  FederationExecutionDoesNotExist,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError)
{
  Message req, rep;

  req.Type             = DESTROY_FEDERATION_EXECUTION;
  req.setFederationName((FederationExecutionName)executionName);

  executeService(&req, &rep);
}

//-----------------------------------------------------------------
// 2.3 Join Federation Execution

FederateHandle RTIambassador::joinFederationExecution(
  const char *yourName,
  const char *executionName,
        FederateAmbassadorPtr federateAmbassadorReference)
throw(
  FederateAlreadyExecutionMember,
  FederationExecutionDoesNotExist,
  CouldNotOpenRID,//CERTI
  ErrorReadingRID,//CERTI
  CouldNotOpenFED,//not implemented
  ErrorReadingFED,//not implemented
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError)
{
  Message req, rep;

  fed_amb =(FederateAmbassador *)federateAmbassadorReference;

  req.Type             = JOIN_FEDERATION_EXECUTION;
  req.setFederateName((FederateName)yourName);
  req.setFederationName((FederationExecutionName)executionName);

  executeService(&req, &rep);
  return(rep.NumeroFedere);
}


//-----------------------------------------------------------------
// 2.4 Resign Federation Execution

void RTIambassador::resignFederationExecution(
  ResignAction theAction) 
throw(
  FederateOwnsAttributes,
  FederateNotExecutionMember,
  InvalidResignAction,
  ConcurrentAccessAttempted,
  RTIinternalError)
{
  Message req, rep;

  // envoyer la requete au RTI 
  req.Type = RESIGN_FEDERATION_EXECUTION;
  req.RAction = theAction;
  executeService(&req, &rep);
}


//-----------------------------------------------------------------
// 2.5 RegisterFederationSynchronizationPoint

void RTIambassador::registerFederationSynchronizationPoint(
  const char *label,
  const char * /*theTag*/)
throw(
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError)
{
  Message req, rep;

 if((strcmp(label,"Freeze") == 0)||(strcmp(label,"Init") == 0)) {
    req.Type     = REQUEST_PAUSE;
    req.setLabel(label);
  }
  else {
    if(strcmp(label,"Unfreeze") == 0) {
      req.Type = REQUEST_RESUME;
      req.setLabel(label);
    }
    else
      throw RTIinternalError();
  }
  executeService(&req, &rep);
}

void RTIambassador::registerFederationSynchronizationPoint(
  const char                */*label*/,
  const char                */*theTag*/,
  const FederateHandleSet&   /*syncSet*/)
throw(
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)//CERTI
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 2.7 Synchronization Point Achieved

void RTIambassador::synchronizationPointAchieved(
  const char *label)
throw(
  SynchronizationPointLabelWasNotAnnounced,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError)
{
  Message req, rep;

  if((strcmp(label,"Freeze") == 0)||(strcmp(label,"Init") == 0)) {
    req.Type     = PAUSE_ACHIEVED;
    req.setLabel(label);
  }
  else {
    if(strcmp(label,"Unfreeze") == 0) {
      req.Type = RESUME_ACHIEVED ;
      req.setLabel(label) ; 
    } else throw RTIinternalError();
  }

  executeService(&req, &rep);
}


//-----------------------------------------------------------------
// 2.9 Request Federation Save

void RTIambassador::requestFederationSave(    
  const char     */*label*/,
  const FedTime&  /*theTime*/)
throw(
  FederationTimeAlreadyPassed,
  InvalidFederationTime,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError,
  UnimplementedService)//CERTI
{
  throw UnimplementedService();
}

void RTIambassador::requestFederationSave( 
  const char */*label*/)
  throw(
	//  InvalidFederationTime,
// 	 FederateNotExecutionMember,
// 	 ConcurrentAccessAttempted,
// 	 SaveInProgress,//not implemented
// 	 RestoreInProgress,//not implemented
// 	 RTIinternalError,
// 	 UnimplementedService)

  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError,
  UnimplementedService) //CERTI
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 2.11 Federate Save Begun

void RTIambassador::federateSaveBegun() 
throw(
  SaveNotInitiated,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  RestoreInProgress,//not implemented
  RTIinternalError,
  UnimplementedService)//CERTI
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 2.12 Federate Save Complete

void 
RTIambassador::federateSaveComplete() 
throw(
  SaveNotInitiated,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  RestoreInProgress,//not implemented
  RTIinternalError,
  UnimplementedService)//CERTI
{
  throw UnimplementedService();
}

void 
RTIambassador::federateSaveNotComplete() 
throw(
  SaveNotInitiated,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  RestoreInProgress,//not implemented
  RTIinternalError,
  UnimplementedService)//CERTI
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 2.14 Request Restore

void RTIambassador::requestFederationRestore(    
  const char */*label*/)
throw(
  SpecifiedSaveLabelDoesNotExist,//CERTI
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError,
  UnimplementedService)//CERTI
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 2.17 Restore Complete

void 
RTIambassador::federateRestoreComplete()  
throw(
  UnknownLabel,
  RestoreNotRequested,
  RTICannotRestore,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RTIinternalError,
  UnimplementedService)//CERTI
{
  throw UnimplementedService();
}

void 
RTIambassador::federateRestoreNotComplete() 
throw(
  UnknownLabel,
  RestoreNotRequested,
  FederateNotExecutionMember,
  RTICannotRestore,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RTIinternalError,
  UnimplementedService)//CERTI
{
  throw UnimplementedService();
}

//void restoreNotAchieved() 
//throw(
//  RestoreNotRequested,
//  FederateNotExecutionMember,
//  ConcurrentAccessAttempted,
//  SaveInProgress,//not implemented
//  RTIinternalError,
//  UnimplementedService)//CERTI
//{
//  throw UnimplementedService();
//}


//*******************************************************************
//              Gestion des declarations
//*******************************************************************


//-----------------------------------------------------------------
// 3.1 Publish Object Class

void RTIambassador::publishObjectClass(
        ObjectClassHandle   theClass,      
  const AttributeHandleSet& attributeList)
throw(
  ObjectClassNotDefined,
  AttributeNotDefined,
  OwnershipAcquisitionPending,//not implemented
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemeted
  RestoreInProgress,//not implemented
  SecurityError,//CERTI
  RTIinternalError)
{
  Message req, rep;
  CAttributeHandleValuePairSet *attributeList_aux=AHStoCAHVPS(&attributeList);
  CAttributeHandleValuePair *tmp;

  // Envoyer la requete au RTI
  req.Type = PUBLISH_OBJECT_CLASS;
  req.objectClassHandle = theClass;
  req.HandleArraySize = attributeList_aux->_size;

  for(int i=0; i<attributeList_aux->_size; i++) {
    tmp                = attributeList_aux->getIeme(i);
    req.HandleArray[i] = tmp->_attrib;
  }

  executeService(&req, &rep);
}

//-----------------------------------------------------------------
// 3.2 UnPublish Object Class

void RTIambassador::unpublishObjectClass(
  ObjectClassHandle theClass) 
throw(
  ObjectClassNotDefined,
  ObjectClassNotPublished,//not implemented
  OwnershipAcquisitionPending,//not implemented
  FederateOwnsAttributes,//CERTI
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemeted
  RestoreInProgress,//not implemented
  SecurityError,//CERTI
  RTIinternalError)
{
        Message req, rep;
 
        // envoyer la requete au RTI
        req.Type = UNPUBLISH_OBJECT_CLASS;
        req.objectClassHandle = theClass;
	executeService(&req, &rep);
}


//-----------------------------------------------------------------
// 3.3 Publish Interaction Class

void RTIambassador::publishInteractionClass(
  InteractionClassHandle theInteraction) 
throw(
  InteractionClassNotDefined,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  SecurityError,//RTI
  RTIinternalError)
{
        Message req, rep;
 
        // envoyer la requete au RTI
        req.Type = PUBLISH_INTERACTION_CLASS;
        req.InteractionHandle = theInteraction;
	executeService(&req, &rep);
}

//-----------------------------------------------------------------
// 3.4 Publish Interaction Class

void RTIambassador::unpublishInteractionClass(
  InteractionClassHandle theInteraction) 
throw(
  InteractionClassNotDefined,
  InteractionClassNotPublished,//not implemented
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  SecurityError,//RTI
  RTIinternalError)
{
        Message req, rep;
 
        // envoyer la requete au RTI
        req.Type = UNPUBLISH_INTERACTION_CLASS;
        req.InteractionHandle = theInteraction;
	executeService(&req, &rep);
}


//-----------------------------------------------------------------
// 3.5 Subscribe Object Class Attribute

void RTIambassador::subscribeObjectClassAttributes(
        ObjectClassHandle   theClass,
  const AttributeHandleSet& attributeList,
        Boolean /*active*/)
throw(
  ObjectClassNotDefined,
  AttributeNotDefined,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  SecurityError,//RTI
  RTIinternalError)
{
  Message req, rep;
  CAttributeHandleValuePairSet *attributeList_aux=AHStoCAHVPS(&attributeList);
  CAttributeHandleValuePair *tmp;

  // envoyer la requete au RTI
  req.Type            = SUBSCRIBE_OBJECT_CLASS_ATTRIBUTE;
  req.objectClassHandle    = theClass;
  req.HandleArraySize = attributeList_aux->_size;

  for(int i=0; i < attributeList_aux->_size; i++) {
    tmp                = attributeList_aux->getIeme(i);
    req.HandleArray[i] = tmp->_attrib;
  }

  executeService(&req, &rep);
}

//-----------------------------------------------------------------
// 3.6 UnSubscribe Object Class Attribute

void RTIambassador::unsubscribeObjectClass(
  ObjectClassHandle theClass) 
throw(
  ObjectClassNotDefined,
  ObjectClassNotSubscribed,//not implemented
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  SecurityError,//RTI
  RTIinternalError)
{
  Message req, rep;
 
  // envoyer la requete au RTI
  req.Type = UNSUBSCRIBE_OBJECT_CLASS_ATTRIBUTE;
  req.objectClassHandle = theClass;
  executeService(&req, &rep);
}


//-----------------------------------------------------------------
// 3.7 Subscribe Interaction Class

void RTIambassador::subscribeInteractionClass(
  InteractionClassHandle theClass,
  Boolean /*active*/)
throw(
  InteractionClassNotDefined,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  FederateLoggingServiceCalls,//not implemented
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  SecurityError,//RTI
  RTIinternalError)
{
        Message req, rep;
 
        // envoyer la requete au RTI
        req.Type = SUBSCRIBE_INTERACTION_CLASS;
        req.InteractionHandle = theClass;
	executeService(&req, &rep);
}

//-----------------------------------------------------------------
// 3.8 UnSubscribe Interaction Class

void RTIambassador::unsubscribeInteractionClass(
  InteractionClassHandle theClass) 
throw(
  InteractionClassNotDefined,
  InteractionClassNotSubscribed,//not implemented
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  SecurityError,//RTI
  RTIinternalError)
{
  Message req, rep;

  // envoyer la requete au RTI
  req.Type = UNSUBSCRIBE_INTERACTION_CLASS;
  req.InteractionHandle = theClass;
  executeService(&req, &rep);
}


//*******************************************************************
//              Gestion des objets
//*******************************************************************


//-----------------------------------------------------------------
// 4.1 Register Object

ObjectHandle RTIambassador::registerObjectInstance(
          ObjectClassHandle theClass,
    const char *theObjectName) 
  throw(
    ObjectClassNotDefined,
    ObjectClassNotPublished,
    InvalidObjectHandle,//RTI
    ObjectAlreadyRegistered,
    FederateNotExecutionMember,
    ConcurrentAccessAttempted,
    SaveInProgress,//not implemented
    RestoreInProgress,//not implemented
    RTIinternalError)
{
  Message req, rep;

  // envoyer la requete au RTI
  req.Type = REGISTER_OBJECT;
  req.setName((char*)theObjectName);
  req.objectClassHandle = theClass;
  executeService(&req, &rep);
	
  return rep.Objectid;
}

ObjectHandle RTIambassador::registerObjectInstance(
          ObjectClassHandle theClass)
  throw(
    ObjectClassNotDefined,
    ObjectClassNotPublished,
    InvalidObjectHandle,//RTI
    ObjectAlreadyRegistered,
    FederateNotExecutionMember,
    ConcurrentAccessAttempted,
    SaveInProgress,//not implemented
    RestoreInProgress,//not implemented
    RTIinternalError)
{
  Message req, rep;

  // envoyer la requete au RTI
  req.Type = REGISTER_OBJECT;
  req.setName("\0");
  req.objectClassHandle = theClass;

  executeService(&req, &rep);
       
  return rep.Objectid;
}


//-----------------------------------------------------------------
// 4.3 Update Attribute Values

EventRetractionHandle                               
RTIambassador::updateAttributeValues(
        ObjectHandle                 theObject,     
  const AttributeHandleValuePairSet& theAttributes,
  const FedTime&                     theTime,       
  const char                         *theTag)        
throw(
  ObjectNotKnown,
  AttributeNotDefined,
  AttributeNotOwned,
  InvalidFederationTime,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError)
{
  ULong longueur;
  Message req, rep;

  CAttributeHandleValuePairSet *theAttributes_aux
    =AHVPStoCAHVPS(&theAttributes);
  CAttributeHandleValuePair *tmp;
  
  // Envoyer la requete au RTI
  req.Type     = UPDATE_ATTRIBUTE_VALUES;
  req.Objectid =(ObjectHandle)theObject;
  req.Date     =(FederationTime)((RTIfedTime&)theTime).getTime();

  req.setTag(theTag);

  req.HandleArraySize = theAttributes_aux->_size;

  for(int i=0; i<theAttributes_aux->_size; i++) {
    tmp                = theAttributes_aux->getIeme(i);
    req.HandleArray[i] = tmp->_attrib;

    // codage
    getObjectToStringLength(tmp->_value.value,tmp->_value.lenght,longueur);
    char *valeur = new char[longueur];
    valeur[0]='\0';  // <-----
    objectToString(tmp->_value.value, tmp->_value.lenght, valeur);
    req.setValue(i,valeur);
  }

  executeService(&req, &rep);

  theAttributes_aux->empty(); // liberation de la memoire

  return(rep.Retract);
}

void updateAttributeValues(
	ObjectHandle                 /*theObject*/,
  const AttributeHandleValuePairSet& /*theAttributes*/,
  const char                         */*theTag*/)
throw(
  ObjectNotKnown,
  AttributeNotDefined,
  AttributeNotOwned,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError,
  UnimplementedService)//CERTI
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 4.5 Send Interaction

EventRetractionHandle                                
RTIambassador::sendInteraction(
        InteractionClassHandle       theInteraction, 
  const ParameterHandleValuePairSet& theParameters,
  const FedTime&                     theTime,        
  const char                         *theTag)         
throw(
  InteractionClassNotDefined,
  InteractionClassNotPublished,
  InteractionParameterNotDefined,
  InvalidFederationTime,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError)
{
  ULong longueur;
  Message req, rep;
  CParameterHandleValuePairSet* theParameters_aux
    =PHVPStoCPHVPS(&theParameters);
  CParameterHandleValuePair *tmp;

  // envoyer la requete au RTI
  req.Type              = SEND_INTERACTION;
  req.InteractionHandle = theInteraction;
  req.Date              =(FederationTime)((RTIfedTime&)theTime).getTime();

  req.setTag(theTag);

  req.HandleArraySize = theParameters_aux->_size;

  for(int i=0; i<theParameters_aux->_size; i++) {
    tmp                = theParameters_aux->getIeme(i);
    req.HandleArray[i] = tmp->_param;

    // codage
    getObjectToStringLength(tmp->_value.value,tmp->_value.lenght,longueur);
    char *valeur = new char[longueur];
    valeur[0]='\0' ;
    objectToString(tmp->_value.value,tmp->_value.lenght, valeur);
    req.setValue(i,valeur);
  }

  executeService(&req, &rep);
  theParameters_aux->empty(); // liberation de la memoire
  return(rep.Retract);
}

void sendInteraction(
	InteractionClassHandle       /*theInteraction*/,
  const ParameterHandleValuePairSet& /*theParameters*/,
  const char                         */*theTag*/)
throw(
  InteractionClassNotDefined,
  InteractionClassNotPublished,
  InteractionParameterNotDefined,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError,
  UnimplementedService)//CERTI
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 4.7 Delete Object

EventRetractionHandle             
RTIambassador::deleteObjectInstance(
        ObjectHandle    theObject,
  const FedTime&        theTime,
  const char            *theTag)
throw(
  ObjectNotKnown,
  DeletePrivilegeNotHeld,
  InvalidFederationTime,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError)
{
  Message req, rep;

  // envoyer la requete au RTI
  req.Type     = DELETE_OBJECT;
  req.Objectid =(ObjectHandle)theObject;
  req.Date     =(FederationTime)((RTIfedTime&)theTime).getTime();

  req.setTag(theTag);

  executeService(&req, &rep);

  return(rep.Retract);
}

void deleteObjectInstance(
	ObjectHandle    /*theObject*/,
  const char            */*theTag*/)
throw(
  ObjectNotKnown,
  DeletePrivilegeNotHeld,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError,
  UnimplementedService)//CERTI
{
  throw UnimplementedService();
}

//-----------------------------------------------------------------
// 4.9 Local Delete Object Instance

void localDeleteObjectInstance(
	ObjectHandle    /*theObject*/)
throw(
  ObjectNotKnown,
  FederateOwnsAttributes,//not implemented
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError,
  UnimplementedService)//CERTI
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 4.10 Change Attribute Transportation Type

void RTIambassador::changeAttributeTransportationType(
        ObjectHandle         theObject,     
  const	AttributeHandleSet&  theAttributes, 
        TransportationHandle theType)       
throw(
  ObjectNotKnown,
  AttributeNotDefined,
  AttributeNotOwned,
  InvalidTransportType,//CERTI
  InvalidTransportationHandle,//not implemented
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError)
{
  Message req, rep;
  CAttributeHandleValuePairSet* theAttributes_aux=AHStoCAHVPS(&theAttributes);
  CAttributeHandleValuePair *tmp;
  TransportType theType_aux;

  theType_aux = ((theType == 1) ? RELIABLE : BEST_EFFORT);

  // Envoyer la requete au RTI
  req.Type            = CHANGE_ATTRIBUTE_TRANSPORT_TYPE;
  req.Objectid        =(ObjectHandle)theObject;
  req.Transport       = theType_aux;

  req.HandleArraySize = theAttributes_aux->_size;

  for(int i=0; i<theAttributes_aux->_size; i++) {
      tmp                = theAttributes_aux->getIeme(i);
      req.HandleArray[i] = tmp->_attrib;
    }

  executeService(&req, &rep);

}

//-----------------------------------------------------------------
// 4.11 Change Interaction Transportation Type

void RTIambassador::changeInteractionTransportationType(
  InteractionClassHandle theClass,
  TransportationHandle   theType)
throw(
  InteractionClassNotDefined,
  InteractionClassNotPublished,
  InvalidTransportType,//CERTI
  InvalidTransportationHandle,//not implemented
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError)
{
  Message req, rep;
  TransportType theType_aux;

  theType_aux = ((theType == 1) ? RELIABLE : BEST_EFFORT);

  // envoyer la requete au RTI
  req.Type = CHANGE_INTERACTION_TRANSPORT_TYPE;
  req.InteractionHandle = theClass;
  req.Transport = theType_aux;
  executeService(&req, &rep);
}

//-----------------------------------------------------------------
// 4.14 Request Attribute Value Update

void RTIambassador::requestObjectAttributeValueUpdate(
        ObjectHandle        /*theObject*/,
  const	AttributeHandleSet& /*theAttributes*/)
throw(
  ObjectNotKnown,// not implemented
  AttributeNotDefined,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError,
  UnimplementedService)//CERTI
{
	throw UnimplementedService();
}

void RTIambassador::requestClassAttributeValueUpdate(
        ObjectClassHandle   /*theClass*/,
  const	AttributeHandleSet& /*theAttributes*/)
throw(
  ObjectClassNotDefined,
  AttributeNotDefined,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError,
  UnimplementedService)//CERTI
{
	throw UnimplementedService();
}

//*******************************************************************
//              Gestion de la propriete
//*******************************************************************




//-----------------------------------------------------------------
// 5.1 UnConditional Attribute Ownership Divestiture

void RTIambassador::unconditionalAttributeOwnershipDivestiture(
        ObjectHandle                      theObject,
  const	AttributeHandleSet&               theAttributes)
throw(
  ObjectNotKnown,
  AttributeNotDefined,
  AttributeNotOwned,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError)
{
  Message req, rep;
	
  // Envoyer la requete au RTI
  req.Type = UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
  req.Objectid =(ObjectHandle)theObject;

  req.HandleArraySize =	theAttributes.size();
	 		
  for(unsigned int i=0; i<theAttributes.size(); i++) {
    req.HandleArray[i] =  theAttributes.getHandle(i);
  }
  
  executeService(&req, &rep);
 
 
}

//-----------------------------------------------------------------
// 5.2 Negotiated Attribute Ownership Divestiture

void RTIambassador::negotiatedAttributeOwnershipDivestiture(
        ObjectHandle                      theObject,
  const AttributeHandleSet&               theAttributes,
  const char                *theTag)
throw(
  ObjectNotKnown,
  AttributeNotDefined,
  AttributeNotOwned,
  AttributeAlreadyBeingDivested,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError)
{
  Message req, rep;

  // Envoyer la requete au RTI
  req.Type = NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
  req.Objectid =(ObjectHandle)theObject;

  req.setTag(theTag);

  req.HandleArraySize = theAttributes.size();
    
  for(unsigned int i=0; i<theAttributes.size(); i++) {
    req.HandleArray[i] =  theAttributes.getHandle(i);
  }
 
  executeService(&req, &rep); 
}


//-----------------------------------------------------------------
// 5.6 Attribute Ownership Acquisition

void RTIambassador::attributeOwnershipAcquisition(
        ObjectHandle            theObject,         
  const AttributeHandleSet&     desiredAttributes, 
  const char*          theTag)            
throw(
  ObjectNotKnown,
  ObjectClassNotPublished,
  AttributeNotDefined,
  AttributeNotPublished,
  FederateOwnsAttributes,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError)
{
  Message req, rep;

  // Envoyer la requete au RTI
  req.Type = ATTRIBUTE_OWNERSHIP_ACQUISITION;
  req.Objectid =(ObjectHandle)theObject;

  req.setTag(theTag);

  req.HandleArraySize = desiredAttributes.size();
    
 for(unsigned int i=0; i<desiredAttributes.size(); i++)
     {
     req.HandleArray[i] =  desiredAttributes.getHandle(i);
    }
 
  executeService(&req, &rep);
 
}

//-----------------------------------------------------------------
// 5.8 Attribute Ownership Release Response

AttributeHandleSet*
RTIambassador::attributeOwnershipReleaseResponse(
        ObjectHandle            theObject,
  const AttributeHandleSet&     theAttributes)
throw(
  ObjectNotKnown,
  AttributeNotDefined,
  AttributeNotOwned,
  FederateWasNotAskedToReleaseAttribute,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError)
{
  Message req, rep;

  // Envoyer la requete au RTI
  req.Type = ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE;
  req.Objectid =(ObjectHandle)theObject;

  req.HandleArraySize = theAttributes.size();
    
 for(unsigned int i=0; i<theAttributes.size(); i++)
     {
     req.HandleArray[i] =  theAttributes.getHandle(i);
    }
 
  executeService(&req, &rep);

  if(rep.Exception == e_NO_EXCEPTION)
  {
    AttributeHandleSet *AttributeSet;
    AttributeSet = AttributeHandleSetFactory::create(rep.HandleArraySize); 
  
    for(unsigned int i = 0 ; i < rep.HandleArraySize; i++) {
      AttributeSet->add(rep.HandleArray[i]);
    }
    return(AttributeSet);
  }

  return NULL;
}

//-----------------------------------------------------------------
// 5.9 Cancel Negociated Attribute Ownership Divestiture

void RTIambassador::cancelnegotiatedAttributeOwnershipDivestiture(
        ObjectHandle            theObject,
  const AttributeHandleSet&     theAttributes)            
throw(
  ObjectNotKnown,
  AttributeNotDefined,
  AttributeNotOwned,
  AttributeDivestitureWasNotRequested,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError)
{
  Message req, rep;

  // Envoyer la requete au RTI
  req.Type = CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
  req.Objectid =(ObjectHandle)theObject;

  req.HandleArraySize = theAttributes.size();
    
  for(unsigned int i=0; i<theAttributes.size(); i++) {
    req.HandleArray[i] =  theAttributes.getHandle(i);
  }
  executeService(&req, &rep);
}

//-----------------------------------------------------------------
// 5.10 Cancel Attribute Ownership Acquisition

void RTIambassador::cancelattributeOwnershipAcquisition(
        ObjectHandle            theObject,
  const AttributeHandleSet&     theAttributes)
throw(
  ObjectNotKnown,
  AttributeNotDefined,
  AttributeAlreadyOwned,
  AttributeAcquisitionWasNotRequested,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError)
{
  Message req, rep;

  // Envoyer la requete au RTI
  req.Type = CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION;
  req.Objectid =(ObjectHandle)theObject;

  req.HandleArraySize = theAttributes.size();
    
  for(unsigned int i=0; i<theAttributes.size(); i++) {
    req.HandleArray[i] =  theAttributes.getHandle(i);
  }
 
  executeService(&req, &rep);
}

//-----------------------------------------------------------------
// 5.12 Attribute Ownership Acquisition If Available

void RTIambassador::attributeOwnershipAcquisitionIfAvailable(
        ObjectHandle            theObject,
  const AttributeHandleSet&     desiredAttributes)
throw(
  ObjectNotKnown,
  ObjectClassNotPublished,
  AttributeNotDefined,
  AttributeNotPublished,
  FederateOwnsAttributes,
 AttributeAlreadyBeingAcquired,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError)

{
  Message req, rep;
 
  // Envoyer la requete au RTI
  req.Type = ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE;
  req.Objectid =(ObjectHandle)theObject;

  req.HandleArraySize = desiredAttributes.size();
    
 for(unsigned int i=0; i<desiredAttributes.size(); i++) {
   req.HandleArray[i] =  desiredAttributes.getHandle(i);
   D.Out(pdTrace, "Objet %u Attribut %u",theObject,req.HandleArray[i]);      
 }
  
  executeService(&req, &rep); 
}

//-----------------------------------------------------------------
// 5.14 Query Attribute Ownership

void RTIambassador::queryAttributeOwnership(
  ObjectHandle        theObject,
  AttributeHandle theAttribute)
throw(
  ObjectNotKnown,
  AttributeNotDefined,
  AttributeNotKnown,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError)
{
  Message req, rep;
 
  // envoyer la requete au RTIA/RTIG
  req.Type         = QUERY_ATTRIBUTE_OWNERSHIP;
  req.Objectid   = theObject;  
  req.AttribHandle = theAttribute;

  executeService(&req, &rep);

}

//-----------------------------------------------------------------
// 5.16 Is Attribute Owned By Federate 

Boolean                     
RTIambassador::isAttributeOwnedByFederate(
  ObjectHandle        theObject,     
  AttributeHandle theAttribute)  
throw(
  ObjectNotKnown,
  AttributeNotDefined,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError)
{
  Message req, rep;
 
  // envoyer la requete au RTIA
  req.Type         = IS_ATTRIBUTE_OWNED_BY_FEDERATE;
  req.Objectid   = theObject;  
  req.AttribHandle = theAttribute;

  executeService(&req, &rep);
   
  return ((strcmp(rep.getTag(),"RTI_TRUE") == 0) ? RTI_TRUE : RTI_FALSE);
}


//*******************************************************************
//              Gestion du temps
//*******************************************************************


//-----------------------------------------------------------------
// 6.1 Enable Time Regulation

void RTIambassador::enableTimeRegulation(
  const FedTime& /*theFederateTime*/,
  const FedTime& /*theLookahead*/)
throw(
  FederationTimeAlreadyPassed,
  TimeRegulationAlreadyEnabled,//not implemented
  EnableTimeRegulationPending,//not implemented
  TimeAdvanceAlreadyInProgress,//not implemented
  InvalidFederationTime,//not implemented
  InvalidLookahead,//not implemented
  ConcurrentAccessAttempted,//not implemented
  FederateNotExecutionMember,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError)
{
  Message req, rep;

  // envoyer la requete au RTI
  req.Type = SET_TIME_REGULATING;
  req.Bool = RTI_TRUE;
  executeService(&req, &rep);
}


//-----------------------------------------------------------------
// 6.3 Disable Time Regulation

void RTIambassador::disableTimeRegulation()
throw(
  FederationTimeAlreadyPassed,
  TimeRegulationWasNotEnabled,//not implemented
  ConcurrentAccessAttempted,
  FederateNotExecutionMember,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError)
{
  Message req, rep;

  // envoyer la requete au RTI
  req.Type = SET_TIME_REGULATING;
  req.Bool = RTI_FALSE;
  executeService(&req, &rep);
}

//-----------------------------------------------------------------
// 6.4 Enable Tome Constrained

void RTIambassador::enableTimeConstrained()
throw(
  TimeConstrainedAlreadyEnabled,//not implemented
  EnableTimeConstrainedPending,//not implemented
  TimeAdvanceAlreadyInProgress,//not implemented
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError)
{
  Message req, rep;

  // envoyer la requete au RTI
  req.Type = SET_TIME_CONSTRAINED;
  req.Bool = RTI_TRUE;
  executeService(&req, &rep);
}

//-----------------------------------------------------------------
// 6.6 Disable Time Constrained

void RTIambassador::disableTimeConstrained()
throw(
  TimeConstrainedWasNotEnabled,//not implemented
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError)
{
  Message req, rep;

  // envoyer la requete au RTI
  req.Type = SET_TIME_CONSTRAINED;
  req.Bool = RTI_FALSE;
  executeService(&req, &rep);
}

//-----------------------------------------------------------------
// 6.7 Time Advance Request

void RTIambassador::timeAdvanceRequest(
 FedTime& theTime)
throw(
  TimeAdvanceAlreadyInProgress,
  FederationTimeAlreadyPassed,
  InvalidFederationTime,
  EnableTimeRegulationPending,//not implemented
  EnableTimeConstrainedPending,//not implemented
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError)
{
  Message req, rep;

  // envoyer la requete au RTI 
  req.Type = TIME_ADVANCE_REQUEST;
  req.Date =(FederationTime)(RTIfedTime(theTime)._fedTime);
  executeService(&req, &rep);
}

//-----------------------------------------------------------------
// 6.8 Time Advance Request Available

void RTIambassador::timeAdvanceRequestAvailable(
  const FedTime& /*theTime*/)
throw(
  InvalidFederationTime,
  FederationTimeAlreadyPassed,
  TimeAdvanceAlreadyInProgress,
  EnableTimeRegulationPending,
  EnableTimeConstrainedPending,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}

//-----------------------------------------------------------------
// 6.9 Next Event Request

void RTIambassador::nextEventRequest(
  const FedTime& theTime)
throw(
  TimeAdvanceAlreadyInProgress,
  FederationTimeAlreadyPassed,
  InvalidFederationTime,
  EnableTimeRegulationPending,//not implemented
  EnableTimeConstrainedPending,//not implemented
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError)
{
  Message req, rep;

  // envoyer la requete au RTI 
  req.Type = NEXT_EVENT_REQUEST;
  req.Date =(FederationTime)((RTIfedTime&)theTime).getTime();
  executeService(&req, &rep);
}
//-----------------------------------------------------------------
// 6.10 Next Event Request Available

void RTIambassador::nextEventRequestAvailable(
  const FedTime& /*theTime*/)
throw(
  InvalidFederationTime,
  FederationTimeAlreadyPassed,
  TimeAdvanceAlreadyInProgress,
  EnableTimeRegulationPending,
  EnableTimeConstrainedPending,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}

//-----------------------------------------------------------------
// 6.11 Flush Queue Request

void RTIambassador::flushQueueRequest(
  const FedTime& /*theTime*/)
throw(
  TimeAdvanceAlreadyInProgress,
  FederationTimeAlreadyPassed,
  InvalidFederationTime,
  EnableTimeRegulationPending,//not implemented
  EnableTimeConstrainedPending,//not implemented
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError)
{
  throw UnimplementedService();
}

//-----------------------------------------------------------------
// 6.13 Enable Asynchronous Delivery

void RTIambassador::enableAsynchronousDelivery()
throw(
  AsynchronousDeliveryAlreadyEnabled,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}

//-----------------------------------------------------------------
// 6.14 Disable Asynchronous Delivery

void RTIambassador::disableAsynchronousDelivery()
throw(
  AsynchronousDeliveryAlreadyDisabled,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}

//-----------------------------------------------------------------
// 6.15 Query LBTS

void RTIambassador::queryLBTS(
 FedTime& theTime)
throw(
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError)
{
  Message req, rep;

  // envoyer la requete au RTI 
  req.Type = REQUEST_LBTS;
  executeService(&req, &rep);

  theTime = RTIfedTime(rep.Date);
}

//-----------------------------------------------------------------
// 6.16 Query Federate Time

void RTIambassador::queryFederateTime(
 FedTime& theTime)
throw(
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError)
{
  Message req, rep;

  // envoyer la requete au RTI 
  req.Type = REQUEST_FEDERATE_TIME;
  executeService(&req, &rep);

  theTime = RTIfedTime(rep.Date);
}

//-----------------------------------------------------------------
// 6.17 Query Minimum Next Event Time

void RTIambassador::queryMinNextEventTime(
 FedTime& theTime)
throw(
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError)
{
 theTime = RTIfedTime(0.0);
}

//-----------------------------------------------------------------
// 6.18 Modify Lookahead

void RTIambassador::modifyLookahead(
  const FedTime& theLookahead) 
throw(
  InvalidFederationTimeDelta,//CERTI
  InvalidLookahead,//not implemented
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError)
{
  Message req, rep;

  // envoyer la requete au RTI 
  req.Type = SET_LOOKAHEAD;
  req.Lookahead =(FederationTimeDelta)((RTIfedTime&)theLookahead).getTime();
  executeService(&req, &rep);
}

//-----------------------------------------------------------------
// 6.19 Query Lookahead

void RTIambassador::queryLookahead(
 FedTime& theTime)
throw(
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError)
{
  Message req, rep;

  // envoyer la requete au RTI 
  req.Type = REQUEST_LOOKAHEAD;
  executeService(&req, &rep);

  RTIfedTime *tmp = new RTIfedTime((Double)rep.Lookahead);
  theTime = *(dynamic_cast<FedTime *>(tmp));
}

//-----------------------------------------------------------------
// 6.20 Retract

void RTIambassador::retract(
  EventRetractionHandle /*theHandle*/)
throw(
  InvalidRetractionHandle,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError,
  UnimplementedService)//CERTI
{
 throw UnimplementedService();
}

//-----------------------------------------------------------------
// 6.22 Change Attribute Order Type

void RTIambassador::changeAttributeOrderType(
        ObjectHandle        theObject,     
  const AttributeHandleSet& theAttributes,
        OrderingHandle      theType)       
throw(
  ObjectNotKnown,
  AttributeNotDefined,
  AttributeNotOwned,
  InvalidOrderType,//CERTI
  InvalidOrderingHandle,//not implemented
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError)
{
  Message req, rep;
  CAttributeHandleValuePairSet* theAttributes_aux = AHStoCAHVPS(&theAttributes);
  CAttributeHandleValuePair *tmp;

  OrderType theType_aux;
  theType_aux = ((theType == 1) ? RECEIVE : TIMESTAMP);

  // Envoyer la requete au RTI

  req.Type            = CHANGE_ATTRIBUTE_ORDER_TYPE;
  req.Objectid        =(ObjectHandle)theObject;
  req.Order           = theType_aux;

  req.HandleArraySize = theAttributes_aux->_size;

  for(int i=0; i<theAttributes_aux->_size; i++) {
    tmp                = theAttributes_aux->getIeme(i);
    req.HandleArray[i] = tmp->_attrib;
  }

  executeService(&req, &rep);
}

//-----------------------------------------------------------------
// 6.23 Change Interaction Order Type

void RTIambassador::changeInteractionOrderType(
  InteractionClassHandle theClass, 
  OrderingHandle         theType)  
throw(
  InteractionClassNotDefined,
  InteractionClassNotPublished,
  InvalidOrderType,//CERTI
  InvalidOrderingHandle,//not implemented
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,//not implemented
  RestoreInProgress,//not implemented
  RTIinternalError)
{
  Message req, rep;

  OrderType theType_aux;
  theType_aux = ((theType == 1) ? RECEIVE : TIMESTAMP);

 // envoyer la requete au RTI
 req.Type = CHANGE_INTERACTION_ORDER_TYPE;
 req.InteractionHandle = theClass;
 req.Order = theType_aux;
 executeService(&req, &rep);
}

//*******************************************************************
//              Gestion de la distribution
//*******************************************************************

//-----------------------------------------------------------------
// 7.1 Create Region

Region* RTIambassador::createRegion( 
  SpaceHandle /*theSpace*/,
  ULong       /*numberOfExtents*/)
throw(
  SpaceNotDefined,
  InvalidExtents,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 7.2 Notify About Region Modification

void notifyAboutRegionModification(
  Region &/*theRegion*/)
throw(
  RegionNotKnown,
  InvalidExtents,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 7.3 Delete Region

void deleteRegion(
  Region */*theRegion*/)
throw(
  RegionNotKnown,
  RegionInUse,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}

//-----------------------------------------------------------------
// 7.4 Register Object Instance With Region

ObjectHandle                          
registerObjectInstanceWithRegion(
 ObjectClassHandle /*theClass*/,
  const char             */*theObject*/,
        AttributeHandle   /*theAttributes[]*/,
        Region           */*theRegions[]*/,
        ULong             /*theNumberOfHandles*/)
throw(
  ObjectClassNotDefined,
  ObjectClassNotPublished,
  AttributeNotDefined,
  AttributeNotPublished,
  RegionNotKnown,
  InvalidRegionContext,
  ObjectAlreadyRegistered,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}

ObjectHandle  
registerObjectInstanceWithRegion(
        ObjectClassHandle /*theClass*/,
        AttributeHandle   /*theAttributes[]*/,
        Region           */*theRegions[]*/,
        ULong             /*theNumberOfHandles*/)
throw(
  ObjectClassNotDefined,
  ObjectClassNotPublished,
  AttributeNotDefined,
  AttributeNotPublished,
  RegionNotKnown,
  InvalidRegionContext,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 7.5 Associate Region For Updates

void associateRegionForUpdates(
 Region             &/*theRegion*/,
 ObjectHandle        /*theObject*/,
  const AttributeHandleSet &/*theAttributes*/)
throw(
  ObjectNotKnown,
  AttributeNotDefined,
  InvalidRegionContext,
  RegionNotKnown,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}

//-----------------------------------------------------------------
// 7.6 UnAssociate Region For Updates

void unassociateRegionForUpdates(
  Region       &/*theRegion*/,
  ObjectHandle  /*theObject*/)
throw(
  ObjectNotKnown,
  InvalidRegionContext,
  RegionNotKnown,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}



//-----------------------------------------------------------------
// 7.7 Subscribe Object Class Attributes With Region

void subscribeObjectClassAttributesWithRegion(
 ObjectClassHandle   /*theClass*/,
 Region             &/*theRegion*/,
  const AttributeHandleSet &/*attributeList*/,
        Boolean        /*active*/)
throw(
  ObjectClassNotDefined,
  AttributeNotDefined,
  RegionNotKnown,
  InvalidRegionContext,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 7.8 UnSubscribe Object Class Attributes With Region

void unsubscribeObjectClassWithRegion(
  ObjectClassHandle /*theClass*/,
  Region           &/*theRegion*/)
throw(
  ObjectClassNotDefined,
  RegionNotKnown,
  ObjectClassNotSubscribed,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 7.9 Subscribe Interaction Class With Region

void subscribeInteractionClassWithRegion(
  InteractionClassHandle /*theClass*/,
  Region                &/*theRegion*/,
  Boolean           /*active*/)
throw(
  InteractionClassNotDefined,
  RegionNotKnown,
  InvalidRegionContext,
  FederateLoggingServiceCalls,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 7.10 UnSubscribe Interaction Class With Region

void unsubscribeInteractionClassWithRegion(
  InteractionClassHandle /*theClass*/,
  Region                &/*theRegion*/)
throw(
  InteractionClassNotDefined,
  InteractionClassNotSubscribed,
  RegionNotKnown,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 7.11 Send Interaction With Region

EventRetractionHandle             
sendInteractionWithRegion(
 InteractionClassHandle       /*theInteraction*/,
  const ParameterHandleValuePairSet &/*theParameters*/,
  const FedTime&                     /*theTime*/,
  const char                        */*theTag*/,
  const Region                      &/*theRegion*/)
throw(
  InteractionClassNotDefined,
  InteractionClassNotPublished,
  InteractionParameterNotDefined,
  InvalidFederationTime,
  RegionNotKnown,
  InvalidRegionContext,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}

void sendInteractionWithRegion(
 InteractionClassHandle       /*theInteraction*/,
  const ParameterHandleValuePairSet &/*theParameters*/,
  const char                        */*theTag*/,
  const Region                      &/*theRegion*/)
throw(
  InteractionClassNotDefined,
  InteractionClassNotPublished,
  InteractionParameterNotDefined,
  RegionNotKnown,
  InvalidRegionContext,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 7.12 Request Class Attribute Value Update With Region

void requestClassAttributeValueUpdateWithRegion(
 ObjectClassHandle   /*theClass*/,
  const AttributeHandleSet &/*theAttributes*/,
  const Region             &/*theRegion*/)
throw(
  ObjectClassNotDefined,
  AttributeNotDefined,
  RegionNotKnown,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//*******************************************************************
//              RTI Support Services
//*******************************************************************


//-----------------------------------------------------------------
// 8.1 Get Object Class Handle

ObjectClassHandle                
RTIambassador::getObjectClassHandle(
  const char *theName) 
throw(
  NameNotFound,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  RTIinternalError)
{
  Message req, rep;

  // envoyer la requete au RTI
  req.Type   = GET_OBJECT_CLASS_HANDLE;
  req.setName((ObjectClassName)theName);

  executeService(&req, &rep);

  return(rep.objectClassHandle);
}


//-----------------------------------------------------------------
// 8.2 Get Object Class Name

char *               
RTIambassador::getObjectClassName(
  ObjectClassHandle theHandle) 
throw(
  ObjectClassNotDefined,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  RTIinternalError)
{
  Message req, rep;

  // envoyer la requete au RTI
  req.Type         = GET_OBJECT_CLASS_NAME;
  req.objectClassHandle = theHandle;

  executeService(&req, &rep);

  return(rep.getName());
}


//-----------------------------------------------------------------
// 8.3 Get Attribute Handle

AttributeHandle                       
RTIambassador::getAttributeHandle(
  const char              *theName,    
        ObjectClassHandle whichClass) 
throw(
  ObjectClassNotDefined,
  NameNotFound,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  RTIinternalError)

{
  Message req, rep;

  // envoyer la requete au RTI
  req.Type         = GET_ATTRIBUTE_HANDLE;
  req.setName((AttributeName)theName);
  req.objectClassHandle = whichClass;

  executeService(&req, &rep);

  return(rep.AttribHandle);
}


//-----------------------------------------------------------------
// 8.4 Get Attribute Name

char *
RTIambassador::getAttributeName(
  AttributeHandle   theHandle,  
  ObjectClassHandle whichClass) 
throw(
  ObjectClassNotDefined,
  AttributeNotDefined,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  RTIinternalError)
{
  Message req, rep;

  // envoyer la requete au RTI
  req.Type         = GET_ATTRIBUTE_NAME;
  req.AttribHandle = theHandle;
  req.objectClassHandle = whichClass;

  executeService(&req, &rep);

  return(rep.getName());
}


//-----------------------------------------------------------------
// 8.5 Get Interaction Class Handle

InteractionClassHandle                
RTIambassador::getInteractionClassHandle(
  const char *theName) 
throw(
  NameNotFound,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  RTIinternalError)
{
  Message req, rep;

  // envoyer la requete au RTI
  req.Type    = GET_INTERACTION_CLASS_HANDLE;
  req.setName((InteractionClassName)theName);

  executeService(&req, &rep);

  return(rep.InteractionHandle);
}


//-----------------------------------------------------------------
// 8.6 Get Interaction Class Name

char *
RTIambassador::getInteractionClassName(
  InteractionClassHandle theHandle) 
throw(
  InteractionClassNotDefined,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  RTIinternalError)
{
  Message req, rep;

  // envoyer la requete au RTI
  req.Type              = GET_INTERACTION_CLASS_NAME;
  req.InteractionHandle = theHandle;

  executeService(&req, &rep);

  return(rep.getName());
}


//-----------------------------------------------------------------
// 8.7 Get Parameter Handle

ParameterHandle                            
RTIambassador::getParameterHandle(
  const char                   *theName,    
        InteractionClassHandle whichClass) 
throw(
  InteractionClassNotDefined,
  NameNotFound,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  RTIinternalError)
{
  Message req, rep;

  // envoyer la requete au RTI
  req.Type              = GET_PARAMETER_HANDLE;
  req.setName((ParameterName)theName);
  req.InteractionHandle = whichClass;

  executeService(&req, &rep);

  return(rep.ParamHandle);
}


//-----------------------------------------------------------------
// 8.8 Get Parameter Name

char *
RTIambassador::getParameterName(
  ParameterHandle        theHandle,  
  InteractionClassHandle whichClass) 
throw(
  InteractionClassNotDefined,
  InteractionParameterNotDefined,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  RTIinternalError)
{
  Message req, rep;

  // envoyer la requete au RTI
  req.Type              = GET_PARAMETER_NAME;
  req.ParamHandle       = theHandle;
  req.InteractionHandle = whichClass;

  executeService(&req, &rep);

  return(rep.getName());
}

//-----------------------------------------------------------------
// 8.9 Get Object Instance Handle

ObjectHandle  
RTIambassador::getObjectInstanceHandle(
  const char */*theName*/)
throw(
    ObjectNotKnown,
    NameNotFound,
    FederateNotExecutionMember,
    ConcurrentAccessAttempted,
    RTIinternalError,
    UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 8.10 Get Object Instance Name

char * 
RTIambassador::getObjectInstanceName(
  ObjectHandle /*theHandle*/)
throw(
  ObjectNotKnown,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 8.11 Get Routing Space Handle

SpaceHandle 
RTIambassador::getRoutingSpaceHandle(
  const char */*theName*/)
throw(
  NameNotFound,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 8.12 Get Routing Space Name

char * 
RTIambassador::getRoutingSpaceName(
  const SpaceHandle /*theHandle*/)
throw(
  SpaceNotDefined,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 8.13 Get Dimension Handle

DimensionHandle
RTIambassador::getDimensionHandle(
  const char         */*theName*/,
        SpaceHandle   /*whichSpace*/)
throw(
  SpaceNotDefined,
  NameNotFound,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 8.14 Get Dimension Name

char *
RTIambassador::getDimensionName(
  DimensionHandle /*theHandle*/,
  SpaceHandle     /*whichSpace*/)
throw(
  SpaceNotDefined,
  DimensionNotDefined,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 8.15 Get Attribute Routing Space Handle

SpaceHandle             
RTIambassador::getAttributeRoutingSpaceHandle(
  AttributeHandle   /*theHandle*/,
  ObjectClassHandle /*whichClass*/)
throw(
  ObjectClassNotDefined,
  AttributeNotDefined,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 8.16 Get Object Class

ObjectClassHandle    
RTIambassador::getObjectClass(
  ObjectHandle /*theObject*/)
throw(
  ObjectNotKnown,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 8.17 Get Interaction Routing Space Handle

SpaceHandle 
RTIambassador::getInteractionRoutingSpaceHandle(
  InteractionClassHandle /*theHandle*/)
throw(
  InteractionClassNotDefined,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 8.18 Get Transportation Handle

TransportationHandle
RTIambassador::getTransportationHandle(
  const char * /*theName*/)
throw(
  NameNotFound,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 8.19 Get Transportation Name

char *             
RTIambassador::getTransportationName(
  TransportationHandle /*theHandle*/) 
throw(
  InvalidTransportationHandle,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 8.20 Get Ordering Handle

OrderingHandle         
RTIambassador::getOrderingHandle(
  const char * /*theName*/) 
throw(
  NameNotFound,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 8.21 Get Ordering Name

char *               
RTIambassador::getOrderingName(
  OrderingHandle /*theHandle*/) 
throw(
  InvalidOrderingHandle,
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 8.22 Enable Class Relevance Advisory Switch

void RTIambassador::enableClassRelevanceAdvisorySwitch()
throw(
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 8.23 Disable Class Relevance Advisory Switch

void RTIambassador::disableClassRelevanceAdvisorySwitch()
throw(
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 8.24 Enable Attribute Relevance Advisory Switch

void RTIambassador::enableAttributeRelevanceAdvisorySwitch()
throw(
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 8.25 Disable Attribute Relevance Advisory Switch

void RTIambassador::disableAttributeRelevanceAdvisorySwitch()
throw(
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 8.26 Enable Attribute Scope Advisory Switch

void RTIambassador::enableAttributeScopeAdvisorySwitch()
throw(
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 8.27 Disable Attribute Scope Advisory Switch

void RTIambassador::disableAttributeScopeAdvisorySwitch()
throw(
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 8.28 Enable Interaction Relevance Advisory Switch

void RTIambassador::enableInteractionRelevanceAdvisorySwitch()
throw(
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}


//-----------------------------------------------------------------
// 8.29 Disable Interaction Relevance Advisory Switch

void RTIambassador::disableInteractionRelevanceAdvisorySwitch()
throw(
  FederateNotExecutionMember,
  ConcurrentAccessAttempted,
  SaveInProgress,
  RestoreInProgress,
  RTIinternalError,
  UnimplementedService)
{
  throw UnimplementedService();
}

//-----------------------------------------------------------------
// 8.30 Tick

Boolean RTIambassador::tick()
  throw(SpecifiedSaveLabelDoesNotExist,
         ConcurrentAccessAttempted,
  RTIinternalError)
{
  int i=0;
  Message vers_RTI, vers_Fed;

  CAttributeHandleValuePairSet theAttributes;
  CParameterHandleValuePairSet theParameters;

  // Lever une exception si appel reentrant
  if(en_service)
    throw ConcurrentAccessAttempted();

  en_service = RTI_TRUE;

  // Prevenir le RTI
  vers_RTI.Type = TICK_REQUEST;

  try {
    vers_RTI.write((SocketUN *) this);
  }
  catch(NetworkError) {
    printf("tick 1.\n");
    printf("LibRTI: Catched NetworkError, throw RTIinternalError.\n");
    throw RTIinternalError();
  }

  while(1) {
 
    // Lire la reponse du RTIA local 

    try {
      vers_Fed.read((SocketUN *) this);
    }
    catch(NetworkError) {
      printf("tick 2.\n");
      printf("LibRTI: Catched NetworkError, throw RTIinternalError.\n");
      throw RTIinternalError();
    }

    // Si c'est de type TICK_REQUEST, il n'y a qu'a traiter l'exception.

    if(vers_Fed.Type == TICK_REQUEST) {
      en_service = RTI_FALSE;
      processException(&vers_Fed);
      return(vers_Fed.Bool);
    }

    // Sinon, le RTI nous demande un service, donc on appele une methode
    // du FederateAmbassador.

    vers_RTI.Exception = e_NO_EXCEPTION;
    vers_RTI.RaisonException[0] = 0;
     
    try {
      
      switch(vers_Fed.Type) {
 
 // 2.6
      case INITIATE_PAUSE:
      fed_amb->announceSynchronizationPoint(vers_Fed.getLabel(),"");
 break;

 // 2.6
      case INITIATE_RESUME:
      fed_amb->announceSynchronizationPoint(vers_Fed.getLabel(),"");
 break;
 
 // 3.9
      case START_REGISTRATION_FOR_OBJECT_CLASS:
 fed_amb->startRegistrationForObjectClass(vers_Fed.objectClassHandle);
 break;

 // 3.10
      case STOP_REGISTRATION_FOR_OBJECT_CLASS:
 fed_amb->stopRegistrationForObjectClass(vers_Fed.objectClassHandle);
 break;

 // 3.11
      case TURN_INTERACTIONS_ON:
 fed_amb->turnInteractionsOn(vers_Fed.InteractionHandle);
 break;

 // 3.12
      case TURN_INTERACTIONS_OFF:
 fed_amb->turnInteractionsOff(vers_Fed.InteractionHandle);
 break;
 
 // 4.2
      case DISCOVER_OBJECT:
        fed_amb->discoverObjectInstance((ObjectHandle)vers_Fed.Objectid,
                                        vers_Fed.objectClassHandle,
(char *)(vers_Fed.getName()));/*FAYET 25.07.01*/

 break;
 
 // 4.4
      case REFLECT_ATTRIBUTE_VALUES: {

	for(i=0; i<vers_Fed.HandleArraySize; i++) {
  
	  CAttributeHandleValuePair *att = new CAttributeHandleValuePair;
	  
	  att->_attrib = vers_Fed.HandleArray[i];
	  
	  // BUG: Le federe s'attend surement a trouver le nom de la
	  // valeur(pour faire un GetWithName notamment.
	  strcpy(att->_value.name,  "");
	  
	  vers_Fed.getValue(i, att->_value.value); 
	  
	  // BUG: Le federe s'attend surement a trouver le type de la valeur.
	  strcpy(att->_value.type,  "");
	  
	  theAttributes.add(att);
	}

	AttributeHandleValuePairSet*
	  theAttributes_aux = CAHVPStoAHVPS(&theAttributes);
	fed_amb->reflectAttributeValues((ObjectHandle)vers_Fed.Objectid,
					*theAttributes_aux,
					RTIfedTime(vers_Fed.Date),
					(char *)vers_Fed.getTag(),
					vers_Fed.Retract);
      }
      break;

      // 4.6
      case RECEIVE_INTERACTION: {
 
 for(i=0; i<vers_Fed.HandleArraySize; i++) {
 
   CParameterHandleValuePair *par = new CParameterHandleValuePair;

   par->_param = vers_Fed.HandleArray[i];

   // BUG: Le federe s'attend surement a trouver le nom de la
   // valeur(pour faire un GetWithName notamment.
   strcpy(par->_value.name,  "");

   vers_Fed.getValue(i, par->_value.value);

   // Pareil pour le type de la valeur.
   strcpy(par->_value.type,  "");

   theParameters.add(par);
 }

 ParameterHandleValuePairSet *theParameters_aux=CPHVPStoPHVPS(&theParameters);
        fed_amb->receiveInteraction(vers_Fed.InteractionHandle,
        *theParameters_aux,
            RTIfedTime(vers_Fed.Date),
(char *)vers_Fed.getTag(),
        vers_Fed.Retract);
      }
      break;

      // 4.8
      case REMOVE_OBJECT:
 fed_amb->removeObjectInstance((ObjectHandle)vers_Fed.Objectid,
           RTIfedTime(vers_Fed.Date),
(char *)vers_Fed.getTag(),
                vers_Fed.Retract);
 break;

 // 4.15
      case PROVIDE_ATTRIBUTE_VALUE_UPDATE:
 // fed_amb->provideAttributeValueUpdate();
 break;

 // 4.??
      case REFLECT_RETRACTION:
 // fed_amb->reflectRetraction();
 break;

 // 5.3
      case REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:{

  AttributeHandleSet *AttributeSet;
  AttributeSet=AttributeHandleSetFactory::create(vers_Fed.HandleArraySize); 
  
   for(int i=0; i<vers_Fed.HandleArraySize; i++)
     {
      AttributeSet->add(vers_Fed.HandleArray[i]);
    }
     
fed_amb->requestAttributeOwnershipAssumption((ObjectHandle)vers_Fed.Objectid,
					     *AttributeSet, 
					(char *)vers_Fed.getTag());

  AttributeSet->empty();     
      }   
 break;

 // 5.7
      case REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:{

  AttributeHandleSet *AttributeSet;
  AttributeSet=AttributeHandleSetFactory::create(vers_Fed.HandleArraySize); 
  
   for(int i=0; i<vers_Fed.HandleArraySize; i++)
     {
      AttributeSet->add(vers_Fed.HandleArray[i]);
    }
     
fed_amb->requestAttributeOwnershipRelease((ObjectHandle)vers_Fed.Objectid,
					  *AttributeSet, 
					(char *)vers_Fed.getTag());

  AttributeSet->empty();     
      }   
 break; 

 // 5.??
      case ATTRIBUTE_OWNERSHIP_UNAVAILABLE:{

  AttributeHandleSet *AttributeSet;
  AttributeSet=AttributeHandleSetFactory::create(vers_Fed.HandleArraySize); 
  
   for(int i=0; i<vers_Fed.HandleArraySize; i++)
     {
      AttributeSet->add(vers_Fed.HandleArray[i]);
    }
      fed_amb->attributeOwnershipUnavailable((ObjectHandle)vers_Fed.Objectid,
					     *AttributeSet);

  AttributeSet->empty();     
      }   
 break;


 // 5.5
      case ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:{

  AttributeHandleSet *AttributeSet;
  AttributeSet=AttributeHandleSetFactory::create(vers_Fed.HandleArraySize); 
  
   for(int i=0; i<vers_Fed.HandleArraySize; i++)
     {
      AttributeSet->add(vers_Fed.HandleArray[i]);
    }
   fed_amb->attributeOwnershipAcquisitionNotification(
		(ObjectHandle)vers_Fed.Objectid,*AttributeSet);

  AttributeSet->empty();     
      }   
 break;


 // 5.4
      case ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:{

  AttributeHandleSet *AttributeSet;
  AttributeSet=AttributeHandleSetFactory::create(vers_Fed.HandleArraySize); 
  
   for(int i=0; i<vers_Fed.HandleArraySize; i++)
     {
      AttributeSet->add(vers_Fed.HandleArray[i]);
    }
      fed_amb->attributeOwnershipDivestitureNotification(
(ObjectHandle)vers_Fed.Objectid,*AttributeSet);

  AttributeSet->empty();     
      }   
 break;

 // 5.11
      case CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:{

  AttributeHandleSet *AttributeSet;
  AttributeSet=AttributeHandleSetFactory::create(vers_Fed.HandleArraySize); 
  
   for(int i=0; i<vers_Fed.HandleArraySize; i++)
     {
      AttributeSet->add(vers_Fed.HandleArray[i]);
    }
      fed_amb->confirmAttributeOwnershipAcquisitionCancellation(
(ObjectHandle)vers_Fed.Objectid,*AttributeSet);

  AttributeSet->empty();     
      }   
 break;
  

 // 5.15
      case INFORM_ATTRIBUTE_OWNERSHIP:
        fed_amb->informAttributeOwnership((ObjectHandle)vers_Fed.Objectid,
                                        vers_Fed.AttribHandle,
                    vers_Fed.NumeroFedere);
 break;
        


 // 5.??
      case ATTRIBUTE_IS_NOT_OWNED:
        fed_amb->attributeIsNotOwned((ObjectHandle)vers_Fed.Objectid,
                                        vers_Fed.AttribHandle);
 break;
    
 // 6.12
      case TIME_ADVANCE_GRANT:
      fed_amb->timeAdvanceGrant(RTIfedTime(vers_Fed.Date));
 break;

      default:
 Sortir("service demande par le RTI inconnu");
      }
    }
    catch(InvalidFederationTime &e)
      {
        vers_RTI.Exception = e_InvalidFederationTime;
        strcpy(vers_RTI.RaisonException, e._reason);
      }
    catch(TimeAdvanceWasNotInProgress &e)
      {
        vers_RTI.Exception = e_TimeAdvanceWasNotInProgress;
        strcpy(vers_RTI.RaisonException, e._reason);
      }
    catch(FederationTimeAlreadyPassed &e)
      {
        vers_RTI.Exception = e_FederationTimeAlreadyPassed;
        strcpy(vers_RTI.RaisonException, e._reason);
      }
    catch(FederateInternalError &e)
      {
        vers_RTI.Exception = e_FederateInternalError;
        strcpy(vers_RTI.RaisonException, e._reason);
      }
    catch(Exception &e)
      {
        vers_RTI.Exception = e_RTIinternalError;
        strcpy(vers_RTI.RaisonException, e._reason);
      }


    // retourner au RTI la reponse du service demande
    vers_RTI.Type = vers_Fed.Type;

    try {
      vers_RTI.write((SocketUN *) this);
    }
    catch(NetworkError) {
      printf("tick 3.\n");
      printf("LibRTI: Catched NetworkError, throw RTIinternalError.\n");
      throw RTIinternalError();
    }

  }
}


Boolean        
RTIambassador::tick(
  TickTime /*minimum*/, 
  TickTime /*maximum*/) 
throw(
  SpecifiedSaveLabelDoesNotExist,
  ConcurrentAccessAttempted,
  RTIinternalError)
{
   int      i=0;
  Message vers_RTI, vers_Fed;

  CAttributeHandleValuePairSet theAttributes;
  CParameterHandleValuePairSet theParameters;

  // Lever une exception si appel reentrant
  if(en_service)
    throw ConcurrentAccessAttempted();

  en_service = RTI_TRUE;

  // Prevenir le RTI
  vers_RTI.Type = TICK_REQUEST;

  try {
    vers_RTI.write((SocketUN *) this);
  }
  catch(NetworkError) {
    printf("LibRTI: Catched NetworkError, throw RTIinternalError.\n");
    throw RTIinternalError();
  }

  while(1) {
  
    // Lire la reponse du RTIA local 

    try {
      vers_Fed.read((SocketUN *) this);
    }
    catch(NetworkError) {
      printf("LibRTI: Catched NetworkError, throw RTIinternalError.\n");
      throw RTIinternalError();
    }

    // Si c'est de type TICK_REQUEST, il n'y a qu'a traiter l'exception.

    if(vers_Fed.Type == TICK_REQUEST) {
      en_service = RTI_FALSE;
      processException(&vers_Fed);
      return(vers_Fed.Bool);
    }

    // Sinon, le RTI nous demande un service, donc on appele une methode
    // du FederateAmbassador.

    vers_RTI.Exception = e_NO_EXCEPTION;
    vers_RTI.RaisonException[0] = 0;
        
    try {
      switch(vers_Fed.Type) {
 
 // 2.6 
      case INITIATE_PAUSE:
 fed_amb->announceSynchronizationPoint(vers_Fed.getLabel(),"");
 break;

 // 2.6
      case INITIATE_RESUME:
 fed_amb->announceSynchronizationPoint(vers_Fed.getLabel(),"");
 break;
 
 // 3.9
      case START_REGISTRATION_FOR_OBJECT_CLASS:
 fed_amb->startRegistrationForObjectClass(vers_Fed.objectClassHandle);
 break;

 // 3.10
      case STOP_REGISTRATION_FOR_OBJECT_CLASS:
 fed_amb->stopRegistrationForObjectClass(vers_Fed.objectClassHandle);
 break;
 
 // 3.11
      case TURN_INTERACTIONS_ON:
 fed_amb->turnInteractionsOn(vers_Fed.InteractionHandle);
 break;

 // 3.12
      case TURN_INTERACTIONS_OFF:
 fed_amb->turnInteractionsOff(vers_Fed.InteractionHandle);
 break;
 
 // 4.2
      case DISCOVER_OBJECT:
        fed_amb->discoverObjectInstance((ObjectHandle)vers_Fed.Objectid,
                                        vers_Fed.objectClassHandle,
(char *)(vers_Fed.getName()));/*FAYET 25.07.01*/
 break;
 
 // 4.4
      case REFLECT_ATTRIBUTE_VALUES: {

 for(i=0; i<vers_Fed.HandleArraySize; i++) {
  
   CAttributeHandleValuePair *att = new CAttributeHandleValuePair;

   att->_attrib = vers_Fed.HandleArray[i];
   
   strcpy(att->_value.name,  "");

   vers_Fed.getValue(i, att->_value.value); 

   // BUG: Le federe s'attend surement a trouver le type de la valeur.
   strcpy(att->_value.type,  "");

   theAttributes.add(att);
 }

 AttributeHandleValuePairSet *theAttributes_aux=CAHVPStoAHVPS(&theAttributes);

        fed_amb->reflectAttributeValues((ObjectHandle)vers_Fed.Objectid,
     *theAttributes_aux,
     RTIfedTime(vers_Fed.Date),
(char *)vers_Fed.getTag(),
     vers_Fed.Retract);
      }
      break;

      // 4.6
      case RECEIVE_INTERACTION: {
 
 for(i=0; i<vers_Fed.HandleArraySize; i++) {
 
   CParameterHandleValuePair *par = new CParameterHandleValuePair;

   par->_param = vers_Fed.HandleArray[i];

   strcpy(par->_value.name,  "");

   vers_Fed.getValue(i, par->_value.value);

   // Pareil pour le type de la valeur.
   strcpy(par->_value.type,  "");

   theParameters.add(par);
 }

 ParameterHandleValuePairSet *theParameters_aux=CPHVPStoPHVPS(&theParameters);

        fed_amb->receiveInteraction(vers_Fed.InteractionHandle,
        *theParameters_aux,
            RTIfedTime(vers_Fed.Date),
(char *)vers_Fed.getTag(),
        vers_Fed.Retract);
      }
      break;

      // 4.8
      case REMOVE_OBJECT:
 fed_amb->removeObjectInstance((ObjectHandle)vers_Fed.Objectid,
           RTIfedTime(vers_Fed.Date),
(char *)vers_Fed.getTag(),
                vers_Fed.Retract);
 break;

 // 4.15
      case PROVIDE_ATTRIBUTE_VALUE_UPDATE:
 // fed_amb->provideAttributeValueUpdate();
 break;

 // 4.??
      case REFLECT_RETRACTION:
 // fed_amb->reflectRetraction();
 break;


 // 5.??
      case REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION:{

  AttributeHandleSet *AttributeSet;
  AttributeSet=AttributeHandleSetFactory::create(vers_Fed.HandleArraySize); 
  
   for(int i=0; i<vers_Fed.HandleArraySize; i++)
     {
      AttributeSet->add(vers_Fed.HandleArray[i]);
    }
     
fed_amb->requestAttributeOwnershipAssumption((ObjectHandle)vers_Fed.Objectid,
					     *AttributeSet, 
					(char *)vers_Fed.getTag());

  AttributeSet->empty();     
      }   
 break;


 // 5.3
      case ATTRIBUTE_OWNERSHIP_UNAVAILABLE:{

  AttributeHandleSet *AttributeSet;
  AttributeSet=AttributeHandleSetFactory::create(vers_Fed.HandleArraySize); 
  
   for(int i=0; i<vers_Fed.HandleArraySize; i++)
     {
      AttributeSet->add(vers_Fed.HandleArray[i]);
    }
      fed_amb->attributeOwnershipUnavailable((ObjectHandle)vers_Fed.Objectid,
					     *AttributeSet);

  AttributeSet->empty();     
      }   
 break;

 // 5.7
      case REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE:{

  AttributeHandleSet *AttributeSet;
  AttributeSet=AttributeHandleSetFactory::create(vers_Fed.HandleArraySize); 
  
   for(int i=0; i<vers_Fed.HandleArraySize; i++)
     {
      AttributeSet->add(vers_Fed.HandleArray[i]);
    }
     
fed_amb->requestAttributeOwnershipRelease((ObjectHandle)vers_Fed.Objectid,
					  *AttributeSet, 
					(char *)vers_Fed.getTag());

  AttributeSet->empty();     
      }   
 break; 
 

 // 5.5
      case ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION:{

  AttributeHandleSet *AttributeSet;
  AttributeSet=AttributeHandleSetFactory::create(vers_Fed.HandleArraySize); 
  
   for(int i=0; i<vers_Fed.HandleArraySize; i++)
     {
      AttributeSet->add(vers_Fed.HandleArray[i]);
    }
      fed_amb->attributeOwnershipAcquisitionNotification(
			(ObjectHandle)vers_Fed.Objectid,*AttributeSet);

  AttributeSet->empty();     
      }   
 break;


 // 5.4
      case ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION:{

  AttributeHandleSet *AttributeSet;
  AttributeSet=AttributeHandleSetFactory::create(vers_Fed.HandleArraySize); 
  
   for(int i=0; i<vers_Fed.HandleArraySize; i++)
     {
      AttributeSet->add(vers_Fed.HandleArray[i]);
    }
      fed_amb->attributeOwnershipDivestitureNotification(
(ObjectHandle)vers_Fed.Objectid,*AttributeSet);

  AttributeSet->empty();     
      }   
 break;
 
 // 5.11
      case CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION:{

  AttributeHandleSet *AttributeSet;
  AttributeSet=AttributeHandleSetFactory::create(vers_Fed.HandleArraySize); 
  
   for(int i=0; i<vers_Fed.HandleArraySize; i++)
     {
      AttributeSet->add(vers_Fed.HandleArray[i]);
    }
      fed_amb->confirmAttributeOwnershipAcquisitionCancellation(
(ObjectHandle)vers_Fed.Objectid,*AttributeSet);

  AttributeSet->empty();     
      }   
 break;


 // 5.15
      case INFORM_ATTRIBUTE_OWNERSHIP:
        fed_amb->informAttributeOwnership((ObjectHandle)vers_Fed.Objectid,
                                        vers_Fed.AttribHandle,
                    vers_Fed.NumeroFedere);
 break;
        


 // 5.??
      case ATTRIBUTE_IS_NOT_OWNED:
        fed_amb->attributeIsNotOwned((ObjectHandle)vers_Fed.Objectid,
                                        vers_Fed.AttribHandle);
 break;

 // 6.12
      case TIME_ADVANCE_GRANT:
      fed_amb->timeAdvanceGrant(RTIfedTime(vers_Fed.Date));
 break;

      default:
 Sortir("service demande par le RTI inconnu");
      }
    }
    catch(InvalidFederationTime &e)
      {
        vers_RTI.Exception = e_InvalidFederationTime;
        strcpy(vers_RTI.RaisonException, e._reason);
      }
    catch(TimeAdvanceWasNotInProgress &e)
      {
        vers_RTI.Exception = e_TimeAdvanceWasNotInProgress;
        strcpy(vers_RTI.RaisonException, e._reason);
      }
    catch(FederationTimeAlreadyPassed &e)
      {
        vers_RTI.Exception = e_FederationTimeAlreadyPassed;
        strcpy(vers_RTI.RaisonException, e._reason);
      }
    catch(FederateInternalError &e)
      {
        vers_RTI.Exception = e_FederateInternalError;
        strcpy(vers_RTI.RaisonException, e._reason);
      }
    catch(Exception &e)
      {
        vers_RTI.Exception = e_RTIinternalError;
        strcpy(vers_RTI.RaisonException, e._reason);
      }


    // retourner au RTI la reponse du service demande
    vers_RTI.Type = vers_Fed.Type;

    try {
      vers_RTI.write((SocketUN *) this);
    }
    catch(NetworkError) {
      printf("tick 3.\n");
      printf("LibRTI: Catched NetworkError, throw RTIinternalError.\n");
      throw RTIinternalError();
    }

  }
}

//-----------------------------------------------------------------
void RTIambassador::processException(Message *msg)
{
  D.Out(pdExcept,"nom de l'exception : %d .",msg->Exception);
  switch(msg->Exception)
    {
    case e_NO_EXCEPTION:
      break;
      
    case e_ArrayIndexOutOfBounds:
      D.Out(pdExcept, "Throwing e_ArrayIndexOutOfBounds exception.");
      throw ArrayIndexOutOfBounds(msg->RaisonException);
      break;
      
    case e_AttributeAlreadyOwned:
      D.Out(pdExcept, "Throwing e_AttributeAlreadyOwned exception.");
      throw AttributeAlreadyOwned(msg->RaisonException);
      break;
      
    case e_AttributeAlreadyBeingAcquired:
      D.Out(pdExcept, "Throwing e_AttributeAlreadyBeingAcquired exception.");
      throw AttributeAlreadyBeingAcquired(msg->RaisonException);
      break;

    case e_AttributeAlreadyBeingDivested:
      D.Out(pdExcept, "Throwing e_AttributeAlreadyBeingDivested exception.");
      throw AttributeAlreadyBeingDivested(msg->RaisonException);
      break;

    case e_AttributeDivestitureWasNotRequested:
      D.Out(pdExcept, 
	     "Throwing e_AttributeDivestitureWasNotRequested exception.");
      throw AttributeDivestitureWasNotRequested(msg->RaisonException);
      break;

    case e_AttributeAcquisitionWasNotRequested:
      D.Out(pdExcept, 
	     "Throwing e_AttributeAcquisitionWasNotRequested exception.");
      throw AttributeAcquisitionWasNotRequested(msg->RaisonException);
      break;
              
    case e_AttributeNotDefined:
      D.Out(pdExcept, "Throwing e_AttributeNotDefined exception.");
      throw AttributeNotDefined(msg->RaisonException); 
      break;

    case e_AttributeNotKnown:
      D.Out(pdExcept, "Throwing e_AttributeNotKnown exception.");
      throw AttributeNotKnown(msg->RaisonException);
      break;

    case e_AttributeNotOwned:
      D.Out(pdExcept, "Throwing e_AttributeNotOwned exception.");
      throw AttributeNotOwned(msg->RaisonException);
      break;

    case e_AttributeNotPublished:
      D.Out(pdExcept, "Throwing e_AttributeNotPublished exception.");
      throw AttributeNotPublished(msg->RaisonException);
      break;

    case e_AttributeNotSubscribed:
      D.Out(pdExcept, "Throwing e_AttributeNotSubscribed exception.");
      throw AttributeNotSubscribed(msg->RaisonException);
      break;

    case e_ConcurrentAccessAttempted:
      D.Out(pdExcept, "Throwing e_ConcurrentAccessAttempted exception.");
      throw ConcurrentAccessAttempted(msg->RaisonException);
      break;

    case e_CouldNotDiscover:
      D.Out(pdExcept, "Throwing e_CouldNotDiscover exception.");
      throw CouldNotDiscover(msg->RaisonException);
      break;

    case e_CouldNotOpenRID:
      D.Out(pdExcept, "Throwing e_CouldNotOpenRID exception.");
      throw CouldNotOpenRID(msg->RaisonException);
      break;

    case e_CouldNotRestore:
      D.Out(pdExcept, "Throwing e_CouldNotRestore exception.");
      throw CouldNotRestore(msg->RaisonException);
      break;

    case e_DeletePrivilegeNotHeld:
      D.Out(pdExcept, "Throwing e_DeletePrivilegeNotHeld exception.");
      throw DeletePrivilegeNotHeld(msg->RaisonException);
      break;

    case e_ErrorReadingRID:
      D.Out(pdExcept, "Throwing e_ErrorReadingRID exception.");
      throw ErrorReadingRID(msg->RaisonException);
      break;

    case e_EventNotKnown:
      D.Out(pdExcept, "Throwing e_EventNotKnown exception.");
      throw EventNotKnown(msg->RaisonException);
      break;

    case e_FederateAlreadyPaused:
      D.Out(pdExcept, "Throwing e_FederateAlreadyPaused exception.");
      throw FederateAlreadyPaused(msg->RaisonException);
      break;

    case e_FederateAlreadyExecutionMember:
      D.Out(pdExcept, "Throwing e_FederateAlreadyExecutionMember exception.");
      throw FederateAlreadyExecutionMember(msg->RaisonException);
      break;

    case e_FederateDoesNotExist:
      D.Out(pdExcept, "Throwing e_FederateDoesNotExist exception.");
      throw FederateDoesNotExist(msg->RaisonException);
      break;

    case e_FederateInternalError:
      D.Out(pdExcept, "Throwing e_FederateInternalError exception.");
      throw FederateInternalError(msg->RaisonException);
      break;

    case e_FederateNameAlreadyInUse:
      D.Out(pdExcept, "Throwing e_FederateNameAlreadyInUse exception.");
      throw FederateNameAlreadyInUse(msg->RaisonException);
      break;

    case e_FederateNotExecutionMember:
      D.Out(pdExcept, "Throwing e_FederateNotExecutionMember exception.");
      throw FederateNotExecutionMember(msg->RaisonException);
      break;

    case e_FederateNotPaused:
      D.Out(pdExcept, "Throwing e_FederateNotPaused exception.");
      throw FederateNotPaused(msg->RaisonException);
      break;

    case e_FederateOwnsAttributes:
      D.Out(pdExcept, "Throwing e_FederateOwnsAttributes exception.");
      throw FederateOwnsAttributes(msg->RaisonException);
      break;

    case e_FederatesCurrentlyJoined:
      D.Out(pdExcept, "Throwing e_FederatesCurrentlyJoined exception.");
      throw FederatesCurrentlyJoined(msg->RaisonException);
      break;

    case e_FederateWasNotAskedToReleaseAttribute:
      D.Out(pdExcept, 
	     "Throwing e_FederateWasNotAskedToReleaseAttribute exception.");
      D.Out(pdDebug, 
	     "Throwing e_FederateWasNotAskedToReleaseAttribute exception.");   
      throw FederateWasNotAskedToReleaseAttribute(msg->RaisonException);
      break;
       
    case e_FederationAlreadyPaused:
      D.Out(pdExcept, "Throwing e_FederationAlreadyPaused exception.");
      throw FederationAlreadyPaused(msg->RaisonException);
      break;

    case e_FederationExecutionAlreadyExists:
      D.Out(pdExcept, "Throwing e_FederationExecutionAlreadyExists excep.");
      throw FederationExecutionAlreadyExists(msg->RaisonException);
      break;

    case e_FederationExecutionDoesNotExist:
      D.Out(pdExcept, "Throwing e_FederationExecutionDoesNotExist except.");
      throw FederationExecutionDoesNotExist(msg->RaisonException);
      break;

    case e_FederationNotPaused:
      D.Out(pdExcept, "Throwing e_FederationNotPaused exception.");
      throw FederationNotPaused(msg->RaisonException);
      break;
      
    case e_FederationTimeAlreadyPassed:
      D.Out(pdExcept, "Throwing e_FederationTimeAlreadyPassed exception.");
      throw FederationTimeAlreadyPassed(msg->RaisonException);
      break;
      
    case e_FederateNotPublishing:
      D.Out(pdExcept, "Throwing e_FederateNotPublishing exception.");
      throw FederateNotPublishing(msg->RaisonException);
      break;

    case e_FederateNotSubscribing:
      D.Out(pdExcept, "Throwing e_FederateNotSubscribing exception.");
      throw FederateNotSubscribing(msg->RaisonException);
      break;
      
    case e_RegionNotKnown:
      D.Out(pdExcept, "Throwing e_RegionNotKnown exception.");
      throw RegionNotKnown(msg->RaisonException);
      break;
      
    case e_IDsupplyExhausted:
      D.Out(pdExcept, "Throwing e_IDsupplyExhausted exception.");
      throw IDsupplyExhausted(msg->RaisonException);
      break;

    case e_InteractionClassNotDefined:
      D.Out(pdExcept, "Throwing e_InteractionClassNotDefined exception.");
      throw InteractionClassNotDefined(msg->RaisonException);
      break;

    case e_InteractionClassNotKnown:
      D.Out(pdExcept, "Throwing e_InteractionClassNotKnown exception.");
      throw InteractionClassNotKnown(msg->RaisonException);
      break;

    case e_InteractionClassNotPublished:
      D.Out(pdExcept, "Throwing e_InteractionClassNotPublished exception.");
      throw InteractionClassNotPublished(msg->RaisonException);
      break;

    case e_InteractionParameterNotDefined:
      D.Out(pdExcept, "Throwing e_InteractionParameterNotDefined exception.");
      throw InteractionParameterNotDefined(msg->RaisonException);
      break;

    case e_InteractionParameterNotKnown:
      D.Out(pdExcept, "Throwing e_InteractionParameterNotKnown exception.");
      throw InteractionParameterNotKnown(msg->RaisonException);
      break;

    case e_InvalidDivestitureCondition:
      D.Out(pdExcept, "Throwing e_InvalidDivestitureCondition exception.");
      throw InvalidDivestitureCondition(msg->RaisonException);
      break;

    case e_InvalidExtents:
      D.Out(pdExcept, "Throwing e_InvalidExtents exception.");
      throw InvalidExtents(msg->RaisonException);
      break;

    case e_InvalidFederationTime:
      D.Out(pdExcept, "Throwing e_InvalidFederationTime exception.");
      throw InvalidFederationTime(msg->RaisonException);
      break;

    case e_InvalidFederationTimeDelta:
      D.Out(pdExcept, "Throwing e_InvalidFederationTimeDelta exception.");
      throw InvalidFederationTimeDelta(msg->RaisonException);
      break;

    case e_InvalidObjectHandle:
      D.Out(pdExcept, "Throwing e_InvalidObjectHandle exception.");
      throw InvalidObjectHandle(msg->RaisonException);
      break;

    case e_InvalidOrderType:
      D.Out(pdExcept, "Throwing e_InvalidOrderType exception.");
      throw InvalidOrderType(msg->RaisonException);
      break;

    case e_InvalidResignAction:
      D.Out(pdExcept, "Throwing e_InvalidResignAction exception.");
      throw InvalidResignAction(msg->RaisonException);
      break;

    case e_InvalidRetractionHandle:
      D.Out(pdExcept, "Throwing e_InvalidRetractionHandle exception.");
      throw InvalidRetractionHandle(msg->RaisonException);
      break;

    case e_InvalidRoutingSpace:
      D.Out(pdExcept, "Throwing e_InvalidRoutingSpace exception.");
      throw InvalidRoutingSpace(msg->RaisonException);
      break;

    case e_InvalidTransportType:
      D.Out(pdExcept, "Throwing e_InvalidTransportType exception.");
      throw InvalidTransportType(msg->RaisonException);
      break;

    case e_MemoryExhausted:
      D.Out(pdExcept, "Throwing e_MemoryExhausted exception.");
      throw MemoryExhausted(msg->RaisonException);
      break;

    case e_NameNotFound:
      D.Out(pdExcept, "Throwing e_NameNotFound exception.");
      throw NameNotFound(msg->RaisonException);
      break;

    case e_NoPauseRequested:
      D.Out(pdExcept, "Throwing e_NoPauseRequested exception.");
      throw NoPauseRequested(msg->RaisonException);
      break;

    case e_NoResumeRequested:
      D.Out(pdExcept, "Throwing e_NoResumeRequested exception.");
      throw NoResumeRequested(msg->RaisonException);
      break;

    case e_ObjectClassNotDefined:
      D.Out(pdExcept, "Throwing e_ObjectClassNotDefined exception.");
      throw ObjectClassNotDefined(msg->RaisonException);
      break;

    case e_ObjectClassNotKnown:
      D.Out(pdExcept, "Throwing e_ObjectClassNotKnown exception.");
      throw ObjectClassNotKnown(msg->RaisonException);
      break;

    case e_ObjectClassNotPublished:
      D.Out(pdExcept, "Throwing e_ObjectClassNotPublished exception.");
      throw ObjectClassNotPublished(msg->RaisonException);
      break;

    case e_ObjectClassNotSubscribed:
      D.Out(pdExcept, "Throwing e_ObjectClassNotSubscribed exception.");
      throw ObjectClassNotSubscribed(msg->RaisonException);
      break;

    case e_ObjectNotKnown:
      D.Out(pdExcept, "Throwing e_ObjectNotKnown exception.");
      throw ObjectNotKnown(msg->RaisonException);
      break;

    case e_ObjectAlreadyRegistered:
      D.Out(pdExcept, "Throwing e_ObjectAlreadyRegistered exception.");
      throw ObjectAlreadyRegistered(msg->RaisonException);
      break;

    case e_RestoreInProgress:
      D.Out(pdExcept, "Throwing e_RestoreInProgress exception.");
      throw RestoreInProgress(msg->RaisonException);
      break;

    case e_RestoreNotRequested:
      D.Out(pdExcept, "Throwing e_RestoreNotRequested exception.");
      throw RestoreNotRequested(msg->RaisonException);
      break;

    case e_RTIinternalError:
      D.Out(pdExcept, "Throwing e_RTIinternalError exception.");
      throw RTIinternalError(msg->RaisonException);
      break;

    case e_SpaceNotDefined:
      D.Out(pdExcept, "Throwing e_SpaceNotDefined exception.");
      throw SpaceNotDefined(msg->RaisonException);
      break;

    case e_SaveInProgress:
      D.Out(pdExcept, "Throwing e_SaveInProgress exception.");
      throw SaveInProgress(msg->RaisonException);
      break;

    case e_SaveNotInitiated:
      D.Out(pdExcept, "Throwing e_SaveNotInitiated exception.");
      throw SaveNotInitiated(msg->RaisonException);
      break;

    case e_SecurityError:
      D.Out(pdExcept, "Throwing e_SecurityError exception.");
      throw SecurityError(msg->RaisonException);
      break;

    case e_SpecifiedSaveLabelDoesNotExist:
      D.Out(pdExcept, "Throwing e_SpecifiedSaveLabelDoesNotExist exception.");
      throw SpecifiedSaveLabelDoesNotExist(msg->RaisonException);
      break;

    case e_TimeAdvanceAlreadyInProgress:
      D.Out(pdExcept, "Throwing e_TimeAdvanceAlreadyInProgress exception.");
      throw TimeAdvanceAlreadyInProgress(msg->RaisonException);
      break;

    case e_TimeAdvanceWasNotInProgress:
      D.Out(pdExcept, "Throwing e_TimeAdvanceWasNotInProgress exception.");
      throw TimeAdvanceWasNotInProgress(msg->RaisonException);
      break;

    case e_TooManyIDsRequested:
      D.Out(pdExcept, "Throwing e_TooManyIDsRequested exception.");
      throw TooManyIDsRequested(msg->RaisonException);
      break;

    case e_UnableToPerformSave:
      D.Out(pdExcept, "Throwing e_UnableToPerformSave exception.");
      throw UnableToPerformSave(msg->RaisonException);
      break;

    case e_UnimplementedService:
      D.Out(pdExcept, "Throwing e_UnimplementedService exception.");
      throw UnimplementedService(msg->RaisonException);
      break;

    case e_UnknownLabel:
      D.Out(pdExcept, "Throwing e_UnknownLabel exception.");
      throw UnknownLabel(msg->RaisonException);
      break;

    case e_ValueCountExceeded:
      D.Out(pdExcept, "Throwing e_ValueCountExceeded exception.");
      throw ValueCountExceeded(msg->RaisonException);
      break;

    case e_ValueLengthExceeded:
      D.Out(pdExcept, "Throwing e_ValueLengthExceeded exception.");
      throw ValueLengthExceeded(msg->RaisonException);
      break;

    default:
      D.Out(pdExcept, "Throwing unknown exception !");
      printf("LibRTI: Receving unknown exception.\n");
      throw RTIinternalError(msg->RaisonException);
      break;
  }
}

}

// $Id: RTIambassador.cc,v 3.3 2002/11/27 23:07:19 breholee Exp $
