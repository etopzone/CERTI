#include "momfederateambassador.h"

#include <algorithm>

#include <cstring>

namespace {

enum class DataType {
    HLAunicodeString,
    HLAhandleList,
    HLAmoduleDesignatorList,
    HLAlogicalTime,
    HLAswitch,
    HLAhandle,
    HLAboolean,
    HLAfederateState,
    HLAtimeState,
    HLAtimeInterval,
    HLAcount,
    HLAmsec
};

std::map<std::wstring, std::map<std::wstring, DataType>> the_mom_classes{
    {L"HLAmanager.HLAfederation",
     {{L"HLAfederationName", DataType::HLAunicodeString},
      {L"HLAfederatesInFederation", DataType::HLAhandleList},
      {L"HLARTIversion", DataType::HLAunicodeString},
      {L"HLAMIMDesignator", DataType::HLAunicodeString},
      {L"HLAFOMmoduleDesignatorList", DataType::HLAmoduleDesignatorList},
      {L"HLAcurrentFDD", DataType::HLAunicodeString},
      {L"HLAtimeImplementationName", DataType::HLAunicodeString},
      {L"HLAlastSaveName", DataType::HLAunicodeString},
      {L"HLAlastSaveTime", DataType::HLAlogicalTime},
      {L"HLAnextSaveName", DataType::HLAunicodeString},
      {L"HLAnextSaveTime", DataType::HLAlogicalTime},
      {L"HLAautoProvide", DataType::HLAswitch}}},
    {L"HLAmanager.HLAfederate",
     {{L"HLAfederateHandle", DataType::HLAhandle},
      {L"HLAfederateName", DataType::HLAunicodeString},
      {L"HLAfederateType", DataType::HLAunicodeString},
      {L"HLAfederateHost", DataType::HLAunicodeString},
      {L"HLARTIversion", DataType::HLAunicodeString},
      {L"HLAFOMmoduleDesignatorList", DataType::HLAmoduleDesignatorList},
      {L"HLAtimeConstrained", DataType::HLAboolean},
      {L"HLAtimeRegulating", DataType::HLAboolean},
      {L"HLAasynchronousDelivery", DataType::HLAboolean},
      {L"HLAfederateState", DataType::HLAfederateState},
      {L"HLAtimeManagerState", DataType::HLAtimeState},
      {L"HLAlogicalTime", DataType::HLAlogicalTime},
      {L"HLAlookahead", DataType::HLAtimeInterval},
      {L"HLAGALT", DataType::HLAlogicalTime},
      {L"HLALITS", DataType::HLAlogicalTime},
      {L"HLAROlength", DataType::HLAcount},
      {L"HLATSOlength", DataType::HLAcount},
      {L"HLAreflectionsReceived", DataType::HLAcount},
      {L"HLAupdatesSent", DataType::HLAcount},
      {L"HLAinteractionsReceived", DataType::HLAcount},
      {L"HLAinteractionsSent", DataType::HLAcount},
      {L"HLAobjectInstancesThatCanBeDeleted", DataType::HLAcount},
      {L"HLAobjectInstancesUpdated", DataType::HLAcount},
      {L"HLAobjectInstancesReflected", DataType::HLAcount},
      {L"HLAobjectInstancesDeleted", DataType::HLAcount},
      {L"HLAobjectInstancesRemoved", DataType::HLAcount},
      {L"HLAobjectInstancesRegistered", DataType::HLAcount},
      {L"HLAobjectInstancesDiscovered", DataType::HLAcount},
      {L"HLAtimeGrantedTime", DataType::HLAmsec},
      {L"HLAtimeAdvancingTime", DataType::HLAmsec},
      {L"HLAconveyRegionDesignatorSets", DataType::HLAswitch},
      {L"HLAconveyProducingFederate", DataType::HLAswitch}}}};

std::wstring decode(const std::wstring& object, const std::wstring& attribute, const VariableLengthData data)
{
    auto type = the_mom_classes[object][attribute];

    std::wstring ret;
    
    if(data.size() == 8 && strncmp(static_cast<const char*>(data.data()), "\4\0\0\0TODO", 8) == 0) {
        return L"Not yet implemented on server side.";
    }

    switch (type) {
    case DataType::HLAunicodeString: {
        auto size = static_cast<const uint32_t*>(data.data())[0];
        ret += L"\"";
        for (auto i(0u); i < size; ++i) {
            ret += static_cast<const char*>(data.data())[4 + i];
        }
        ret += L"\"";
    } break;
    case DataType::HLAhandle:
    case DataType::HLAmsec:
    case DataType::HLAcount:
        ret += std::to_wstring(*static_cast<const uint32_t*>(data.data()));
        break;
    case DataType::HLAboolean:
    case DataType::HLAswitch:
        ret += *static_cast<const uint32_t*>(data.data()) ? L"True" : L"False";
        break;
    case DataType::HLAhandleList: {
        if (data.size() == 0) {
            ret = L"{}";
        }
        else {
            auto size = static_cast<const uint32_t*>(data.data())[0];
            ret = L"{";
            for (auto i(0u); i < size; ++i) {
                if (i != 0) {
                    ret += L", ";
                }
                ret += std::to_wstring(static_cast<const uint32_t*>(data.data())[4 + 4 * i]);
            }
            ret += L"}";
        }

    } break;
    default:
        ret = L"To be decoded";
    }

    return ret;
}
}

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

    if (v.size() == 0) {
        os << "empty";
    }
    else {
        os << "size: " << v.size() << ", data: [ ";
        for (auto i(0u); i < v.size(); ++i) {
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

MOMFederateAmbassador::MOMFederateAmbassador(rti1516e::RTIambassador& ambassador,
                                             const std::wstring& federation_name,
                                             const std::wstring& federate_name)
    : my_ambassador{ambassador}, my_federation_name{federation_name}, my_federate_name{federate_name}
{
}

void MOMFederateAmbassador::connect()
{
    my_ambassador.connect(*this, rti1516e::HLA_EVOKED);
    std::cout << "*\tAmbassador connected" << std::endl << std::endl;
}

void MOMFederateAmbassador::createFederationExecution()
{
    std::wcout << "=>\tcreateFederationExecution <" << my_federation_name << ">" << std::endl;

    my_ambassador.createFederationExecution(my_federation_name, L"Test.xml");
    std::wcout << "*\tFederation created" << std::endl;
}

void MOMFederateAmbassador::joinFederationExecution()
{
    std::wcout << "=>\tjoinFederationExecution <" << my_federation_name << ">" << std::endl;

    my_ambassador.joinFederationExecution(my_federate_name, my_federation_name);
    std::cout << "*\tFederation joined" << std::endl;
}

void MOMFederateAmbassador::subscribeObjectClasses()
{
    std::cout << "=>\tsubscribeObjectClasses" << std::endl;

    for (const auto& pair : the_mom_classes) {
        auto class_handle = objectClassHandle(pair.first);

        rti1516e::AttributeHandleSet attributes;
        for (const auto& attr_pair : pair.second) {
            attributes.insert(attributeHandle(pair.first, attr_pair.first));
        }

        std::wcout << "  subscribeObjectClassAttributes for " << pair.first << std::endl;
        my_ambassador.subscribeObjectClassAttributes(class_handle, attributes);
    }
}

void MOMFederateAmbassador::publishAndsubscribeInteractions()
{
    std::cout << "=>\tpublishAndsubscribeInteractions" << std::endl;

    /*for (const auto& pair : the_mom_classes) {
        auto class_handle = objectClassHandle(pair.first);

        rti1516e::AttributeHandleSet attributes;
        for (const auto& attr_pair : pair.second) {
            attributes.insert(attributeHandle(pair.first, attr_pair.first));
        }

        std::wcout << "  subscribeObjectClassAttributes for " << pair.first << std::endl;
        my_ambassador.subscribeObjectClassAttributes(class_handle, attributes);
    }*/
    my_ambassador.publishInteractionClass(my_ambassador.getInteractionClassHandle(L"HLAmanager.HLAfederate.HLArequest.HLArequestPublications"));
    my_ambassador.subscribeInteractionClass(my_ambassador.getInteractionClassHandle(L"HLAmanager.HLAfederate.HLAreport.HLAreportObjectClassPublication"));
    my_ambassador.subscribeInteractionClass(my_ambassador.getInteractionClassHandle(L"HLAmanager.HLAfederate.HLAreport.HLAreportInteractionPublication"));
}

void MOMFederateAmbassador::connectionLost(std::wstring const& faultDescription) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << faultDescription << ">" << std::endl;
}

void MOMFederateAmbassador::reportFederationExecutions(
    FederationExecutionInformationVector const& theFederationExecutionInformationList) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theFederationExecutionInformationList << ">" << std::endl;
}

