// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: BasicMessage.hh,v 3.7.2.1 2008/03/18 15:55:56 erk Exp $
// ----------------------------------------------------------------------------

#ifndef LIBCERTI_BASIC_MESSAGE
#define LIBCERTI_BASIC_MESSAGE

#include "Extent.hh"
#include "MessageBuffer.hh"
#include "BaseRegion.hh"
#include <vector>

namespace certi {

/** 
 * Base class for messages. 
 * It contains common attributes and methods
 * for Message and NetworkMessage classes.
 * \sa Message, NetworkMessage
 */
class CERTI_EXPORT BasicMessage
{
public:
    void setExtents(const std::vector<Extent> &);
    const std::vector<Extent> &getExtents() const ;

    void setRegions(const BaseRegion **, int);
    void setRegions(const std::vector<RegionHandle> &);
    const std::vector<RegionHandle> &getRegions() const ;
	 
protected:
    void readExtents();
    void writeExtents() ;

    void readRegions();
    void writeRegions();

    std::vector<Extent> extents ;
    std::vector<RegionHandle> regions ;
    MessageBuffer msgBuf;
};

} // namespace certi

#endif // LIBCERTI_BASIC_MESSAGE

// $Id: BasicMessage.hh,v 3.7.2.1 2008/03/18 15:55:56 erk Exp $
