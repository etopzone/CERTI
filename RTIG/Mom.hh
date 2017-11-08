// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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
// ----------------------------------------------------------------------------

#ifndef _CERTI_RTIG_MOM_HH
#define _CERTI_RTIG_MOM_HH

#include <unordered_map>

#include "Federation.hh"
#include "RootObject.hh"

namespace certi {
    class RootObject;

namespace rtig {
    class Federation;

class Mom {
public:
    static bool isAvailableInRootObjectAndCompliant(const RootObject& root);
    
    Mom(const FederateHandle handle, Federation& federation, RootObject& root);
    
    FederateHandle getHandle() const;
    
    // Initialization
    void publishObjects();
    void publishAndSubscribeInteractions();
    void registerFederation();

    // Conditional Attributes
    void updateFederatesInFederation();
    void updateNextSave(const std::string& label, const FederationTime& time);

    // Support
    void provideAttributeValueUpdate(const ObjectHandle& object,
                                     const std::vector<AttributeHandle>& attributes);
    
private:
    void display() const;
    /** This handle is used to detect MOM interactions.
     * It does not really belong to the federation.
     */
    const FederateHandle my_handle;
    
    Federation& my_federation;
    RootObject& my_root;

    ObjectHandle my_federation_object;
    
    std::unordered_map<std::string, ObjectClassHandle> my_object_class_cache;
    std::unordered_map<std::string, AttributeHandle> my_attribute_cache;
    std::unordered_map<std::string, InteractionClassHandle> my_interaction_class_cache;

    std::unordered_map<AttributeHandle, AttributeValue_t> my_attribute_values_cache;

    AttributeValue_t valueFromBuffer(MessageBuffer& mb);
};

}
} // namespace certi/rtig

#endif // _CERTI_RTIG_MOM_HH