void MOMFederateAmbassador::synchronizationPointRegistrationSucceeded(std::wstring const& label) throw(
    FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << label << ">" << std::endl;
}

void MOMFederateAmbassador::synchronizationPointRegistrationFailed(
    std::wstring const& label, SynchronizationPointFailureReason reason) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << label << ", " << reason << ">" << std::endl;
}

void MOMFederateAmbassador::announceSynchronizationPoint(
    std::wstring const& label, VariableLengthData const& theUserSuppliedTag) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << label << ", " << theUserSuppliedTag << ">" << std::endl;
}

void MOMFederateAmbassador::federationSynchronized(
    std::wstring const& label, FederateHandleSet const& failedToSyncSet) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << label << ", " << failedToSyncSet << ">" << std::endl;
}

void MOMFederateAmbassador::initiateFederateSave(std::wstring const& label) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << label << ">" << std::endl;
}

void MOMFederateAmbassador::initiateFederateSave(std::wstring const& label,
                                                 LogicalTime const& theTime) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << label << theTime.toString() << ">" << std::endl;
}

void MOMFederateAmbassador::federationSaved() throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << ">" << std::endl;
}

void MOMFederateAmbassador::federationNotSaved(SaveFailureReason theSaveFailureReason) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theSaveFailureReason << ">" << std::endl;
}

