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
// $Id: libhla.hh,v 1.6 2011/04/26 11:07:38 erk Exp $
// ----------------------------------------------------------------------------

#ifndef LIBHLA_HH_INCLUDED
#define LIBHLA_HH_INCLUDED

#ifdef _MSC_VER
#ifndef FAKED_INTTYPES_DEFINED
#define FAKED_INTTYPES_DEFINED
typedef unsigned __int64  uint64_t;
typedef signed __int64    int64_t;
typedef unsigned __int32  uint32_t;
typedef signed __int32    int32_t;
typedef unsigned __int16  uint16_t;
typedef signed __int16    int16_t;
typedef unsigned __int8   uint8_t;
typedef signed __int8     int8_t;
typedef short int         int_least16_t;
#endif
#else
#include <inttypes.h>
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
    #pragma warning(disable: 4251)
    #define ANY_DLL_EXPORT __declspec(dllexport)
    #define ANY_DLL_IMPORT __declspec(dllimport)
    #define ANY_DLL_LOCAL
#else
    #if (__GNUC__ >= 4)
       #define ANY_DLL_EXPORT __attribute__ ((visibility("default")))
       #define ANY_DLL_IMPORT __attribute__ ((visibility("default")))
       #define ANY_DLL_LOCAL  __attribute__ ((visibility("hidden")))
    #else
       #define ANY_DLL_EXPORT
       #define ANY_DLL_IMPORT
       #define ANY_DLL_LOCAL
    #endif
#endif

#if defined(HLA_EXPORTS)
    #define HLA_EXPORT ANY_DLL_EXPORT
#else
    #define HLA_EXPORT ANY_DLL_IMPORT
#endif

#include <string>
#include <sstream>

#define LIBHLA_UINT64_SWAP_LE_BE_CONSTANT(val)	((uint64_t) ( \
      (((uint64_t) (val) &						\
	(uint64_t) LIBHLA_INT64_CONSTANT(0x00000000000000ffU)) << 56) |	\
      (((uint64_t) (val) &						\
	(uint64_t) LIBHLA_INT64_CONSTANT(0x000000000000ff00U)) << 40) |	\
      (((uint64_t) (val) &						\
	(uint64_t) LIBHLA_INT64_CONSTANT(0x0000000000ff0000U)) << 24) |	\
      (((uint64_t) (val) &						\
	(uint64_t) LIBHLA_INT64_CONSTANT(0x00000000ff000000U)) <<  8) |	\
      (((uint64_t) (val) &						\
	(uint64_t) LIBHLA_INT64_CONSTANT(0x000000ff00000000U)) >>  8) |	\
      (((uint64_t) (val) &						\
	(uint64_t) LIBHLA_INT64_CONSTANT(0x0000ff0000000000U)) >> 24) |	\
      (((uint64_t) (val) &						\
	(uint64_t) LIBHLA_INT64_CONSTANT(0x00ff000000000000U)) >> 40) |	\
      (((uint64_t) (val) &						\
	(uint64_t) LIBHLA_INT64_CONSTANT(0xff00000000000000U)) >> 56)))

#ifdef HOST_IS_BIG_ENDIAN
  #define LIBHLA_UINT64_SWAP_LE_BE(val) (LIBHLA_UINT64_SWAP_LE_BE_CONSTANT(val))
  #define LIBHLA_UINT64_TO_BE(val)	   (val)
  #define LIBHLA_UINT64_TO_LE(val)	   (LIBHLA_UINT64_SWAP_LE_BE (val))
#else
  #define LIBHLA_UINT64_SWAP_LE_BE(val) (LIBHLA_UINT64_SWAP_LE_BE_CONSTANT(val))
  #define LIBHLA_UINT64_TO_BE(val)	   (LIBHLA_UINT64_SWAP_LE_BE (val))
  #define LIBHLA_UINT64_TO_LE(val)      (val)
#endif

#define LIBHLA_UINT64_FROM_BE(val)	            (LIBHLA_UINT64_TO_BE(val))
#define LIBHLA_UINT64_FROM_LE(val)	            (LIBHLA_UINT64_TO_LE(val))
#define LIBHLA_ENCODE_DOUBLE_TO_UINT64BE(val)    (LIBHLA_UINT64_TO_BE   (*(uint64_t*)(val)))
#define LIBHLA_DECODE_DOUBLE_FROM_UINT64BE(val)  (LIBHLA_UINT64_FROM_BE (*(uint64_t*)(val)))

/*
 * Basic bit swapping functions
 */
#define LIBHLA_UINT16_SWAP_BYTES(val)	((uint16_t) ( \
    (((uint16_t) (val) & (uint16_t) 0x00ffU) << 8) |  \
    (((uint16_t) (val) & (uint16_t) 0xff00U) >> 8)))

#define LIBHLA_UINT32_SWAP_BYTES(val)	((uint32_t) (     \
    (((uint32_t) (val) & (uint32_t) 0x000000ffU) << 24) | \
    (((uint32_t) (val) & (uint32_t) 0x0000ff00U) <<  8) | \
    (((uint32_t) (val) & (uint32_t) 0x00ff0000U) >>  8) | \
    (((uint32_t) (val) & (uint32_t) 0xff000000U) >> 24)))

#define LIBHLA_UINT64_SWAP_BYTES(val)	((uint64_t) (                   \
      (((uint64_t) (val) &						\
	(uint64_t) LIBHLA_INT64_CONSTANT(0x00000000000000ffU)) << 56) |	\
      (((uint64_t) (val) &						\
	(uint64_t) LIBHLA_INT64_CONSTANT(0x000000000000ff00U)) << 40) |	\
      (((uint64_t) (val) &						\
	(uint64_t) LIBHLA_INT64_CONSTANT(0x0000000000ff0000U)) << 24) |	\
      (((uint64_t) (val) &						\
	(uint64_t) LIBHLA_INT64_CONSTANT(0x00000000ff000000U)) <<  8) |	\
      (((uint64_t) (val) &						\
	(uint64_t) LIBHLA_INT64_CONSTANT(0x000000ff00000000U)) >>  8) |	\
      (((uint64_t) (val) &						\
	(uint64_t) LIBHLA_INT64_CONSTANT(0x0000ff0000000000U)) >> 24) |	\
      (((uint64_t) (val) &						\
	(uint64_t) LIBHLA_INT64_CONSTANT(0x00ff000000000000U)) >> 40) |	\
      (((uint64_t) (val) &						\
	(uint64_t) LIBHLA_INT64_CONSTANT(0xff00000000000000U)) >> 56)))

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

class HLA_EXPORT Exception
{
public:
    const std::string _reason;
    const char         *_name;

    Exception(const std::string &reason)
        : _reason(reason) { }
    virtual ~Exception() { }
};

#define LIBHLA_EXCEPTION(A) \
    class HLA_EXPORT A : public Exception { \
    public: \
        A(const std::string &reason) : Exception(reason) { _name = #A;} \
};
} // namespace libhla

#endif // LIBHLA_HH_INCLUDED

