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
// $Id: ObjectManagement.hh,v 3.24 2008/07/09 13:48:28 erk Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_RTIA_OM
#define _CERTI_RTIA_OM

#include "RootObject.hh"

#define IDRESERVED 4294000000
#define NOTLINKED -1

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
    ObjectHandle registerObject(ObjectClassHandle theClassHandle,
                                const char *theObjectName,
                                FederationTime date,
                                FederationTime heure,
                                TypeException &e);

    EventRetractionHandle
    updateAttributeValues(ObjectHandle theObjectHandle,
                          std::vector<AttributeHandle> &attribArray,
                          std::vector <ValueLengthPair> &valueArray,
                          UShort attribArraySize,
                          FederationTime theTime,
                          std::string theTag,
                          TypeException &e);

    void updateAttributeValues(ObjectHandle theObjectHandle,
                          std::vector<AttributeHandle> &attribArray,
                          std::vector <ValueLengthPair> &valueArray,
                          UShort attribArraySize,
                          std::string theTag,
                          TypeException &e);

    void discoverObject(ObjectHandle theObjectHandle,
                        ObjectClassHandle theObjectClassHandle,
                        const char *theObjectName,
                        FederationTime theTime,
                        EventRetractionHandle theHandle,
                        TypeException &e);

    void reflectAttributeValues(ObjectHandle theObjectHandle,
                                std::vector <AttributeHandle> &attribArray,
                                std::vector <ValueLengthPair> &valueArray,
                                UShort attribArraySize,
                                FederationTime theTime,
                                const char *theTag,
                                EventRetractionHandle theHandle,
                                TypeException &e);

   void reflectAttributeValues(ObjectHandle theObjectHandle,
                                std::vector <AttributeHandle> &attribArray,
                                std::vector <ValueLengthPair> &valueArray,
                                UShort attribArraySize,
                                const char *theTag,
                                TypeException &e);

    EventRetractionHandle
    sendInteraction(InteractionClassHandle theInteraction,
                    std::vector <ParameterHandle> &paramArray,
                    std::vector <ParameterLengthPair> &valueArray,
                    UShort paramArraySize,
                    FederationTime theTime,
                    std::string theTag,
		    RegionHandle,
                    TypeException &e);

    void
    sendInteraction(InteractionClassHandle theInteraction,
                    std::vector <ParameterHandle> &paramArray,
                    std::vector <ParameterLengthPair> &valueArray,
                    UShort paramArraySize,
                    std::string theTag,
		    RegionHandle,
                    TypeException &e);

    void receiveInteraction(InteractionClassHandle theInteraction,
                            std::vector <ParameterHandle> &paramArray,
                            std::vector <ParameterLengthPair> &valueArray,
                            UShort paramArraySize,
                            FederationTime theTime,
                            const char *theTag,
                            EventRetractionHandle theHandle,
                            TypeException &e);

    void receiveInteraction(InteractionClassHandle theInteraction,
                            std::vector <ParameterHandle> &paramArray,
                            std::vector <ParameterLengthPair> &valueArray,
                            UShort paramArraySize,
                            const char *theTag,
                            TypeException &e);

    EventRetractionHandle deleteObject(ObjectHandle theObjectHandle,
    				       FederationTime theTime,	
                                       std::string theTag,
                                       TypeException &e);

    void deleteObject(ObjectHandle theObjectHandle,
    		      std::string theTag,
		      TypeException &e);

    void removeObject(ObjectHandle theObjectHandle,
                      FederateHandle theFederateHandle,
		      FederationTime theTime,
                      const char *theTag,
                      EventRetractionHandle theHandle,
                      TypeException &e);

    void removeObject(ObjectHandle theObjectHandle,
                      FederateHandle theFederateHandle,
                      const char *theTag,
                      TypeException &e);

    void removeObject(ObjectHandle theObject,
                      ObjectRemovalReason theReason,
                      TypeException &e);

    EventRetractionHandle
    changeAttributeTransportType(ObjectHandle theObjectHandle,
                                 std::vector <AttributeHandle> &attribArray,
                                 UShort attribArraySize,
                                 TransportType theType,
                                 TypeException &e);

    EventRetractionHandle
    changeAttributeOrderType(ObjectHandle theObjectHandle,
                             std::vector <AttributeHandle> &attribArray,
                             UShort attribArraySize,
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
                                      std::vector <AttributeHandle> &attribArray,
                                      UShort attribArraySize,
                                      TypeException &e);

    void provideAttributeValueUpdate(ObjectHandle theObject,
                                      std::vector <AttributeHandle> &theAttributes,
                                      UShort attribArraySize,
                                     TypeException &e);

    void retract(EventRetractionHandle theHandle, TypeException &e);

    void reflectRetraction(EventRetractionHandle theHandle,
                           TypeException &e);

    void setAttributeScopeAdvisorySwitch(bool state,
                                         TypeException &e);

    // 6.13
    void 
    attributesInScope(ObjectHandle theObject,
                      const std::vector <AttributeHandle> &attribArray,
                      const UShort attribArraySize,
                      TypeException &e);

    // 6.14
    void 
    attributesOutOfScope(ObjectHandle theObject,
                      const std::vector <AttributeHandle> &attribArray,
                      const UShort attribArraySize,
                      TypeException &e);

    void setAttributeRelevanceAdvisorySwitch(bool state,
                                             TypeException &e);

    // 6.17
    void 
    turnUpdatesOnForObjectInstance(ObjectHandle theObject,
                      const std::vector <AttributeHandle> &attribArray,
		      const UShort attribArraySize,
                      TypeException &e);

    // 6.18
    void 
    turnUpdatesOffForObjectInstance(ObjectHandle theObject,
                      const std::vector <AttributeHandle> &attribArray,
                      const UShort attribArraySize,
                      TypeException &e);

    // RTI Support Services
    ObjectClassHandle getObjectClassHandle(const char *theName);
    std::string getObjectClassName(ObjectClassHandle);

    ObjectHandle getObjectInstanceHandle(const char *);
    const char *getObjectInstanceName(ObjectHandle);

    AttributeHandle getAttributeHandle(const char *theName,
                                       ObjectClassHandle theClassHandle);

    const char *getAttributeName(AttributeHandle theHandle,
                                 ObjectClassHandle theClassHandle);

    InteractionClassHandle getInteractionClassHandle(const char *theName);

    const char *getInteractionClassName(InteractionClassHandle theClassHandle);

    ParameterHandle getParameterHandle(const char *theParameterName,
                                       InteractionClassHandle theClassHandle);

    const char *getParameterName(ParameterHandle theParameterHandle,
                                 InteractionClassHandle theClassHandle);

    ObjectClassHandle getObjectClass(ObjectHandle);

    TimeManagement *tm ;

protected:
    Communications *comm ;
    Queues *queues ;
    FederationManagement *fm ;
    RootObject *rootObject ;

};

}} // namespace certi/rtia

#endif // _CERTI_RTIA_OM

// $Id: ObjectManagement.hh,v 3.24 2008/07/09 13:48:28 erk Exp $