void MOMFederateAmbassador::federationSaveStatusResponse(
    FederateHandleSaveStatusPairVector const& theFederateStatusVector) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theFederateStatusVector << ">" << std::endl;
}

void MOMFederateAmbassador::requestFederationRestoreSucceeded(std::wstring const& label) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << label << ">" << std::endl;
}

void MOMFederateAmbassador::requestFederationRestoreFailed(std::wstring const& label) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << label << ">" << std::endl;
}

void MOMFederateAmbassador::federationRestoreBegun() throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << ">" << std::endl;
}

void MOMFederateAmbassador::initiateFederateRestore(std::wstring const& label,
                                                    std::wstring const& federateName,
                                                    FederateHandle handle) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << label << ", " << federateName << ", " << handle << ">" << std::endl;
}

void MOMFederateAmbassador::federationRestored() throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <"
               << ">" << std::endl;
}

void MOMFederateAmbassador::federationNotRestored(RestoreFailureReason theRestoreFailureReason) throw(
    FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theRestoreFailureReason << ">" << std::endl;
}

void MOMFederateAmbassador::federationRestoreStatusResponse(
    FederateRestoreStatusVector const& theFederateRestoreStatusVector) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theFederateRestoreStatusVector << ">" << std::endl;
}

void MOMFederateAmbassador::startRegistrationForObjectClass(ObjectClassHandle theClass) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theClass << ">" << std::endl;
}

void MOMFederateAmbassador::stopRegistrationForObjectClass(ObjectClassHandle theClass) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theClass << ">" << std::endl;
}

void MOMFederateAmbassador::turnInteractionsOn(InteractionClassHandle theHandle) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theHandle << ">" << std::endl;
}

void MOMFederateAmbassador::turnInteractionsOff(InteractionClassHandle theHandle) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theHandle << ">" << std::endl;
}

void MOMFederateAmbassador::objectInstanceNameReservationSucceeded(std::wstring const& theObjectInstanceName) throw(
    FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theObjectInstanceName << ">" << std::endl;
}

void MOMFederateAmbassador::objectInstanceNameReservationFailed(std::wstring const& theObjectInstanceName) throw(
    FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theObjectInstanceName << ">" << std::endl;
}

void MOMFederateAmbassador::multipleObjectInstanceNameReservationSucceeded(
    std::set<std::wstring> const& theObjectInstanceNames) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theObjectInstanceNames << ">" << std::endl;
}

void MOMFederateAmbassador::multipleObjectInstanceNameReservationFailed(
    std::set<std::wstring> const& theObjectInstanceNames) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theObjectInstanceNames << ">" << std::endl;
}

