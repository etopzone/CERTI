// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
//
// This file is part of CERTI
//
// CERTI is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// $Id: ObjectManagement.hh,v 3.39 2011/03/25 20:53:54 erk Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_RTIA_OM
#define _CERTI_RTIA_OM

#include <libCERTI/RootObject.hh>

namespace certi {
namespace rtia {

class Communications;
class Queues;
class FederationManagement;
class TimeManagement;

class ObjectManagement {
public:
    ObjectManagement(Communications* GC, FederationManagement* GF, RootObject* theRootObj);
    ~ObjectManagement();

    // Object Management services
    void reserveObjectName(const std::string& newObjName, Exception::Type& e);

    ObjectHandle registerObject(ObjectClassHandle the_class,
                                const std::string& theObjectName,
                                FederationTime,
                                FederationTime,
                                Exception::Type& e);

    /** updateAttributeValues with time
     *    @param theObjectHandle Object handle
     *    @param attribArray attribute handles array (pointer)
     *    @param valueArray value array (pointer)
     *    @param attribArraySize attribute and value array size
     *    @param theTime time of the federation
     *    @param theTag user tag (pointer)
     *    @param e exception address (may be modified)
     */
    EventRetractionHandle updateAttributeValues(ObjectHandle theObjectHandle,
                                                const std::vector<AttributeHandle>& attribArray,
                                                const std::vector<AttributeValue_t>& valueArray,
                                                uint32_t attribArraySize,
                                                FederationTime theTime,
                                                const std::string& theTag,
                                                Exception::Type& e);

    /** updateAttributeValues without time
     *    @param theObjectHandle Object handle
     *    @param attribArray attribute handles array (pointer)
     *    @param valueArray value array (pointer)
     *    @param attribArraySize attribute and value array size
     *    @param theTag user tag (pointer)
     *    @param e exception address (may be modified)
     */
    void updateAttributeValues(ObjectHandle theObjectHandle,
                               const std::vector<AttributeHandle>& attribArray,
                               const std::vector<AttributeValue_t>& valueArray,
                               uint32_t attribArraySize,
                               const std::string& theTag,
                               Exception::Type& e);

    void discoverObject(ObjectHandle the_object,
                        ObjectClassHandle the_class,
                        const std::string& the_name,
                        FederationTime the_time,
                        EventRetractionHandle the_event,
                        Exception::Type& e);

    void reflectAttributeValues(ObjectHandle the_object,
                                const std::vector<AttributeHandle>& the_attributes,
                                const std::vector<AttributeValue_t>& the_values,
                                uint16_t the_size,
                                FederationTime the_time,
                                const std::string& the_tag,
                                EventRetractionHandle the_event,
                                Exception::Type& e);

    void reflectAttributeValues(ObjectHandle the_object,
                                const std::vector<AttributeHandle>& the_attributes,
                                const std::vector<AttributeValue_t>& the_values,
                                uint16_t the_size,
                                const std::string& the_tag,
                                Exception::Type& e);

    EventRetractionHandle sendInteraction(InteractionClassHandle theInteraction,
                                          const std::vector<ParameterHandle>& paramArray,
                                          const std::vector<ParameterValue_t>& valueArray,
                                          uint32_t paramArraySize,
                                          FederationTime theTime,
                                          const std::string& theTag,
                                          RegionHandle region,
                                          Exception::Type& e);

    void sendInteraction(InteractionClassHandle theInteraction,
                         const std::vector<ParameterHandle>& paramArray,
                         const std::vector<ParameterValue_t>& valueArray,
                         uint32_t paramArraySize,
                         const std::string& theTag,
                         RegionHandle region,
                         Exception::Type& e);

    void receiveInteraction(InteractionClassHandle the_interaction,
                            const std::vector<ParameterHandle>& the_parameters,
                            const std::vector<ParameterValue_t>& the_values,
                            uint16_t the_size,
                            FederationTime the_time,
                            const std::string& the_tag,
                            EventRetractionHandle the_event,
                            Exception::Type& e);

    void receiveInteraction(InteractionClassHandle the_interaction,
                            const std::vector<ParameterHandle>& the_parameters,
                            const std::vector<ParameterValue_t>& the_values,
                            uint16_t the_size,
                            const std::string& the_tag,
                            Exception::Type& e);

    EventRetractionHandle
    deleteObject(ObjectHandle theObjectHandle, FederationTime theTime, const std::string& theTag, Exception::Type& e);

    void deleteObject(ObjectHandle theObjectHandle, const std::string& theTag, Exception::Type& e);

    /// Delete all objects [instance] owned by this federate.
    void deleteAllObjects(Exception::Type& e);

    void removeObject(ObjectHandle the_object,
                      FederateHandle the_federate,
                      FederationTime theTime,
                      const std::string& the_tag,
                      EventRetractionHandle the_event,
                      Exception::Type& e);

    void
    removeObject(ObjectHandle the_object, FederateHandle the_federate, const std::string& the_tag, Exception::Type& e);

    void removeObject(ObjectHandle theObject, ObjectRemovalReason theReason, Exception::Type& e);

    EventRetractionHandle changeAttributeTransportType(ObjectHandle theObjectHandle,
                                                       const std::vector<AttributeHandle>& attribArray,
                                                       uint32_t attribArraySize,
                                                       TransportType theType,
                                                       Exception::Type& e);

