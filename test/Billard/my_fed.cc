// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002  ONERA
//
// This file is part of CERTI
//
// CERTI is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// $Id: my_fed.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include <memory.h>

#include "my_fed.hh"
#include "constants.hh"

#include "PrettyDebug.hh"

static pdCDebug D("FEDAMB", "(Fed_Amba) - ");

//
// constructeur
//
//
Fed::Fed(RTI::RTIambassador *rtia)
{
  RTIA = rtia;
  Granted = RTI_FALSE;
  InPause = RTI_FALSE;
  RemoteCount = 0;
  D.Out(pdInit, "Federate Ambassador created.");
}

//
// destructeur
//
//
Fed::~Fed()
{
  D.Out(pdTerm, "Federate Ambassador destroyed.");
}

//
// DeleteObjects
//

void Fed::DeleteObjects(const FedTime& DeletionTime)
{
  try
  {
    RTIA->deleteObjectInstance(Local.ID, DeletionTime, "DO");
    D.Out(pdRegister, "Local object deleted from federation.");
  }
  catch(Exception &e)
  {
    D.Out(pdExcept,"**** Exception delete object : %d", &e);
  }
}

//
// discoverObject
//

void Fed::discoverObjectInstance(
        ObjectHandle              theObject,      
        ObjectClassHandle     theObjectClass, 
  const char                  *theObjectName)
throw(
  CouldNotDiscover,
  ObjectClassNotKnown,
  InvalidFederationTime,
  FederateInternalError)
{
  if(theObjectClass != BilleClassID)
    {
      D.Out(pdError, "Object of Unknown Class discovered.");
      throw RTIinternalError();
    }
  Remote[RemoteCount].ID = theObject;
  printf("Maintenant, je connais l'objet %d\n", theObject);
  RemoteCount++;
}

// 
// GetHandles
//

void Fed::GetHandles(void)
{
  // Classes d'Objets

  BilleClassID = RTIA->getObjectClassHandle(CLA_BILLE);

  BouleClassID = RTIA->getObjectClassHandle(CLA_BOULE);

  D.Out(pdInit, "BilleClassID = %d, BouleClassID = %d.", 
	 BilleClassID, BouleClassID);

  // Attributs des classes d'Objets

  AttrXID      = RTIA->getAttributeHandle(ATT_POSITION_X, BilleClassID);
  AttrYID      = RTIA->getAttributeHandle(ATT_POSITION_Y, BilleClassID);
  AttrColorID  = RTIA->getAttributeHandle(ATT_COLOR,      BouleClassID);

  D.Out(pdInit, "AttrXID = %d, AttrYID = %d, AttrColorID = %d.",
	 AttrXID, AttrYID, AttrColorID);
  
  // Interactions

  BingClassID   = RTIA->getInteractionClassHandle(INT_BING);
  ParamBoulID   = RTIA->getParameterHandle(PAR_BOUL, BingClassID);
  ParamDXID     = RTIA->getParameterHandle(PAR_DX, BingClassID);
  ParamDYID     = RTIA->getParameterHandle(PAR_DY, BingClassID);

  D.Out(pdInit, "BingClassID = %d, DX_ID = %d, DY_ID = %d",
	 BingClassID, ParamDXID, ParamDYID);

}

//
// announceSynchronizationPoint
//
void Fed::announceSynchronizationPoint(
  const char *label,
  const char *tag) 
throw(
  FederateInternalError)
{
  if((strcmp(label,"Freeze") == 0)||(strcmp(label,"Init") == 0)) {
    InPause = RTI_TRUE;
    strcpy(CurrentPauseLabel, label);
    D.Out(pdProtocol, "announceSynchronizationPoint.");
  }
  else {
    if(strcmp(label,"Unfreeze") == 0) {
      InPause = RTI_FALSE;
      strcpy(CurrentPauseLabel, label);
      D.Out(pdProtocol, "announceSynchronizationPoint.");
    }
    else
      throw RTIinternalError();
  }
}

//
// synchronisationPointRegistrationSucceeded
//

void Fed::synchronizationPointRegistrationSucceeded(
  const char *label) 
throw(
  FederateInternalError)
{
  D.Out(pdProtocol,"CALLBACK : synchronizationPointRegistrationSucceeded with label %s",
	label);
}

//
// synchronisationPointRegistrationFailed
//

void Fed::synchronizationPointRegistrationFailed(
  const char *label) 