void MOMFederateAmbassador::discoverObjectInstance(
    ObjectInstanceHandle theObject,
    ObjectClassHandle theObjectClass,
    std::wstring const& theObjectInstanceName) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theObject << ", " << theObjectClass << ", " << theObjectInstanceName
               << ">" << std::endl;

    if (theObjectClass == objectClassHandle(L"HLAmanager.HLAfederation")) {
        my_federation = theObject;
    }
    else if (theObjectClass == objectClassHandle(L"HLAmanager.HLAfederate")) {
        my_federates.push_back(theObject);
    }
    else {
        std::wcout << "Unknown class" << std::endl;
    }

    std::wcout << "=>\trequestAttributeValueUpdate <" << theObjectInstanceName << ">" << std::endl;

    std::string tag;
    my_ambassador.requestAttributeValueUpdate(
        theObject, my_attributes_of_interest[theObjectClass], {tag.c_str(), tag.size()});
}

void MOMFederateAmbassador::discoverObjectInstance(ObjectInstanceHandle theObject,
                                                   ObjectClassHandle theObjectClass,
                                                   std::wstring const& theObjectInstanceName,
                                                   FederateHandle producingFederate) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theObject << ", " << theObjectClass << ", " << theObjectInstanceName
               << ", " << producingFederate << ">" << std::endl;
}

void MOMFederateAmbassador::reflectAttributeValues(ObjectInstanceHandle theObject,
                                                   AttributeHandleValueMap const& theAttributeValues,
                                                   VariableLengthData const& theUserSuppliedTag,
                                                   OrderType sentOrder,
                                                   TransportationType theType,
                                                   SupplementalReflectInfo theReflectInfo) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theObject << ", " << theAttributeValues << ", " << theUserSuppliedTag
               << ", " << sentOrder << ", " << theType << ", " << theReflectInfo << ">" << std::endl;

    for (const auto& pair : theAttributeValues) {
        my_data[theObject][pair.first] = pair.second;
    }

    displayData();
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
    std::wcout << ">>" << __func__ << " <" << theObject << ", " << theAttributeValues << ", " << theUserSuppliedTag
               << ", " << sentOrder << ", " << theType << ", " << theTime.toString() << ", " << receivedOrder << ", "
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
    std::wcout << ">>" << __func__ << " <" << theObject << ", " << theAttributeValues << ", " << theUserSuppliedTag
               << ", " << sentOrder << ", " << theType << ", " << theTime.toString() << ", " << receivedOrder << ", "
               << theHandle << ", " << theReflectInfo << ">" << std::endl;
}

void MOMFederateAmbassador::receiveInteraction(InteractionClassHandle theInteraction,
                                               ParameterHandleValueMap const& theParameterValues,
                                               VariableLengthData const& theUserSuppliedTag,
                                               OrderType sentOrder,
                                               TransportationType theType,
                                               SupplementalReceiveInfo theReceiveInfo) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theInteraction << ", " << theParameterValues << ", " << theUserSuppliedTag
               << ", " << sentOrder << ", " << theType << ", " << theReceiveInfo << ">" << std::endl;
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
    std::wcout << ">>" << __func__ << " <" << theInteraction << ", " << theParameterValues << ", " << theUserSuppliedTag
               << ", " << sentOrder << ", " << theType << ", " << theTime.toString() << ", " << receivedOrder << ", "
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
    std::wcout << ">>" << __func__ << " <" << theInteraction << ", " << theParameterValues << ", " << theUserSuppliedTag
               << ", " << sentOrder << ", " << theType << ", " << theTime.toString() << ", " << receivedOrder << ", "
               << theHandle << ", " << theReceiveInfo << ">" << std::endl;
}

void MOMFederateAmbassador::removeObjectInstance(ObjectInstanceHandle theObject,
                                                 VariableLengthData const& theUserSuppliedTag,
                                                 OrderType sentOrder,
                                                 SupplementalRemoveInfo theRemoveInfo) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theObject << ", " << theUserSuppliedTag << ", " << sentOrder << ", "
               << theRemoveInfo << ">" << std::endl;
               
    my_data.erase(theObject);
    
    auto it = std::find(begin(my_federates), end(my_federates), theObject);
    if(it != std::end(my_federates)) {
        my_federates.erase(it);
    }
    
    displayData();
}

