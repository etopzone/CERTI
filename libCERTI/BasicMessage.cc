// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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
// $Id: BasicMessage.cc,v 3.1 2003/11/10 14:43:01 breholee Exp $
// ----------------------------------------------------------------------------

#include "BasicMessage.hh"
#include "PrettyDebug.hh"

#include <cassert>

using std::vector ;
using std::endl ;

static pdCDebug D("MESSAGE", __FILE__);

namespace certi {

// ----------------------------------------------------------------------------
/** Set extents
 */
void
BasicMessage::setExtents(const vector<Extent> &e)
{
//     extents.clear();
//     std::copy(e.begin(), e.end(), std::back_inserter(extents));
    extents = e ;
    assert(extents.size() == e.size());
}

// ----------------------------------------------------------------------------
/** Get extents
 */
const vector<Extent> &
BasicMessage::getExtents() const
{
    return extents ;
}

// ----------------------------------------------------------------------------
/** Write the 'extent' Message attribute into the body. Format : number of
    extents. If not zero, number of dimensions. The list of extents. Extent
    format: list of ranges. Range format: lower bound, upper bound.
 */
void
BasicMessage::writeExtents(MessageBody &body) const
{
    D[pdDebug] << "Write " << extents.size() << " extents" << endl ;

    body.writeLongInt(extents.size());
    if (extents.size() > 0) {
	int n = extents[0].size();
	body.writeLongInt(n);

	for (int i = 0 ; i < extents.size(); ++i) {
	    const Extent &e = extents[i] ;

	    for (int h = 1 ; h <= n ; ++h) {
		body.writeLongInt(e.getRangeLowerBound(h));
		body.writeLongInt(e.getRangeUpperBound(h));
	    }
	}
    }
}

// ----------------------------------------------------------------------------
/** Set the 'extent' attribute with the values found in a message body.
    \param body Message body to look into
    \sa BasicMessage::writeExtents, Extent
 */
void
BasicMessage::readExtents(const MessageBody &body)
{
    long nb_extents = body.readLongInt();
    D[pdDebug] << "Read " << nb_extents << " extents" << endl ;

    extents.clear();    
    if (nb_extents > 0) {
	extents.reserve(nb_extents);
	long nb_dimensions = body.readLongInt();
	for (long i = 0 ; i < nb_extents ; ++i) {
	    Extent e(nb_dimensions);
	    for (long h = 0 ; h < nb_dimensions ; ++h) {
		e.setRangeLowerBound(h, body.readLongInt());
		e.setRangeUpperBound(h, body.readLongInt());
	    }
	    extents.push_back(e);
	}
    }
}

} // namespace certi

// $Id: BasicMessage.cc,v 3.1 2003/11/10 14:43:01 breholee Exp $
