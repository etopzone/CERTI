// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI
//
// CERTI is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// $Id: Federate.hh,v 3.9 2003/05/23 16:40:04 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_RTIG_FEDERATE_HH
#define _CERTI_RTIG_FEDERATE_HH

// Project
#include "RTItypes.hh"

// Standard
#include <list>

namespace certi {
namespace rtig {

/*! This class manages the federate status and other relevant information.
 */
class Federate
{
public:
    Federate(const char *the_name, FederateHandle)
        throw (MemoryExhausted, RTIinternalError);
    ~Federate();

    FederateHandle getHandle() const ;
    void setHandle(FederateHandle the_handle);

    const char *getName() const ;
    bool isConstrained() const ;
    bool isRegulator() const ;
    void setConstrained(bool);
    void setRegulator(bool);

    bool isSaving() const ;
    bool isRestoring() const ;
    void setSaving(bool);
    void setRestoring(bool);

    void addSynchronizationLabel(const char *)
        throw (RTIinternalError);
    void removeSynchronizationLabel(const char *)
        throw (RTIinternalError);
    bool isSynchronizationLabel(const char *) const ;

private:
    FederateHandle handle ; //!< Federate ID.
    char *name ; //!< Federate name.
    /*! = false by default -- Used only on the RTIA, because on RTIG there is a
      upper level list of regulator Federates (in Federation).
    */
    bool regulator ;
    bool constrained ; //!< = false by default.

    std::list<char *> synchronizationLabels ; // List of labels to synchronize.

    bool saving ; //!< True when saving has been initiated on federate.
    bool restoring ; //!< True when restoring has been initiated on federate.
};

}}

#endif // _CERTI_RTIG_FEDERATE_HH

// $Id: Federate.hh,v 3.9 2003/05/23 16:40:04 breholee Exp $