void MOMFederateAmbassador::removeObjectInstance(ObjectInstanceHandle theObject,
                                                 VariableLengthData const& theUserSuppliedTag,
                                                 OrderType sentOrder,
                                                 LogicalTime const& theTime,
                                                 OrderType receivedOrder,
                                                 SupplementalRemoveInfo theRemoveInfo) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theObject << ", " << theUserSuppliedTag << ", " << sentOrder << ", "
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
    std::wcout << ">>" << __func__ << " <" << theObject << ", " << theUserSuppliedTag << ", " << sentOrder << ", "
               << theTime.toString() << ", " << receivedOrder << ", " << theHandle << ", " << theRemoveInfo << ">"
               << std::endl;
}

void MOMFederateAmbassador::attributesInScope(ObjectInstanceHandle theObject,
                                              AttributeHandleSet const& theAttributes) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theObject << ", " << theAttributes << ">" << std::endl;
}

void MOMFederateAmbassador::attributesOutOfScope(ObjectInstanceHandle theObject,
                                                 AttributeHandleSet const& theAttributes) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theObject << ", " << theAttributes << ">" << std::endl;
}

void MOMFederateAmbassador::provideAttributeValueUpdate(
    ObjectInstanceHandle theObject,
    AttributeHandleSet const& theAttributes,
    VariableLengthData const& theUserSuppliedTag) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theObject << ", " << theAttributes << ", " << theUserSuppliedTag << ">"
               << std::endl;
}

void MOMFederateAmbassador::turnUpdatesOnForObjectInstance(
    ObjectInstanceHandle theObject, AttributeHandleSet const& theAttributes) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theObject << ", " << theAttributes << ">" << std::endl;
}

void MOMFederateAmbassador::turnUpdatesOnForObjectInstance(
    ObjectInstanceHandle theObject,
    AttributeHandleSet const& theAttributes,
    std::wstring const& updateRateDesignator) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theObject << ", " << theAttributes << ", " << updateRateDesignator << ">"
               << std::endl;
}

void MOMFederateAmbassador::turnUpdatesOffForObjectInstance(
    ObjectInstanceHandle theObject, AttributeHandleSet const& theAttributes) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theObject << ", " << theAttributes << ">" << std::endl;
}

void MOMFederateAmbassador::confirmAttributeTransportationTypeChange(
    ObjectInstanceHandle theObject,
    AttributeHandleSet theAttributes,
    TransportationType theTransportation) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theObject << ", " << theAttributes << ", " << theTransportation << ">"
               << std::endl;
}

void MOMFederateAmbassador::reportAttributeTransportationType(
    ObjectInstanceHandle theObject,
    AttributeHandle theAttribute,
    TransportationType theTransportation) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theObject << ", " << theAttribute << ", " << theTransportation << ">"
               << std::endl;
}

void MOMFederateAmbassador::confirmInteractionTransportationTypeChange(
    InteractionClassHandle theInteraction, TransportationType theTransportation) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theInteraction << ", " << theTransportation << ">" << std::endl;
}

void MOMFederateAmbassador::reportInteractionTransportationType(
    FederateHandle federateHandle,
    InteractionClassHandle theInteraction,
    TransportationType theTransportation) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << federateHandle << ", " << theInteraction << ", " << theTransportation
               << ">" << std::endl;
}

void MOMFederateAmbassador::requestAttributeOwnershipAssumption(
    ObjectInstanceHandle theObject,
    AttributeHandleSet const& offeredAttributes,
    VariableLengthData const& theUserSuppliedTag) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theObject << ", " << offeredAttributes << ", " << theUserSuppliedTag
               << ">" << std::endl;
}

void MOMFederateAmbassador::requestDivestitureConfirmation(
    ObjectInstanceHandle theObject, AttributeHandleSet const& releasedAttributes) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theObject << ", " << releasedAttributes << ">" << std::endl;
}

void MOMFederateAmbassador::attributeOwnershipAcquisitionNotification(
    ObjectInstanceHandle theObject,
    AttributeHandleSet const& securedAttributes,
    VariableLengthData const& theUserSuppliedTag) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theObject << ", " << securedAttributes << ", " << theUserSuppliedTag
               << ">" << std::endl;
}

void MOMFederateAmbassador::attributeOwnershipUnavailable(
    ObjectInstanceHandle theObject, AttributeHandleSet const& theAttributes) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theObject << ", " << theAttributes << ">" << std::endl;
}

