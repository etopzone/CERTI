// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
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
// ----------------------------------------------------------------------------

#ifndef LIBCERTI_ROOT_OBJECT
#define LIBCERTI_ROOT_OBJECT

#include <vector>

#include "HandleManager.hh"
#include "MessageEvent.hh"
#include "NameReservation.hh"
#include "RoutingSpace.hh"
#include "SecurityServer.hh"
#include <include/certi.hh>

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
class NM_Join_Federation_Execution;
class NM_Additional_Fom_Module;

/** The RootObject is literally the "root" object
 * of the HLA object class hierarchy.
 * One RootObject is created for each federation both
 * on the RTIG and on each RTIA instance of corresponding
 * federate which has joined the federation.
 * It contains the object (resp. interaction) classes
 * hierarchy with the attached attributes (resp. parameters).
 * It contains the object instances which has been registered
 * by all federate (if on RTIG) or by local federate (if on RTIA).
 */
class CERTI_EXPORT RootObject {
public:
    /** RootObject constructor.
     * @param[in] security_server the SocketServer proxy which is used to
     *            retrieve the socket link between RTIG and RTIA.
     *            This may be NULL on the RTIA.
     */
    RootObject(SecurityServer* security_server = nullptr, const bool temporary = false);
    
    /** RootObject destructor.
     * Will delete all object or interaction classes.
     */
    ~RootObject();

    /// Print the Root Object tree to the standard output.
    void display() const;

    /// Print the Root Object tree to the standard output.
    void displaySmall() const;
    
    /// Check if we can add a module (this) into a global root object (parameter)
    bool canBeAddedTo(const RootObject& main_root);

    /** Return the security LevelID corresponding to a security level name.
     * @param[in] levelName the security level name
     * @return the LevelID of the level whose name is levelName if
     *                     a security server exists
     *         PublicLevelID if no security server exists.
     */
    SecurityLevelID getSecurityLevelID(const std::string& levelName);

    void registerFederate(const std::string& the_federate, SecurityLevelID the_level_id);

    // Data Distribution Management

    /** Add a routing space.
     *
     * The actual routing space is a copy of the one provided as parameter, and the handle may be modified.
     * \param rs Routing space to add
     */
    void addRoutingSpace(const RoutingSpace& rs);

    RoutingSpace& getRoutingSpace(SpaceHandle);

    SpaceHandle getRoutingSpaceHandle(const std::string&) const;

    const std::string& getRoutingSpaceName(SpaceHandle) const;

    void addRegion(RTIRegion*);

    RegionHandle createRegion(SpaceHandle, unsigned long);

    void deleteRegion(RegionHandle);

    RTIRegion* getRegion(RegionHandle);

    void modifyRegion(RegionHandle, const std::vector<Extent>&);

    // Object Management
    bool reserveObjectInstanceName(FederateHandle the_federate, const std::string& the_object_name);

    Responses registerObjectInstance(FederateHandle, ObjectClassHandle, ObjectHandle, const std::string&);

    FederateHandle requestObjectOwner(FederateHandle theFederateHandle, ObjectHandle theObject);

    Responses deleteObjectInstance(FederateHandle, ObjectHandle, FederationTime theTime, const std::string& theTag);

    Responses deleteObjectInstance(FederateHandle, ObjectHandle, const std::string& theTag);

    Responses killFederate(FederateHandle);

    // Access to elements of the RootObject hierarchy
    ObjectAttribute* getObjectAttribute(ObjectHandle, AttributeHandle);

    ObjectClass* getObjectClass(ObjectClassHandle);

    Interaction* getInteractionClass(InteractionClassHandle);

    Object* getObject(ObjectHandle);

    ObjectClassAttribute* getObjectClassAttribute(ObjectHandle, AttributeHandle);

    void getAllObjectInstancesFromFederate(FederateHandle the_federate, std::vector<ObjectHandle>& handles);

    /**
     * Add an object class to the ObjectRoot
     * @param[in,out] currentOC the object class to be added
     * @param[in,out] parentOC the parent object class of currentOC
     *                      this may be NULL.
     */
    void addObjectClass(ObjectClass* currentOC, ObjectClass* parentOC);

    /**
     * Add an interaction class to the ObjectRoot
     * @param[in] currentIC the interaction class to be added.
     * @param[in] parentIC the parent interaction class of currentIC
     *                      this may be NULL.
     */
    void addInteractionClass(Interaction* currentIC, Interaction* parentIC);
    
    /**
     * Serialize the federate object model into a message buffer.
     */
    void convertToSerializedFOM(NM_Join_Federation_Execution& message);
    
    /**
     * Serialize the federate object model into a message buffer.
     */
    void convertToSerializedFOM(NM_Additional_Fom_Module& message);

    /**
     * Deserialize the federate object model from a message buffer.
     */
    void rebuildFromSerializedFOM(const NM_Join_Federation_Execution& message);

    /**
     * Deserialize the federate object model from a message buffer.
     */
    void rebuildFromSerializedFOM(const NM_Additional_Fom_Module& message);
    
    int getFreeObjectClassHandle();
    int getFreeInteractionClassHandle();
    int getFreeDimensionHandle();
    int getFreeParameterHandle();
    int getFreeSpaceHandle();

private:
    std::vector<RoutingSpace> spaces;
    /**
     * The associated socket server.
     */
    SecurityServer* server;

    // Regions
    std::list<RTIRegion*> regions;
    HandleManager<RegionHandle> regionHandles;
    
public: // FIXME encapsulation

    /**
     * The set of object classes.
     * This is created when parsing the FOM.
     */
    ObjectClassSet* ObjectClasses;

    /**
     * The set of interactions classes found
     * This is created when parsing the FOM.
     */
    InteractionSet* Interactions;

    /**
     * The set of registered object instance.
     */
    ObjectSet* objects;

    /**
     * The set of reserved names.
     */
    NameReservationSet* reservedNames;
    
    int freeObjectClassHandle {0};
    int freeInteractionClassHandle {0};
    int freeDimensionHandle {0};
    int freeParameterHandle {0};
    int freeSpaceHandle {0};
};

} // namespace certi

#endif // LIBCERTI_ROOT_OBJECT
