// ----------------------------------------------------------------------------
// HLAenumeratedType.hh - IEEE 1516.2 compliant datatypes
// Copyright (C) 2008  Petr Gotthard <petr.gotthard@centrum.cz>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1, as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// $Id: HLAenumeratedType.hh,v 1.3 2009/06/24 12:33:31 gotthardp Exp $
// ----------------------------------------------------------------------------

#ifndef _HLATYPES_ENUMERATEDTYPE_HH
#define _HLATYPES_ENUMERATEDTYPE_HH

namespace libhla {

/**
 * @page certi_HLAenumeratedType Enumeration Types
 *
 * The template
\verbatim
 HLAenumeratedType<ENUMERATION, REPRESENTATION>
\endverbatim
 * defines an user-convenient ENUMERATION stored using given REPRESENTATION.
 *
 * The data can be accessed in an usual way.
 *
 * Some models may use one enumerated value in multiple enumerations. To avoid
 * name collisions it's recommended to put the ENUMERATION in an individual namespace.
 *
 * For example:
\verbatim
 +------------+----------------+------------+--------+-----------+
 | Name       | Representation | Enumerator | Values | Semantics |
 +------------+----------------+------------+--------+-----------+
 |            |                | HLAfalse   | 0      |           |
 | HLAboolean | HLAinteger32BE +------------+--------+-----------+
 |            |                | HLAfalse   | 1      |           |
 +------------+----------------+------------+--------+-----------+
 
 namespace __HLAboolean {
 enum __enum {
   HLAfalse = 0,
   HLAtrue = 1
 };
 }
 typedef HLAenumeratedType<__HLAboolean::__enum, HLAinteger32BE> HLAboolean;
 HLAdata<HLAboolean> value;

 value = HLAtrue;
\endverbatim
 */

// HLA enumerated type, enumeration <E> with representation <R>.
template<class E, class R>
struct HLAenumeratedType
{
    HLAenumeratedType& operator = (const E& data)
    {
        *(R*)this = data;
        return *this;
    }

    HLAenumeratedType& operator = (const int& data)
    {
        *(R*)this = data;
        return *this;
    }

    operator E() const
    {
        int result = *(R*)this;
        return (E)result;
    }

    operator int() const
    { return *(R*)this; }

    static const size_t emptysizeof()
    { return R::emptysizeof(); }

    static const size_t __sizeof()
    { return R::__sizeof(); }

    void copy(void* source)
    { ((R*)this)->copy(source); }

    static const size_t m_octetBoundary = R::m_octetBoundary;
    static const bool m_isVariable = false;
};

/* IEEE 1516.2, Table 27:
 * Enumerated datatype table
 */
enum __HLAboolean {
    HLAfalse = 0,
    HLAtrue = 1
};
typedef HLAenumeratedType<__HLAboolean, HLAinteger32BE> HLAboolean;

} // namespace libhla

#endif // _HLATYPES_ENUMERATEDTYPE_HH

// $Id: HLAenumeratedType.hh,v 1.3 2009/06/24 12:33:31 gotthardp Exp $

