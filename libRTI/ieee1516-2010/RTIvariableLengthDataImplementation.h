#ifndef RTI_VariableLengthDataImplementation_h
#define RTI_VariableLengthDataImplementation_h

namespace rti1516e
{
class VariableLengthDataImplementation
{
public:
    VariableLengthDataImplementation();
    // Caller is free to delete inData after the call
    VariableLengthDataImplementation(void const * inData, size_t inSize);
    // Caller is free to delete rhs after the call
    VariableLengthDataImplementation(VariableLengthDataImplementation const & rhs);

    ~VariableLengthDataImplementation();

    // Caller is free to delete rhs after the call
    VariableLengthDataImplementation &
    operator=(VariableLengthDataImplementation const & rhs);

    // Caller is free to delete inData after the call
    void setData(void const * inData, unsigned long inSize);

    // Caller is responsible for ensuring that the data that is
    // pointed to is valid for the lifetime of this object, or past
    // the next time this object is given new data.
    void setDataPointer(void* inData, unsigned long inSize);

    // Caller gives up ownership of inData to this object.
    // This object assumes the responsibility of deleting inData
    // when it is no longer needed.
    void takeDataPointer(void* inData, unsigned long inSize);

    void const *getData() { return _data; }
    unsigned long getSize() { return _size; }

private:
    void *_data;
    unsigned long _size;
    bool _dataOwner;
};
}

#endif // RTI_VariableLengthDataImplementation_h