throw(
  FederateInternalError)
{

   D.Out(pdProtocol,"****** CALLBACK : synchronizationPointRegistrationFailed with label %s",
        label);
}

//
// federationSynchronized
//

void Fed::federationSynchronized(
  const char *label)
throw(
  FederateInternalError)
{

  InPause = RTI_FALSE;
  D.Out(pdProtocol,"CALLBACK : federationSynchronized with label %s",label);
}

//
// PublishAndSubscribe
//

void Fed::PublishAndsubscribe(void)
{ 
  AttributeHandleSet                 *AttributeSet;
//  CAttributeHandleValuePair          *Attribute;

  // Get all class and attributes handles
  GetHandles();

  // Add PositionX et PositionY to the attribute set

  AttributeSet=AttributeHandleSetFactory::create(3);
  AttributeSet->add(AttrXID);
  AttributeSet->add(AttrYID);

  
  // Subscribe to Bille objects.

  RTIA->subscribeObjectClassAttributes(BilleClassID, *AttributeSet,RTI_TRUE);

  // Publish Boule Objects.

  AttributeSet->add(AttrColorID);

  RTIA->publishObjectClass(BouleClassID, *AttributeSet);

  // Publish and subscribe to Bing interactions

  RTIA->subscribeInteractionClass(BingClassID,RTI_TRUE);
  RTIA->publishInteractionClass(BingClassID);

  AttributeSet->empty();

  D.Out(pdInit, "Local Objects and Interactions published and subscribed.");
}

//
// receiveInteraction
//

void Fed::receiveInteraction(
        InteractionClassHandle        theInteraction, 
  const	ParameterHandleValuePairSet&  theParameters,  
  const FedTime&                      theTime,        
  const char                          *theTag,         
        EventRetractionHandle         theHandle)      
throw(
  InteractionClassNotKnown,
  InteractionParameterNotKnown,
  InvalidFederationTime,
  FederateInternalError)
{
  char  *parmValue;
  ULong valueLength;
  int dx1,dy1;
  Boolean bille = RTI_FALSE;

  D.Out(pdTrace,"Fed : receiveInteraction");
  if(theInteraction != BingClassID) {
    D.Out(pdError,"CALLBACK receiveInteraction : Unknown Interaction received");
    exit(-1);
  }
  
  D.Out(pdDebug,"receiveInteraction - nb attributs= %d",
	theParameters.size());

  for(int j=0 ; j < theParameters.size() ; j++) {
    ParameterHandle handle = theParameters.getHandle(j) ;
    
    valueLength = theParameters.getValueLength(j);
    parmValue = new char[valueLength];
    theParameters.getValue(j, parmValue, valueLength);
      
    if(handle == ParamDXID) {  
      if(parmValue != NULL) {
	dx1 = atoi(parmValue);
//	Local.dx = atof(parmValue);
	D.Out(pdDebug,"receiveInteraction(*) - dx= %s",parmValue);
	D.Out(pdDebug,"receiveInteraction - dx= %f",Local.dx);
	delete [] parmValue;
      }
      else
	D.Out(pdError,"Missing Attribute in RAV.") ;
    }
    else 
      if(handle == ParamDYID) {
	if(parmValue != NULL) {
	  dy1 = atoi(parmValue);
//	  Local.dy = atof(parmValue);
	  D.Out(pdDebug,"receiveInteraction(*) - dy= %s",parmValue);
	  D.Out(pdDebug,"receiveInteraction - dy= %f",Local.dy);
	  delete [] parmValue;
	}
	else
	  D.Out(pdError,"Missing Attribute in RAV.") ;
      }
      else
	if(handle == ParamBoulID) {	  
	  if(parmValue != NULL) {
	    D.Out(pdDebug,"receiveInteraction(***) - Local.ID= %d",
		  Local.ID);
	    D.Out(pdDebug,"receiveInteraction(***) - parmValue= %s",
		  parmValue);
	    if(Local.ID == atof(parmValue))
	      bille = RTI_TRUE;
	    else
	      bille = RTI_FALSE;
	  }
	  else
	    D.Out(pdError,"Unrecognized parameter handle");
	}
  }
  if(bille) {
    Local.dx = dx1;
    Local.dy = dy1;
    D.Out(pdDebug,"**** receiveInteraction(*) - dx1 = %f, local.dx = %f",dx1,Local.dx);
    D.Out(pdDebug,"**** receiveInteraction(*) - dy1 = %f, local.dy = %f",dy1,Local.dy);
  }
}


