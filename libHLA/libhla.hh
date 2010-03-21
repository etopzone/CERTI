// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This program is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// $Id: libhla.hh,v 1.1 2010/03/21 17:10:10 erk Exp $
// ----------------------------------------------------------------------------

#ifndef LIBHLA_HH_INCLUDED
#define LIBHLA_HH_INCLUDED

#ifdef _MSC_VER
typedef unsigned __int64  uint64_t;
typedef signed __int64    int64_t;
typedef unsigned __int32  uint32_t;
typedef signed __int32    int32_t;
typedef unsigned __int16  uint16_t;
typedef signed __int16    int16_t;
typedef unsigned __int8   uint8_t;
typedef signed __int8     int8_t;
#else
#include <inttypes.h>
#endif

#if defined(_WIN32)
    #pragma warning(disable: 4251)
    #if defined(HLA_EXPORTS)
        #define HLA_EXPORT __declspec(dllexport)
    #else
        #define HLA_EXPORT __declspec(dllimport)
    #endif
#else
    #define HLA_EXPORT
#endif

#include <string>
#include <sstream>

/**
 * @defgroup libHLA The libHLA library.
 * @ingroup CERTI_Libraries
 * The HLA library contains RTI-agnostic functions and classes
 * that may be used by a federate application.
 */
namespace libhla {

/**
 * Helper class to simplify string construction. Implemented as
 * a stringstream wrapper.
 *
 * For example:
 * throw AttributeNotDefined(stringize() << "value: " << number);
 */
template<typename C>
struct basic_stringize
{
  template<typename T>
  basic_stringize<C> & operator << (const T& t)
  {
    m_s << t;
    return *this;
  }

  // note: must not return reference
  operator const std::basic_string<C>() const
  {
    return m_s.str();
  }

private:
  std::basic_stringstream<C> m_s;
};

typedef basic_stringize<char> stringize;
typedef basic_stringize<wchar_t> wstringize;

} // namespace libhla

#endif // LIBHLA_HH_INCLUDED

