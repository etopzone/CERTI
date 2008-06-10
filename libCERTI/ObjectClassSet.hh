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
// $Id: ObjectClassSet.hh,v 3.28 2008/06/10 13:41:47 rousse Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_OBJECT_CLASS_SET_HH
#define _CERTI_OBJECT_CLASS_SET_HH

// forward declaration 
namespace certi {
	class Object;
	class ObjectClass;
	class SecurityServer;
}  // namespace certi

// CERTI headers
#include "certi.hh"
#include <list>
#include <string>
#include <map>

namespace certi {

/**
 * This class represents a set of object classes. 
 */
class CERTI_EXPORT ObjectClassSet 
{

public:
    ObjectClassSet(SecurityServer *theSecurityServer, bool isRootClassSet=false);
    ~ObjectClassSet();

    void addClass(ObjectClass *theClass) throw (RTIinternalError);
    void buildParentRelation(ObjectClass *Child, ObjectClass *Parent);
    void display() const ;

    // RTI Support Services
    AttributeHandle getAttributeHandle(const char *the_name,
                                       ObjectClassHandle the_class) const
        throw (NameNotFound, ObjectClassNotDefined, RTIinternalError);

    const char *getAttributeName(AttributeHandle the_handle,
                                 ObjectClassHandle the_class) const
        throw (AttributeNotDefined, ObjectClassNotDefined, RTIinternalError);

    ObjectClassHandle getObjectClassHandle(std::string) const
        throw (NameNotFound);

    std::string getObjectClassName(ObjectClassHandle the_handle) const
        throw (ObjectClassNotDefined, RTIinternalError);

    ObjectClass *getWithHandle(ObjectClassHandle theHandle) const
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
                      std::string theTag)
        throw (DeletePrivilegeNotHeld, ObjectNotKnown, RTIinternalError);

    void deleteObject(FederateHandle theFederateHandle,
                      ObjectHandle theObjectHandle,
                      std::string theTag)
        throw (DeletePrivilegeNotHeld, ObjectNotKnown, RTIinternalError);

    void registerObjectInstance(FederateHandle, Object *, ObjectClassHandle)
        throw (InvalidObjectHandle, ObjectClassNotDefined,
               ObjectClassNotPublished, ObjectAlreadyRegistered,
               RTIinternalError);

    void updateAttributeValues(FederateHandle theFederateHandle,
                               ObjectHandle theObjectHandle,
                               std::vector <AttributeHandle> &theAttribArray,
                               ValueLengthPair *theValueArray,
                               UShort theArraySize,
                               FederationTime theTime,
                               const char *theUserTag)
        throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
               RTIinternalError, InvalidObjectHandle);

    void updateAttributeValues(FederateHandle theFederateHandle,
                               ObjectHandle theObjectHandle,
                               std::vector <AttributeHandle> &theAttribArray,
                               ValueLengthPair *theValueArray,
                               UShort theArraySize,
                               const char *theUserTag)
        throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
               RTIinternalError, InvalidObjectHandle);


    // Ownership Management
    void negotiatedAttributeOwnershipDivestiture(FederateHandle,
                                                 ObjectHandle theObjectHandle,
                                                 std::vector <AttributeHandle> &,
                                                 UShort theListSize,
                                                 const char *theTag)
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
                                       const char *theTag)
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


	typedef std::map<ObjectClassHandle,ObjectClass*,std::less<ObjectClassHandle> > Handle2ObjectClassMap_t;
	typedef std::map<std::string,ObjectClass*,std::less<std::string> > Name2ObjectClassMap_t; 
	typedef Handle2ObjectClassMap_t::const_iterator handledOC_const_iterator; 
	typedef Name2ObjectClassMap_t::const_iterator namedOC_const_iterator;
	
	namedOC_const_iterator NamedBegin() const {
		return OCFromName.begin();
	}
	
	namedOC_const_iterator NamedEnd() const {
			return OCFromName.end();
	}
	
	const size_t size() {return OCFromName.size();}
	
private:	
	Handle2ObjectClassMap_t OCFromHandle;	
	Name2ObjectClassMap_t   OCFromName;
	bool                    isRootClassSet;
	
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

// $Id: ObjectClassSet.hh,v 3.28 2008/06/10 13:41:47 rousse Exp $