void MOMFederateAmbassador::requestAttributeOwnershipRelease(
    ObjectInstanceHandle theObject,
    AttributeHandleSet const& candidateAttributes,
    VariableLengthData const& theUserSuppliedTag) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theObject << ", " << candidateAttributes << ", " << theUserSuppliedTag
               << ">" << std::endl;
}

void MOMFederateAmbassador::confirmAttributeOwnershipAcquisitionCancellation(
    ObjectInstanceHandle theObject, AttributeHandleSet const& theAttributes) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theObject << ", " << theAttributes << ">" << std::endl;
}

void MOMFederateAmbassador::informAttributeOwnership(ObjectInstanceHandle theObject,
                                                     AttributeHandle theAttribute,
                                                     FederateHandle theOwner) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theObject << ", " << theAttribute << ", " << theOwner << ">" << std::endl;
}

void MOMFederateAmbassador::attributeIsNotOwned(ObjectInstanceHandle theObject,
                                                AttributeHandle theAttribute) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theObject << ", " << theAttribute << ">" << std::endl;
}

void MOMFederateAmbassador::attributeIsOwnedByRTI(ObjectInstanceHandle theObject,
                                                  AttributeHandle theAttribute) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theObject << ", " << theAttribute << ">" << std::endl;
}

void MOMFederateAmbassador::timeRegulationEnabled(LogicalTime const& theFederateTime) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theFederateTime.toString() << ">" << std::endl;
}

void MOMFederateAmbassador::timeConstrainedEnabled(LogicalTime const& theFederateTime) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theFederateTime.toString() << ">" << std::endl;
}

void MOMFederateAmbassador::timeAdvanceGrant(LogicalTime const& theTime) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theTime.toString() << ">" << std::endl;
}

void MOMFederateAmbassador::requestRetraction(MessageRetractionHandle theHandle) throw(FederateInternalError)
{
    std::wcout << ">>" << __func__ << " <" << theHandle << ">" << std::endl;
}

ObjectClassHandle MOMFederateAmbassador::objectClassHandle(const std::wstring& object_class_name)
{
    try {
        return my_object_class_cache.at(object_class_name);
    }
    catch (std::out_of_range& e) {
        std::wcout << "->\tgetObjectClassHandle for " << object_class_name << std::endl;
        auto handle = my_ambassador.getObjectClassHandle(object_class_name);
        my_object_class_cache.insert(std::make_pair(object_class_name, handle));
        return handle;
    }
}

AttributeHandle MOMFederateAmbassador::attributeHandle(const std::wstring& object_name,
                                                       const std::wstring& attribute_name)
{
    try {
        return my_attribute_cache.at(std::make_pair(object_name, attribute_name));
    }
    catch (std::out_of_range& e) {
        std::wcout << "->\tgetAttributeHandle for " << attribute_name << std::endl;
        auto object_class = objectClassHandle(object_name);
        auto handle = my_ambassador.getAttributeHandle(object_class, attribute_name);
        my_attribute_cache.insert(std::make_pair(std::make_pair(object_name, attribute_name), handle));
        my_attributes_of_interest[object_class].insert(handle);
        return handle;
    }
}

std::wstring MOMFederateAmbassador::attributeName(const std::wstring& object_class_name,
                                                  const rti1516e::AttributeHandle handle)
{
    for (const auto& pair : my_attribute_cache) {
        if (pair.second == handle && pair.first.first == object_class_name) {
            return pair.first.second;
        }
    }
    return L"Unknown";
}

void MOMFederateAmbassador::displayData()
{
    std::wcout << std::endl;

    std::wcout << "+ Federation" << std::endl;
    for (const auto& pair : my_data[my_federation]) {
        auto attribute_name = attributeName(L"HLAmanager.HLAfederation", pair.first);
        std::wcout << " - " << attribute_name << ": "
                   << decode(L"HLAmanager.HLAfederation", attribute_name, pair.second) << std::endl;
    }
    std::wcout << std::endl;

    for (const auto& fed : my_federates) {
        std::wcout << "+ Federate" << std::endl;
        for (const auto& pair : my_data[fed]) {
            auto attribute_name = attributeName(L"HLAmanager.HLAfederate", pair.first);
            std::wcout << " - " << attribute_name << ": "
                       << decode(L"HLAmanager.HLAfederate", attribute_name, pair.second) << std::endl;
        }
        std::wcout << std::endl;
    }
}
