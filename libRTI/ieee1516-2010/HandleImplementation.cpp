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
#include "RTIvariableLengthDataImplementation.h"
#include <RTI/Handle.h>
#include <climits>
#include <cstring>
#include <functional>
#include <sstream>
#include <stdint.h>

#include "PMurHash.h"

namespace rti1516e {

/* Constructs an invalid handle                           */
HandleImplementation::HandleImplementation() : _value(ULONG_MAX)
{
}

HandleImplementation::HandleImplementation(VariableLengthData const& encodedValue) : _value(ULONG_MAX)
{
    ULong val = 0;
    const size_t size = sizeof(val);
    unsigned char buf[size];

    if (encodedValue.size() != size) {
        throw CouldNotDecode(L"Encoded value has an unexpected size.");
    }

    memcpy(&val, encodedValue.data(), size);
    for (::uint32_t i = 0; i < sizeof(val); i++) {
        buf[size - i - 1] = (char) val & 0xFF;
        val = val >> 8;
    }

    //copy buffer to _value
    ULong newValue;
    memcpy(&newValue, buf, size);
    _value = newValue;
}

HandleImplementation::~HandleImplementation() throw()
{
}

HandleImplementation::HandleImplementation(HandleImplementation const& rhs) : _value(ULONG_MAX)
{
    _value = rhs._value;
}

/* Indicates whether this handle is valid                 */
bool HandleImplementation::isValid() const
{
    if (_value == ULONG_MAX)
        return false;
    else
        return true;
}

long HandleImplementation::hash() const
{
    long retval;
    //from <functional>
    // more details at http://www.cplusplus.com/reference/functional/hash/
    //should think about a non-c++11 approach if needed
    //  std::hash<std::wstring> localhash;
    //  return localhash(toString());

    // Use Portable MurmurHash implementation from http://code.google.com/p/smhasher/
    std::wstring strval = toString();
    // if long a 32 bits integer then hash is computed in one piece
    if (sizeof(long) == sizeof(uint32_t)) {
        retval = PMurHash32(0, strval.c_str(), strval.length());
    }
    // otherwise long is 64 bits so hash should be computed in two 32 bits pieces
    else {
        int l = strval.length();
        retval = PMurHash32(0, strval.c_str(), l / 2);
        retval |= (retval >> 32) | PMurHash32(0, strval.c_str() + l / 2, l - l / 2);
    }
    return retval;
}

/* Generate an encoded value that can be used to send     */
/* handles to other federates in updates or interactions. */
VariableLengthData HandleImplementation::encode() const
{
    unsigned char buf[sizeof(_value)];
    encode(buf, sizeof(_value));
    VariableLengthData c(buf, sizeof(_value));
    return c;
}

/* Alternate encode for directly filling a buffer         */
unsigned long HandleImplementation::encodedLength() const
{
    return sizeof(_value);
}
unsigned long HandleImplementation::encode(void* buffer, unsigned long bufferSize) const throw(CouldNotEncode)
{
    if (bufferSize < sizeof(_value))
        throw CouldNotEncode(L"Not enough room in buffer to encode handle");

    unsigned long val = _value;
    char* buf = (char*) buffer;
    for (uint32_t i = 0; i < sizeof(_value); i++) {
        buf[sizeof(_value) - i - 1] = (char) val & 0xFF;
        val = val >> 8;
    }
    return sizeof(_value);
}

std::wstring HandleImplementation::toString() const
{
    std::wostringstream ost;
    ost << _value;
    return ost.str();
}

#define DECLARE_HANDLE_IMPLEMENTATION_CLASS(HIK)                                                                       \
                                                                                                                       \
    /* Constructs an invalid handle */                                                                                 \
    HIK::HIK() : HandleImplementation()                                                                                \
    {                                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    HIK::HIK(VariableLengthData const& encodedValue) : HandleImplementation(encodedValue)                              \
    {                                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    HIK::~HIK() noexcept                                                                                               \
    {                                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    HIK::HIK(HIK const& rhs) : HandleImplementation()                                                                  \
    {                                                                                                                  \
        _value = rhs._value;                                                                                           \
    }                                                                                                                  \
                                                                                                                       \
    HIK& HIK::operator=(HIK const& rhs)                                                                                \
    {                                                                                                                  \
        if (this != &rhs) {                                                                                            \
            _value = rhs._value;                                                                                       \
        }                                                                                                              \
        return *this;                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    /* All invalid handles are equivalent */                                                                           \
    bool HIK::operator==(HIK const& rhs) const                                                                         \
    {                                                                                                                  \
        return _value == rhs._value;                                                                                   \
    }                                                                                                                  \
    bool HIK::operator!=(HIK const& rhs) const                                                                         \
    {                                                                                                                  \
        return _value != rhs._value;                                                                                   \
    }                                                                                                                  \
    bool HIK::operator<(HIK const& rhs) const                                                                          \
    {                                                                                                                  \
        return _value < rhs._value;                                                                                    \
    }                                                                                                                  \
                                                                                                                       \
/* end DECLARE_HANDLE_IMPLEMENTATION_CLASS */

DECLARE_HANDLE_IMPLEMENTATION_CLASS(FederateHandleImplementation)
DECLARE_HANDLE_IMPLEMENTATION_CLASS(ObjectClassHandleImplementation)
DECLARE_HANDLE_IMPLEMENTATION_CLASS(InteractionClassHandleImplementation)
DECLARE_HANDLE_IMPLEMENTATION_CLASS(ObjectInstanceHandleImplementation)
DECLARE_HANDLE_IMPLEMENTATION_CLASS(AttributeHandleImplementation)
DECLARE_HANDLE_IMPLEMENTATION_CLASS(ParameterHandleImplementation)
DECLARE_HANDLE_IMPLEMENTATION_CLASS(DimensionHandleImplementation)
//DECLARE_HANDLE_IMPLEMENTATION_CLASS(MessageRetractionHandleImplementation)
DECLARE_HANDLE_IMPLEMENTATION_CLASS(RegionHandleImplementation)

MessageRetractionHandleImplementation::MessageRetractionHandleImplementation() : HandleImplementation()
{
}

MessageRetractionHandleImplementation::MessageRetractionHandleImplementation(
    MessageRetractionHandleImplementation const& rhs)
    : HandleImplementation()
{
    _value = rhs._value;
}

MessageRetractionHandleImplementation::MessageRetractionHandleImplementation(VariableLengthData const& encodedValue)
{
    ULong val1 = 0;
    ULong val2 = 0;
    const size_t size = sizeof(val1);
    unsigned char buf1[size];
    unsigned char buf2[size];

    if (encodedValue.size() != 2 * size) {
        throw CouldNotDecode(L"Encoded value has an unexpected size.");
    }

    memcpy(&val1, encodedValue.data(), size);
    memcpy(&val2, (ULong*) encodedValue.data() + 1, size);

    // _value
    for (uint32_t i = 0; i < sizeof(val1); i++) {
        buf1[size - i - 1] = (char) val1 & 0xFF;
        val1 = val1 >> 8;
    }
    //copy buf1 to _value
    ULong newValue;
    memcpy(&newValue, buf1, size);
    _value = newValue;

    // _serialNum
    for (uint32_t i = 0; i < sizeof(val2); i++) {
        buf2[size - i - 1] = (char) val2 & 0xFF;
        val2 = val2 >> 8;
    }
    //copy buf2 to _serailNum
    ULong newSerialNum;
    memcpy(&newSerialNum, buf2, size);
    _serialNum = newSerialNum;
}

MessageRetractionHandleImplementation::~MessageRetractionHandleImplementation() noexcept
{
}

MessageRetractionHandleImplementation& MessageRetractionHandleImplementation::
operator=(MessageRetractionHandleImplementation const& rhs)
{
    if (this != &rhs) {
        _value = rhs._value;
    }
    return *this;
}

bool MessageRetractionHandleImplementation::operator==(MessageRetractionHandleImplementation const& rhs) const
{
    bool isEqual = (_value == rhs._value) && (_serialNum == rhs._serialNum);
    return isEqual;
}

bool MessageRetractionHandleImplementation::operator!=(MessageRetractionHandleImplementation const& rhs) const
{
    bool isEqual = (_value == rhs._value) && (_serialNum == rhs._serialNum);
    return !isEqual;
}

bool MessageRetractionHandleImplementation::operator<(MessageRetractionHandleImplementation const& rhs) const
{
    return _value < rhs._value;
}

/* Generate an encoded value that can be used to send     */
/* handles to other federates in updates or interactions. */
VariableLengthData MessageRetractionHandleImplementation::encode() const
{
    unsigned char buf[sizeof(_value) + sizeof(_serialNum)];
    encode(buf, sizeof(_value) + sizeof(_serialNum));
    VariableLengthData c(buf, sizeof(_value) + sizeof(_serialNum));
    return c;
}

/* Alternate encode for directly filling a buffer         */
unsigned long MessageRetractionHandleImplementation::encodedLength() const
{
    return sizeof(_value) + sizeof(_serialNum);
}

unsigned long MessageRetractionHandleImplementation::encode(void* buffer, unsigned long bufferSize) const
    throw(CouldNotEncode)
{
    if (bufferSize < (sizeof(_value) + sizeof(_serialNum)))
        throw CouldNotEncode(L"Not enough room in buffer to encode handle");

    unsigned long val = _value;
    unsigned long serialNum = _serialNum;
    char* buf = (char*) buffer;
    for (uint32_t i = 0; i < sizeof(_value); i++) {
        buf[sizeof(_value) - i - 1] = (char) val & 0xFF;
        val = val >> 8;
    }
    for (uint32_t i = 0; i < sizeof(_serialNum); i++) {
        buf[sizeof(_value) + sizeof(_serialNum) - i - 1] = (char) serialNum & 0xFF;
        serialNum = serialNum >> 8;
    }
    return sizeof(_value) + sizeof(_serialNum);
}

} // end namespace rti1516
