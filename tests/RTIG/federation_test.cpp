#include <gtest/gtest.h>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>

#include <RTIG/Federation.hh>

#include <libCERTI/SocketServer.hh>

#include <libCERTI/AuditFile.hh>

#include <config.h>

#include "temporaryenvironmentlocation.h"
#include "temporaryfedfile.h"

using ::certi::rtig::Federation;
using ::certi::SocketServer;
using ::certi::AuditFile;

using ::certi::RTIinternalError;
using ::certi::CouldNotOpenFED;

TEST(FederationTest, ConstructorGoodInit)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};
    
    TemporaryFedFile tmp{"Sample.fed"};

    Federation("name", 1, s, a, "Sample.fed", 0);
}

#ifdef FEDERATION_USES_MULTICAST
TEST(FederationTest, MulticastConstructorThrowsOnNullMC)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};
    
    ASSERT_THROW(Federation("multicast", 1, s, a, nullptr, 0), RTIinternalError);
}
#endif

TEST(FederationTest, ConstructorThrowsOnNullHandle)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};

    ASSERT_THROW(Federation("name", 0, s, a, "Test.fed", 0), RTIinternalError);
}

TEST(FederationTest, ConstructorThrowsOnEmptyName)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};

    ASSERT_THROW(Federation("", 1, s, a, "Test.fed", 0), RTIinternalError);
}

TEST(FederationTest, ConstructorFailIfUnableToFindFed)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};

    ASSERT_THROW(Federation("", 1, s, a, "", 0), RTIinternalError);
}

TEST(FederationTest, ConstructorFindsFedInSameFolder)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};
    
    TemporaryFedFile tmp{"SampleLocal.fed"};

    ASSERT_NO_THROW(Federation("local", 1, s, a, "SampleLocal.fed", 0));
}

TEST(FederationTest, ConstructorFindsFedInCertiFomPath)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};
    
    TemporaryEnvironmentLocation env{"CERTI_FOM_PATH"};
    
    TemporaryFedFile tmp{env.path() + "SampleFomPath.fed"};

    ASSERT_NO_THROW(Federation("fom_path", 1, s, a, "SampleFomPath.fed", 0));
}

TEST(FederationTest, ConstructorFindsFedInCertiHome)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};
    
    TemporaryEnvironmentLocation env{"CERTI_HOME"};
    
    TemporaryFedFile tmp{"SampleCertiHome.fed"};

    ASSERT_NO_THROW(Federation("certi_home", 1, s, a, "SampleCertiHome.fed", 0));
}

TEST(FederationTest, ConstructorFindsFedInPackageInstallPrefix)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};
    
    TemporaryFedFile tmp{PACKAGE_INSTALL_PREFIX "/share/federations/" "SampleInstallPrefix.fed"};

    ASSERT_NO_THROW(Federation("install_prefix", 1, s, a, "SampleInstallPrefix.fed", 0));
}

TEST(FederationTest, ConstructorFailsIfFileIsUnopenable)
{
    SocketServer s{new certi::SocketTCP{}, nullptr};
    AuditFile a{"tmp"};
    
//     TemporaryFedFile tmp{"SampleUnopenable.fed", true};
    
    ASSERT_THROW(Federation("local", 1, s, a, "/root/", 2), CouldNotOpenFED);
}
