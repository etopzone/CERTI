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
// $Id: OwnershipManagement.cc,v 3.1 2002/11/26 15:48:01 breholee Exp $
// ---------------------------------------------------------------------------

#include "OwnershipManagement.hh"

namespace certi {
namespace rtia {

static pdCDebug D("RTIA_GP", "(RTIA GP ) - ");
// ------------------
// -- Constructeur --
// ------------------

OwnershipManagement::OwnershipManagement(Communications *GC,
					 Queues *GQueues,
					 FederationManagement *GF,
					 RootObject *theRootObj)
{
  _GC = GC;
  _GQueues = GQueues;
  _GF = GF;
  _theRootObj = theRootObj;
}

// -----------------
// -- Destructeur --
// -----------------

OwnershipManagement::~OwnershipManagement()
{
}




// ------------------------------------
// -- . isAttributeOwnedByFederate --
// ------------------------------------

const char * 
OwnershipManagement::attributeOwnedByFederate(ObjectHandle theObject, 
					      AttributeHandle theAttribute,
					      TypeException &e)
{
  NetworkMessage req,rep;
 
  D.Out(pdDebug, 
	"IS_ATTRIBUTE_OWNED_BY_FEDERATE de l'attribut %u de l'objet %u",
	theAttribute, theObject );
 
  req.Type = m_IS_ATTRIBUTE_OWNED_BY_FEDERATE;
  req.NumeroFederation = _GF->_numero_federation;
  req.NumeroFedere = _GF->federate;
  req.Objectid = theObject;
  req.HandleArray[0] = theAttribute;
  req.HandleArraySize = 1;
 
  D.Out(pdDebug,"Demandeur %u ", _GF->federate);
 
  _GC->sendMessage(&req);
 
  //AttendreMsg ne sert que pour verifier que la transmission a ete OK
  _GC->waitMessage(&rep, 
		   m_IS_ATTRIBUTE_OWNED_BY_FEDERATE, 
		   req.NumeroFedere);
 
  e = rep.Exception;
 
  if(e == e_NO_EXCEPTION)
    return(rep.Label); 
  else
    return("\0");
}



// ------------------------------------
// -- . queryAttributeOwnership --
// ------------------------------------

void 
OwnershipManagement::queryAttributeOwnership( ObjectHandle theObject, 
					      AttributeHandle theAttribute,
					      TypeException &e)
{
  NetworkMessage req,rep;
 
  D.Out(pdDebug, 
	"QUERY_ATTRIBUTE_OWNERSHIP de l'attribut %u de l'objet %u",
	theAttribute, theObject );

  req.Type = m_QUERY_ATTRIBUTE_OWNERSHIP;
  req.NumeroFederation = _GF->_numero_federation;
  req.NumeroFedere = _GF->federate;
  req.Objectid = theObject;
  req.HandleArray[0] = theAttribute;
  req.HandleArraySize = 1;

  D.Out(pdDebug,"Demandeur %u ", _GF->federate);
 
  _GC->sendMessage(&req);

  //AttendreMsg ne sert que pour verifier que la transmission a ete OK
  _GC->waitMessage(&rep, m_QUERY_ATTRIBUTE_OWNERSHIP, req.NumeroFedere);
 
  e = rep.Exception;

}

// -----------------------------------------------
// -- . negotiatedAttributeOwnershipDivestiture --
// -----------------------------------------------

void 
OwnershipManagement::
negotiatedAttributeOwnershipDivestiture(ObjectHandle theObject, 
					AttributeHandle *attribArray,
					UShort attribArraySize,
					const char *theTag, 
					TypeException &e)

{
  NetworkMessage req,rep;
 
  req.Type = m_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
  req.NumeroFederation = _GF->_numero_federation;
  req.NumeroFedere = _GF->federate;
  req.Objectid = theObject;
  req.HandleArraySize = attribArraySize;
 
  for(int i = 0;i < attribArraySize;i++) 
    req.HandleArray [i] = attribArray [i];
 
  strcpy(req.Label, theTag);
 
  D.Out(pdDebug,"NEGOTIATED_DIVESTITURE Demandeur %u ", _GF->federate);
 
  _GC->sendMessage(&req);
 
  _GC->waitMessage(&rep, m_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE, 
		   req.NumeroFedere);
 
  e = rep.Exception;

}

// ------------------------------------------------------
// -- . cancelNnegotiatedAttributeOwnershipDivestiture --
// ------------------------------------------------------

void 
OwnershipManagement::
cancelnegotiatedAttributeOwnershipDivestiture(ObjectHandle theObject,
					      AttributeHandle *attribArray,
					      UShort attribArraySize,
					      TypeException &e) 
{
  NetworkMessage req,rep;
 
  req.Type = m_CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
  req.NumeroFederation = _GF->_numero_federation;
  req.NumeroFedere = _GF->federate;
  req.Objectid = theObject;
  req.HandleArraySize = attribArraySize;

  for(int i = 0;i < attribArraySize;i++) 
    req.HandleArray [i] = attribArray [i];
 
  D.Out(pdDebug,"CANCEL_NEGOTIATED_DIVESTITURE Demandeur %u ", 
	_GF->federate);
 
  _GC->sendMessage(&req);
 
  _GC->waitMessage(&rep, 
		   m_CANCEL_NEGOTIATED_ATTRIBUTE_OWNERSHIP_DIVESTITURE,
		   req.NumeroFedere);
 
  e = rep.Exception; 
}


// ------------------------------------------------
// -- . attributeOwnershipAcquisitionIfAvailable --
// ------------------------------------------------

void 
OwnershipManagement::
attributeOwnershipAcquisitionIfAvailable(ObjectHandle theObject,
					 AttributeHandle *attribArray,
					 UShort attribArraySize,
					 TypeException &e)

{
  NetworkMessage req,rep;
 
  req.Type = m_ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE;
  req.NumeroFederation = _GF->_numero_federation;
  req.NumeroFedere = _GF->federate;
  req.Objectid = theObject;
  req.HandleArraySize = attribArraySize;

  for(int i = 0;i < attribArraySize;i++) 
    req.HandleArray [i] = attribArray [i];
 
  D.Out(pdDebug,"AcquisitionIfAvailable Demandeur %u ", _GF->federate);
 
  _GC->sendMessage(&req);

  _GC->waitMessage(&rep, m_ATTRIBUTE_OWNERSHIP_ACQUISITION_IF_AVAILABLE, 
		   req.NumeroFedere);
 
  e = rep.Exception;

}


// --------------------------------------------------
// -- . unconditionalAttributeOwnershipDivestiture --
// --------------------------------------------------

void 
OwnershipManagement::
unconditionalAttributeOwnershipDivestiture(ObjectHandle theObject,
					   AttributeHandle *attribArray,
					   UShort attribArraySize,
					   TypeException &e)
 
{
  NetworkMessage req,rep;
 
  req.Type = m_UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE;
  req.NumeroFederation = _GF->_numero_federation;
  req.NumeroFedere = _GF->federate;
  req.Objectid = theObject;
  req.HandleArraySize = attribArraySize;

  for(int i = 0;i < attribArraySize;i++) 
    req.HandleArray [i] = attribArray [i];
 
  D.Out(pdDebug,"UNCONDITIONAL_DIVESTITURE Demandeur %u ", 
	_GF->federate);
 
  _GC->sendMessage(&req);

  _GC->waitMessage(&rep, m_UNCONDITIONAL_ATTRIBUTE_OWNERSHIP_DIVESTITURE, 
		   req.NumeroFedere);
 
  e = rep.Exception;

}

// -------------------------------------
// -- . attributeOwnershipAcquisition --
// -------------------------------------

void 
OwnershipManagement::
attributeOwnershipAcquisition(ObjectHandle theObject,
			      AttributeHandle *attribArray,
			      UShort attribArraySize,
			      const char *theTag, 
			      TypeException &e)
 
{
  NetworkMessage req,rep;
 

  req.Type = m_ATTRIBUTE_OWNERSHIP_ACQUISITION;
  req.NumeroFederation = _GF->_numero_federation;
  req.NumeroFedere = _GF->federate;
  req.Objectid = theObject;
  req.HandleArraySize = attribArraySize;

  for(int i = 0;i < attribArraySize;i++) 
    req.HandleArray [i] = attribArray [i];
 
  strcpy(req.Label, theTag);
 
  D.Out(pdDebug,"OWNERSHIP_ACQUISITION Demandeur %u ", _GF->federate);
 
  _GC->sendMessage(&req);

  _GC->waitMessage(&rep, m_ATTRIBUTE_OWNERSHIP_ACQUISITION, 
		   req.NumeroFedere);
 
  e = rep.Exception;

}


// ----------------------------------------
// -- attributeOwnershipRealeaseResponse --
// ----------------------------------------

AttributeHandleSet* 
OwnershipManagement::
attributeOwnershipRealeaseResponse(ObjectHandle theObject, 
				   AttributeHandle *attribArray,
				   UShort attribArraySize,
				   TypeException &e)
 
{
  NetworkMessage req,rep;
 
  req.Type = m_ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE;
  req.NumeroFederation = _GF->_numero_federation;
  req.NumeroFedere = _GF->federate; 
  req.Objectid = theObject;
  req.HandleArraySize = attribArraySize;

  D.Out(pdDebug, "RELEASE_RESPONSE Objet %u HandleArraySize %u",
	theObject,req.HandleArraySize);

  for(int i = 0; i < attribArraySize; i++) {
    req.HandleArray [i] = attribArray[i];
  }

 
  _GC->sendMessage(&req);

  _GC->waitMessage(&rep, m_ATTRIBUTE_OWNERSHIP_RELEASE_RESPONSE, 
		   req.NumeroFedere);
 
  e = rep.Exception;
 
  if(e == e_NO_EXCEPTION)
    {
      AttributeHandleSet *AttributeSet;
      AttributeSet=AttributeHandleSetFactory::create(rep.HandleArraySize); 
 
      for(int i = 0 ; i < rep.HandleArraySize; i++)
	{
	  AttributeSet->add(rep.HandleArray[i]);
	}
 
      return(AttributeSet); 
    } 
  else
    return NULL;
}


// -------------------------------------------
// -- . cancelAttributeOwnershipAcquisition --
// -------------------------------------------

void 
OwnershipManagement::
cancelattributeOwnershipAcquisition(ObjectHandle theObject,
				    AttributeHandle *attribArray,
				    UShort attribArraySize,
				    TypeException &e)
 
{
  NetworkMessage req,rep;
 
  req.Type = m_CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION;
  req.NumeroFederation = _GF->_numero_federation;
  req.NumeroFedere = _GF->federate;
  req.Objectid = theObject;
  req.HandleArraySize = attribArraySize;

  for(int i = 0;i < attribArraySize;i++) 
    req.HandleArray [i] = attribArray [i];
 
  D.Out(pdDebug,"CANCEL_ACQUISITION Demandeur %u ", _GF->federate);
 
  _GC->sendMessage(&req);

  _GC->waitMessage(&rep, m_CANCEL_ATTRIBUTE_OWNERSHIP_ACQUISITION, 
		   req.NumeroFedere);
 
  e = rep.Exception;

}


// --------------------------------
// -- . informAttributeOwnership --
// --------------------------------

void 
OwnershipManagement::informAttributeOwnership(ObjectHandle theObject, 
					      AttributeHandle theAttribute,
					      FederateHandle theOwner,
					      TypeException &)
{
  Message req, rep;
 
  req.Type = INFORM_ATTRIBUTE_OWNERSHIP;
  req.Objectid = theObject;
  req.AttribHandle = theAttribute;
  req.NumeroFedere = theOwner;

  _GC->requestFederateService(&req, &rep);

}


// ---------------------------
// -- . attributeIsNotOwned --
// ---------------------------

void 
OwnershipManagement::attributeIsNotOwned(ObjectHandle theObject, 
					 AttributeHandle theAttribute,
					 FederateHandle,
					 TypeException &)

{
  Message req, rep;
 
  req.Type = ATTRIBUTE_IS_NOT_OWNED;
  req.Objectid = theObject;
  req.AttribHandle = theAttribute;
  // req.FederateHandle = theOwner;


  _GC->requestFederateService(&req, &rep);

}


// -----------------------------------
// -- attributeOwnershipUnavailable --
// -----------------------------------

void 
OwnershipManagement::
attributeOwnershipUnavailable(ObjectHandle theObject, 
			      AttributeHandle *attribArray,
			      UShort attribArraySize,
			      FederateHandle,
			      TypeException &)

{
  Message req, rep;
 
  req.Type = ATTRIBUTE_OWNERSHIP_UNAVAILABLE;
  req.Objectid = theObject;
 
  req.HandleArraySize = attribArraySize;
  D.Out(pdDebug, "OWNERSHIP_UNAVAILABLE Objet %u HandleArraySize %u",
	theObject,req.HandleArraySize);
  for(int i = 0; i < attribArraySize; i++) {
    req.HandleArray [i] = attribArray[i];
    D.Out(pdDebug, "Objet %u Attribut %u",theObject,req.HandleArray [i]); 
  }

  _GC->requestFederateService(&req, &rep);

}


// -----------------------------------------------
// -- attributeOwnershipAcquisitionNotification --
// -----------------------------------------------

void 
OwnershipManagement::
attributeOwnershipAcquisitionNotification(ObjectHandle theObject, 
					  AttributeHandle *attribArray,
					  UShort attribArraySize,
					  FederateHandle,
					  TypeException &) 
{
  Message req, rep;
 
  req.Type = ATTRIBUTE_OWNERSHIP_ACQUISITION_NOTIFICATION;
  req.Objectid = theObject;
 
  req.HandleArraySize = attribArraySize;
  D.Out(pdDebug, "ACQUISITION_NOTIFICATION Objet %u HandleArraySize %u",
	theObject,req.HandleArraySize);
  for(int i = 0; i < attribArraySize; i++) {
    req.HandleArray [i] = attribArray[i];
    D.Out(pdDebug, "Objet %u Attribut %u",theObject,req.HandleArray [i]); 
  }

  _GC->requestFederateService(&req, &rep);

}

// -----------------------------------------
// -- requestAttributeOwnershipAssumption --
// -----------------------------------------

void 
OwnershipManagement::
requestAttributeOwnershipAssumption(ObjectHandle theObject, 
				    AttributeHandle *attribArray,
				    UShort attribArraySize,
				    FederateHandle,
				    char *theTag,
				    TypeException &)
{
  Message req, rep;
 
  req.Type = REQUEST_ATTRIBUTE_OWNERSHIP_ASSUMPTION;
  req.Objectid = theObject;
 
  req.HandleArraySize = attribArraySize;
  D.Out(pdDebug, "OWNERSHIP_ASSUMPTION Objet %u HandleArraySize %u",
	theObject,req.HandleArraySize);
  for(int i = 0; i < attribArraySize; i++) {
    req.HandleArray [i] = attribArray[i];
    D.Out(pdDebug, "Objet %u Attribut %u",theObject,req.HandleArray [i]); 
  }

  req.setTag(theTag);

  _GC->requestFederateService(&req, &rep);

}

// --------------------------------------
// -- requestAttributeOwnershipRelease --
// --------------------------------------

void 
OwnershipManagement::
requestAttributeOwnershipRelease(ObjectHandle theObject, 
				 AttributeHandle *attribArray,
				 UShort attribArraySize,
				 char *theTag,
				 TypeException &)
{
  Message req, rep;
 
  req.Type = REQUEST_ATTRIBUTE_OWNERSHIP_RELEASE;
  req.Objectid = theObject;
 
  req.HandleArraySize = attribArraySize;
  D.Out(pdDebug, "OWNERSHIP_RELEASE Objet %u HandleArraySize %u",
	theObject,req.HandleArraySize);
  for(int i = 0; i < attribArraySize; i++) {
    req.HandleArray [i] = attribArray[i];
    D.Out(pdDebug, "Objet %u Attribut %u",theObject,req.HandleArray [i]); 
  }

  req.setTag(theTag);

  _GC->requestFederateService(&req, &rep);

}

// -----------------------------------------------
// -- attributeOwnershipDivestitureNotification --
// -----------------------------------------------

void 
OwnershipManagement::
attributeOwnershipDivestitureNotification(ObjectHandle theObject, 
					  AttributeHandle *attribArray,
					  UShort attribArraySize,
					  TypeException &) 
{
  Message req, rep;
 
  req.Type = ATTRIBUTE_OWNERSHIP_DIVESTITURE_NOTIFICATION;
  req.Objectid = theObject;
 
  req.HandleArraySize = attribArraySize;
  D.Out(pdDebug, "DIVESTITURE_NOTIFICATION Objet %u HandleArraySize %u",
	theObject,req.HandleArraySize);
  for(int i = 0; i < attribArraySize; i++) {
    req.HandleArray [i] = attribArray[i];
    D.Out(pdDebug, "Objet %u Attribut %u",theObject,req.HandleArray [i]); 
  }

  _GC->requestFederateService(&req, &rep);

}

// ------------------------------------------------------
// -- confirmattributeOwnershipAcquisitionCancellation --
// ------------------------------------------------------

void 
OwnershipManagement::
confirmAttributeOwnershipAcquisitionCancellation(
						 ObjectHandle theObject, 
						 AttributeHandle *attribArray,
						 UShort attribArraySize,
						 TypeException &) 
{
  Message req, rep;
 
  req.Type = CONFIRM_ATTRIBUTE_OWNERSHIP_ACQUISITION_CANCELLATION;
  req.Objectid = theObject;
 
  req.HandleArraySize = attribArraySize;
  D.Out(pdDebug, 
	"CONFIRM_ACQUISITION_CANCELLATION Objet %u HandleArraySize %u",
	theObject,req.HandleArraySize);
  for(int i = 0; i < attribArraySize; i++) {
    req.HandleArray [i] = attribArray[i];
    D.Out(pdDebug, "Objet %u Attribut %u",theObject,req.HandleArray [i]); 
  }

  _GC->requestFederateService(&req, &rep);

}

}
}

// $Id: OwnershipManagement.cc,v 3.1 2002/11/26 15:48:01 breholee Exp $