//
// reflectAttributeValues
//

void Fed::reflectAttributeValues(
        ObjectHandle                  theObject,     
  const	AttributeHandleValuePairSet&  theAttributes, 
  const FedTime&                      theTime,       
  const char                          *theTag,        
        EventRetractionHandle         theHandle)     
throw(
  ObjectNotKnown,
  AttributeNotKnown,
  InvalidFederationTime,
  FederateInternalError)
{
  D.Out(pdTrace,"reflectAttributeValues"); 

  int             i=0;
  float           oldx, oldy;
  AttributeHandle *attribut;
  ULong           valueLength;
  char            *attrValue;

  for(i=0; i<RemoteCount; i++)
    {
      if(Remote[i].ID == theObject)
	break;
    }
  
  if(i == RemoteCount)
    D.Out(pdError,"Fed: ERREUR: objet id non trouve(%d).", theObject);
  else
    {
      Remote[i].Effacer();

      D.Out(pdDebug,"reflectAttributeValues - nb attributs= %d",
	    theAttributes.size());
      for(int j=0; j<theAttributes.size(); j++)
	{
	  AttributeHandle handle = theAttributes.getHandle(j);
	  valueLength = theAttributes.getValueLength(j);
	  attrValue = new char[valueLength];
	  theAttributes.getValue(j,attrValue,valueLength);

	  if(handle == AttrXID)
	    {
	      if(attrValue != NULL)
		{
		  oldx = Remote[i].x;
		  Remote[i].x = atof(attrValue);
		  Remote[i].dx = Remote[i].x - oldx;
		  D.Out(pdDebug,"reflectAttributeValues - x= %f, dx= %f",
			Remote[i].x,Remote[i].dx);
		  delete [] attrValue;
		}
	      else
		D.Out(pdError,"Fed: ERREUR: missing Attribute.");
	    }
	  else if(handle == AttrYID)
	    {
	      if(attrValue != NULL)
		{
		  oldy = Remote[i].y;
		  Remote[i].y = atof(attrValue);
		  Remote[i].dy = Remote[i].y - oldy;
		  D.Out(pdDebug,"reflectAttributeValues - y= %f, dy= %f",
			Remote[i].y,Remote[i].dy);
		  delete [] attrValue;
		}
	      else
		D.Out(pdError,"Fed: ERREUR: missing Attribute.");
	    }
	  else
	    D.Out(pdError,"Fed: ERREUR: handle inconnu.");
	}
      Remote[i].Afficher();
    } 
}

//
// RegisterObjects
//

void Fed::RegisterObjects(void)
{
  Local.ID = RTIA->registerObjectInstance(BouleClassID);
  D.Out(pdRegister, "Local object registered under ID %d.", Local.ID);
}

//
// removeObject
//

void Fed::removeObjectInstance(
        ObjectHandle          theObject, 
//        ObjectRemovalReason   theReason, 
  const FedTime&              theTime,   
  const char                  *theTag,    
        EventRetractionHandle theHandle) 
throw(
  ObjectNotKnown,
  InvalidFederationTime,
  FederateInternalError)
{
  int i=0;
  
  for(i=0; i<RemoteCount; i++)
    {
      if(Remote[i].ID == theObject)
	break;
    }

  if(i == RemoteCount)
    printf("Fed: ERREUR: objet id non trouve(%d)\n", theObject);
  else
    {
      printf("Fed: RemoveObject id=%d\n", theObject);     
      Remote[i].Effacer();
      RemoteCount--;
    }
}

//
// SendInteraction
//

