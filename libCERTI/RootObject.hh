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
// $Id: RootObject.hh,v 3.30 2008/10/30 16:01:37 erk Exp $
// ----------------------------------------------------------------------------

#ifndef LIBCERTI_ROOT_OBJECT
#define LIBCERTI_ROOT_OBJECT

// forward declaration
namespace certi {
	class Object;
	class ObjectSet;
	class ObjectAttribute;
	class ObjectClass;
	class ObjectClassSet;
	class ObjectClassAttribute;
	class Interaction;
	class InteractionSet;
	class RTIRegion;
	class RoutingSpace;
}  // namespace certi

#include "certi.hh"
#include "SecurityServer.hh"
#include "HandleManager.hh"
#include "RoutingSpace.hh"

#include <vector>

namespace certi {

/**
 * The RootObject is literally the "root" object
 * of the HLA object class hierarchy.
 * One RootObject is created for each federation both
 * on the RTIG and on each RTIA instance of corresponding
 * federate which has joined the federation.
 * It contains the object (resp. interaction) classes
 * hierarchy with the attached attributes (resp. parameters).
 * It contains the object instances which has been registered
 * by all federate (if on RTIG) or by local federate (if on RTIA).
 */
class CERTI_EXPORT RootObject
{
public:

	/**
	 * RootObject constructor.
	 * @param[in] security_server the SocketServer proxy which is used to
	 *            retrieve the socket link between RTIG and RTIA.
	 *            This may be NULL on the RTIA.
	 */
    RootObject(SecurityServer *security_server);

    /**
     * RootObject destructor.
     * Will delete all object or interaction classes.
     */
    ~RootObject();

    void display() const;

    SecurityLevelID GetSecurityLevelID(const char *the_name);

    void registerFederate(const char *the_federate,
                          SecurityLevelID the_level_id);

    // Data Distribution Management
    void addRoutingSpace(const RoutingSpace &);
    RoutingSpace &getRoutingSpace(SpaceHandle) throw (SpaceNotDefined);
    SpaceHandle getRoutingSpaceHandle(std::string) throw (NameNotFound);
    std::string getRoutingSpaceName(SpaceHandle) throw (SpaceNotDefined);

    void addRegion(RTIRegion *);
    RegionHandle createRegion(SpaceHandle, unsigned long) throw (SpaceNotDefined);
    void deleteRegion(RegionHandle) throw (RegionNotKnown, RegionInUse);
    RTIRegion *getRegion(RegionHandle) throw (RegionNotKnown);
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

    FederateHandle requestObjectOwner(FederateHandle theFederateHandle, ObjectHandle theObject)
        throw (ObjectNotKnown);

    void deleteObjectInstance(FederateHandle, ObjectHandle,
    			      FederationTime theTime, std::string theTag)
        throw (DeletePrivilegeNotHeld, ObjectNotKnown, RTIinternalError);

    void deleteObjectInstance(FederateHandle, ObjectHandle, std::string theTag)
        throw (DeletePrivilegeNotHeld, ObjectNotKnown, RTIinternalError);

    void killFederate(FederateHandle) throw (RTIinternalError);

    // Access to elements of the RootObject hierarchy
    ObjectAttribute *getObjectAttribute(ObjectHandle, AttributeHandle);
    ObjectClass *getObjectClass(ObjectClassHandle);
    Interaction *getInteractionClass(InteractionClassHandle);
    Object *getObject(ObjectHandle);
    ObjectClassAttribute *getObjectClassAttribute(ObjectHandle,
						  AttributeHandle);

    /**
     * Add an object class to the ObjectRoot
     * @param[in,out] currentOC the object class to be added
     * @param[in,out] parentOC the parent object class of currentOC
     *                      this may be NULL.
     */
    void addObjectClass(ObjectClass* currentOC,ObjectClass* parentOC);

    /**
     * Add an interaction class to the ObjectRoot
     * @param[in] currentIC the interaction class to be added.
     */
    void addInteractionClass(Interaction* currentIC);

    /**
     * The set of object classes.
     * This is created when parsing the FOM.
     */
    ObjectClassSet *ObjectClasses;

    /**
     * The set of interactions classes found
     * This is created when parsing the FOM.
     */
    InteractionSet *Interactions;
    /**
     * The set of registered object instance.
     */
    ObjectSet      *objects;

private:

    std::vector<RoutingSpace> spaces;
    /**
     * The associated socket server.
     */
    SecurityServer           *server;

    // Regions
    std::list<RTIRegion *> regions ;
    HandleManager<RegionHandle> regionHandles ;
};

} // namespace certi

#endif // LIBCERTI_ROOT_OBJECT

// $Id: RootObject.hh,v 3.30 2008/10/30 16:01:37 erk Exp $
