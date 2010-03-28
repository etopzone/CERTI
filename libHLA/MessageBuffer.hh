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
// ----------------------------------------------------------------------------

#ifndef LIBHLA_MESSAGE_BUFFER_HH
#define LIBHLA_MESSAGE_BUFFER_HH

#include "libhla.hh"
#include <string>

#define DEFAULT_MESSAGE_BUFFER_SIZE 255

namespace libhla {

/**
 * MessageBuffer is a class managing a byte buffer for
 * portable heterogeneous Message exchange.
 * It provides member functions to read and write basics types
 * [unsigned] int 8/16/32/64, float, double, byte etc...
 * The message buffer will encode the written [type] data with proper
 * byte ordering and padding.
 * A message buffer handles heterogeneous write/read pair, in fact
 * it the central class for heterogeneity handling.
 * One must read from the buffer in the exact order the write was done.
 * MessageBuffer is dynamically sized, however reallocation are
 * only done when needed.
 */
class HLA_EXPORT MessageBuffer
{
public:

	/**
	 * Return true if the host is BidEndian
	 */
	static const bool HostIsBigEndian();

	/**
	 * Return true if the host is LittleEndian
	 */
	static const bool HostIsLittleEndian();

	/**
	 * Show n bytes of data content in hexa on stdout. 
	 */
	static void show(const void* data, uint32_t n);
	
	/* 
	 * We reserve 5 bytes at the beginning of the buffer
	 * The first byte is a bitset which is used to 
	 * to tell if the buffer is big or little endian
	 * The 4 following bytes is for an uint32_t which
	 * may be used to store the buffer size
	 */ 
	static const uint8_t reservedBytes;

	LIBHLA_EXCEPTION(MessageBufferError)

	/**
	 * Default message buffer constructor.
	 * The default message buffer size is DEFAULT_MESSAGE_BUFFER_SIZE.
	 */
	MessageBuffer();

	/**
	 * Constructor with size.
	 * Build a MessageBuffer with provisionned bufSize buffer.
	 * @param[in] bufferMaxSize, the [initial] maixmum size of the buffer
	 */
	MessageBuffer(uint32_t bufferMaxSize);

	/**
	 * Destructor 
	 */
	~MessageBuffer();

	/**
	 * Return the current buffer size (in bytes).
	 * This is the size in bytes 
	 * of the element that were written to the buffer.
	 * @return the current buffer size
	 */
	uint32_t size() const;

	/**
	 * Return the current maximum buffer size (in bytes).
	 * This is the size of the allocated buffer.
	 * @return the current buffer maximum size
	 */
	uint32_t maxSize() const;

	/**
	 * Assume the buffer is big endian.
	 */
	void
	assumeBufferIsBigEndian();

	/**
	 * Assume the buffer is little endian.
	 */
	void
	assumeBufferIsLittleEndian();

	/**
	 * Reset buffer state
	 * This method should be called if ones want
	 * to re-use a buffer in order to avoid reallocation.
	 */
	void
	reset();

	/**
	 * Seek buffer in order to write at specified place
	 * Will set the write pointer to the seeked offset.
	 */
	void seek_write(uint32_t offset);

	/**
	 * Resize the current maximum buffer size (in bytes).
	 * This is the size of the allocated buffer.
	 * @param[in] the new buffer maximum size
	 * @return the new buffer maximum size
	 */
	uint32_t resize(uint32_t newSize);

	/**
	 * Assume that the underlying buffer has 
	 * the specified size.
	 * This may be used after ones get raw pointer 
	 * to the underlying buffer and has written to the buffer
	 * using memcpy/recv.
	 * @param[in] size the assumed size
	 */
	void assumeSize(uint32_t size);
	
	/**
	 * Assume that the underlying buffer has the size
	 * specified by the reserved bytes header.
	 * The method will checked the endianity of the buffer
	 * from the first reserved byte and then assume that
	 * the buffer has size specified by the following 4 bytes.
	 * A reallocation of the underlying memory buffer will
	 * be done if necessary.
	 */
	void assumeSizeFromReservedBytes();

#define DECLARE_SIGNED(type)				\
	int32_t						\
	write_##type##s(const type##_t* data, uint32_t n) {		\
	return write_u##type##s(reinterpret_cast<const u##type##_t*>(data),n);	\
}							\
\
int32_t						\
read_##type##s(type##_t* data, uint32_t n) {		\
	return read_u##type##s(reinterpret_cast<u##type##_t*>(data),n);	\
}   							\


