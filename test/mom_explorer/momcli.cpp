#include "momcli.h"

#include <algorithm>
#include <iostream>
#include <map>

#include <RTI/Enums.h>
#include <RTI/RTI1516.h>
#include <RTI/RTI1516fedTime.h>

MomCli::MomCli(rti1516e::RTIambassador& amb, const std::wstring& federation_name) : cli(), my_ambassador{amb}
{
    registerCommand("tick", [&]() {
        std::wcout << "=>evokeCallback(0.1)\n";
        my_ambassador.evokeCallback(0.1);
    });

    registerCommand("resign federation execution", [&]() {
        std::wcout << "=>resignFederationExecution(NO_ACTION)\n";
        my_ambassador.resignFederationExecution(rti1516e::NO_ACTION);
    });

    registerCommand("destroy federation execution", [&]() {
        std::wcout << "=>destroyFederationExecution <" << federation_name << ">\n";
        my_ambassador.destroyFederationExecution(federation_name);

        return EXIT_SUCCESS;
    });

    registerCommand("enable time regulation", [&]() {
        std::wcout << "=>enableTimeRegulation\n";
        my_ambassador.enableTimeRegulation(RTI1516fedTimeInterval(1.0));
    });

    registerCommand("enable time constrained", [&]() {
        std::wcout << "=>enableTimeConstrained\n";
        my_ambassador.enableTimeConstrained();
    });

    registerCommand("disable time regulation", [&]() {
        std::wcout << "=>disableTimeRegulation\n";
        my_ambassador.disableTimeRegulation();
    });

    registerCommand("disable time constrained", [&]() {
        std::wcout << "=>disableTimeConstrained\n";
        my_ambassador.disableTimeConstrained();
    });

    registerCommand("federate adjust timing", [&]() {
        std::wcout << "=>\n";

        int handle = std::stoi(requestArgument("handle"));

        sendInteraction(L"HLAmanager.HLAfederate.HLArequest.HLArequestPublications",
                        {{L"HLAmanager.HLAfederate", {&handle, 4}}});
    });
    registerCommand("federate adjust modifyAttributeState", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate adjust switches", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate request publications", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate request subscriptions", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate request objectInstancesThatCanBeDeleted", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate request objectInstancesUpdated", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate request objectInstancesReflected", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate request updatesSent", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate request interactionsSent", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate request reflectionsReceived", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate request interactionsReceived", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate request objectInstanceInformation", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate request FOMmoduleData", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service resignFederationExecution", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service synchronizationPointAchieved", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service federateSaveBegun", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service federateSaveComplete", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service federateRestoreComplete", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service publishObjectClassAttributes", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service unpublishObjectClassAttributes", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service publishInteractionClass", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service unpublishInteractionClass", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service subscribeObjectClassAttributes", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service unsubscribeObjectClassAttributes", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service subscribeInteractionClass", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service unsubscribeInteractionClass", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service deleteObjectInstance", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service localDeleteObjectInstance", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service requestAttributeTransportationTypeChange", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service requestInteractionTransportationTypeChange", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service unconditionalAttributeOwnershipDivestiture", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service enableTimeRegulation", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service disableTimeRegulation", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service enableTimeConstrained", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service disableTimeConstrained", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service timeAdvanceRequest", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service timeAdvanceRequestAvailable", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service nextMessageRequest", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service nextMessageRequestAvailable", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service flushQueueRequest", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service enableAsynchronousDelivery", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service disableAsynchronousDelivery", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service modifyLookahead", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service changeAttributeOrderType", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federate service changeInteractionOrderType", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federation adjust switches", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federation request synchronizationPoints", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federation request synchronizationPointStatus", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federation request FOMmoduleData", [&]() {
        std::wcout << "=>\n";

    });
    registerCommand("federation request MIMData", [&]() {
        std::wcout << "=>\n";

    });
}

void MomCli::sendInteraction(const std::wstring& interaction,
                             std::map<std::wstring, rti1516e::VariableLengthData> parameters)
{
    auto ich = my_ambassador.getInteractionClassHandle(interaction);

    std::map<rti1516e::ParameterHandle, rti1516e::VariableLengthData> params;

    std::transform(begin(parameters),
                   end(parameters),
                   std::inserter(params, begin(params)),
                   [&](decltype(parameters)::value_type p) {
                       return std::make_pair(my_ambassador.getParameterHandle(ich, p.first), p.second);
                   });

    my_ambassador.sendInteraction(ich, params, {});
}
