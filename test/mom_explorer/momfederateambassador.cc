#include "momfederateambassador.h"

// ostream types

std::wostream& operator<<(std::wostream& os, const FederationExecutionInformation& v);

std::wostream& operator<<(std::wostream& os, const VariableLengthData& v);

std::wostream& operator<<(std::wostream& os, const SupplementalReflectInfo& v);

std::wostream& operator<<(std::wostream& os, const SupplementalReceiveInfo& v);

std::wostream& operator<<(std::wostream& os, const SupplementalRemoveInfo& v);

std::wostream& operator<<(std::wostream& os, const FederateRestoreStatus& v);

// ostream containers

template <class F, class S>
std::wostream& operator<<(std::wostream& os, const std::pair<F, S>& v)
{
    os << "p{ first: " << v.first << ", second: " << v.second << " } ";
    return os;
}

template <class T>
std::wostream& operator<<(std::wostream& os, const std::vector<T>& v)
{
    os << "v{ ";
    for (const auto& element : v) {
        os << element << ", ";
    }
    os << "} ";
    return os;
}

template <class T>
std::wostream& operator<<(std::wostream& os, const std::set<T>& v)
{
    os << "s{ ";
    for (const auto& element : v) {
        os << element << ", ";
    }
    os << "} ";
    return os;
}

template <class K, class V>
std::wostream& operator<<(std::wostream& os, const std::map<K, V>& v)
{
    os << "m{ ";
    for (const auto& element : v) {
        os << element.first << " = " << element << ", ";
    }
    os << "} ";
    return os;
}

std::wostream& operator<<(std::wostream& os, const FederationExecutionInformation& v)
{
    return os << "FEI [name: " << v.federationExecutionName << ", time: " << v.logicalTimeImplementationName << " ] ";
}

std::wostream& operator<<(std::wostream& os, const VariableLengthData& v)
{
    os << "VLD [";
    
    if(v.size() == 0) {
        os << "empty";
    }
    else {
        os << "size: " << v.size() << ", data: [ ";
        for(auto i(0u); i < v.size(); ++i) {
            os << static_cast<const uint8_t*>(v.data())[i] << " ";
        }
        os << "]";
    }
    os << "]";
    return os;
}

std::wostream& operator<<(std::wostream& os, const SupplementalReflectInfo& v)
{
    return os << "SRefI [hasProducingFederate: " << v.hasProducingFederate << ", hasSentRegions: " << v.hasSentRegions
              << ", producingFederate: " << v.producingFederate << ", sentRegions: " << v.sentRegions << " ] ";
}

std::wostream& operator<<(std::wostream& os, const SupplementalReceiveInfo& v)
{
    return os << "SRecI [hasProducingFederate: " << v.hasProducingFederate << ", hasSentRegions: " << v.hasSentRegions
              << ", producingFederate: " << v.producingFederate << ", sentRegions: " << v.sentRegions << " ] ";
}

std::wostream& operator<<(std::wostream& os, const SupplementalRemoveInfo& v)
{
    return os << "SRemI [hasProducingFederate: " << v.hasProducingFederate
              << ", producingFederate: " << v.producingFederate << " ] ";
}

std::wostream& operator<<(std::wostream& os, const FederateRestoreStatus& v)
{
    return os << "FRS [preRestoreHandle: " << v.preRestoreHandle << ", postRestoreHandle: " << v.postRestoreHandle
              << ", status: " << v.status << "] ";
}

/*
std::wostream& operator<<(std::wostream& os, const XXX& v) {
    
    return os;
}
*/

MOMFederateAmbassador::MOMFederateAmbassador(rti1516e::RTIambassador& ambassador) : my_ambassador{ambassador}
{
}

void MOMFederateAmbassador::connectionLost(std::wstring const& faultDescription) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << faultDescription << ">" << std::endl;
}

void MOMFederateAmbassador::reportFederationExecutions(
    FederationExecutionInformationVector const& theFederationExecutionInformationList) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theFederationExecutionInformationList << ">" << std::endl;
}

void MOMFederateAmbassador::synchronizationPointRegistrationSucceeded(std::wstring const& label) throw(
    FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << label << ">" << std::endl;
}