void Fed::sendInteraction(const FedTime& InteractionTime,
			   ObjectHandle Id)
{
  char buf[512];
  ParameterHandleValuePairSet  *ParameterSet=NULL ;
  
  ParameterSet = ParameterSetFactory::create( 3 );
  
  sprintf(buf, "%d",Id);
  ParameterSet->add(ParamBoulID, buf, strlen(buf)+1) ;

  D.Out(pdDebug,"SendInteraction");
  D.Out(pdDebug,"SendInteraction - ParamBoulID= %u",
	ParamBoulID);
  D.Out(pdDebug,"SendInteraction - x= %d",
	Id);
  D.Out(pdDebug,"SendInteraction - buf= %s",
	buf);

//  D.Out(pdDebug,"SendInteraction - ParamBoulID= %u, x= %f, buf= %s",
//	ParamBoulID,Id,buf);
  
  sprintf(buf, "%f",Local.dx);
  ParameterSet->add(ParamDXID, buf, strlen(buf)+1) ;
  D.Out(pdDebug,"SendInteraction - ParamDXID= %u, x= %f, buf= %s",
	ParamDXID,Local.dx,buf);
  
  sprintf(buf, "%f",Local.dy);
  ParameterSet->add(ParamDYID, buf, strlen(buf)+1) ;
  D.Out(pdDebug,"SendInteraction - ParamDYID= %u, x= %f, buf= %s",
	ParamDYID,Local.dy,buf);
  
  D.Out(pdRegister,"Sending interaction(DX= %f, DY= %f).",Local.dx,Local.dy);

  try
  {
     RTIA->sendInteraction(BingClassID, *ParameterSet,
			  InteractionTime, "") ;
  }
  catch( Exception& e )
  {
       D.Out(pdExcept,"**** Exception sending interaction : %d", &e);
  }
  ParameterSet->empty();
}

//
// SendUpdate
//

void Fed::SendUpdate(const FedTime& UpdateTime)
{
  char buf[512] ;
  AttributeHandleValuePairSet  *AttributeSet;
  
  AttributeSet = AttributeSetFactory::create( 3 );

  D.Out(pdTrace, "SendUpdate.");

  sprintf(buf, "%f",Local.x);
  AttributeSet->add(AttrXID, buf, strlen(buf)+1) ;
  D.Out(pdDebug,"SendUpdate - AttrXID= %u, x= %f, size= %u",
	AttrXID,Local.x,AttributeSet->size());
  
  sprintf(buf, "%f",Local.y);
  AttributeSet->add(AttrYID, buf, strlen(buf)+1) ;
  D.Out(pdDebug,"SendUpdate - AttrYID= %u, y= %f, size= %u",
	AttrYID,Local.y,AttributeSet->size());
  
  sprintf(buf, "%f",Local.Color);
  AttributeSet->add(AttrColorID, buf, strlen(buf)+1) ;
  D.Out(pdDebug,"SendUpdate - AttrColorID= %u, color= %f, size= %u",
	AttrColorID,Local.color,AttributeSet->size());
  
  try
  {
    RTIA->updateAttributeValues(Local.ID,   *AttributeSet,
				  UpdateTime, "");
  }
  catch( Exception& e )
  {
    D.Out(pdExcept,"**** Exception updating attribute values: %d",&e) ;
  }
  AttributeSet->empty();
}

void Fed::turnInteractionsOn(
  InteractionClassHandle theHandle) 
throw(
  InteractionClassNotPublished,
  FederateInternalError)
{
}

void Fed::turnInteractionsOff(
  InteractionClassHandle theHandle) 
throw(
  InteractionClassNotPublished,
  FederateInternalError)
{
}

void Fed::startRegistrationForObjectClass(
        ObjectClassHandle   theClass)
throw(
  ObjectClassNotPublished,
  FederateInternalError)
{
}

void Fed::stopRegistrationForObjectClass(
        ObjectClassHandle   theClass)
