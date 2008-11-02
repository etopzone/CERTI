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
// $Id: TreeNamedAndHandledSet.hh,v 1.2 2008/11/02 00:26:41 erk Exp $
// ----------------------------------------------------------------------------

#ifndef _TreeNamedAndHandledSet_HH
#define _TreeNamedAndHandledSet_HH

// CERTI headers
#include "certi.hh"
#include "Named.hh"

// System headers
#include <string>
#include <map>

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
template<typename ObjectType>
class CERTI_EXPORT TreeNamedAndHandledSet
{

public:
	/**
	 * The handle type should be defined embedded in the ObjectType itself.
	 */
	typedef typename ObjectType::handle_t            HandleType;
	/**
	 * In the same way the exception which may be thrown
	 * by most of the method should be defined embedded
	 * in the ObjectType itself.
	 */
	typedef typename ObjectType::ObjectNotDefinedException ObjectNotDefinedException;

	TreeNamedAndHandledSet(bool isRootSet=false);
	~TreeNamedAndHandledSet();

	/**
	 * Add an object to the set.
	 * @param[in] object the object to be added
	 * @throw RTIinternalError the object cannot be stored in the set
	 *                         may be because of a name collision
	 */
	void add(ObjectType *object) throw (RTIinternalError);

	/**
	 * Build inheritance relation between two objects class.
	 * @param[in,out] child the future child object class
	 * @param[in,out] parent the parent object class
	 * @post the child and parent object classes are linked
	 *       with inheritance relationship.
	 */
	void buildParentRelation(ObjectType *child, ObjectType *parent);

	/**
	 * Get the handle corresponding to the name.
	 * @param[in] name the name of the object
	 * @return the handle corresponding to the given name
	 * @throw NameNotFound the name was not found in the set
	 */
	HandleType getHandleFromName(const std::string name) const
	throw (NameNotFound);

	/**
	 * Get the name corresponding to the handle.
	 * @param[in] handle the handle of the object
	 * @return the name corresponding to the given handle
	 * @throw NameNotFound the handle was not found in the set
	 */
	std::string getNameFromHandle(HandleType handle) const
	throw (ObjectNotDefinedException);

	/**
	 * Get the object corresponding to the handle.
	 * @param[in] handle the handle of the object
	 * @return the pointer to object corresponding to the given handle
	 * @throw NameNotFound the handle was not found in the set
	 */
	ObjectType* getObjectFromHandle(HandleType handle) const
	throw (ObjectNotDefinedException);

	/**
	 * Map from ObjectType::handle_t to ObjectType.
	 */
	typedef std::map<HandleType,ObjectType*,std::less<HandleType> > Handle2ObjectMap_t;
	typedef typename Handle2ObjectMap_t::const_iterator handled_const_iterator;
	typedef typename Handle2ObjectMap_t::iterator       handled_iterator;

	handled_const_iterator handled_begin() const {
		return fromHandle.begin();
	}

	handled_const_iterator handled_end() const {
		return fromHandle.end();
	}
	/**
	 * Map from name to ObjectType::handle_t.
	 */
	typedef std::map<std::string,ObjectType*,std::less<std::string> > Name2ObjectMap_t;
	typedef typename Name2ObjectMap_t::const_iterator named_const_iterator;
	typedef typename Name2ObjectMap_t::const_iterator const_iterator;
	typedef typename Name2ObjectMap_t::iterator       iterator;

	named_const_iterator begin() const {
		return fromName.begin();
	}

	named_const_iterator end() const {
		return fromName.end();
	}

	/**
	 * Return the size of the set, i.e.
	 * the number of object in the set.
	 * @return the size of the set
	 */
	const size_t size() {return fromName.size();}

protected:
	Handle2ObjectMap_t fromHandle;
	Name2ObjectMap_t   fromName;
	/* The "Root" set is owning the object
	 * and will destroy objects in its destructor.
	 * Non "Root" set won't destroy anything beside itself.
	 */
	bool               isRootSet;
};

template <typename ObjectType>
TreeNamedAndHandledSet<ObjectType>::TreeNamedAndHandledSet(bool isRootSet) {
   this->isRootSet = isRootSet;
} /* end of TreeNamedAndHandledSet (constructor) */