void MOMFederateAmbassador::synchronizationPointRegistrationFailed(
    std::wstring const& label, SynchronizationPointFailureReason reason) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << label << ", " << reason << ">" << std::endl;
}

void MOMFederateAmbassador::announceSynchronizationPoint(
    std::wstring const& label, VariableLengthData const& theUserSuppliedTag) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << label << ", " << theUserSuppliedTag << ">" << std::endl;
}

void MOMFederateAmbassador::federationSynchronized(
    std::wstring const& label, FederateHandleSet const& failedToSyncSet) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << label << ", " << failedToSyncSet << ">" << std::endl;
}

void MOMFederateAmbassador::initiateFederateSave(std::wstring const& label) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << label << ">" << std::endl;
}

void MOMFederateAmbassador::initiateFederateSave(std::wstring const& label,
                                                 LogicalTime const& theTime) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << label << theTime.toString() << ">" << std::endl;
}

void MOMFederateAmbassador::federationSaved() throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << ">" << std::endl;
}

void MOMFederateAmbassador::federationNotSaved(SaveFailureReason theSaveFailureReason) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theSaveFailureReason << ">" << std::endl;
}

void MOMFederateAmbassador::federationSaveStatusResponse(
    FederateHandleSaveStatusPairVector const& theFederateStatusVector) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theFederateStatusVector << ">" << std::endl;
}

void MOMFederateAmbassador::requestFederationRestoreSucceeded(std::wstring const& label) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << label << ">" << std::endl;
}

void MOMFederateAmbassador::requestFederationRestoreFailed(std::wstring const& label) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << label << ">" << std::endl;
}

void MOMFederateAmbassador::federationRestoreBegun() throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << ">" << std::endl;
}

void MOMFederateAmbassador::initiateFederateRestore(std::wstring const& label,
                                                    std::wstring const& federateName,
                                                    FederateHandle handle) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << label << ", " << federateName << ", " << handle << ">" << std::endl;
}

void MOMFederateAmbassador::federationRestored() throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <"
               << ">" << std::endl;
}

void MOMFederateAmbassador::federationNotRestored(RestoreFailureReason theRestoreFailureReason) throw(
    FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theRestoreFailureReason << ">" << std::endl;
}

void MOMFederateAmbassador::federationRestoreStatusResponse(
    FederateRestoreStatusVector const& theFederateRestoreStatusVector) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theFederateRestoreStatusVector << ">" << std::endl;
}

void MOMFederateAmbassador::startRegistrationForObjectClass(ObjectClassHandle theClass) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theClass << ">" << std::endl;
}

void MOMFederateAmbassador::stopRegistrationForObjectClass(ObjectClassHandle theClass) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theClass << ">" << std::endl;
}

void MOMFederateAmbassador::turnInteractionsOn(InteractionClassHandle theHandle) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theHandle << ">" << std::endl;
}

void MOMFederateAmbassador::turnInteractionsOff(InteractionClassHandle theHandle) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theHandle << ">" << std::endl;
}

void MOMFederateAmbassador::objectInstanceNameReservationSucceeded(std::wstring const& theObjectInstanceName) throw(
    FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObjectInstanceName << ">" << std::endl;
}

void MOMFederateAmbassador::objectInstanceNameReservationFailed(std::wstring const& theObjectInstanceName) throw(
    FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObjectInstanceName << ">" << std::endl;
}

void MOMFederateAmbassador::multipleObjectInstanceNameReservationSucceeded(
    std::set<std::wstring> const& theObjectInstanceNames) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObjectInstanceNames << ">" << std::endl;
}

void MOMFederateAmbassador::multipleObjectInstanceNameReservationFailed(
    std::set<std::wstring> const& theObjectInstanceNames) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObjectInstanceNames << ">" << std::endl;
}

