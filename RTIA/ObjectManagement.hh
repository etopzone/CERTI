// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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
// $Id: ObjectManagement.hh,v 3.8 2003/10/13 09:57:56 breholee Exp $
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

class ObjectManagement
{

public:
    ObjectManagement(Communications *, FederationManagement *, RootObject *);
    ~ObjectManagement();

    void requestID(ObjectHandlecount idCount,
                   ObjectHandle &firstID,
                   ObjectHandle &lastID,
                   TypeException &e);

    // Object Management services
    ObjectHandle registerObject(ObjectClassHandle theClassHandle,
                                const char *theObjectName,
                                FederationTime date,
                                FederationTime heure,
                                TypeException &e);

    EventRetractionHandle
    updateAttributeValues(ObjectHandle theObjectHandle,
                          AttributeHandle *attribArray,
                          AttributeValue *valueArray,
                          UShort attribArraySize,
                          FederationTime theTime,
                          const char *theTag,
                          TypeException &e);

    void discoverObject(ObjectHandle theObjectHandle,
                        ObjectClassHandle theObjectClassHandle,
                        const char *theObjectName,
                        FederationTime theTime,
                        EventRetractionHandle theHandle,
                        TypeException &e);

    void reflectAttributeValues(ObjectHandle theObjectHandle,
                                AttributeHandle *attribArray,
                                AttributeValue *valueArray,
                                UShort attribArraySize,
                                FederationTime theTime,
                                const char *theTag,
                                EventRetractionHandle theHandle,
                                TypeException &e);

    EventRetractionHandle
    sendInteraction(InteractionClassHandle theInteraction,
                    ParameterHandle *paramArray,
                    ParameterValue *valueArray,
                    UShort paramArraySize,
                    FederationTime theTime,
                    const char *theTag,
                    TypeException &e);

    void receiveInteraction(InteractionClassHandle theInteraction,
                            ParameterHandle *paramArray,
                            ParameterValue *valueArray,
                            UShort paramArraySize,
                            FederationTime theTime,
                            const char *theTag,
                            EventRetractionHandle theHandle,
                            TypeException &e);

    EventRetractionHandle deleteObject(ObjectHandle theObjectHandle,
                                       const char *theTag,
                                       TypeException &e);

    void removeObject(ObjectHandle theObjectHandle,
                      FederateHandle theFederateHandle,
                      const char *theTag,
                      EventRetractionHandle theHandle,
                      TypeException &e);

    void removeObject(ObjectHandle theObject,
                      ObjectRemovalReason theReason,
                      TypeException &e);

    EventRetractionHandle
    changeAttributeTransportType(ObjectHandle theObjectHandle,
                                 AttributeHandle *attribArray,
                                 UShort attribArraySize,
                                 TransportType theType,
                                 TypeException &e);

    EventRetractionHandle
    changeAttributeOrderType(ObjectHandle theObjectHandle,
                             AttributeHandle *attribArray,
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

    EventRetractionHandle
    requestObjectAttributeValueUpdate(ObjectHandle theObjectHandle,
                                      AttributeHandle *attribArray,
                                      UShort attribArraySize,
                                      TypeException &e);

    void provideAttributeValueUpdate(ObjectHandle theObject,
                                     AttributeValue &theAttributes,
                                     TypeException &e);

    void retract(EventRetractionHandle theHandle, TypeException &e);

    void reflectRetraction(EventRetractionHandle theHandle,
                           TypeException &e);

    // RTI Support Services
    ObjectClassHandle getObjectClassHandle(const char *theName);
    const char *getObjectClassName(ObjectClassHandle theHandle);

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

protected:
    Communications *comm ;
    Queues *queues ;
    FederationManagement *fm ;
    RootObject *rootObject ;
};

}} // namespace certi/rtia

#endif // _CERTI_RTIA_OM

// $Id: ObjectManagement.hh,v 3.8 2003/10/13 09:57:56 breholee Exp $
