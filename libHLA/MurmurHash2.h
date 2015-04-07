//-----------------------------------------------------------------------------
// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#ifndef _MURMURHASH2_H_
#define _MURMURHASH2_H_

//-----------------------------------------------------------------------------


#include "libhla.hh"

namespace libhla {
namespace hash {
//-----------------------------------------------------------------------------

HLA_EXPORT uint32_t MurmurHash2        ( const void * key, int len, uint32_t seed );
HLA_EXPORT uint64_t MurmurHash64A      ( const void * key, int len, uint64_t seed );
HLA_EXPORT uint64_t MurmurHash64B      ( const void * key, int len, uint64_t seed );
HLA_EXPORT uint32_t MurmurHash2A       ( const void * key, int len, uint32_t seed );
HLA_EXPORT uint32_t MurmurHashNeutral2 ( const void * key, int len, uint32_t seed );
HLA_EXPORT uint32_t MurmurHashAligned2 ( const void * key, int len, uint32_t seed );

//-----------------------------------------------------------------------------

} /* end of namespace hash */
} /* end of namespace libhla */

#endif // _MURMURHASH2_H_

