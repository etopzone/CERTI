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
// $Id: ObjectClassSet.hh,v 3.23 2007/10/16 09:28:21 erk Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_OBJECT_CLASS_SET_HH
#define _CERTI_OBJECT_CLASS_SET_HH

#include "ObjectClass.hh"
#include "SecurityServer.hh"

#include <list>
#include <string>
#include <map>

namespace certi {

/*! Class ObjectClassSet, qui est la racine de l'arborescence des
  classes d'objets.
*/
class CERTI_EXPORT ObjectClassSet 
{

public:
    ObjectClassSet(SecurityServer *theSecurityServer);
    ~ObjectClassSet();

    void addClass(ObjectClass *theClass);
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
                 AttributeHandle *theAttributeList,
                 UShort theListSize,
                 bool PubOrUnpub)
        throw (ObjectClassNotDefined, AttributeNotDefined, RTIinternalError,
               SecurityError);

    void subscribe(FederateHandle, ObjectClassHandle, AttributeHandle *,
                   int theListSize, const RTIRegion * = 0)
        throw (ObjectClassNotDefined, AttributeNotDefined, RTIinternalError,
               SecurityError);

    // Object Instance Management
    void deleteObject(FederateHandle theFederateHandle,
                      ObjectHandle theObjectHandle,
                      const char *theTag)
        throw (DeletePrivilegeNotHeld, ObjectNotKnown, RTIinternalError);

    void registerObjectInstance(FederateHandle, Object *, ObjectClassHandle)
        throw (InvalidObjectHandle, ObjectClassNotDefined,
               ObjectClassNotPublished, ObjectAlreadyRegistered,
               RTIinternalError);

    void updateAttributeValues(FederateHandle theFederateHandle,
                               ObjectHandle theObjectHandle,
                               AttributeHandle *theAttribArray,
                               ValueLengthPair *theValueArray,
                               UShort theArraySize,
                               FederationTime theTime,
                               const char *theUserTag)
        throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
               RTIinternalError, InvalidObjectHandle);

    void updateAttributeValues(FederateHandle theFederateHandle,
                               ObjectHandle theObjectHandle,
                               AttributeHandle *theAttribArray,
                               ValueLengthPair *theValueArray,
                               UShort theArraySize,
                               const char *theUserTag)
        throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
               RTIinternalError, InvalidObjectHandle);


    // Ownership Management
    void negotiatedAttributeOwnershipDivestiture(FederateHandle,
                                                 ObjectHandle theObjectHandle,
                                                 AttributeHandle *,
                                                 UShort theListSize,
                                                 const char *theTag)
        throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
               AttributeAlreadyBeingDivested, RTIinternalError);


    void attributeOwnershipAcquisitionIfAvailable(FederateHandle,
                                                  ObjectHandle theObjectHandle,
                                                  AttributeHandle*,
                                                  UShort theListSize)
        throw (ObjectNotKnown, ObjectClassNotPublished, AttributeNotDefined,
               AttributeNotPublished, FederateOwnsAttributes,
               AttributeAlreadyBeingAcquired, RTIinternalError);

    void unconditionalAttributeOwnershipDivestiture(FederateHandle,
                                                    ObjectHandle,
                                                    AttributeHandle*,
                                                    UShort theListSize)
        throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
               RTIinternalError);

    void attributeOwnershipAcquisition(FederateHandle theFederateHandle,
                                       ObjectHandle theObjectHandle,
                                       AttributeHandle *theAttributeList,
                                       UShort theListSize,
                                       const char *theTag)
        throw (ObjectNotKnown, ObjectClassNotPublished, AttributeNotDefined,
               AttributeNotPublished, FederateOwnsAttributes, RTIinternalError);

    AttributeHandleSet *attributeOwnershipReleaseResponse(FederateHandle,
                                                          ObjectHandle,
                                                          AttributeHandle *,
                                                          UShort)
        throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
               FederateWasNotAskedToReleaseAttribute, RTIinternalError);

    void cancelAttributeOwnershipAcquisition(FederateHandle theFederateHandle,
                                             ObjectHandle theObjectHandle,
                                             AttributeHandle *theAttributeList,
                                             UShort theListSize)
        throw (ObjectNotKnown, AttributeNotDefined, AttributeAlreadyOwned,
               AttributeAcquisitionWasNotRequested, RTIinternalError);

    Object *getObject(ObjectHandle) const throw (ObjectNotKnown);

private:
	typedef std::map<ObjectClassHandle,ObjectClass*,std::less<ObjectClassHandle> > Handle2ObjectClassMap_t;
	typedef std::map<std::string,ObjectClass*,std::less<std::string> > Name2ObjectClassMap_t; 
	typedef Handle2ObjectClassMap_t::const_iterator handledOC_const_iterator; 
	typedef Name2ObjectClassMap_t::const_iterator namedOC_const_iterator;
	Handle2ObjectClassMap_t OCFromHandle;	
	Name2ObjectClassMap_t OCFromName;
	
	ObjectClassHandle
	getFlatObjectClassHandle(std::string class_name) const throw (NameNotFound);
    /*! This object will help to find the TCPLink associated with a Federate.
      This reference is passed to all new ObjectClass.
    */
    SecurityServer *server ;

    ObjectClass *getInstanceClass(ObjectHandle theObjectHandle) const
        throw (ObjectNotKnown);
};

} // namespace certi

#endif // _CERTI_OBJECT_CLASS_SET_HH

// $Id: ObjectClassSet.hh,v 3.23 2007/10/16 09:28:21 erk Exp $
