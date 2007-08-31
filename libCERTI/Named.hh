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
// $Id: Named.hh,v 3.2 2007/08/31 12:47:40 erk Exp $
// ----------------------------------------------------------------------------

#ifndef LIBCERTI_NAMED_HH
#define LIBCERTI_NAMED_HH

#include <string>

namespace certi {

/**
 * A based class used for CERTI named object.
 */
class Named
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
    Named(const std::string name);

    /**
     *  Set name.
     *  @param[in] name the new name
     */
    void setName(const std::string name);
    
    /**
     *  Set name (char* version)
     *  @param[in] name the new name
     */
    void setName(const char* name);
    
    /** 
     * Get name.
     * @return name value
     */
    const std::string getName() const ;
    
    /** 
     * Get C-style name.
     * @return pointer on name, as const char *
     */
    const char *getCName() const ;
    
    /** 
     * Check if the name matches the provided parameter.
     * @param name String to compare
     * @return true if strings match
     */
    bool isNamed(const std::string& name) const ;

protected:
    std::string name ;
};

} // certi

#endif // LIBCERTI_NAMED_HH

// $Id: Named.hh,v 3.2 2007/08/31 12:47:40 erk Exp $