void MOMFederateAmbassador::discoverObjectInstance(
    ObjectInstanceHandle theObject,
    ObjectClassHandle theObjectClass,
    std::wstring const& theObjectInstanceName) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObject << ", " << theObjectClass << ", " << theObjectInstanceName << ">"
               << std::endl;

    std::wcout << "=>requestAttributeValueUpdate <" << theObjectInstanceName << ">" << std::endl;

    AttributeHandleSet attributes;
    for (auto attribute : {L"HLAfederationName",
                           L"HLAfederatesInFederation",
                           L"HLARTIversion",
                           L"HLAMIMDesignator",
                           L"HLAFOMmoduleDesignatorList",
                           L"HLAcurrentFDD",
                           L"HLAtimeImplementationName",
                           L"HLAlastSaveName",
                           L"HLAlastSaveTime",
                           L"HLAnextSaveName",
                           L"HLAnextSaveTime",
                           L"HLAautoProvide"}) {
        std::wcout << "  getAttributeHandle for " << attribute << std::endl;
        attributes.insert(my_ambassador.getAttributeHandle(theObjectClass, attribute));
    }

    std::string tag;
    my_ambassador.requestAttributeValueUpdate(theObject, attributes, {tag.c_str(), tag.size()});
}

void MOMFederateAmbassador::discoverObjectInstance(ObjectInstanceHandle theObject,
                                                   ObjectClassHandle theObjectClass,
                                                   std::wstring const& theObjectInstanceName,
                                                   FederateHandle producingFederate) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObject << ", " << theObjectClass << ", " << theObjectInstanceName << ", "
               << producingFederate << ">" << std::endl;
}

void MOMFederateAmbassador::reflectAttributeValues(ObjectInstanceHandle theObject,
                                                   AttributeHandleValueMap const& theAttributeValues,
                                                   VariableLengthData const& theUserSuppliedTag,
                                                   OrderType sentOrder,
                                                   TransportationType theType,
                                                   SupplementalReflectInfo theReflectInfo) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObject << ", " << theAttributeValues << ", " << theUserSuppliedTag << ", "
               << sentOrder << ", " << theType << ", " << theReflectInfo << ">" << std::endl;
}

void MOMFederateAmbassador::reflectAttributeValues(ObjectInstanceHandle theObject,
                                                   AttributeHandleValueMap const& theAttributeValues,
                                                   VariableLengthData const& theUserSuppliedTag,
                                                   OrderType sentOrder,
                                                   TransportationType theType,
                                                   LogicalTime const& theTime,
                                                   OrderType receivedOrder,
                                                   SupplementalReflectInfo theReflectInfo) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObject << ", " << theAttributeValues << ", " << theUserSuppliedTag << ", "
               << sentOrder << ", " << theType << ", " << theTime.toString() << ", " << receivedOrder << ", "
               << theReflectInfo << ">" << std::endl;
}

void MOMFederateAmbassador::reflectAttributeValues(ObjectInstanceHandle theObject,
                                                   AttributeHandleValueMap const& theAttributeValues,
                                                   VariableLengthData const& theUserSuppliedTag,
                                                   OrderType sentOrder,
                                                   TransportationType theType,
                                                   LogicalTime const& theTime,
                                                   OrderType receivedOrder,
                                                   MessageRetractionHandle theHandle,
                                                   SupplementalReflectInfo theReflectInfo) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObject << ", " << theAttributeValues << ", " << theUserSuppliedTag << ", "
               << sentOrder << ", " << theType << ", " << theTime.toString() << ", " << receivedOrder << ", "
               << theHandle << ", " << theReflectInfo << ">" << std::endl;
}

void MOMFederateAmbassador::receiveInteraction(InteractionClassHandle theInteraction,
                                               ParameterHandleValueMap const& theParameterValues,
                                               VariableLengthData const& theUserSuppliedTag,
                                               OrderType sentOrder,
                                               TransportationType theType,
                                               SupplementalReceiveInfo theReceiveInfo) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theInteraction << ", " << theParameterValues << ", " << theUserSuppliedTag << ", "
               << sentOrder << ", " << theType << ", " << theReceiveInfo << ">" << std::endl;
}

