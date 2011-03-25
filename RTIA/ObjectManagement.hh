// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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

#include "RootObject.hh"

namespace certi {
namespace rtia {

class Communications ;
class Queues ;
class FederationManagement ;
class TimeManagement ;

class ObjectManagement
{

public:
    ObjectManagement(Communications *, FederationManagement *, RootObject *);
    ~ObjectManagement();

    // Object Management services
    void reserveObjectName(const std::string &, TypeException &);

    ObjectHandle registerObject(ObjectClassHandle theClassHandle,
                                const std::string& theObjectName,
                                FederationTime date,
                                FederationTime heure,
                                TypeException &e);

    EventRetractionHandle
    updateAttributeValues(ObjectHandle theObjectHandle,
                          const std::vector<AttributeHandle> &attribArray,
                          const std::vector<AttributeValue_t> &valueArray,
                          uint32_t attribArraySize,
                          FederationTime theTime,
                          const std::string& theTag,
                          TypeException &e);

    void updateAttributeValues(ObjectHandle theObjectHandle,
                          const std::vector<AttributeHandle> &attribArray,
                          const std::vector<AttributeValue_t> &valueArray,
                          uint32_t attribArraySize,
                          const std::string& theTag,
                          TypeException &e);

    void discoverObject(ObjectHandle theObjectHandle,
                        ObjectClassHandle theObjectClassHandle,
                        const std::string& theObjectName,
                        FederationTime theTime,
                        EventRetractionHandle theHandle,
                        TypeException &e);

    void reflectAttributeValues(ObjectHandle theObjectHandle,
                                const std::vector <AttributeHandle> &attribArray,
                                const std::vector <AttributeValue_t> &valueArray,
                                uint16_t attribArraySize,
                                FederationTime theTime,
                                const std::string& theTag,
                                EventRetractionHandle theHandle,
                                TypeException &e);

   void reflectAttributeValues(ObjectHandle theObjectHandle,
                                const std::vector <AttributeHandle> &attribArray,
                                const std::vector <AttributeValue_t> &valueArray,
                                uint16_t attribArraySize,
                                const std::string& theTag,
                                TypeException &e);

    EventRetractionHandle
    sendInteraction(InteractionClassHandle theInteraction,
                    const std::vector <ParameterHandle> &paramArray,
                    const std::vector <ParameterValue_t> &valueArray,
                    uint32_t paramArraySize,
                    FederationTime theTime,
                    const std::string& theTag,
		    RegionHandle,
                    TypeException &e);

    void
    sendInteraction(InteractionClassHandle theInteraction,
                    const std::vector <ParameterHandle> &paramArray,
                    const std::vector <ParameterValue_t> &valueArray,
                    uint32_t paramArraySize,
                    const std::string& theTag,
		    RegionHandle,
                    TypeException &e);

    void receiveInteraction(InteractionClassHandle theInteraction,
                            const std::vector <ParameterHandle> &paramArray,
                            const std::vector <ParameterValue_t> &valueArray,
                            uint16_t paramArraySize,
                            FederationTime theTime,
                            const std::string& theTag,
                            EventRetractionHandle theHandle,
                            TypeException &e);

    void receiveInteraction(InteractionClassHandle theInteraction,
                            const std::vector <ParameterHandle> &paramArray,
                            const std::vector <ParameterValue_t> &valueArray,
                            uint16_t paramArraySize,
                            const std::string& theTag,
                            TypeException &e);

    EventRetractionHandle deleteObject(ObjectHandle theObjectHandle,
    				       FederationTime theTime,
                                       const std::string& theTag,
                                       TypeException &e);

    void deleteObject(ObjectHandle theObjectHandle,
    		      const std::string& theTag,
		      TypeException &e);

    /**
     * Delete all objects [instance] owned by this federate.
     */
	void deleteAllObjects(TypeException &e);

    void removeObject(ObjectHandle theObjectHandle,
                      FederateHandle theFederateHandle,
		      FederationTime theTime,
                      const std::string& theTag,
                      EventRetractionHandle theHandle,
                      TypeException &e);

    void removeObject(ObjectHandle theObjectHandle,
                      FederateHandle theFederateHandle,
                      const std::string& theTag,
                      TypeException &e);

    void removeObject(ObjectHandle theObject,
                      ObjectRemovalReason theReason,
                      TypeException &e);

    EventRetractionHandle
    changeAttributeTransportType(ObjectHandle theObjectHandle,
                                 const std::vector <AttributeHandle> &attribArray,
                                 uint32_t attribArraySize,
                                 TransportType theType,
                                 TypeException &e);

    EventRetractionHandle
    changeAttributeOrderType(ObjectHandle theObjectHandle,
                             const std::vector <AttributeHandle> &attribArray,
                             uint32_t attribArraySize,
                             OrderType theType,
                             TypeException &e);

