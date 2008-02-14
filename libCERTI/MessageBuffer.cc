// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005 ONERA
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// ----------------------------------------------------------------------------

#include "MessageBuffer.hh"
#include <cassert>
#include <sstream>

namespace certi {

void MessageBuffer::initialize() {
	buffer = NULL;
	bufferMaxSize = DEFAULT_MESSAGE_BUFFER_SIZE;
	bufferHasMyEndianness = true;
	writeOffset = 0;
	readOffset = 0;
} /* end of MessageBuffer::initialize() */

MessageBuffer::MessageBuffer() {
	initialize();
	reallocate(DEFAULT_MESSAGE_BUFFER_SIZE);
} /* end of MessageBuffer::MessageBuffer() */

MessageBuffer::MessageBuffer(uint32_t n) {
	initialize();
	reallocate(n);
} /* end of MessageBuffer::MessageBuffer(uint32_t) */

/* 
 * FIXME we may put a COMPILED FLAG here in order
 * to prevent reallocation beside the first one (when buffer==NULL)
 */
void MessageBuffer::reallocate(uint32_t n) {
	uint8_t* oldbuf = buffer;
	/* (re)allocation is done iff
	 *  - buffer is NULL (never allocated)
	 *  or
	 *  - requested size exceed current bufferMaxSize
	 */
	if ((NULL==buffer) || (n > bufferMaxSize)) {
		// FIXME should try/catch for alloc error
		// ands wrap-up inside RTIinternalError
		buffer = new uint8_t[n];
		bufferMaxSize = n;
		/* 
		 * If oldbuf wasn't null then copy
		 * oldbuf in the new buf 
		 * the writeOffset should be valid.
		 */
		if (NULL!=oldbuf) {
			memcpy(buffer, oldbuf, writeOffset);
			delete[] oldbuf;
			oldbuf = NULL;
		}
	}
} /* end of MessageBuffer::MessageBuffer(uint32_t) */

MessageBuffer::~MessageBuffer() {
	if (NULL!=buffer) {
		delete[] buffer;
	}
} /* end of MessageBuffer::~MessageBuffer() */

uint32_t MessageBuffer::size() const {
	return writeOffset;
}

uint32_t MessageBuffer::maxSize() const {
	return bufferMaxSize;
}

void MessageBuffer::assumeBufferIsBigEndian() {
#ifdef HOST_IS_BIG_ENDIAN
	this->bufferHasMyEndianness = true;
#else    
	this->bufferHasMyEndianness = false;
#endif
} /* end of MessageBuffer::assumeBufferIsBigEndian() */

void MessageBuffer::assumeBufferIsLittleEndian() {
#ifdef HOST_IS_BIG_ENDIAN
	this->bufferHasMyEndianness = false;
#else    
	this->bufferHasMyEndianness = true;
#endif    
} /* end of MessageBuffer::assumeBufferIsLittleEndian() */

void MessageBuffer::resetBuffer() {
	bufferHasMyEndianness = true;
	writeOffset = 0;
	readOffset = 0;
} /* MessageBuffer::resetBuffer() */

int32_t MessageBuffer::write_uint8s(const uint8_t* data, uint32_t n) {

	if (n >= (bufferMaxSize - writeOffset)) {
		/* reallocate buffer on-demand */
		reallocate(bufferMaxSize+ (n-(bufferMaxSize-writeOffset))
				+ DEFAULT_MESSAGE_BUFFER_SIZE);
	}
	/* copy data */
	memcpy(buffer+writeOffset, data, n);
	/* update write offset */
	writeOffset += n;
	return (writeOffset-n);
} /* end of MessageBuffer::write_uint8s(uint8_t*, uint32_t) */

int32_t MessageBuffer::read_uint8s(uint8_t* data, uint32_t n) {

	if (n + readOffset > writeOffset) {
		std::stringstream smsg;
		smsg << __func__ << "invalid read of siwritePtrze <" << n
				<< "> inside a buffer of readable size <"
				<< (int32_t)writeOffset-readOffset << "> (writeOffset="
				<<writeOffset << ",readOffset="<<readOffset <<").";
		throw RTIinternalError(smsg.str().c_str());
	}

	memcpy(data, buffer+readOffset, n);
	readOffset += n;
	return (readOffset-n);
} /* end of MessageBuffer::read_uint8s(uint8_t*, uint32_t) */

int32_t MessageBuffer::write_uint16s(const uint16_t* data, uint32_t n) {
	uint32_t i;
	uint16_t* buffer_uint16;

	if ((2*n) >= (bufferMaxSize - writeOffset)) {
		/* reallocate buffer on-demand */
		reallocate(bufferMaxSize+ (2*n)-(bufferMaxSize - writeOffset)
				+ DEFAULT_MESSAGE_BUFFER_SIZE);
	}

	/* do not swap byte if it is not necessary */
	if (bufferHasMyEndianness) {
		memcpy(buffer+writeOffset, data, 2*n);
		writeOffset += 2*n;
	} else {
		buffer_uint16 = reinterpret_cast<uint16_t*>(buffer+writeOffset);
		for (i=0; i<n; ++i) {
			buffer_uint16[i] = CERTI_UINT16_SWAP_BYTES(data[i]);
			writeOffset += 2;
		}
	}
	return (writeOffset-2*n);
} /* end of MessageBuffer::write_uint16s(uint16_t*, uint32_t) */

int32_t MessageBuffer::read_uint16s(uint16_t* data, uint32_t n) {
	uint32_t i;
	uint16_t* buffer_uint16;

	if (2*n + readOffset > writeOffset) {
		std::stringstream smsg;
		smsg << __func__ << "invalid read of size <" << 2*n
				<< "> inside a buffer of readable size <"
				<< (int32_t)writeOffset-readOffset << "> (writeOffset="
				<<writeOffset << ",readOffset="<<readOffset <<").";
		throw RTIinternalError(smsg.str().c_str());
	}

	/* do not swap byte if it is not necessary */
	if (bufferHasMyEndianness) {
		memcpy(data, buffer+readOffset, 2*n);
		readOffset += 2*n;
	} else {
		buffer_uint16 = reinterpret_cast<uint16_t*>(buffer+readOffset);
		for (i=0; i<n; ++i) {
			data[i] = CERTI_UINT16_SWAP_BYTES(buffer_uint16[i]);
			readOffset += 2;
		}
	}
	return (readOffset-2*n);
} /* end of MessageBuffer::read_uint16s(uint16_t*, uint32_t) */

int32_t MessageBuffer::write_uint32s(const uint32_t* data, uint32_t n) {
	uint32_t i;
	uint32_t* buffer_uint32;

	if ((4*n) >= (bufferMaxSize - writeOffset)) {
		/* reallocate buffer on-demand */
		reallocate(bufferMaxSize+ (4*n)-(bufferMaxSize - writeOffset)
				+ DEFAULT_MESSAGE_BUFFER_SIZE);
	}

	/* do not swap byte if it is not necessary */
	if (bufferHasMyEndianness) {
		memcpy(buffer+writeOffset, data, 4*n);
		writeOffset += 4*n;
	} else {
		buffer_uint32 = reinterpret_cast<uint32_t*>(buffer+writeOffset);
		for (i=0; i<n; ++i) {
			buffer_uint32[i] = CERTI_UINT32_SWAP_BYTES(data[i]);
			writeOffset += 4;
		}
	}
	return (writeOffset-4*n);
} /* end of write_uint32s */

int32_t MessageBuffer::read_uint32s(uint32_t* data, uint32_t n) {
	uint32_t i;
	uint32_t* buffer_uint32;

	if (4*n + readOffset > writeOffset) {
		std::stringstream smsg;
		smsg << __func__ << "invalid read of size <" << 4*n
				<< "> inside a buffer of readable size <"
				<< (int32_t)writeOffset-readOffset << "> (writeOffset="
				<<writeOffset << ",readOffset="<<readOffset <<").";
		throw RTIinternalError(smsg.str().c_str());
	}

	/* do not swap byte if it is not necessary */
	if (bufferHasMyEndianness) {
		memcpy(data, buffer+readOffset, 4*n);
		readOffset += 4*n;
	} else {
		buffer_uint32 = reinterpret_cast<uint32_t*>(buffer+readOffset);
		for (i=0; i<n; ++i) {
			data[i] = CERTI_UINT32_SWAP_BYTES(buffer_uint32[i]);
			readOffset += 4;
		}
	}
	return (readOffset-4*n);
}

int32_t MessageBuffer::write_uint64s(const uint64_t* data, uint32_t n) {
	uint32_t i;
	uint32_t* buffer_uint32;
	const uint32_t* data32;

	if ((8*n) >= (bufferMaxSize - writeOffset)) {
		/* reallocate buffer on-demand */
		reallocate(bufferMaxSize+ (8*n)-(bufferMaxSize - writeOffset)
				+ DEFAULT_MESSAGE_BUFFER_SIZE);
	}

	/* do not swap byte if it is not necessary */
	if (bufferHasMyEndianness) {
		memcpy(buffer+writeOffset, data, 8*n);
		writeOffset += 8*n;
	} else {
		buffer_uint32 = reinterpret_cast<uint32_t*>(buffer+writeOffset);
		data32 = reinterpret_cast<const uint32_t*>(data);
		for (i=0; i<2*n; ++i) {
			buffer_uint32[i] = CERTI_UINT32_SWAP_BYTES(data32[i]);
			writeOffset += 4;
		}
	}
	return (writeOffset-8*n);
}

int32_t MessageBuffer::read_uint64s(uint64_t* data, uint32_t n) {
	uint32_t i;
	uint32_t* buffer_uint32;
	uint32_t* data32;

	if (8*n + readOffset > writeOffset) {
		std::stringstream smsg;
		smsg << __func__ << "invalid read of size <" << 4*n
				<< "> inside a buffer of readable size <"
				<< (int32_t)writeOffset-readOffset << "> (writeOffset="
				<<writeOffset << ",readOffset="<<readOffset <<").";
		throw RTIinternalError(smsg.str().c_str());
	}

	/* do not swap byte if it is not necessary */
	if (bufferHasMyEndianness) {
		memcpy(data, buffer+readOffset, 8*n);
		readOffset += 8*n;
	} else {
		buffer_uint32 = reinterpret_cast<uint32_t*>(buffer+readOffset);
		data32 = reinterpret_cast<uint32_t*>(data);
		for (i=0; i<2*n; ++i) {
			data32[i] = CERTI_UINT32_SWAP_BYTES(buffer_uint32[i]);
			readOffset += 4;
		}
	}
	return (readOffset-8*n);
}

int32_t MessageBuffer::write_floats(const float* data, uint32_t n) {
	const uint32_t* data32;
	data32 = reinterpret_cast<const uint32_t*>(data);	
	return write_uint32s(data32,n);
}

int32_t MessageBuffer::read_floats(float* data, uint32_t n) {
	uint32_t* data32;
	data32 = reinterpret_cast<uint32_t*>(data);	
	return read_uint32s(data32,n);	
}

int32_t MessageBuffer::write_doubles(const double* data, uint32_t n) {
	const uint64_t* data64;
	data64 = reinterpret_cast<const uint64_t*>(data);	
	return write_uint64s(data64,n);	
}

int32_t MessageBuffer::read_doubles(double* data, uint32_t n) {
	uint64_t* data64;
	data64 = reinterpret_cast<uint64_t*>(data);	
	return read_uint64s(data64,n);
}

} // certi

