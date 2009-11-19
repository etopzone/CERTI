// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: ObjectClassSet.hh,v 3.37 2009/11/19 18:15:31 erk Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_OBJECT_CLASS_SET_HH
#define _CERTI_OBJECT_CLASS_SET_HH

// forward declaration
namespace certi {
class Object;
class SecurityServer;
}  // namespace certi

// CERTI headers
#include "certi.hh"
#include "ObjectClass.hh"
#include "TreeNamedAndHandledSet.hh"

// System headers
#include <list>
#include <string>
#include <map>

namespace certi {

/**
 * This class represents a set of object classes.
 */
class CERTI_EXPORT ObjectClassSet : public TreeNamedAndHandledSet<ObjectClass>
{

public:
	ObjectClassSet(SecurityServer *theSecurityServer, bool isRootClassSet=false);
	~ObjectClassSet();

	/**
	 * Add an object class to the set.
	 * @param[in] theClass the object class to be added
	 * @param[in] parentClass the parent class (may be NULL)
	 */
	void addClass(ObjectClass *theClass,ObjectClass *parentClass) throw (RTIinternalError);

	// RTI Support Services
	AttributeHandle getAttributeHandle(const std::string& the_name,
			ObjectClassHandle the_class) const
			throw (NameNotFound, ObjectClassNotDefined, RTIinternalError);

	const std::string& getAttributeName(AttributeHandle the_handle,
			ObjectClassHandle the_class) const
			throw (AttributeNotDefined, ObjectClassNotDefined, RTIinternalError);

	ObjectClassHandle getObjectClassHandle(const std::string&) const
	throw (NameNotFound);

	const std::string& getObjectClassName(ObjectClassHandle the_handle) const
	throw (ObjectClassNotDefined);

	void killFederate(FederateHandle theFederate)
	throw ();

	// Object Class Management
	void publish(FederateHandle theFederateHandle,
			ObjectClassHandle theClassHandle,
			std::vector <AttributeHandle> &theAttributeList,
			UShort theListSize,
			bool PubOrUnpub)
	throw (ObjectClassNotDefined, AttributeNotDefined, RTIinternalError,
			SecurityError);

	void subscribe(FederateHandle, ObjectClassHandle, std::vector <AttributeHandle> &,
			int theListSize, const RTIRegion * = 0)
	throw (ObjectClassNotDefined, AttributeNotDefined, RTIinternalError,
			SecurityError);

	// Object Instance Management
	void deleteObject(FederateHandle theFederateHandle,
			ObjectHandle theObjectHandle,
			FederationTime theTime,
			const std::string& theTag)
	throw (DeletePrivilegeNotHeld, ObjectNotKnown, RTIinternalError);

	void deleteObject(FederateHandle theFederateHandle,
			ObjectHandle theObjectHandle,
			const std::string& theTag)
	throw (DeletePrivilegeNotHeld, ObjectNotKnown, RTIinternalError);

	void registerObjectInstance(FederateHandle, Object *, ObjectClassHandle)
	throw (InvalidObjectHandle, ObjectClassNotDefined,
			ObjectClassNotPublished, ObjectAlreadyRegistered,
			RTIinternalError);

	void updateAttributeValues(FederateHandle theFederateHandle,
			ObjectHandle theObjectHandle,
			std::vector <AttributeHandle> &theAttribArray,
			std::vector <AttributeValue_t> &theValueArray,
			UShort theArraySize,
			FederationTime theTime,
			const std::string& theUserTag)
	throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
			RTIinternalError, InvalidObjectHandle);

	void updateAttributeValues(FederateHandle theFederateHandle,
			ObjectHandle theObjectHandle,
			std::vector <AttributeHandle> &theAttribArray,
			std::vector <AttributeValue_t> &theValueArray,
			UShort theArraySize,
			const std::string& theUserTag)
	throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
			RTIinternalError, InvalidObjectHandle);


	// Ownership Management
	void negotiatedAttributeOwnershipDivestiture(FederateHandle,
			ObjectHandle theObjectHandle,
			std::vector <AttributeHandle> &,
			UShort theListSize,
			const std::string& theTag)
	throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
			AttributeAlreadyBeingDivested, RTIinternalError);


	void attributeOwnershipAcquisitionIfAvailable(FederateHandle,
			ObjectHandle theObjectHandle,
			std::vector <AttributeHandle>&,
			UShort theListSize)
	throw (ObjectNotKnown, ObjectClassNotPublished, AttributeNotDefined,
			AttributeNotPublished, FederateOwnsAttributes,
			AttributeAlreadyBeingAcquired, RTIinternalError);

	void unconditionalAttributeOwnershipDivestiture(FederateHandle,
			ObjectHandle,
			std::vector <AttributeHandle>&,
			UShort theListSize)
	throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
			RTIinternalError);

	void attributeOwnershipAcquisition(FederateHandle theFederateHandle,
			ObjectHandle theObjectHandle,
			std::vector <AttributeHandle> &theAttributeList,
			UShort theListSize,
			const std::string& theTag)
	throw (ObjectNotKnown, ObjectClassNotPublished, AttributeNotDefined,
			AttributeNotPublished, FederateOwnsAttributes, RTIinternalError);

	AttributeHandleSet *attributeOwnershipReleaseResponse(FederateHandle,
			ObjectHandle,
			std::vector <AttributeHandle> &,
			UShort)
	throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
			FederateWasNotAskedToReleaseAttribute, RTIinternalError);

	void cancelAttributeOwnershipAcquisition(FederateHandle theFederateHandle,
			ObjectHandle theObjectHandle,
			std::vector <AttributeHandle> &theAttributeList,
			UShort theListSize)
	throw (ObjectNotKnown, AttributeNotDefined, AttributeAlreadyOwned,
			AttributeAcquisitionWasNotRequested, RTIinternalError);

	Object *getObject(ObjectHandle) const throw (ObjectNotKnown);

private:

	/**
	 * This object will help to find the TCPLink associated with a Federate.
	 * This reference is passed to all new ObjectClass.
	 */
	SecurityServer *server ;

	ObjectClass *getInstanceClass(ObjectHandle theObjectHandle) const
	throw (ObjectNotKnown);
};

} // namespace certi

#endif // _CERTI_OBJECT_CLASS_SET_HH

// $Id: ObjectClassSet.hh,v 3.37 2009/11/19 18:15:31 erk Exp $
