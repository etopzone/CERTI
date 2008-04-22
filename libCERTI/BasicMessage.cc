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
// $Id: BasicMessage.cc,v 3.9.2.3 2008/04/22 08:41:07 erk Exp $
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
BasicMessage::setExtents(const std::vector<Extent> &e)
{
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

void 
BasicMessage::serialize(MessageBuffer& msgBuffer) {
	/* Write Extent */
	D[pdDebug] << "Serialize " << extents.size() << " extent(s)" << endl;
	msgBuffer.write_int32(static_cast<int32_t>(extents.size()));
	if (extents.size() > 0) {
		int n = extents[0].size();
		msgBuffer.write_int32(static_cast<int32_t>(n));
		D[pdDebug] << "Extent with " << n << " range(s)" << endl;
		for (unsigned int i = 0; i < extents.size(); ++i) {
			const Extent &e = extents[i];

			for (int h = 1; h <= n; ++h) {
				msgBuffer.write_int32(static_cast<int32_t>(e.getRangeLowerBound(h)));
				msgBuffer.write_int32(static_cast<int32_t>(e.getRangeUpperBound(h)));
			}
		}
	}
	/* Write Region */
	int32_t n = regions.size();
	msgBuffer.write_int32(n);
	for (int i = 0; i < n; ++i) {
		msgBuffer.write_int32(regions[i]);
	}
} /* end of serialize */

void 
BasicMessage::deserialize(MessageBuffer& msgBuffer) {
	/* Deserialize Extents */
	int32_t nb_extents;
	int32_t temp;
	msgBuffer.read_int32(&nb_extents);
	D[pdDebug] << "Deserialize " << nb_extents << " extent(s)" << endl;

	extents.clear();
	if (nb_extents > 0) {
		extents.reserve(nb_extents);
		int32_t nb_dimensions;
		msgBuffer.read_int32(&nb_dimensions);
		D[pdDebug] << "Extent with " << nb_dimensions << " range(s)" << endl;
		for (long i = 0; i < nb_extents; ++i) {
			Extent e(nb_dimensions);

			for (long h = 1; h <= nb_dimensions; ++h) {
				msgBuffer.read_int32(&temp);
				e.setRangeLowerBound(h, temp);
				msgBuffer.read_int32(&temp);
				e.setRangeUpperBound(h, temp);
			}
			extents.push_back(e);
		}
	}
	/* Deserialize Regions */
	int32_t n;
	msgBuffer.read_int32(&n);
	regions.clear();
	regions.reserve(n);
	for (int i = 0; i < n; ++i) {
		msgBuffer.read_int32(&temp);
		regions.push_back(temp);
	}

} /* end of serialize */

// ----------------------------------------------------------------------------
/** Write the 'extent' Message attribute into the body. Format : number of
    extents. If not zero, number of dimensions. The list of extents. Extent
    format: list of ranges. Range format: lower bound, upper bound.
 */
void
BasicMessage::writeExtents(MessageBody &body) const
{
    D[pdDebug] << "Write " << extents.size() << " extent(s)" << endl ;

    body.writeLongInt(extents.size());
    if (extents.size() > 0) {
	int n = extents[0].size();
	body.writeLongInt(n);
	D[pdDebug] << "Extent with " << n << " range(s)" << endl ;

	for (unsigned int i = 0 ; i < extents.size(); ++i) {
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
    D[pdDebug] << "Read " << nb_extents << " extent(s)" << endl ;

    extents.clear();    
    if (nb_extents > 0) {
	extents.reserve(nb_extents);
	long nb_dimensions = body.readLongInt();
	D[pdDebug] << "Extent with " << nb_dimensions << " range(s)" << endl ;
	for (long i = 0 ; i < nb_extents ; ++i) {
	    Extent e(nb_dimensions);
	    for (long h = 1 ; h <= nb_dimensions ; ++h) {
		e.setRangeLowerBound(h, body.readLongInt());
		e.setRangeUpperBound(h, body.readLongInt());
	    }
	    extents.push_back(e);
	}
    }
}

// ----------------------------------------------------------------------------
void
BasicMessage::writeRegions(MessageBody &body)
{
    long n = regions.size();
    body.writeLongInt(n);
    for (int i = 0 ; i < n ; ++i) {
	body.writeLongInt(regions[i]);
    }
}

// ----------------------------------------------------------------------------
void
BasicMessage::readRegions(const MessageBody &body)
{
    long n = body.readLongInt();
    regions.clear();
    regions.reserve(n);
    for (int i = 0; i < n; ++i) {
	regions.push_back(body.readLongInt());
    }
}

// ----------------------------------------------------------------------------
void
BasicMessage::setRegions(const BaseRegion *reg[], int size)
{
    regions.resize(size);
    
    for (int i = 0 ; i < size ; ++i) {
        regions[i] = reg[i]->getHandle();
    }
}

void
BasicMessage::setRegions(const std::vector<RegionHandle> &src)
{
    regions = src ;
}

// ----------------------------------------------------------------------------
const std::vector<RegionHandle> &
BasicMessage::getRegions() const
{
    return regions ;
}

} // namespace certi

// $Id: BasicMessage.cc,v 3.9.2.3 2008/04/22 08:41:07 erk Exp $
