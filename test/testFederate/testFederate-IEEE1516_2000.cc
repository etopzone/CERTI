/*
 * Following code is IEEE-1516_2000 specific
 */
#ifndef IEEE1516_2000
#define IEEE1516_2000
#endif

#include "testFederate.hh"

#include <iostream>

bool testFederate::createFederationExecution(std::wstring federationName, std::wstring FOMFile) {
    this->federationName = federationName;
    this->FOMFile        = FOMFile;

    try {
        rtiAmb->createFederationExecution(federationName,FOMFile);
    } catch (rti1516::Exception& e) {
        std::wcout << L"CFE caught Error " << e.what() <<std::endl;
        status = false;
    }
    return status;
}

bool testFederate::destroyFederationExecution() {
    try {
        rtiAmb->destroyFederationExecution(federationName);
    }
    catch (rti1516::Exception& e) {
        std::wcout << L"DFE caught Error " << e.what() <<std::endl;
        status = false;
    }
    return status;
}

bool testFederate::joinFederationExecution() {
    try {
        rtiAmb->joinFederationExecution(name,federationName,*this);
    }
    catch (rti1516::Exception& e) {
        std::wcout << L"JFE caught Error " << e.what() <<std::endl;
    }

    return status;
}

bool testFederate::resignFederationExecution() {
    try {
           rtiAmb->resignFederationExecution(rti1516::CANCEL_THEN_DELETE_THEN_DIVEST);
       }
       catch (rti1516::Exception& e) {
           std::wcout << L"RFE caught Error " << e.what() <<std::endl;
       }
    return status;
}

bool testFederate::specificInitialize() {
    try {
        std::vector< std::wstring > constructorArgs;

        std::auto_ptr <rti1516::RTIambassadorFactory> rtiAmbFactory(new rti1516::RTIambassadorFactory());
        std::auto_ptr <rti1516::RTIambassador > rtiAmbAP = rtiAmbFactory->createRTIambassador(constructorArgs);
        rtiAmb = rtiAmbAP.release();
    } catch (rti1516::Exception& e) {
        std::wcout << L"RTIambassador creation caught Error " << e.what() <<std::endl;
        status = false;
    }
    return status;
}
