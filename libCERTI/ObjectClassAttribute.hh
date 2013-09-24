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
// ----------------------------------------------------------------------------

#ifndef CERTI_OBJECT_CLASS_ATTRIBUTE_HH
#define CERTI_OBJECT_CLASS_ATTRIBUTE_HH

// forward declarations
namespace certi {
	class ObjectClassBroadcastList;
	class RTIRegion;
	class SecurityServer;
}  // namespace certi

// CERTI headers
#include "certi.hh"
#include "SecurityLevel.hh"
#include "Subscribable.hh"

#include <set>

namespace certi {

/*! This class descrives an object class attribute (handle, level id, ordering,
  transportation mode and name). This class also keeps track of published and
  subscribed federates.
*/
class CERTI_EXPORT ObjectClassAttribute : public Subscribable {

public:
    /**
     * A set of federate handles. The corresponding federates are publishers of
     * the object class attribute.
     */
    typedef std::set<FederateHandle> PublishersList_t;

    ObjectClassAttribute(const std::string& name, AttributeHandle attributeHandle);
    ObjectClassAttribute(const ObjectClassAttribute& objectClassAttribute);
    virtual ~ObjectClassAttribute();

    void display() const ;

    AttributeHandle getHandle() const ;

    void setSpace(SpaceHandle);
    SpaceHandle getSpace() const ;

    // Security methods
    virtual void checkFederateAccess(FederateHandle the_federate, const std::string& reason) const ;

    /**
     * Return true if specified federate is publisher of the attribute.
     * @param[in] federate the handle of the federate
     * @return true if the specified federate handle belongs to the set of publishers of this attribute
     *         false otherwise
     */
    bool isPublishing(FederateHandle federate) const;
    /**
     * Add the federate to the set of publishers of this attribute.
     * @param[in] federate the handle of the federate
     */
    void publish(FederateHandle federate) throw (RTIinternalError, SecurityError);

    /**
     * Remove the federate from the set of publishers of this attribute.
     * @param[in] federate the handle of the federate
     */
    void unpublish(FederateHandle) throw (RTIinternalError, SecurityError);

    // Update attribute values
    void updateBroadcastList(ObjectClassBroadcastList *ocb_list,
                             const RTIRegion *region = NULL);

    /**
     * Getter for the attributes publisher list.
     * param[out] PublishersList_t @see ObjectClassAttribute::PublisherList_t
     */
    const PublishersList_t& getPublishers(void) const { return publishers; }

    // Attributes
    SecurityLevelID level ;
    OrderType order ;
    TransportType transport ;
    SecurityServer *server ;

private:
	/*
	 * private default constructor with no code
	 * one should not call it.
	 */
	ObjectClassAttribute();

    void deletePublisher(FederateHandle);

    const AttributeHandle handle ; //!< The attribute handle.
    SpaceHandle space ; //!< Routing space

    PublishersList_t publishers ; //!< The publisher's list.
};

} // namespace

#endif // CERTI_OBJECT_CLASS_ATTRIBUTE_HH