#define DECLARE_SINGLE_READ_WRITE(type,suffix)     \
DECLARE_SINGLE_READ_WRITE_(type,type##suffix)

#define DECLARE_SINGLE_READ_WRITE2(type)     \
DECLARE_SINGLE_READ_WRITE_(type,type)


#define DECLARE_SINGLE_READ_WRITE_(type,datatype)     \
	int32_t						\
	write_##type(const datatype data) {		\
	return write_##type##s(&data,1);	\
}							\
\
int32_t						\
read_##type(datatype* data) {		\
	return read_##type##s(data,1);	\
}                                    \
\
datatype read_##type() {\
	datatype retval;     \
	read_##type##s(&retval,1);\
	return retval; \
} 


	int32_t
	write_uint8s(const uint8_t* data, uint32_t n);

	int32_t
	read_uint8s(uint8_t* data, uint32_t n);

	DECLARE_SINGLE_READ_WRITE(uint8,_t)
	DECLARE_SIGNED(int8)
	DECLARE_SINGLE_READ_WRITE(int8,_t)

	int32_t
	write_chars(const char* data, uint32_t n) {
		return write_uint8s(reinterpret_cast<const uint8_t*>(data),n);
	}

	int32_t
	read_chars(char* data, uint32_t n) {
		return read_uint8s(reinterpret_cast<uint8_t*>(data),n);
	}
	DECLARE_SINGLE_READ_WRITE2(char)

#define write_bytes  write_chars
#define read_bytes   read_chars
#define write_byte   write_char
#define read_byte    read_char

	int32_t
	write_uint16s(const uint16_t* data, uint32_t n);
	int32_t
	read_uint16s(uint16_t* data, uint32_t n);

	DECLARE_SINGLE_READ_WRITE(uint16,_t)
	DECLARE_SIGNED(int16)
	DECLARE_SINGLE_READ_WRITE(int16,_t)

	int32_t
	write_uint32s(const uint32_t* data, uint32_t n);

	int32_t
	read_uint32s(uint32_t* data, uint32_t n);

	DECLARE_SINGLE_READ_WRITE(uint32,_t)
	DECLARE_SIGNED(int32)
	DECLARE_SINGLE_READ_WRITE(int32,_t)

	int32_t
	write_uint64s(const uint64_t* data, uint32_t n);
	int32_t
	read_uint64s(uint64_t* data, uint32_t n);

	DECLARE_SINGLE_READ_WRITE(uint64,_t)
	DECLARE_SIGNED(int64)
	DECLARE_SINGLE_READ_WRITE(int64,_t)

	int32_t
	write_floats(const float* data, uint32_t n);
	int32_t
	read_floats(float* data, uint32_t n);

	DECLARE_SINGLE_READ_WRITE2(float)

	int32_t
	write_doubles(const double* data, uint32_t n);
	int32_t
	read_doubles(double* data, uint32_t n);

	DECLARE_SINGLE_READ_WRITE2(double)

	int32_t
	write_string(const std::string& str);

	std::string
	read_string();

        void
	read_string(std::string&);
	
	int32_t
	write_bool(const bool toggle) {
		if(toggle) {
			return write_uint8(1);
		} else {
			return write_uint8(0);
		}
	};
	
	bool
	read_bool() {	
		return (1==read_uint8());
	};

	MessageBuffer& operator<<(const uint8_t data) {
		this->write_uint8(data);
		return *this;
	}
	
	/**
	 * Update the reserved bytes area.
	 * @post the reserved bytes are up-to-date.
	 */
	void updateReservedBytes();
	
	/*
	 * Pseudo index operator.
	 * This will be used in send/receive socket call.
	 * @return address of the underlying buffer + offset.
	 */
	void* operator ()(uint32_t offset);
	
private:

	/** The buffer itself */
	uint8_t* buffer;
	/** The provisioned buffer size */
	uint32_t bufferMaxSize;
	/** Endianness toggle */
	bool bufferHasMyEndianness;	
	/** 
	 * The write offset is the offset of the buffer
	 * where the next write operation will write to.
	 */
	uint32_t writeOffset;	

	/** 
	 * The read offset is the offset of the buffer
	 * where the next read operation will read from.
	 */
	uint32_t readOffset;	

	void initialize();
	/**
	 * Reallocate the underlying buffer to size n.
	 * Reallocation will be done only if it is necessary.
	 */
	void reallocate(uint32_t n);

	/**
	 * Update reserved bytes in order to indicate
	 * the actual size of the buffer.
	 */
	void setSizeInReservedBytes(uint32_t n);
};

} // libhla

#endif // LIBHLA_MESSAGE_BUFFER_HH
