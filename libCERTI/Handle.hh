// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003  ONERA
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

#ifndef _CERTI_HANDLE_HH
#define _CERTI_HANDLE_HH

#include <cstdint>

#include "StrongType.hh"

namespace certi {

typedef uint32_t Handle;

typedef Handle FederateHandle;
// // // // //     using FederateHandle = NamedType<Handle, struct FederateHandleParameter>;
//         typedef Handle   FederationHandle;
using FederationHandle
    = NamedType<Handle, struct FederationHandleTypeParameter, Comparable, Printable, Hashable, Invalidable>;

typedef Handle ObjectClassHandle;
typedef Handle InteractionClassHandle;
typedef Handle AttributeHandle;
typedef Handle ParameterHandle;
typedef Handle ObjectHandle;
typedef Handle DimensionHandle;
typedef Handle SpaceHandle;
typedef Handle EventRetractionHandle;
typedef Handle RegionHandle;
}

#endif // _CERTI_HANDLE_HH