    EventRetractionHandle
    changeInteractionTransportType(InteractionClassHandle theClassID,
                                   TransportType theType,
                                   TypeException &e);

    EventRetractionHandle
    changeInteractionOrderType(InteractionClassHandle theClassID,
                               OrderType theType,
                               TypeException &e);

    void
    requestObjectAttributeValueUpdate(ObjectHandle theObjectHandle,
                                      const std::vector <AttributeHandle> &attribArray,
                                      uint32_t attribArraySize,
                                      TypeException &e);

	void requestClassAttributeValueUpdate(ObjectClassHandle theClass,
										  const std::vector <AttributeHandle> &attribs,
										  uint32_t attribArraySize,
										  TypeException &e);

    void provideAttributeValueUpdate(ObjectHandle theObject,
                                      const std::vector <AttributeHandle> &theAttributes,
                                      uint32_t attribArraySize,
                                     TypeException &e);

    void retract(EventRetractionHandle theHandle, TypeException &e);

    void reflectRetraction(EventRetractionHandle theHandle,
                           TypeException &e);

    /**
     * Transmits the Networkmessage NM_Set_Attribute_Scope_Advisory_Switch to 
     * RTIG. The transmission sets the AttributeScopeAdvisory switch at RTIG 
     * side to the value of the input parameter state. The invocation is caused
     * by a successfull transmission of the message 
     * ENABLE_ATTRIBUTE_SCOPE_ADVISORY_SWITCH from the federate to RTIA.
     * @param[in] state is the new value for the switch, true means enable,
     * false means disable
     * @param[in,out] e is a reference to a possible exception
     */
    void setAttributeScopeAdvisorySwitch(bool state,
                                         TypeException &e);

    // 6.13
    void
    attributesInScope(ObjectHandle theObject,
                      const std::vector <AttributeHandle> &attribArray,
                      const uint16_t attribArraySize,
                      TypeException &e);

    // 6.14
    void
    attributesOutOfScope(ObjectHandle theObject,
                      const std::vector <AttributeHandle> &attribArray,
                      const uint16_t attribArraySize,
                      TypeException &e);

    /**
     * Transmits the Networkmessage NM_Set_Attribute_Relevance_Advisory_Switch 
     * to RTIG. The transmission sets the AttributeRelevanceAdvisory switch at 
     * RTIG side to the value of the input parameter state. The invocation is 
     * caused by a successfull transmission of the message 
     * ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY_SWITCH from the federate to RTIA.
     * @param[in] state is the new value for the switch, true means enable, 
     * false means disable
     * @param[in,out] e is a reference to a possible exception
     */
    void setAttributeRelevanceAdvisorySwitch(bool state,
                                             TypeException &e);

    // 6.17
    void
    turnUpdatesOnForObjectInstance(ObjectHandle theObject,
                      const std::vector <AttributeHandle> &attribArray,
		      const uint16_t attribArraySize,
                      TypeException &e);

    // 6.18
    void
    turnUpdatesOffForObjectInstance(ObjectHandle theObject,
                      const std::vector <AttributeHandle> &attribArray,
                      const uint16_t attribArraySize,
                      TypeException &e);

	// 1516 - 6.3
	void nameReservationSucceeded(const std::string &reservedName);
	void nameReservationFailed(const std::string &reservedName);


    // RTI Support Services
    ObjectClassHandle getObjectClassHandle(const std::string& theName);
    const std::string& getObjectClassName(ObjectClassHandle);

    ObjectHandle getObjectInstanceHandle(const std::string&);
    const std::string& getObjectInstanceName(ObjectHandle);

    AttributeHandle getAttributeHandle(const std::string& theName,
                                       ObjectClassHandle theClassHandle);

    const std::string& getAttributeName(AttributeHandle theHandle,
                                 ObjectClassHandle theClassHandle);

    InteractionClassHandle getInteractionClassHandle(const std::string& theName);

    const std::string& getInteractionClassName(InteractionClassHandle theClassHandle);

    ParameterHandle getParameterHandle(const std::string& theParameterName,
                                       InteractionClassHandle theClassHandle);

    const std::string& getParameterName(ParameterHandle theParameterHandle,
                                 InteractionClassHandle theClassHandle);

    ObjectClassHandle getObjectClass(ObjectHandle);

    TransportType getTransportationHandle(const std::string& theName);
    const std::string& getTransportationName(TransportType theType);

    OrderType getOrderingHandle(const std::string& theName);
    const std::string& getOrderingName(OrderType theType);

    TimeManagement *tm ;

protected:
    Communications *comm ;
    Queues *queues ;
    FederationManagement *fm ;
    RootObject *rootObject ;

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

}} // namespace certi/rtia

#endif // _CERTI_RTIA_OM

// $Id: ObjectManagement.hh,v 3.39 2011/03/25 20:53:54 erk Exp $
