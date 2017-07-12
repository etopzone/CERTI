#ifndef TEMPORARYENVIRONMENTLOCATION_H
#define TEMPORARYENVIRONMENTLOCATION_H

#include <string>

class TemporaryEnvironmentLocation {
public:
    TemporaryEnvironmentLocation(const std::string& name);
    ~TemporaryEnvironmentLocation();

    std::string path() const;

private:
    std::string my_name;
    std::string my_lastValue;
};

#endif // TEMPORARYENVIRONMENTLOCATION_H
