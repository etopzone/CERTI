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
#ifndef HandleImplementation_h
#define HandleImplementation_h

#include <RTI/Exception.h>
#include <RTI/SpecificConfig.h>
#include <RTI/VariableLengthData.h>
#include <string>

// TODO Text below should be fixed. (Copied from Handle definition macro.)
// The following macro is used to define each of the Handle classes
// that are used by the RTI's API, e.g. AttributeHandle, ParameterHandle, etc.
// Each kind of handle contains the same set of operators and functions, but
// each is a separate class for type safety.  The encode method can be used to
// generate an encoded value for a handle, that can be sent to other federates
// as an attribute or parameter.  (Use RTIambassador functions to reconstruct a
// handle from an encoded value).  RTI implementations contain definitions
// for each kind of the HandleKindImplementation classes (e.g.
// AttributeHandleImplementation), but these classes are not needed by
// federate code.

typedef unsigned long ULong;

namespace rti1516 {
class RTI_EXPORT HandleImplementation {
protected:
    /* Constructs an invalid handle                           */
    HandleImplementation();

public:
    HandleImplementation(HandleImplementation const& rhs);

    explicit HandleImplementation(VariableLengthData const& encodedValue);

    virtual ~HandleImplementation() throw();

    /* Indicates whether this handle is valid                 */
    virtual bool isValid() const;

    /* Generate an encoded value that can be used to send     */
    /* handles to other federates in updates or interactions. */
    virtual VariableLengthData encode() const;

    /* Alternate encode for directly filling a buffer         */
    virtual unsigned long encodedLength() const;
    virtual unsigned long encode(void* buffer, unsigned long bufferSize) const throw(CouldNotEncode);

    virtual std::wstring toString() const;

    ULong getValue() const
    {
        return _value;
    }

    void setValue(ULong val)
    {
        _value = val;
    }

protected:
    ULong _value;
};
}

#define DEFINE_HANDLE_IMPLEMENTATION_CLASS(HandleKind)                                                                 \
                                                                                                                       \
    /* Each handle class generated by this macro provides the following interface */                                   \
    class RTI_EXPORT HandleKind##Implementation : public HandleImplementation {                                        \
    public:                                                                                                            \
        /* Constructs an invalid handle */                                                                             \
        HandleKind##Implementation();                                                                                  \
                                                                                                                       \
        HandleKind##Implementation(HandleKind##Implementation const& rhs);                                             \
                                                                                                                       \
        explicit HandleKind##Implementation(VariableLengthData const& encodedValue);                                   \
                                                                                                                       \
        virtual ~HandleKind##Implementation() noexcept;                                                                \
                                                                                                                       \
        HandleKind##Implementation& operator=(HandleKind##Implementation const& rhs);                                  \
                                                                                                                       \
        /* All invalid handles are equivalent */                                                                       \
        virtual bool operator==(HandleKind##Implementation const& rhs) const;                                          \
        virtual bool operator!=(HandleKind##Implementation const& rhs) const;                                          \
        virtual bool operator<(HandleKind##Implementation const& rhs) const;                                           \
    };

namespace rti1516 {

// All of the RTI API's Handle classes are defined
// by invoking the macro above.
DEFINE_HANDLE_IMPLEMENTATION_CLASS(FederateHandle)
DEFINE_HANDLE_IMPLEMENTATION_CLASS(ObjectClassHandle)
DEFINE_HANDLE_IMPLEMENTATION_CLASS(InteractionClassHandle)
DEFINE_HANDLE_IMPLEMENTATION_CLASS(ObjectInstanceHandle)
DEFINE_HANDLE_IMPLEMENTATION_CLASS(AttributeHandle)
DEFINE_HANDLE_IMPLEMENTATION_CLASS(ParameterHandle)
DEFINE_HANDLE_IMPLEMENTATION_CLASS(DimensionHandle)
DEFINE_HANDLE_IMPLEMENTATION_CLASS(RegionHandle)

class MessageRetractionHandleImplementation : public HandleImplementation {
public:
    /* Constructs an invalid handle                           */
    MessageRetractionHandleImplementation();

    MessageRetractionHandleImplementation(MessageRetractionHandleImplementation const& rhs);

    explicit MessageRetractionHandleImplementation(VariableLengthData const& encodedValue);

    virtual ~MessageRetractionHandleImplementation() throw();

    virtual MessageRetractionHandleImplementation& operator=(MessageRetractionHandleImplementation const& rhs);

    /* All invalid handles are equivalent                     */
    virtual bool operator==(MessageRetractionHandleImplementation const& rhs) const;
    virtual bool operator!=(MessageRetractionHandleImplementation const& rhs) const;
    virtual bool operator<(MessageRetractionHandleImplementation const& rhs) const;

    /* Generate an encoded value that can be used to send     */
    /* handles to other federates in updates or interactions. */
    virtual VariableLengthData encode() const;

    /* Alternate encode for directly filling a buffer         */
    virtual unsigned long encodedLength() const;
    virtual unsigned long encode(void* buffer, unsigned long bufferSize) const throw(CouldNotEncode);

    ULong getSerialNum() const
    {
        return _serialNum;
    }

    void setSerialNum(ULong sn)
    {
        _serialNum = sn;
    }

protected:
    ULong _serialNum;
};
}

#endif // RTI_HandleImplementation_h
