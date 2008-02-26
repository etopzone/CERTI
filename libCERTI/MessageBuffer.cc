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
#include <iomanip>

namespace certi {

const bool 
MessageBuffer::HostIsBigEndian() {
#ifdef HOST_IS_BIG_ENDIAN
	return true;
#else    
	return false;
#endif
}

const bool 
MessageBuffer::HostIsLittleEndian() {
#ifdef HOST_IS_BIG_ENDIAN
	return false;
#else    
	return true;
#endif
}

void
MessageBuffer::show(const void* data, uint32_t n) {
   uint32_t i;
   const uint8_t* u8data = reinterpret_cast<const uint8_t*>(data);
   for (i=0;i<n;++i) {
	   if ((0==(i%2))&&(i>0)) {
	   		   printf(" ");
	   }
	   printf("%02X",u8data[i]);	  
   }   
}

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
	this->bufferHasMyEndianness = HostIsBigEndian();
} /* end of MessageBuffer::assumeBufferIsBigEndian() */

void MessageBuffer::assumeBufferIsLittleEndian() {
	this->bufferHasMyEndianness = HostIsLittleEndian();    
} /* end of MessageBuffer::assumeBufferIsLittleEndian() */

void MessageBuffer::resetBuffer() {
	bufferHasMyEndianness = true;
	writeOffset           = 0;
	readOffset            = 0;
} /* MessageBuffer::resetBuffer() */

uint32_t MessageBuffer::resize(uint32_t newSize) {
	reallocate(newSize);
}
void MessageBuffer::assumeSize(uint32_t size) {
	/* this is done in order to overflow 
	 * buffer max size but this may well be
	 * an error (FIXME should throw an exception ?) 
	 */
	if (size <= bufferMaxSize) {
		writeOffset       = size;
	} else {
	    writeOffset       = bufferMaxSize;
	}
	readOffset            = 0;
}

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
	uint16_t an_uint16;

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
		for (i=0; i<n; ++i) {
			an_uint16 = CERTI_UINT16_SWAP_BYTES(data[i]);
			memcpy(buffer+writeOffset, &an_uint16, 2);
			writeOffset += 2;
		}
	}
	return (writeOffset-2*n);
} /* end of MessageBuffer::write_uint16s(uint16_t*, uint32_t) */

int32_t MessageBuffer::read_uint16s(uint16_t* data, uint32_t n) {
	uint32_t i;
	uint16_t an_uint16;

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
		for (i=0; i<n; ++i) {
			memcpy(&an_uint16, buffer+readOffset,2);
			data[i] = CERTI_UINT16_SWAP_BYTES(an_uint16);
			readOffset += 2;
		}
	}
	return (readOffset-2*n);
} /* end of MessageBuffer::read_uint16s(uint16_t*, uint32_t) */

int32_t MessageBuffer::write_uint32s(const uint32_t* data, uint32_t n) {
	uint32_t i;
	uint32_t an_uint32;

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
		for (i=0; i<n; ++i) {
			an_uint32 = CERTI_UINT32_SWAP_BYTES(data[i]);
			memcpy(buffer+writeOffset,&an_uint32,4);
			writeOffset += 4;
		}
	}
	return (writeOffset-4*n);
} /* end of write_uint32s */

int32_t MessageBuffer::read_uint32s(uint32_t* data, uint32_t n) {
	uint32_t i;
	uint32_t an_uint32;

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
		
		for (i=0; i<n; ++i) {
			memcpy(&an_uint32,buffer+readOffset,4);
			data[i] = CERTI_UINT32_SWAP_BYTES(an_uint32);
			readOffset += 4;
		}
	}
	return (readOffset-4*n);
}

int32_t MessageBuffer::write_uint64s(const uint64_t* data, uint32_t n) {
	uint32_t i;
	union deux32 {
		uint32_t    ui32[2];
		uint64_t    ui64;
	} a_deux32;
	uint32_t an_uint32;

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
		for (i=0; i<n; ++i) {
			a_deux32.ui64 = data[i];
			an_uint32 = CERTI_UINT32_SWAP_BYTES(a_deux32.ui32[1]);
			memcpy(buffer+writeOffset,&an_uint32,4);
			writeOffset += 4;
			an_uint32 = CERTI_UINT32_SWAP_BYTES(a_deux32.ui32[0]);
			memcpy(buffer+writeOffset,&an_uint32,4);
			writeOffset += 4;
		}
	}
	return (writeOffset-8*n);
}

int32_t MessageBuffer::read_uint64s(uint64_t* data, uint32_t n) {
	uint32_t i;
	union deux32 {
			uint32_t    ui32[2];
			uint64_t    ui64;
	} a_deux32;
	uint32_t an_uint32;	

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
		for (i=0; i<n; ++i) {			
			memcpy(&an_uint32,buffer+readOffset,4);			
			a_deux32.ui32[1] = CERTI_UINT32_SWAP_BYTES(an_uint32);
			readOffset += 4;
			
			memcpy(&an_uint32,buffer+readOffset,4);
			a_deux32.ui32[0] = CERTI_UINT32_SWAP_BYTES(an_uint32);
			readOffset += 4;
			
			data[i] = a_deux32.ui64;			
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

int32_t
MessageBuffer::write_string(const std::string& str) {
   write_int32(str.length());
   return write_chars(str.c_str(),str.length());
}
  
std::string 
MessageBuffer::read_string() {
 std::string retval;
 int32_t len;
 char* buf;
 
 read_int32(&len);
 buf = new char[len+1];
 read_chars(buf,len);
 /* terminate the string */
 buf[len] = '\0';
 /* build an std::string and retrun it */
 retval = std::string(buf);
 delete[] buf;
 return retval;
}

MessageBuffer::operator const void*(){
	return buffer;
}

MessageBuffer::operator void*() {
	return buffer;
}


} // certi

