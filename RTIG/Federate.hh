// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
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
// $Id: Federate.hh,v 3.8 2003/05/15 20:57:41 breholee Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_RTIG_FEDERATE_HH
#define _CERTI_RTIG_FEDERATE_HH

#include <config.h>

#include <list>
#include <cstring>
#include <string>

#include "RTItypes.hh"
// #include "SocketTCP.hh"

namespace certi {
namespace rtig {

class Federate
{
private:
    FederateHandle handle ; //!< Federate ID.
    char *name ; //!< Federate name.
    /*! = false by default -- Used only on the RTIA, because on RTIG there is a
      upper level list of regulator Federates (in Federation).
    */
    bool regulator ;
    bool constrained ; //!< = false by default.

    std::list<char *> synchronizationLabels ; // List of labels to synchronize.

public:
    Federate(const char *the_name, FederateHandle)
        throw (MemoryExhausted, RTIinternalError);
    ~Federate(void);

    FederateHandle getHandle(void) const ;
    void setHandle(FederateHandle the_handle);

    const char *getName(void) const ;
    bool isConstrained(void) const ;
    bool isRegulator(void) const ;
    void setConstrained(bool);
    void setRegulator(bool);

    bool isSaving(void) const ;
    bool isRestoring(void) const ;
    void setSaving(bool);
    void setRestoring(bool);

    void addSynchronizationLabel(const char *)
        throw (RTIinternalError);
    void removeSynchronizationLabel(const char *)
        throw (RTIinternalError);
    bool isSynchronizationLabel(const char *) const ;

private:
    bool saving ;
    bool restoring ;
};

}}

#endif // _CERTI_RTIG_FEDERATE_HH

// $Id: Federate.hh,v 3.8 2003/05/15 20:57:41 breholee Exp $
