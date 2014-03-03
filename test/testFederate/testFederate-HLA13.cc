/*
 * Following code is HLA 1.3 specific
 */
#ifndef HLA13
#define HLA13
#endif

#include "testFederate.hh"
#include <iostream>

bool testFederate::createFederationExecution(std::wstring federationName, std::wstring FOMFile) {
    this->federationName = federationName;
    this->FOMFile        = FOMFile;

    try {
        rtiAmb->createFederationExecution(getString(federationName).c_str(),getString(FOMFile).c_str());
    } catch (RTI::Exception& e) {
        std::cout << "CFE caught " << e._name << " reason " << e._reason <<std::endl;
        status = false;
    }
    return status;
}

bool testFederate::destroyFederationExecution() {
    try {
        rtiAmb->destroyFederationExecution(getString(federationName).c_str());
    }
    catch (RTI::Exception& e) {
        std::cout << "DFE caught " << e._name << " reason " << e._reason <<std::endl;
        status = false;
    }
    return status;
}

bool testFederate::joinFederationExecution() {
    try {
        rtiAmb->joinFederationExecution(getString(name).c_str(),getString(federationName).c_str(),this);
    }
    catch (RTI::Exception& e) {
        std::cout << "JFE caught " << e._name << " reason " << e._reason <<std::endl;
    }

    return status;
}

bool testFederate::resignFederationExecution() {
    try {
           rtiAmb->resignFederationExecution(RTI::DELETE_OBJECTS_AND_RELEASE_ATTRIBUTES);
       }
       catch (RTI::Exception& e) {
           std::cout << "RFE caught " << e._name << " reason " << e._reason <<std::endl;
       }
    return status;
}

bool testFederate::specificInitialize() {
    try {
        rtiAmb = new RTIambassador_t();
    } catch (RTI::RTIinternalError& e) {
        std::cout << "RTIambassador caught " << e._name << " reason " << e._reason <<std::endl;
        status = false;
    }
    return status;
}
