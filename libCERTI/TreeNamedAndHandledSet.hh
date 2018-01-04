// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2008  ONERA
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
// $Id: TreeNamedAndHandledSet.hh,v 1.10 2009/11/19 18:15:31 erk Exp $
// ----------------------------------------------------------------------------

#ifndef _TreeNamedAndHandledSet_HH
#define _TreeNamedAndHandledSet_HH

// CERTI headers
#include "Named.hh"
#include <include/certi.hh>

// System headers
#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <string>

namespace certi {

/**
 * This templated class represents a set of objects whose type
 * is template ObjectType. The ObjectType type must have the
 * the following characteristics:
 * <ul>
 *   <li> object of this type must have a name which may
 *        be obtained with a
 *        "std::string ObjectType::getName() const"
 *        method,
 *        </li>
 *   <li> it must have an associated handle whose type is
 *        ObjectType::handle_t which may be retrieved by a
 *        "ObjectType::handle_t ObjectType::getHandle() const"
 *        method,
 *        </li>
 *   <li> the set of name should follow a "tree-like" structure
 *        for which the object named "Parent.Child" is the "Child"
 *        of object "Parent".
 *        </li>
 * </ul>
 * The TreeNamedAndHandledSet then build a two-way map
 * from handle to name and name to handle.
 */
template <typename ObjectType>
class TreeNamedAndHandledSet {
public:
    /**
     * The handle type should be defined embedded in the ObjectType itself.
     */
    typedef typename ObjectType::handle_t HandleType;
    /**
     * In the same way the exception which may be thrown
     * by most of the method should be defined embedded
     * in the ObjectType itself.
     */
    typedef typename ObjectType::ObjectNotDefinedException ObjectNotDefinedException;

    TreeNamedAndHandledSet(const std::string& setName, bool isRootSet = false);
    ~TreeNamedAndHandledSet();

    const std::string& getSetName() const
    {
        return setName;
    };

    /**
     * Add an object to the set and build parent <--> child relationship.
     * @param[in,out] child the object to be added
     *                the child will be linked to his parent.
     * @param[in,out] parent the parent object of the child
     *                the parent will get updated with a link to the
     *                new child.
     * @throw RTIinternalError the object cannot be stored in the set
     *                         may be because of a name collision
     */
    void add(ObjectType* child, ObjectType* parent = NULL);

    /**
     * Get the handle corresponding to the name.
     * @param[in] name the name of the object
     * @return the handle corresponding to the given name
     * @throw NameNotFound the name was not found in the set
     */
    HandleType getHandleFromName(const std::string& name) const;

    /**
     * Get the name corresponding to the handle.
     * @param[in] handle the handle of the object
     * @return the name corresponding to the given handle
     * @throw NameNotFound the handle was not found in the set
     */
    const std::string& getNameFromHandle(HandleType handle) const;

    /**
     * Get the object corresponding to the handle.
     * @param[in] handle the handle of the object
     * @return the pointer to object corresponding to the given handle
     * @throw NameNotFound the handle was not found in the set
     */
    ObjectType* getObjectFromHandle(HandleType handle) const;

    /**
     * Map from ObjectType::handle_t to ObjectType.
     */
    typedef std::map<HandleType, ObjectType*, std::less<HandleType>> Handle2ObjectMap_t;
    typedef typename Handle2ObjectMap_t::const_iterator handled_const_iterator;
    typedef typename Handle2ObjectMap_t::iterator handled_iterator;

    handled_const_iterator handled_begin() const
    {
        return fromHandle.begin();
    }

    handled_const_iterator handled_end() const
    {
        return fromHandle.end();
    }
    /**
     * Map from name to ObjectType::handle_t.
     */
    typedef std::map<std::string, ObjectType*, std::less<std::string>> Name2ObjectMap_t;
    typedef typename Name2ObjectMap_t::const_iterator named_const_iterator;
    typedef typename Name2ObjectMap_t::const_iterator const_iterator;
    typedef typename Name2ObjectMap_t::iterator iterator;

    named_const_iterator begin() const
    {
        return fromName.begin();
    }

    named_const_iterator end() const
    {
        return fromName.end();
    }

    /**
     * Return the size of the set, i.e.
     * the number of object in the set.
     * @return the size of the set
     */
    size_t size() const
    {
        return fromName.size();
    }

    /** Returns a set of all handles in this tree **/
    std::set<HandleType> handles()
    {
        std::set<HandleType> ret;
        for (const auto& pair : fromHandle) {
            ret.insert(pair.first);
        }
        return ret;
    }

