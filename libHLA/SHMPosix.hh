#ifndef SHM_POSIX_H
#define SHM_POSIX_H

// Specifics includes
#include "SHM.hh"

namespace libhla {
namespace ipc {

class HLA_EXPORT SHMPosix : public SHM {
private:
    int _Id; // identifiant du segment memoire affecté dans la methode : Open()

public:
    SHMPosix(const std::string& SHMName, const int SHMSize, const bool True);
    SHMPosix(const std::string& SHMName, const int SHMSize);
    virtual ~SHMPosix();

    void Open();
    void Attach();
    void Close();
};
} /* end namespace ipc  */
} /* end namespace libhla */
#endif
