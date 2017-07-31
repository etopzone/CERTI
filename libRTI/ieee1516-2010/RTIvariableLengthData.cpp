#include <RTI/VariableLengthData.h>
#include "RTIvariableLengthDataImplementation.h"
#include <stdlib.h>
#include <string.h>

namespace rti1516e
{
	// ******************************************************
	// ********* VariableLengthData implementation **********
	// ******************************************************

	VariableLengthData::VariableLengthData()
		: _impl(0)
	{
	}

	// Caller is free to delete inData after the call
	VariableLengthData::VariableLengthData(void const * inData, size_t inSize)
		: _impl(0)
	{
		_impl = new VariableLengthDataImplementation(inData, inSize);
	}

	// Caller is free to delete rhs after the call
	VariableLengthData::VariableLengthData(VariableLengthData const & rhs)
		: _impl(0)
	{
		_impl = new VariableLengthDataImplementation(*(rhs._impl));
	}

	VariableLengthData::~VariableLengthData()
	{
		delete _impl;
	}

	VariableLengthData &
		VariableLengthData::operator=(VariableLengthData const & rhs)
	{
		// Beware of self assignment
		if (this != &rhs)
		{
			delete _impl;
			_impl = new VariableLengthDataImplementation(*(rhs._impl));
		}

		return *this;
	}

	void const * 
		VariableLengthData::data() const
	{
		if (_impl != 0)
		{
			return _impl->getData();
		} else
		{
			return 0;
		}
	}
	size_t 
		VariableLengthData::size() const
	{
		if (_impl != 0)
		{
			return _impl->getSize();
		} else
		{
			return 0;
		}
	}

	// Caller is free to delete inData after the call
	void VariableLengthData::setData(void const * inData, size_t inSize)
	{
		if (_impl != 0)
		{
			_impl->setData(inData, inSize);
		} else
		{
			_impl = new VariableLengthDataImplementation(inData, inSize);
		}
	}

	// Caller is responsible for ensuring that the data that is 
	// pointed to is valid for the lifetime of this object, or past
	// the next time this object is given new data.
	void VariableLengthData::setDataPointer(void* inData, size_t inSize)
	{
		if (_impl == 0)
		{
			_impl = new VariableLengthDataImplementation();
		}
		_impl->setDataPointer(inData, inSize);
	}

	// Caller gives up ownership of inData to this object.
	// This object assumes the responsibility of deleting inData
	// when it is no longer needed.
	// The allocation of inData is assumed to have been through an array
	// alloctor (e.g., char* data = new char[20]. If the data was allocated
	// in some other fashion, a deletion function must be supplied.
	void VariableLengthData::takeDataPointer(void* inData, size_t inSize, VariableLengthDataDeleteFunction /*func*/)
	{
		if (_impl == 0)
		{
			_impl = new VariableLengthDataImplementation();
		}
		_impl->takeDataPointer(inData, inSize);
	}

	// ********************************************************************
	// ********* VariableLengthDataImplementation implementation **********
	// ********************************************************************

	VariableLengthDataImplementation::VariableLengthDataImplementation()
		: _data(0)
		, _size(0)
		, _dataOwner(false)
	{
	}

	// Caller is free to delete inData after the call
	VariableLengthDataImplementation::VariableLengthDataImplementation(void const * inData, size_t inSize)
		: _data(0)
		, _size(inSize)
		, _dataOwner(true)
	{
		_data = malloc(inSize+1);
		memcpy(_data, inData, _size);
	}

	// Caller is free to delete rhs after the call
	VariableLengthDataImplementation::VariableLengthDataImplementation(VariableLengthDataImplementation const & rhs)
		: _data(0)
		, _size(rhs._size)
		, _dataOwner(true)
	{
		_data = malloc(rhs._size);
		memcpy(_data, rhs._data, _size);
	}

	VariableLengthDataImplementation::~VariableLengthDataImplementation()
	{
		if (_data && _dataOwner)
		{
			free(_data);
		}
	}

	// Caller is free to delete rhs after the call
	VariableLengthDataImplementation &
		VariableLengthDataImplementation::operator=(VariableLengthDataImplementation const & rhs)
	{
		// Beware of self assignment
		if (this != &rhs)
		{
			setData(rhs._data, rhs._size);
		}

		return *this;
	}

	// Caller is free to delete inData after the call
	void VariableLengthDataImplementation::setData(void const * inData, unsigned long inSize)
	{
			if (_data && _dataOwner)
			{
				free(_data);
			}
			_size = inSize;
			_dataOwner = true;
			_data = malloc(_size);
			memcpy(_data, inData, _size);
	}

	// Caller is responsible for ensuring that the data that is 
	// pointed to is valid for the lifetime of this object, or past
	// the next time this object is given new data.
	void VariableLengthDataImplementation::setDataPointer(void* inData, unsigned long inSize)
	{
			if (_data && _dataOwner)
			{
				free(_data);
			}
			_size = inSize;
			_dataOwner = false;
			_data = inData;
	}

	// Caller gives up ownership of inData to this object.
	// This object assumes the responsibility of deleting inData
	// when it is no longer needed.
	void VariableLengthDataImplementation::takeDataPointer(void* inData, unsigned long inSize)
	{
			if (_data && _dataOwner)
			{
				free(_data);
			}
			_size = inSize;
			_dataOwner = true;
			_data = inData;
	}


} // end namespace rti1516e
