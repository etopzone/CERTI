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
// ----------------------------------------------------------------------------

#ifndef _CERTI_OBJECT_CLASS_HH
#define _CERTI_OBJECT_CLASS_HH

// forward declarations
namespace certi {
class    Object;
class    ObjectAttribute;
class    ObjectClass;
class    ObjectClassSet;
class    ObjectClassAttribute;
class    ObjectClassBroadcastList;
}

// CERTI headers
#include "certi.hh"
#include "SecurityServer.hh"
#include "Named.hh"
#include "GAV.hh"
#include "NM_Classes.hh"

// Standard
#include <map>
#include <string>

namespace certi {

/**
 *  OMT object class.
 * It also contains:
 *  <ul>
 *    <li> the set of subclasses. </li>
 *    <li> the instance list from this class. </li>
 *  </ul>
 */
class CERTI_EXPORT ObjectClass : public Named {

public:

	/**
	 * The type of the handle of this class.
	 */
	typedef ObjectClassHandle handle_t;
	/**
	 * The type of the exception to when such object is not found
	 */
	typedef ObjectClassNotDefined ObjectNotDefinedException;

	/**
	 * The type for the attribute by handle map.
	 */
	typedef std::map<AttributeHandle, ObjectClassAttribute*> HandleClassAttributeMap;

	/**
	 * The type for the object instance by handle map.
	 */
	typedef std::map<ObjectHandle, Object*> HandleObjectMap;

	/**
	 * Create an objectClass.
	 * @param[in] name the object class name
	 * @param[in] handle the object class handle value
	 */
	ObjectClass(const std::string& name, ObjectClassHandle handle);

	/**
	 * Destroy an object class.
	 */
	virtual ~ObjectClass();

	void display() const ;

	/**
	 * Get the handle for this object class.
	 * @return the associated object class handle.
	 */
	ObjectClassHandle getHandle() const ;

	/**
	 * Get the super class handle.
	 * @return the super class handle
	 */
	ObjectClassHandle getSuperclass() const { return superClass ; }

	/**
	 * Add a subclass to this object class.
	 * @param[in] child the new child object class
	 */
	void addSubClass(ObjectClass *child);

	/**
	 * Retrieve a sub class by its name.
	 * @param[in] subClassName the name of the subclass
	 * @return the sub class object class.
	 */
	ObjectClass* getSubClassByName(const std::string& subClassName);
	/**
	 * Get the whole set of subclasses.
	 */
	ObjectClassSet* getSubClasses() {return subClasses;}

	/**
	 *  Checks whether if federate is allowed to access the object class
	 *  @throw SecurityError if the Federate is not allowed to access the
	 *         Object Class, and print an Audit message containing reason.
	 */
	void checkFederateAccess(FederateHandle, const std::string& reason)
	throw (SecurityError);

	SecurityLevelID getSecurityLevelId() const { return securityLevelId ; }

	void setSecurityLevelId(SecurityLevelID newLevelID) throw (SecurityError);

	AttributeHandle addAttribute(ObjectClassAttribute *the_attribute,
			bool is_inherited = false);

	// Publication and Subscription
	void publish(FederateHandle theFederateHandle,
			const std::vector <AttributeHandle> &theAttributeList,
			bool PubOrUnpub)
	throw (AttributeNotDefined, RTIinternalError, SecurityError);

    /**
     * Subscribes a federate to some of this class attributes,
     * with a particular region.
     * @param[in] federate Federate to subscribe
     * @param[in] attributes the attribute-handle list
     * @param[in] region Subscription region. Use 0 for default region.
     * @return true if the federate needs to discover objects of this
     * class because of this subscription
     */
    bool subscribe(FederateHandle federate, const std::vector <AttributeHandle>& attributes, const RTIRegion * region)
        throw (AttributeNotDefined, RTIinternalError, SecurityError);

	void unsubscribe(FederateHandle, const RTIRegion *);
	void unsubscribe(FederateHandle);

	// Ownership Management
	ObjectClassBroadcastList *
	negotiatedAttributeOwnershipDivestiture(FederateHandle theFederateHandle,
                        Object* object,
			const std::vector <AttributeHandle> &theAttributeList,
			const std::string& theTag)
			throw (AttributeNotDefined, AttributeNotOwned,
					AttributeAlreadyBeingDivested, RTIinternalError);


	void
	attributeOwnershipAcquisitionIfAvailable(FederateHandle theFederateHandle,
			Object* object,
			const std::vector <AttributeHandle> &theAttributeList)
	throw (ObjectClassNotPublished, AttributeNotDefined,
			AttributeNotPublished, FederateOwnsAttributes,
			AttributeAlreadyBeingAcquired, RTIinternalError);

	ObjectClassBroadcastList *
	unconditionalAttributeOwnershipDivestiture(FederateHandle,
			Object* object,
			const std::vector <AttributeHandle> &)
			throw (AttributeNotDefined, AttributeNotOwned,
					RTIinternalError);

	void
	attributeOwnershipAcquisition(FederateHandle theFederateHandle,
			Object* object,
			const std::vector <AttributeHandle> &theAttributeList,
			const std::string& theTag)
	throw (ObjectClassNotPublished, AttributeNotDefined,
			AttributeNotPublished, FederateOwnsAttributes, RTIinternalError);

	AttributeHandleSet *
	attributeOwnershipReleaseResponse(FederateHandle theFederateHandle,
			Object* object,
			const std::vector <AttributeHandle> &theAttributeList)
			throw (AttributeNotDefined, AttributeNotOwned,
					FederateWasNotAskedToReleaseAttribute, RTIinternalError);

