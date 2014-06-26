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

bool testFederate::getHandles() {
    status = false;
    try {
        OCH_Data = rtiAmb->getObjectClassHandle("Data");
        AH_Attr1 = rtiAmb->getAttributeHandle("Attr1",OCH_Data);
        AH_Attr2 = rtiAmb->getAttributeHandle("Attr2",OCH_Data);
        ICH_Message = rtiAmb->getInteractionClassHandle("Message");
        PH_Param1 = rtiAmb->getParameterHandle("Param1",ICH_Message);
        PH_Param2 = rtiAmb->getParameterHandle("Param2",ICH_Message);
        status = true;
    } catch (RTI::Exception& e) {
        std::cout << "getHandles() caught " << e._name << " reason " << e._reason <<std::endl;
        status = false;
    }
    return status;
}

bool testFederate::publishObjectClassAttribute() { return false;}
bool testFederate::unpublishObjectClassAttribute() { return false;}
bool testFederate::publishInteractionClass() { return false;}
bool testFederate::unpublishInteractionClass() { return false;}

bool testFederate::subscribeObjectClassAttribute() { return false;}
bool testFederate::unsubscribeObjectClassAttribute() { return false;}
bool testFederate::subscribreInteractionClass() { return false;}
bool testFederate::unsubscribreInteractionClass() { return false;}

bool testFederate::evoke() { return false;}
bool testFederate::TAR() { return false;}
bool testFederate::TARA() { return false;}
bool testFederate::enableTR() { return false;}
bool testFederate::disableTR() { return false;}
bool testFederate::enableAD() { return false;}
bool testFederate::disableAD() { return false;}

bool testFederate::specificInitialize() {
    try {
        rtiAmb = new RTIambassador_t();
    } catch (RTI::RTIinternalError& e) {
        std::cout << "RTIambassador caught " << e._name << " reason " << e._reason <<std::endl;
        status = false;
    }
    connected = true;
    return status;
}