throw(
  ObjectClassNotPublished,
  FederateInternalError)
{
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * timeAdvanceGrant
 */
void Fed::timeAdvanceGrant(const FedTime& theTime) 
  throw(InvalidFederationTime, TimeAdvanceWasNotInProgress, 
	 FederationTimeAlreadyPassed, FederateInternalError) {

  Granted = RTI_TRUE;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * timeRegulationEnabled
 */
void Fed::timeRegulationEnabled(const FedTime& theTime) 
  throw(InvalidFederationTime, EnableTimeRegulationWasNotPending,
	FederateInternalError) {
  
  return;
}

// <OWM>

//---------------------------------------------------------------------------
// requestAttributeOwnershipRelease
//---------------------------------------------------------------------------

void 
Fed::requestAttributeOwnershipRelease( 
					    ObjectHandle theObject, // supplied C1
					    const AttributeHandleSet& candidateAttributes, // supplied C4
					    const char *theTag) // supplied C4
  throw(
	 ObjectNotKnown,
	 AttributeNotKnown,
	 AttributeNotOwned,
	 FederateInternalError)
{
  cout << "requestAttributeOwnershipRelease..." << endl;
}
void Fed::informAttributeOwnership(
				    ObjectHandle theObject, // supplied C1
				    AttributeHandle theAttribute, // supplied C1
				    FederateHandle theOwner) // supplied C1
  throw(
	 ObjectNotKnown,
	 AttributeNotKnown,
	 FederateInternalError)
{
  cout << "CALLBACK informAttributeOwnership : Attribute " << theAttribute << " de l'objet " << theObject <<
    " propriété de " << theOwner << endl;
}
void Fed::attributeIsNotOwned(
			       ObjectHandle theObject, // supplied C1
			       AttributeHandle theAttribute) // supplied C1
  throw(
	 ObjectNotKnown,
	 AttributeNotKnown,
	 FederateInternalError)
{
  cout << "CALLBACK attributeIsNotOwned : Attribute " << theAttribute << " de l'objet " << theObject << endl;
}

void Fed::attributeOwnershipUnavailable(
					 ObjectHandle theObject, 
					 const AttributeHandleSet& 	offeredAttributes) 
  throw(
	 ObjectNotKnown,
	 AttributeNotDefined,	
	 AttributeAlreadyOwned,
	 AttributeAcquisitionWasNotRequested,
	 FederateInternalError)
{
  cout << "CALLBACK attributeOwnershipUnavailable Objet "<< theObject << endl;
	 

  //Attributs indisponibles
	
  cout << "CALLBACK Nb d'attributs dans l'AttributeHandleSet : " << offeredAttributes.size() << endl;
	
  for(int j=0 ; j < offeredAttributes.size() ; j++)
    {
 
      AttributeHandle handle = offeredAttributes.getHandle(j) ;
 
      if(handle == AttrXID)
	{ 
	  cout << "CALLBACK AttrXID indisponible" << endl;	
	}
      else if(handle == AttrYID)
	{
	  cout << "CALLBACK AttrYID indisponible" << endl;	
	}
      // 		 else if(handle == AAttrTestID)
      // {
      // 					cout << "CALLBACK TEST indisponible" << endl;	
      // 					}
      else
	cout << endl << "CALLBACK Unrecognized handle" << endl ;
    }
}	

//---------------------------------------------------------------------------
// attributeOwnershipAcquisitionNotification
//---------------------------------------------------------------------------

void Fed::attributeOwnershipAcquisitionNotification(
						     ObjectHandle theObject, // supplied C1
						     const AttributeHandleSet& securedAttributes) // supplied C4
  throw(
	 ObjectNotKnown,
	 AttributeNotKnown,
	 AttributeAcquisitionWasNotRequested,
	 AttributeAlreadyOwned,
	 AttributeNotPublished,
	 FederateInternalError)
{
 //  cout << "attributeOwnershipAcquisitionNotification de : " << theObject << endl;

//   //Attributs dont l'acquisition a été obtenue
	
//   //	cout << "Nb d'attributs dans l'AttributeHandleSet : " << securedAttributes.size() << endl;
	
//   for(int j=0 ; j < securedAttributes.size() ; j++)
//     {
 
//       AttributeHandle handle = securedAttributes.getHandle(j) ;
 
//       if(handle == AttrXID)
// 	{ 
// 	  cout << "Acquisition de AttrXID" << endl;	
// 	}
//       else if(handle == AttrYID)
// 	{
// 	  cout << "Acquisition de AttrYID" << endl;	
// 	}
//       else if(handle == Privilege_To_Delete)
// 	{
// 	  cout << "Acquisition de Privilege_To_Delete" << endl;	
// 	}
//       else
// 	cout << endl << "Unrecognized handle" << endl ;
//     }

//   int i =0;
	
//   if( theObject != Local .ID)
//     {
//       for(i = 0; i <=(RemoteCount-1); i++)
// 	{
// 	  //cout << "Remote[" << i << "]=" << Remote[i].ID << endl;		
// 	  if(theObject == Remote[i].ID )
// 	    {
// 	      cout << "Acquisition effectuée " << endl; 			
// 	      Je_Gere_Boule = RTI_TRUE;
// 	      Local.ID=theObject;		 
// 	      Local.dx=Remote[i].dx;
// 	      Local.dy=Remote[i].dy;
// 	      Local.x=Remote[i].x;
// 	      Local.y=Remote[i].y;							 
// 	      //	 	Remote[i].ID = 0;//
// 	      break;
// 	    }
// 	}
//       //Tassement du tableau
//       for(int j = i; j <=(RemoteCount-2); j++)
// 	Remote[j]	= Remote[j+1];
//       RemoteCount--;	 
//     } 
//   else
//     cout << "Acquisition de " << theObject << " déjà éffectué" << endl;
				
}


//---------------------------------------------------------------------------
// attributeOwnershipDivestitureNotification
//---------------------------------------------------------------------------

void Fed::attributeOwnershipDivestitureNotification(
						     ObjectHandle theObject, // supplied C1
						     const AttributeHandleSet& releasedAttributes) // supplied C4
  throw(
	 ObjectNotKnown,
	 AttributeNotKnown,
	 AttributeNotOwned,
	 AttributeDivestitureWasNotRequested,
	 FederateInternalError)
{
//   cout << "attributeOwnershipDivestitureNotification..." << endl;
//   if( RemoteCount < 20 )
//     {
//       cout << "Cession effectuée !" << endl; 
//       Je_Gere_Boule = RTI_FALSE;
//       Remote[RemoteCount]=(CBille)Local;
//       Local.ID=0;
//       RemoteCount++;
//     }
//   else
//     {
//       cout << "Tableau plein " << endl;
//     }		
//   Don_En_Cours= RTI_FALSE;		 
}


//---------------------------------------------------------------------------
// requestAttributeOwnershipAssumption
//---------------------------------------------------------------------------

void Fed::requestAttributeOwnershipAssumption(
					       ObjectHandle theObject, // supplied C1
					       const AttributeHandleSet& offeredAttributes, // supplied C4
					       const char *theTag) // supplied C4
  throw(
	 ObjectNotKnown,
	 AttributeNotKnown,
	 AttributeAlreadyOwned,
	 AttributeNotPublished,
	 FederateInternalError)
{
//   cout << "requestAttributeOwnershipAssumption..." << endl;
	 
//   //Attributs offerts
	
//   //	cout << "Nb d'attributs dans l'AttributeHandleSet : " << offeredAttributes.size() << endl;
	
//   for(int j=0 ; j < offeredAttributes.size() ; j++)
//     {
 
//       AttributeHandle handle = offeredAttributes.getHandle(j) ;
 
//       if(handle == AttrXID)
// 	{ 
// 	  cout << "AttrXID disponible" << endl;	
// 	}
//       else if(handle == AttrYID)
// 	{
// 	  cout << "AttrYID disponible" << endl;	
// 	}
//       else if(handle == Privilege_To_Delete)
// 	{
// 	  cout << "Privilege_To_Delete disponible" << endl;	
// 	}
//       else
// 	cout << endl << "Unrecognized handle" << endl ;
//     }
//   if(!Je_Gere_Boule)
//     {			 
//       Boule_Disponible = RTI_TRUE;
//       ObjectOffert = theObject;
//     }
}

//---------------------------------------------------------------------------
// timeAdvanceGrant
//---------------------------------------------------------------------------

// void Fed::timeAdvanceGrant(
// 			   const FedTime& theTime) 
//   throw(
// 	 InvalidFederationTime,
// 	 TimeAdvanceWasNotInProgress,
// 	 FederateInternalError)
// {
//   Granted = RTI_TRUE;
//   // cout << endl << "CALLBACK : timeAdvanceGrant, time = " <<((RTIfedTime&)theTime).getTime() << endl ;
// }

void Fed::confirmAttributeOwnershipAcquisitionCancellation(
							    ObjectHandle theObject, // supplied C1
							    const AttributeHandleSet& theAttributes) // supplied C4
  throw(
	 ObjectNotKnown,
	 AttributeNotKnown,
	 AttributeAlreadyOwned,
	 AttributeAcquisitionWasNotCanceled,
	 FederateInternalError)
{

//   cout << " confirmAttributeOwnershipAcquisitionCancellation de : " << theObject << endl;

//   int i;

//   //Attributs dont l'acquisition a été obtenue
	
//   for(int j=0 ; j < theAttributes.size() ; j++)
//     {
 
//       AttributeHandle handle = theAttributes.getHandle(j) ;
 
//       if(handle == AttrXID)
// 	{ 
// 	  cout << "Annulation de AttrXID" << endl;	
// 	}
//       else if(handle == AttrYID)
// 	{
// 	  cout << "Annulation de AttrYID" << endl;	
// 	}
//       else if(handle == Privilege_To_Delete)
// 	{
// 	  cout << "Annulation de Privilege_To_Delete" << endl;	
// 	}
//       else
// 	cout << endl << "Unrecognized handle" << endl ;
//     }
				
				
}

// EOF $Id: my_fed.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $
