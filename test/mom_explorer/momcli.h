#ifndef MOMCLI_H
#define MOMCLI_H

#include "cli.h"

#include <RTI/RTIambassador.h>

class MomCli : public cli {
public:
    MomCli(rti1516e::RTIambassador& amb, const std::wstring& federation_name);

private:
    void sendInteraction(const std::wstring& interaction,
                         std::map<std::wstring, rti1516e::VariableLengthData> parameters);

    rti1516e::RTIambassador& my_ambassador;
};

#endif // MOMCLI_H