    EventRetractionHandle changeAttributeOrderType(ObjectHandle theObjectHandle,
                                                   const std::vector<AttributeHandle>& attribArray,
                                                   uint32_t attribArraySize,
                                                   OrderType theType,
                                                   Exception::Type& e);

    EventRetractionHandle
    changeInteractionTransportType(InteractionClassHandle id, TransportType theType, Exception::Type& e);

    EventRetractionHandle changeInteractionOrderType(InteractionClassHandle id, OrderType theType, Exception::Type& e);

    void requestObjectAttributeValueUpdate(ObjectHandle handle,
                                           const std::vector<AttributeHandle>& attribs,
                                           uint32_t attribArraySize,
                                           Exception::Type& e);

    void requestClassAttributeValueUpdate(ObjectClassHandle theClass,
                                          const std::vector<AttributeHandle>& attribs,
                                          uint32_t attribArraySize,
                                          Exception::Type& e);

    void provideAttributeValueUpdate(ObjectHandle the_object,
                                     const std::vector<AttributeHandle>& the_attributes,
                                     uint32_t attribArraySize,
                                     Exception::Type&);

    void retract(EventRetractionHandle theHandle, Exception::Type& e);

    void reflectRetraction(EventRetractionHandle, Exception::Type& e);

    /** Transmits the Networkmessage NM_Set_Attribute_Scope_Advisory_Switch to 
     * RTIG. The transmission sets the AttributeScopeAdvisory switch at RTIG 
     * side to the value of the input parameter state. The invocation is caused
     * by a successfull transmission of the message 
     * ENABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH from the federate to RTIA.
     * @param[in] state is the new value for the switch, true means enable,
     * false means disable
     * @param[in,out] e is a reference to a possible exception
     */
    void setAttributeScopeAdvisorySwitch(bool state, Exception::Type& e);

    // 6.13
    void attributesInScope(ObjectHandle theObject,
                           const std::vector<AttributeHandle>& attribArray,
                           const uint16_t attribArraySize,
                           Exception::Type& e);

    // 6.14
    void attributesOutOfScope(ObjectHandle theObject,
                              const std::vector<AttributeHandle>& attribArray,
                              const uint16_t attribArraySize,
                              Exception::Type& e);

    /** Transmits the Networkmessage NM_Set_Attribute_Relevance_Advisory_Switch 
     * to RTIG. The transmission sets the AttributeRelevanceAdvisory switch at 
     * RTIG side to the value of the input parameter state. The invocation is 
     * caused by a successfull transmission of the message 
     * ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH from the federate to RTIA.
     * @param[in] state is the new value for the switch, true means enable, 
     * false means disable
     * @param[in,out] e is a reference to a possible exception
     */
    void setAttributeRelevanceAdvisorySwitch(bool state, Exception::Type& e);

    // 6.17
    void turnUpdatesOnForObjectInstance(ObjectHandle theObject,
                                        const std::vector<AttributeHandle>& attribArray,
                                        const uint16_t attribArraySize,
                                        Exception::Type& e);

    // 6.18
    void turnUpdatesOffForObjectInstance(ObjectHandle theObject,
                                         const std::vector<AttributeHandle>& attribArray,
                                         const uint16_t attribArraySize,
                                         Exception::Type& e);

    // 1516 - 6.3
    void nameReservationSucceeded(const std::string& reservedName);
    void nameReservationFailed(const std::string& reservedName);

    // RTI Support Services
    ObjectClassHandle getObjectClassHandle(const std::string& theName);
    const std::string& getObjectClassName(ObjectClassHandle);

    ObjectHandle getObjectInstanceHandle(const std::string&);
    const std::string& getObjectInstanceName(ObjectHandle);

    AttributeHandle getAttributeHandle(const std::string& theName, ObjectClassHandle theClassHandle);

    const std::string& getAttributeName(AttributeHandle theHandle, ObjectClassHandle theClassHandle);

    InteractionClassHandle getInteractionClassHandle(const std::string& theName);

    const std::string& getInteractionClassName(InteractionClassHandle theClassHandle);

    ParameterHandle getParameterHandle(const std::string& theParameterName, InteractionClassHandle theClassHandle);

    const std::string& getParameterName(ParameterHandle theParameterHandle, InteractionClassHandle theClassHandle);

    ObjectClassHandle getObjectClass(ObjectHandle);

    TransportType getTransportationHandle(const std::string& theName);
    const std::string& getTransportationName(TransportType theType);

    OrderType getOrderingHandle(const std::string& theName);
    const std::string& getOrderingName(OrderType theType);

    TimeManagement* tm;

protected:
    Communications* comm;
    Queues* queues;
    FederationManagement* fm;
    RootObject* rootObject;

private:
    struct TransportTypeList {
        std::string name;
        TransportType type;
    };
    static const TransportTypeList transportTypeList[];

    struct OrderTypeList {
        std::string name;
        OrderType type;
    };
    static const OrderTypeList orderTypeList[];
};
}
} // namespace certi/rtia

#endif // _CERTI_RTIA_OM

// $Id: ObjectManagement.hh,v 3.39 2011/03/25 20:53:54 erk Exp $