    /** Returns a set of all names in this tree **/
    std::set<std::string> names()
    {
        std::set<std::string> ret;
        for (const auto& pair : fromName) {
            ret.insert(pair.first);
        }
        return ret;
    }

protected:
    Handle2ObjectMap_t fromHandle;
    Name2ObjectMap_t fromName;
    /*
     * The "Root" set is owning the object
     * and will destroy objects in its destructor.
     * Non "Root" set won't destroy anything beside itself.
     */
    bool isRootSet;
    /*
     * The name of the set.
     * Mainly used for displaying the set.
     */
    std::string setName;

private:
};

template <typename ObjectType>
TreeNamedAndHandledSet<ObjectType>::TreeNamedAndHandledSet(const std::string& theSetName, bool theIsRootSet)
{
    this->setName = theSetName;
    this->isRootSet = theIsRootSet;
} /* end of TreeNamedAndHandledSet (constructor) */

template <typename ObjectType>
TreeNamedAndHandledSet<ObjectType>::~TreeNamedAndHandledSet()
{
    /* clear name map */
    fromName.clear();
    /*
     * If we are Root Set (the class set owned by RootObject)
     *    we delete the content
     * If not we only clear the map in order to avoid double deletion.
     * The "Non Root" set are those who are not "owning"
     * the stored object.
     */
    if (isRootSet) {
        while (!fromHandle.empty()) {
            delete (fromHandle.begin()->second);
            fromHandle.erase(fromHandle.begin());
        }
    }
    else {
        fromHandle.clear();
    }
} /* end of ~TreeNamedAndHandledSet (destructor) */

template <typename ObjectType>
void TreeNamedAndHandledSet<ObjectType>::add(ObjectType* child, ObjectType* parent)
{
    // build hierarchical name if a parent is given
    if (parent) {
        std::string parentName = parent->getName();
        /* Inclusion or exclusion of those prefix is optional
         * see IEEE-1516.1-2000 - 10.1.1 Names
         * Do not build HLA root name in the hierarchical name
         */
        if (!((parentName == "ObjectRoot") || (parentName == "InteractionRoot") || (parentName == "HLAobjectRoot")
              || (parentName == "HLAinteractionRoot"))) {
            child->setName(parentName + "." + child->getName());
        }
        //std::cout << "Adding child :" << child->getName() << std::endl;
        parent->addSubClass(child);
    }

    // Check whether addition of this object class will generate a name collision or not.
    // i.e. we may not add an object class of the SAME name to the object class set
    auto findit = fromName.find(child->getName());
    if (findit != fromName.end()) {
        throw RTIinternalError("Name collision another object class named <" + child->getName() + "> with handle <"
                               + std::to_string(findit->second->getHandle())
                               + "> was found when trying to add identically named object class with handle <"
                               + std::to_string(child->getHandle())
                               + ">");
    }
    // store ref to new object in Object from Handle Map
    fromHandle[child->getHandle()] = child;

    // store ref to new object in Object from Name Map
    fromName[child->getName()] = child;
} /* end of add */

template <typename ObjectType>
typename TreeNamedAndHandledSet<ObjectType>::HandleType
TreeNamedAndHandledSet<ObjectType>::getHandleFromName(const std::string& name) const
{
    std::string sname;
    std::string prefix;

    sname = name;
    prefix = Named::getNextClassName(sname);
    /*
     * Inclusion or exclusion of those prefix is optional
     * see IEEE-1516.1-2000 - 10.1.1 Names
     * Do not build HLA root name in the hierarchical name
     */
    if (!((prefix == "ObjectRoot") || (prefix == "InteractionRoot") || (prefix == "HLAobjectRoot")
          || (prefix == "HLAinteractionRoot"))) {
        sname = name;
    }
    /*
     * First try to find the named object
     * This should be an efficient binary_search
     */
    named_const_iterator findit = fromName.find(sname);
    //std::cout << "Looking for " << sname << std::endl;
    /* If found return the handle */
    if (findit != fromName.end()) {
        return findit->second->getHandle();
    }

    /*
     * If not found then look for shortcut name
     * this is a CERTI non-standard behavior
     */
    if (!Named::isQualifiedClassName(sname)) {
        /* linear search in the whole set */
        for (findit = fromName.begin(); findit != fromName.end(); ++findit) {
            if (Named::getLeafClassName(findit->first) == sname) {
                return findit->second->getHandle();
            }
            else {
                //std::cout << Named::getLeafClassName(findit->first) << "- and -" << sname << " - do not match" << std::endl;
            }
        }
    }

    /* every search has failed */
    throw NameNotFound(name);
} /* end of getObjectClassHandle */

template <typename ObjectType>
const std::string& TreeNamedAndHandledSet<ObjectType>::getNameFromHandle(HandleType handle) const
{
    return getObjectFromHandle(handle)->getName();
} /* end of getNameFromHandle */

template <typename ObjectType>
ObjectType* TreeNamedAndHandledSet<ObjectType>::getObjectFromHandle(HandleType handle) const
{
    handled_const_iterator iter;

    iter = fromHandle.find(handle);

    if (iter != fromHandle.end()) {
        return iter->second;
    }
    else {
        std::stringstream msg;
        msg << "Unknown Object Handle <" << handle << ">";
        throw ObjectNotDefinedException(msg.str());
    }
} /* end of getObjectFromHandle */

template <typename ObjectType>
std::ostream& operator<<(std::ostream& os, const TreeNamedAndHandledSet<ObjectType>& set)
{
    os << set.getSetName() << " : " << set.size() << std::endl;
    for (const auto& element : set) {
        // FIXME currently the display method of ObjectClass
        // and Interaction is not <iostream> oriented.
        // will update that later
        element.second->display();
    }
    return os;
}

} // namespace certi

#endif // _TreeNamedAndHandledSet_HH
