// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2014  ONERA
//
// This file is part of CERTI-libRTI
//
// CERTI-libRTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libRTI is distributed in the hope that it will be useful, but
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
#include "HandleImplementation.h"
#include <RTI/Handle.h>
#include <sstream>

namespace rti1516 {

#define DECLARE_HANDLE_CLASS(HandleKind)                                                                               \
                                                                                                                       \
    /* Constructs an invalid handle                           */                                                       \
    HandleKind::HandleKind() : _impl(0)                                                                                \
    {                                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    HandleKind::HandleKind(HandleKind##Implementation* impl) : _impl(0)                                                \
    {                                                                                                                  \
        _impl = impl;                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    HandleKind::HandleKind(VariableLengthData const& encodedValue) : _impl(0)                                          \
    {                                                                                                                  \
        _impl = new HandleKind##Implementation(encodedValue);                                                          \
    }                                                                                                                  \
                                                                                                                       \
    HandleKind::~HandleKind() throw()                                                                                  \
    {                                                                                                                  \
        delete _impl;                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    HandleKind::HandleKind(HandleKind const& rhs) : _impl(0)                                                           \
    {                                                                                                                  \
        if (rhs._impl != 0)                                                                                            \
            _impl = new HandleKind##Implementation(*rhs._impl);                                                        \
    }                                                                                                                  \
                                                                                                                       \
    HandleKind& HandleKind::operator=(HandleKind const& rhs)                                                           \
    {                                                                                                                  \
        if (this != &rhs) {                                                                                            \
            delete _impl;                                                                                              \
            if (0 != rhs._impl)                                                                                        \
                _impl = new HandleKind##Implementation(*(rhs._impl));                                                  \
            else                                                                                                       \
                _impl = 0;                                                                                             \
        }                                                                                                              \
        return *this;                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    /* Indicates whether this handle is valid                 */                                                       \
    bool HandleKind::isValid() const                                                                                   \
    {                                                                                                                  \
        if (_impl == 0)                                                                                                \
            return false;                                                                                              \
        else                                                                                                           \
            return _impl->isValid();                                                                                   \
    }                                                                                                                  \
                                                                                                                       \
    /* All invalid handles are equivalent                     */                                                       \
    bool HandleKind::operator==(HandleKind const& rhs) const                                                           \
    {                                                                                                                  \
        if (_impl == 0 || rhs.getImplementation() == 0)                                                                \
            return false;                                                                                              \
        else                                                                                                           \
            return ((*_impl) == (*rhs.getImplementation()));                                                           \
    }                                                                                                                  \
    bool HandleKind::operator!=(HandleKind const& rhs) const                                                           \
    {                                                                                                                  \
        if (_impl == 0 || rhs.getImplementation() == 0)                                                                \
            return false;                                                                                              \
        else                                                                                                           \
            return ((*_impl) != (*rhs.getImplementation()));                                                           \
    }                                                                                                                  \
    bool HandleKind::operator<(HandleKind const& rhs) const                                                            \
    {                                                                                                                  \
        if (_impl == 0 || rhs.getImplementation() == 0)                                                                \
            return false;                                                                                              \
        else                                                                                                           \
            return ((*_impl) < (*rhs.getImplementation()));                                                            \
    }                                                                                                                  \
                                                                                                                       \
    /* Generate an encoded value that can be used to send     */                                                       \
    /* handles to other federates in updates or interactions. */                                                       \
    VariableLengthData HandleKind::encode() const                                                                      \
    {                                                                                                                  \
        return _impl->encode();                                                                                        \
    }                                                                                                                  \
                                                                                                                       \
    /* Alternate encode for directly filling a buffer         */                                                       \
    unsigned long HandleKind::encodedLength() const                                                                    \
    {                                                                                                                  \
        return _impl->encodedLength();                                                                                 \
    }                                                                                                                  \
    unsigned long HandleKind::encode(void* buffer, unsigned long bufferSize) const throw(CouldNotEncode)               \
    {                                                                                                                  \
        return _impl->encode(buffer, bufferSize);                                                                      \
    }                                                                                                                  \
                                                                                                                       \
    std::wstring HandleKind::toString() const                                                                          \
    {                                                                                                                  \
        if (_impl == NULL)                                                                                             \
            return L"";                                                                                                \
        std::wstring implStr = _impl->toString();                                                                      \
        std::wstringstream ss;                                                                                         \
        ss << #HandleKind << "_" << implStr;                                                                           \
        return ss.str();                                                                                               \
    }                                                                                                                  \
                                                                                                                       \
    const HandleKind##Implementation* HandleKind::getImplementation() const                                            \
    {                                                                                                                  \
        return _impl;                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    HandleKind##Implementation* HandleKind::getImplementation()                                                        \
    {                                                                                                                  \
        return _impl;                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    /* Output operator for Handles          */                                                                         \
    std::wostream& operator<<(std::wostream& str, HandleKind const& /*h*/)                                             \
    {                                                                                                                  \
        return str;                                                                                                    \
    }                                                                                                                  \
/* end DECLARE_HANDLE_CLASS */

DECLARE_HANDLE_CLASS(FederateHandle)
DECLARE_HANDLE_CLASS(ObjectClassHandle)
DECLARE_HANDLE_CLASS(InteractionClassHandle)
DECLARE_HANDLE_CLASS(ObjectInstanceHandle)
DECLARE_HANDLE_CLASS(AttributeHandle)
DECLARE_HANDLE_CLASS(ParameterHandle)
DECLARE_HANDLE_CLASS(DimensionHandle)
DECLARE_HANDLE_CLASS(MessageRetractionHandle)
DECLARE_HANDLE_CLASS(RegionHandle)

} // end namespace rti1516
