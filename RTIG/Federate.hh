// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This program is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// $Id: Federate.hh,v 3.11 2005/04/13 12:04:01 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef CERTI_RTIG_FEDERATE_HH
#define CERTI_RTIG_FEDERATE_HH

#include "certi.hh"

#include <vector>
#include <string>

namespace certi {
namespace rtig {

/*! This class manages the federate status and other relevant information.
 */
class Federate
{
public:
    Federate(const char *the_name, FederateHandle) throw (RTIinternalError);

    FederateHandle getHandle() const { return handle ; };
    void setHandle(FederateHandle h) { handle = h ; };

    const char *getName() const { return name.c_str(); };
    bool isConstrained() const { return constrained ; };
    bool isRegulator() const { return regulator ; };
    void setConstrained(bool c) { constrained = c ; };
    void setRegulator(bool r) { regulator = r ; };

    bool isSaving() const { return saving ; };
    bool isRestoring() const { return restoring ; };
    void setSaving(bool s) { saving = s ; };
    void setRestoring(bool r) { restoring = r ; };

    void addSynchronizationLabel(const char *) throw (RTIinternalError);
    void removeSynchronizationLabel(const char *) throw (RTIinternalError);
    bool isSynchronizationLabel(const char *) const ;

private:
    FederateHandle handle ; //!< Federate ID.
    std::string name ; //!< Federate name.
    /*! = false by default -- Used only on the RTIA, because on RTIG there is a
      upper level list of regulator Federates (in Federation).
    */
    bool regulator ;
    bool constrained ; //!< = false by default.

    typedef std::vector<std::string> SyncList ;
    SyncList syncLabels ; // List of labels to synchronize.

    bool saving ; //!< True when saving has been initiated on federate.
    bool restoring ; //!< True when restoring has been initiated on federate.
};

}} // namespace certi::rtig

#endif // CERTI_RTIG_FEDERATE_HH

// $Id: Federate.hh,v 3.11 2005/04/13 12:04:01 breholee Exp $
