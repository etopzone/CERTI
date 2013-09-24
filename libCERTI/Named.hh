// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2004  ONERA
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

#ifndef LIBCERTI_NAMED_HH
#define LIBCERTI_NAMED_HH

#include "certi.hh"

#include <string>

namespace certi {

/**
 * A base class used for CERTI named object.
 * This class should be used to handle HLA named object
 * the setName method should enforce HLA rules for naming
 * scheme as specified in
 * IEEE-1516.2-2000 (§3.3.1 Names)
 * "IEEE Standard for Modeling and Simulation (M&S) High Level
 *  Architecture (HLA)—Object Model Template (OMT) Specification"
 */
class CERTI_EXPORT Named
{
public:
	/**
	 * Default constructor.
	 * Name initialized with the empty string.
	 */
    Named();

    /**
     * Constructor with inital value.
     * @param[in] name Initial name
     */
    Named(const std::string& newName);

    /**
     * Virtual destructor.
     * Necessary for a class having virtual methods.
     */
    virtual ~Named();

    /**
     *  Set name.
     *  @param[in] name the new name
     */
    virtual void setName(const std::string& newName);

    /**
     * Get name.
     * @return name value
     */
    virtual const std::string& getName() const ;

    /**
     * Check if the name matches the provided parameter.
     * @param name String to compare
     * @return true if strings match
     */
    bool isNamed(const std::string& newName) const ;

	/**
	 * This method check if the provided name
	 * respects the HLA named object specifications, IEEE-1516.2-2000 (§3.3.1 Names).
	 * @param[in] name the name to be checked
	 * @return true if the name is a valid HLA name, false otherwise.
	 */
	static bool validateHLAName(const std::string& name);

	/**
	 * Check if the provided name is a qualified class name.
	 * Qualified HLA class name are those beginning at
	 * root class name. In a qualified class name each
	 * component is separated by a dot '.' like in:
	 * "ObjectRoot.Bille.PositionX"
	 * @param[in] name the name to be checked
	 * @return true if the name is qualified one
	 */
	static bool isQualifiedClassName(const std::string& name);

	/**
	 * Get next class name component.
	 * @param[in,out] qualifiedClassName
	 * @return the next (leading) class name
	 */
	static std::string getNextClassName(std::string& qualifiedClassName);

	/**
	 * Get leaf class name component.
	 * @param[in] qualifiedClassName
	 * @return the leaf class name
	 */
	static std::string getLeafClassName(const std::string& qualifiedClassName);

	class IsNamed {
		public:
			IsNamed(const std::string& named) : named(named) {};
			bool operator()(const Named& namedObject) {
				return (namedObject.getName() == named);
			}
		private:
			std::string named;
	};

protected:

    std::string name ;
};

} // certi

#endif // LIBCERTI_NAMED_HH