template <typename ObjectType>
TreeNamedAndHandledSet<ObjectType>::~TreeNamedAndHandledSet() {
	/* clear name map */
	fromName.clear();
	/*
	 * If we are Root Set (the class set owned by RootObject)
	 *    we delete the content
	 * If not we only clear the map in order to avoid double deletion.
	 *
	 * FIXME EN: this is a trick in order because we do not
	 *           really maintain a tree of ObjectClass in order
	 *           to support flat object class name
	 *           ("Boule" instead of "Bille.Boule")
	 *           We may get rid of this as soon as we want to support
	 *           same name for object class in different branch of the tree.
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
void
TreeNamedAndHandledSet<ObjectType>::add(ObjectType *object)
	throw (RTIinternalError) {
	typename Name2ObjectMap_t::iterator findit;
	std::stringstream             msg;

    /*
     * Check whether addition of this object class
     * will generate a name collision or not.
     * i.e. we may not add an object class of the SAME
     * name to the object class set
     */
    findit = fromName.find(object->getName());
    if (findit != fromName.end()) {
    	msg << "Name collision another object class named <"
    	    << object->getName()
    	    << "> with handle <"
    	    << findit->second->getHandle()
    	    << "> was found when trying to add identically named object class with handle <"
    	    << object->getHandle();
    	throw RTIinternalError(msg.str().c_str());
    }
    /* store ref to new object in Object from Handle Map */
    fromHandle[object->getHandle()] = object;
    /* store ref to new object in Object from Name Map */
    fromName[object->getName()] = object;
} /* end of add */

template <typename ObjectType>
void
TreeNamedAndHandledSet<ObjectType>::buildParentRelation(ObjectType* child, ObjectType* parent) {
    child->setSuperclass(parent->getHandle());
    child->setSecurityLevelId(parent->getSecurityLevelId());
    parent->addSubclass(child);
    parent->addToChild(child);
}

template <typename ObjectType>
typename TreeNamedAndHandledSet<ObjectType>::HandleType
TreeNamedAndHandledSet<ObjectType>::getHandleFromName(std::string name) const
    throw (NameNotFound) {

    std::string                                currentName;
    std::string                              remainingName;
    HandleType                               currentHandle;
    ObjectType*                              currentObject;
    TreeNamedAndHandledSet<ObjectType> const*   currentSet;
	named_const_iterator                              iter;

	currentSet    = this;
	remainingName = name;
    /*
     * If the name is qualified (a.k.a. hierarchical name) like "Bille.Boule"
     * then iterate through subClass in order to reach the leaf "unqualified name"
     */
    while (Named::isQualifiedClassName(remainingName)) {
    	/*
    	 * The first current should be the name of
    	 * of a child of the current set
    	 */
    	currentName = Named::getNextClassName(remainingName);
		/*
		 * Get the handle of the child
		 * NOTE that we won't recurse more than once here
		 * since the provided 'currentName' is not qualified 'by design'
		 * The recursive deepness is at most 2.
		 */
		currentHandle = currentSet->getHandleFromName(currentName);
		/* Get the corresponding object */
		currentObject = currentSet->getObjectFromHandle(currentHandle);
		/* now update currentClassSet */
		currentSet    = currentObject->getSubClasses();
    }

    /*
     * Now the current classClassSet should be a leaf
     * so that we can search in the
     */
    iter = currentSet->fromName.find(remainingName);

	if (iter != currentSet->fromName.end()) {
		return iter->second->getHandle();
	} else {
		throw NameNotFound(name.c_str());
	}
} /* end of getObjectClassHandle */

template <typename ObjectType>
std::string
TreeNamedAndHandledSet<ObjectType>::getNameFromHandle(HandleType handle) const
throw (ObjectNotDefinedException) {

    return getObjectFromHandle(handle)->getName();
} /* end of getNameFromHandle */

template <typename ObjectType>
ObjectType*
TreeNamedAndHandledSet<ObjectType>::getObjectFromHandle(HandleType handle) const
	throw (ObjectNotDefinedException) {

	std::stringstream       msg;
	handled_const_iterator iter;

	iter = fromHandle.find(handle);

	if (iter != fromHandle.end()) {
		return iter->second;
	} else {
        msg << "Unknown Object Handle <" << handle << ">";
		throw ObjectNotDefinedException(msg.str().c_str());
	}
} /* end of getObjectFromHandle */

} // namespace certi



#endif // _TreeNamedAndHandledSet_HH

