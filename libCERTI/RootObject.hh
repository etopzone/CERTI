// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003, 2004  ONERA
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
// $Id: RootObject.hh,v 3.20 2004/01/09 16:17:43 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef LIBCERTI_ROOT_OBJECT
#define LIBCERTI_ROOT_OBJECT

#include "InteractionSet.hh"
#include "ObjectClassSet.hh"
#include "ObjectSet.hh"
#include "RegionImp.hh"
#include "RoutingSpace.hh"
#include "SecurityServer.hh"
#include "HandleManager.hh"

#include <vector>

namespace certi {

class RootObject
{
public:
    RootObject(SecurityServer *);
    ~RootObject();

    void display() const ;

    SecurityLevelID GetSecurityLevelID(const char *the_name);

    void registerFederate(const char *the_federate,
                          SecurityLevelID the_level_id);

    // Data Distribution Management
    void addRoutingSpace(const RoutingSpace &);
    RoutingSpace &getRoutingSpace(SpaceHandle) throw (SpaceNotDefined);
    SpaceHandle getRoutingSpaceHandle(std::string) throw (NameNotFound);
    std::string getRoutingSpaceName(SpaceHandle) throw (SpaceNotDefined);

    void addRegion(RegionImp *);
    RegionHandle createRegion(SpaceHandle, long) throw (SpaceNotDefined);
    void deleteRegion(RegionHandle) throw (RegionNotKnown, RegionInUse);
    RegionImp *getRegion(RegionHandle) throw (RegionNotKnown);
    void modifyRegion(RegionHandle, const std::vector<Extent> &)
	throw (RegionNotKnown, InvalidExtents);

    // Object Management
    void registerObjectInstance(FederateHandle, ObjectClassHandle, ObjectHandle,
                                const char *)
        throw (InvalidObjectHandle,
               ObjectClassNotDefined,
               ObjectClassNotPublished,
               ObjectAlreadyRegistered,
               RTIinternalError);
    
    void deleteObjectInstance(FederateHandle, ObjectHandle, const char *)
        throw (DeletePrivilegeNotHeld, ObjectNotKnown, RTIinternalError);

    void killFederate(FederateHandle) throw (RTIinternalError);

    // Access to elements of the RootObject hierarchy    
    ObjectAttribute *getObjectAttribute(ObjectHandle, AttributeHandle);
    ObjectClass *getObjectClass(ObjectClassHandle);
    Interaction *getInteractionClass(InteractionClassHandle);
    Object *getObject(ObjectHandle);    
    ObjectClassAttribute *getObjectClassAttribute(ObjectHandle,
						  AttributeHandle);
    
    // -- Attributes
    ObjectClassSet *ObjectClasses ;
    InteractionSet *Interactions ;
    ObjectSet *objects ;

private:
    std::vector<RoutingSpace> spaces ;
    SecurityServer *server ;

    // Regions
    std::list<RegionImp *> regions ;
    HandleManager<RegionHandle> regionHandles ;
};

} // namespace certi

#endif // LIBCERTI_ROOT_OBJECT

// $Id: RootObject.hh,v 3.20 2004/01/09 16:17:43 breholee Exp $
