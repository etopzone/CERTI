// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: InteractionSet.hh,v 3.3 2003/01/17 17:43:11 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_INTERACTION_SET_HH
#define _CERTI_INTERACTION_SET_HH

#include <stdio.h>

#include "Interaction.hh"
#include "SecurityServer.hh"
#include "PrettyDebug.hh"
#include "InteractionBroadcastList.hh"

namespace certi {

class InteractionSet : private List <Interaction *> 
{

public:

  // --------------------
  // -- Public Methods --
  // --------------------

  InteractionSet(SecurityServer *theSecurityServer);

  // Destructor
  ~InteractionSet();

  void addClass(Interaction *theClass);
  // No memory is allocated, please don't free the pointed object!

  // Build a Parent-Child relation between two object class, by setting
  // the Child's Parent handle, and registering the Child in the Parent's
  // SonSet. 
  // Also copy all Parent's Attributes in the Child Class.
  void buildParentRelation(Interaction *Child,
			   Interaction *Parent);

  // Print the Interactions tree to the standard output.
  void display(void);

  // --------------------------
  // -- RTI Support Services --
  // --------------------------

  InteractionClassHandle 
  getInteractionClassHandle(const char* the_name)
      throw(InteractionClassNotDefined,
            RTIinternalError);

  const char* 
  getInteractionClassName(InteractionClassHandle the_handle)
      throw(InteractionClassNotDefined,
            RTIinternalError);

  ParameterHandle 
  getParameterHandle(const char*            the_name,
                     InteractionClassHandle the_class)
      throw(InteractionParameterNotDefined,
            InteractionClassNotDefined,
            RTIinternalError);

  const char*
  getParameterName(ParameterHandle        the_handle,
                   InteractionClassHandle the_class)
      throw(InteractionParameterNotDefined,
            InteractionClassNotDefined,
            RTIinternalError);

  void killFederate(FederateHandle theFederate)
    throw();

  // ----------------------------------
  // -- Interaction Class Management --
  // ----------------------------------

  void publish(FederateHandle theFederateHandle,
	       InteractionClassHandle theInteractionHandle,
	       bool PubOrUnpub)
    throw(FederateNotPublishing,
	  InteractionClassNotDefined,
	  RTIinternalError,
	  SecurityError);

  void subscribe(FederateHandle theFederateHandle,
		 InteractionClassHandle theInteractionHandle,
		 bool SubOrUnsub)
    throw(FederateNotSubscribing,
	  InteractionClassNotDefined,
	  RTIinternalError,
	  SecurityError);


  // -------------------------------------
  // -- Interaction Instance Management --
  // -------------------------------------

  // Return no exception if the Interaction is valid for a SendInteraction,
  // but do not broadcast it.(to be used on the RTIA for pre-checking).
  void isReady(FederateHandle theFederateHandle,
	       InteractionClassHandle theInteraction,
	       ParameterHandle *paramArray,
	       UShort paramArraySize)
    throw(FederateNotPublishing,
	  InteractionClassNotDefined,
	  InteractionParameterNotDefined,
	  RTIinternalError);

  void broadcastInteraction(FederateHandle theFederateHandle,
			    InteractionClassHandle theInteractionHandle,
			    ParameterHandle *theParameterList,
			    ParameterValue *theValueList,
			    UShort theListSize,
			    FederationTime theTime,
			    const char*  theTag)
    throw(FederateNotPublishing,
	  InteractionClassNotDefined,
	  InteractionParameterNotDefined,
	  RTIinternalError);

private:
 
  // ------------------------
  // -- Private Attributes --
  // ------------------------

  // This object will help to find the TCPLink associated with a Federate.
  // This reference is passed to all new ObjectClass.
  SecurityServer *server ;

  // ---------------------
  // -- Private Methods --
  // ---------------------

  Interaction *getByHandle(InteractionClassHandle theHandle)
    throw(InteractionClassNotDefined,
	  RTIinternalError);
 
};

}

#endif // _CERTI_INTERACTION_SET_HH

// $Id: InteractionSet.hh,v 3.3 2003/01/17 17:43:11 breholee Exp $
