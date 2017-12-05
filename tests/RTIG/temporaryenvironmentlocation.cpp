#include "temporaryenvironmentlocation.h"

#include <cstdlib>
#include <iostream>

namespace {
static constexpr int SETENV_REPLACE{1};
static const std::string ENV_VARIABLE_DID_NOT_EXIST{"DefaultLastValue"};
}

namespace {
static constexpr bool debug_disabled{true};
}

#define CERR                                                                                                           \
    if (debug_disabled) {                                                                                              \
    }                                                                                                                  \
    else                                                                                                               \
        ::std::cerr << "[TemporaryEnvironmentLocation]\t"

TemporaryEnvironmentLocation::TemporaryEnvironmentLocation(const std::string& name)
    : my_name{name}, my_lastValue{ENV_VARIABLE_DID_NOT_EXIST}
{
    CERR << "Create temporary folder " << path() << std::endl;
    auto result = system(std::string("mkdir " + path()).c_str());
    (void) result;

    auto lastValue = std::getenv(my_name.c_str());
    if (lastValue) {
        my_lastValue = lastValue;
        CERR << "Backup environment " << my_name << " = " << my_lastValue << std::endl;
    }
    else {
        CERR << "Environment " << my_name << " did not exist" << std::endl;
    }

    CERR << "Setup environment " << my_name << " = " << path() << std::endl;
    setenv(my_name.c_str(), path().c_str(), SETENV_REPLACE);
}

TemporaryEnvironmentLocation::~TemporaryEnvironmentLocation()
{
    CERR << "Destroy temporary folder " << path() << std::endl;
#ifdef _WIN32
    auto result = system(std::string("rmdir /S /Q " + path()).c_str());
#else
    auto result = system(std::string("rm -rf " + path()).c_str());
#endif
    (void) result;

    if (my_lastValue == ENV_VARIABLE_DID_NOT_EXIST) {
        CERR << "Rollback environment, unset " << my_name << std::endl;
        unsetenv(my_name.c_str());
    }
    else {
        CERR << "Rollback environment, set " << my_name << " = " << my_lastValue << std::endl;
        setenv(my_name.c_str(), my_lastValue.c_str(), SETENV_REPLACE);
    }
}

std::string TemporaryEnvironmentLocation::path() const
{
#ifdef _WIN32
    return ".\\test-temp\\" + my_name + "\\";
#else
    return "/tmp/" + my_name + "/";
#endif
}
