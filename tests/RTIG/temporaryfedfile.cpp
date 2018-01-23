#include "temporaryfedfile.h"

#include <fstream>
#include <iostream>

namespace {
static constexpr bool debug_disabled{true};
}

#define CERR                                                                                                           \
    if (debug_disabled) {                                                                                              \
    }                                                                                                                  \
    else                                                                                                               \
        ::std::cerr << "[TemporaryFedFile]\t"

TemporaryFedFile::TemporaryFedFile(const std::string& path) : my_path{path}
{
    createFed();
}

TemporaryFedFile::~TemporaryFedFile()
{
    CERR << "Destroying temporary fed file " << my_path << std::endl;
    std::remove(my_path.c_str());
}

std::string TemporaryFedFile::path() const
{
    return my_path;
}

void TemporaryFedFile::createFed()
{
    CERR << "Creating temporary fed file " << my_path << std::endl;
    std::ofstream file;
    file.open(my_path);
    file << "(Fed" << std::endl
         << "  (Federation testFederation)" << std::endl
         << "  (FedVersion v1.3)" << std::endl
         << "  (Spaces" << std::endl
         << "  )" << std::endl
         << "  (Objects" << std::endl
         << "    (Class ObjectRoot" << std::endl
         << "      (Attribute privilegeToDelete RELIABLE TIMESTAMP)" << std::endl
         << "      (Class RTIprivate)" << std::endl
         << "      (Class Data" << std::endl
         << "        (Attribute Attr1 RELIABLE TIMESTAMP)" << std::endl
         << "        (Attribute Attr2 RELIABLE TIMESTAMP)" << std::endl
         << "      )" << std::endl
         << "    )" << std::endl
         << "  )" << std::endl
         << "  (Interactions" << std::endl
         << "    (Class InteractionRoot RELIABLE RECEIVE" << std::endl
         << "      (Class RTIprivate RELIABLE RECEIVE)" << std::endl
         << "      (Class Message RELIABLE TIMESTAMP" << std::endl
         << "        (Parameter Param1)" << std::endl
         << "        (Parameter Param2)" << std::endl
         << "      )" << std::endl
         << "    )" << std::endl
         << "  )" << std::endl
         << ")" << std::endl;

    file.close();
}