void MOMFederateAmbassador::receiveInteraction(InteractionClassHandle theInteraction,
                                               ParameterHandleValueMap const& theParameterValues,
                                               VariableLengthData const& theUserSuppliedTag,
                                               OrderType sentOrder,
                                               TransportationType theType,
                                               LogicalTime const& theTime,
                                               OrderType receivedOrder,
                                               SupplementalReceiveInfo theReceiveInfo) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theInteraction << ", " << theParameterValues << ", " << theUserSuppliedTag << ", "
               << sentOrder << ", " << theType << ", " << theTime.toString() << ", " << receivedOrder << ", "
               << theReceiveInfo << ">" << std::endl;
}

void MOMFederateAmbassador::receiveInteraction(InteractionClassHandle theInteraction,
                                               ParameterHandleValueMap const& theParameterValues,
                                               VariableLengthData const& theUserSuppliedTag,
                                               OrderType sentOrder,
                                               TransportationType theType,
                                               LogicalTime const& theTime,
                                               OrderType receivedOrder,
                                               MessageRetractionHandle theHandle,
                                               SupplementalReceiveInfo theReceiveInfo) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theInteraction << ", " << theParameterValues << ", " << theUserSuppliedTag << ", "
               << sentOrder << ", " << theType << ", " << theTime.toString() << ", " << receivedOrder << ", "
               << theHandle << ", " << theReceiveInfo << ">" << std::endl;
}

void MOMFederateAmbassador::removeObjectInstance(ObjectInstanceHandle theObject,
                                                 VariableLengthData const& theUserSuppliedTag,
                                                 OrderType sentOrder,
                                                 SupplementalRemoveInfo theRemoveInfo) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObject << ", " << theUserSuppliedTag << ", " << sentOrder << ", "
               << theRemoveInfo << ">" << std::endl;
}

void MOMFederateAmbassador::removeObjectInstance(ObjectInstanceHandle theObject,
                                                 VariableLengthData const& theUserSuppliedTag,
                                                 OrderType sentOrder,
                                                 LogicalTime const& theTime,
                                                 OrderType receivedOrder,
                                                 SupplementalRemoveInfo theRemoveInfo) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObject << ", " << theUserSuppliedTag << ", " << sentOrder << ", "
               << theTime.toString() << ", " << receivedOrder << ", " << theRemoveInfo << ">" << std::endl;
}

void MOMFederateAmbassador::removeObjectInstance(ObjectInstanceHandle theObject,
                                                 VariableLengthData const& theUserSuppliedTag,
                                                 OrderType sentOrder,
                                                 LogicalTime const& theTime,
                                                 OrderType receivedOrder,
                                                 MessageRetractionHandle theHandle,
                                                 SupplementalRemoveInfo theRemoveInfo) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObject << ", " << theUserSuppliedTag << ", " << sentOrder << ", "
               << theTime.toString() << ", " << receivedOrder << ", " << theHandle << ", " << theRemoveInfo << ">"
               << std::endl;
}

void MOMFederateAmbassador::attributesInScope(ObjectInstanceHandle theObject,
                                              AttributeHandleSet const& theAttributes) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObject << ", " << theAttributes << ">" << std::endl;
}

void MOMFederateAmbassador::attributesOutOfScope(ObjectInstanceHandle theObject,
                                                 AttributeHandleSet const& theAttributes) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObject << ", " << theAttributes << ">" << std::endl;
}

void MOMFederateAmbassador::provideAttributeValueUpdate(
    ObjectInstanceHandle theObject,
    AttributeHandleSet const& theAttributes,
    VariableLengthData const& theUserSuppliedTag) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObject << ", " << theAttributes << ", " << theUserSuppliedTag << ">"
               << std::endl;
}

void MOMFederateAmbassador::turnUpdatesOnForObjectInstance(
    ObjectInstanceHandle theObject, AttributeHandleSet const& theAttributes) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObject << ", " << theAttributes << ">" << std::endl;
}

void MOMFederateAmbassador::turnUpdatesOnForObjectInstance(
    ObjectInstanceHandle theObject,
    AttributeHandleSet const& theAttributes,
    std::wstring const& updateRateDesignator) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObject << ", " << theAttributes << ", " << updateRateDesignator << ">"
               << std::endl;
}

void MOMFederateAmbassador::turnUpdatesOffForObjectInstance(
    ObjectInstanceHandle theObject, AttributeHandleSet const& theAttributes) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObject << ", " << theAttributes << ">" << std::endl;
}