	void cancelAttributeOwnershipAcquisition(FederateHandle theFederateHandle,
			Object* object,
			const std::vector <AttributeHandle> &theAttributeList)
	throw (AttributeNotDefined, AttributeAlreadyOwned,
			AttributeAcquisitionWasNotRequested, RTIinternalError);

	// RTI Support Services
	AttributeHandle getAttributeHandle(const std::string& theName) const
	throw (NameNotFound, RTIinternalError);

	const std::string& getAttributeName(AttributeHandle theHandle) const
	throw (AttributeNotDefined, RTIinternalError);

	ObjectClassBroadcastList *killFederate(FederateHandle theFederate)
	throw ();

	ObjectClassAttribute *getAttribute(AttributeHandle the_handle) const
	throw (AttributeNotDefined);

	bool hasAttribute(AttributeHandle theHandle) const;

	// Instance Management
	ObjectClassBroadcastList *deleteInstance(FederateHandle theFederateHandle,
			Object* object,
			const FederationTime& theTime,
			const std::string& theUserTag)
	throw (DeletePrivilegeNotHeld,
			ObjectNotKnown,
			RTIinternalError);

	ObjectClassBroadcastList *deleteInstance(FederateHandle theFederateHandle,
			Object* object,
			const std::string& theUserTag)
	throw (DeletePrivilegeNotHeld,
			ObjectNotKnown,
			RTIinternalError);

	bool isInstanceInClass(ObjectHandle theID);

	ObjectClassBroadcastList *
	registerObjectInstance(FederateHandle, Object *, ObjectClassHandle)
	throw (ObjectClassNotPublished, ObjectAlreadyRegistered,
			RTIinternalError);

	void broadcastClassMessage(ObjectClassBroadcastList *ocb_list,
			const Object * = NULL);

	ObjectClassBroadcastList *
	updateAttributeValues(FederateHandle, Object *, const std::vector <AttributeHandle> &,
			const std::vector <AttributeValue_t> &, int, FederationTime, const std::string&)
			throw (AttributeNotDefined, AttributeNotOwned,
					RTIinternalError, InvalidObjectHandle);

	ObjectClassBroadcastList *
	updateAttributeValues(FederateHandle, Object *, const std::vector <AttributeHandle> &,
			const std::vector <AttributeValue_t> &, int, const std::string&)
			throw (AttributeNotDefined, AttributeNotOwned,
					RTIinternalError, InvalidObjectHandle);

	void recursiveDiscovering(FederateHandle, ObjectClassHandle)
	throw (ObjectClassNotDefined);

        /**
         * Getter for the attribute list of the object class.
         * param[out] AttributeList_t @see ObjectClass::AttributeList_t
         */
	const HandleClassAttributeMap& getHandleClassAttributeMap(void) const
        { return _handleClassAttributeMap; }

	//! This Object help to find a TCPLink from a Federate Handle.
	SecurityServer *server ;

	const HandleObjectMap& getClassInstances(void) const
		{ return _handleObjectMap; }
private:
	/*
	 * private default constructor with no code
	 * one should not call it.
	 */
	ObjectClass();

	void addInheritedClassAttributes(ObjectClass *child);

	void sendToFederate(NetworkMessage *msg, FederateHandle theFederate);

	/**
	 * Simple private inner-class.
	 */
	class  DiffusionPair {
	public:
	       DiffusionPair(FederateHandle federate, AttributeHandle attribute)
	       {this->federate=federate; this->attribute=attribute;}
	       FederateHandle federate ;
	       AttributeHandle attribute ;
	};

	typedef std::vector<DiffusionPair> CDiffusion;

	/**
	 * Send the message (msg) to owners using the diffusion list.
	 */
	void sendToOwners(CDiffusion& diffusionList,
			Object* object,
			FederateHandle theFederate,
			const std::string& theTag,
			NM_Attribute_Ownership_Base& msg);

	void sendMessage(NetworkMessage *msg, FederateHandle theDest);

	bool isFederatePublisher(FederateHandle the_federate) const ;
	bool isSubscribed(FederateHandle) const ;

	// The second parameter is the Class of whose behalf the message
	// are sent. If SDM is called on the original class, the Federate
	// may be a subscriber of the class without stopping the
	// process(because he has just subscribed)
	//
	// Return RTI_TRUE if the same SendDiscoverMessages method must be called
	// on the child classes of this class.
	// Return RTI_FALSE if no messages were sent because the Federate had
	// already receive DO messages for this class(and all child classes).
	bool sendDiscoverMessages(FederateHandle, ObjectClassHandle);

	// Attributes
	const ObjectClassHandle handle ;

	/**
	 * greatest subscriber handle
	 */
	FederateHandle maxSubscriberHandle ;

	/**
	 * The security level ID attached to this object class.
	 * default level for non inherited attributes.
	 */
	SecurityLevelID securityLevelId ;

    /**
     * All attributes, indexed by handle.
     */
    HandleClassAttributeMap _handleClassAttributeMap;

    /**
     * All objects of this class, indexed by handle.
     */
	HandleObjectMap _handleObjectMap;

	/**
	 * The super class handle.
	 * 0 if they aren't any.
	 */
	ObjectClassHandle superClass;

	/**
	 * The set of object classes sub classes of this object class
	 */
	ObjectClassSet*   subClasses;

	/* The message buffer used to send Network messages */
	libhla::MessageBuffer NM_msgBufSend;
};

} // namespace certi

#endif // _CERTI_OBJECT_CLASS_HH