void MOMFederateAmbassador::confirmAttributeTransportationTypeChange(
    ObjectInstanceHandle theObject,
    AttributeHandleSet theAttributes,
    TransportationType theTransportation) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObject << ", " << theAttributes << ", " << theTransportation << ">"
               << std::endl;
}

void MOMFederateAmbassador::reportAttributeTransportationType(
    ObjectInstanceHandle theObject,
    AttributeHandle theAttribute,
    TransportationType theTransportation) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObject << ", " << theAttribute << ", " << theTransportation << ">"
               << std::endl;
}

void MOMFederateAmbassador::confirmInteractionTransportationTypeChange(
    InteractionClassHandle theInteraction, TransportationType theTransportation) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theInteraction << ", " << theTransportation << ">" << std::endl;
}

void MOMFederateAmbassador::reportInteractionTransportationType(
    FederateHandle federateHandle,
    InteractionClassHandle theInteraction,
    TransportationType theTransportation) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << federateHandle << ", " << theInteraction << ", " << theTransportation << ">"
               << std::endl;
}

void MOMFederateAmbassador::requestAttributeOwnershipAssumption(
    ObjectInstanceHandle theObject,
    AttributeHandleSet const& offeredAttributes,
    VariableLengthData const& theUserSuppliedTag) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObject << ", " << offeredAttributes << ", " << theUserSuppliedTag << ">"
               << std::endl;
}

void MOMFederateAmbassador::requestDivestitureConfirmation(
    ObjectInstanceHandle theObject, AttributeHandleSet const& releasedAttributes) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObject << ", " << releasedAttributes << ">" << std::endl;
}

void MOMFederateAmbassador::attributeOwnershipAcquisitionNotification(
    ObjectInstanceHandle theObject,
    AttributeHandleSet const& securedAttributes,
    VariableLengthData const& theUserSuppliedTag) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObject << ", " << securedAttributes << ", " << theUserSuppliedTag << ">"
               << std::endl;
}

void MOMFederateAmbassador::attributeOwnershipUnavailable(
    ObjectInstanceHandle theObject, AttributeHandleSet const& theAttributes) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObject << ", " << theAttributes << ">" << std::endl;
}

void MOMFederateAmbassador::requestAttributeOwnershipRelease(
    ObjectInstanceHandle theObject,
    AttributeHandleSet const& candidateAttributes,
    VariableLengthData const& theUserSuppliedTag) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObject << ", " << candidateAttributes << ", " << theUserSuppliedTag << ">"
               << std::endl;
}

void MOMFederateAmbassador::confirmAttributeOwnershipAcquisitionCancellation(
    ObjectInstanceHandle theObject, AttributeHandleSet const& theAttributes) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObject << ", " << theAttributes << ">" << std::endl;
}

void MOMFederateAmbassador::informAttributeOwnership(ObjectInstanceHandle theObject,
                                                     AttributeHandle theAttribute,
                                                     FederateHandle theOwner) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObject << ", " << theAttribute << ", " << theOwner << ">" << std::endl;
}

void MOMFederateAmbassador::attributeIsNotOwned(ObjectInstanceHandle theObject,
                                                AttributeHandle theAttribute) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObject << ", " << theAttribute << ">" << std::endl;
}

void MOMFederateAmbassador::attributeIsOwnedByRTI(ObjectInstanceHandle theObject,
                                                  AttributeHandle theAttribute) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theObject << ", " << theAttribute << ">" << std::endl;
}

void MOMFederateAmbassador::timeRegulationEnabled(LogicalTime const& theFederateTime) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theFederateTime.toString() << ">" << std::endl;
}

void MOMFederateAmbassador::timeConstrainedEnabled(LogicalTime const& theFederateTime) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theFederateTime.toString() << ">" << std::endl;
}

void MOMFederateAmbassador::timeAdvanceGrant(LogicalTime const& theTime) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theTime.toString() << ">" << std::endl;
}

void MOMFederateAmbassador::requestRetraction(MessageRetractionHandle theHandle) throw(FederateInternalError)
{
    std::wcout << ">>" <<  __func__ << " <" << theHandle << ">" << std::endl;
}
