/* ----------------------------------------------------------------------------
 * CERTI - HLA RunTime Infrastructure
 * Copyright (C) 2002-2005  ONERA
 *
 * This program is free software ; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation ; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY ; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program ; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * ---------------------------------------------------------------------------- */

#ifndef MSG_BUFFER_H
#define MSG_BUFFER_H

#ifdef _MSC_VER
#ifndef FAKED_INTTYPES_DEFINED
#define FAKED_INTTYPES_DEFINED
typedef unsigned __int64  uint64_t;
typedef signed __int64    int64_t;
typedef unsigned __int32  uint32_t;
typedef signed __int32    int32_t;
typedef unsigned __int16  uint16_t;
typedef signed __int16    int16_t;
typedef unsigned __int8   uint8_t;
typedef signed __int8     int8_t;
typedef short int         int_least16_t;
#endif
#else
#include <inttypes.h>
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
    #pragma warning(disable: 4251)
    #define ANY_DLL_EXPORT __declspec(dllexport)
    #define ANY_DLL_IMPORT __declspec(dllimport)
    #define ANY_DLL_LOCAL
#else
    #if (__GNUC__ >= 4)
       #define ANY_DLL_EXPORT __attribute__ ((visibility("default")))
       #define ANY_DLL_IMPORT __attribute__ ((visibility("default")))
       #define ANY_DLL_LOCAL  __attribute__ ((visibility("hidden")))
    #else
       #define ANY_DLL_EXPORT
       #define ANY_DLL_IMPORT
       #define ANY_DLL_LOCAL
    #endif
#endif

#if defined(HLA_EXPORTS)
    #define MB_EXPORT ANY_DLL_EXPORT
#else
    #define MB_EXPORT ANY_DLL_IMPORT
#endif

/* define our own boolean type */
#ifndef bool_t
typedef uint8_t bool_t;
#define bFALSE  0
#define bTRUE   1
#endif

#define MB_UINT64_SWAP_LE_BE_CONSTANT(val)  ((uint64_t) ( \
      (((uint64_t) (val) &                      \
    (uint64_t) MB_INT64_CONSTANT(0x00000000000000ffU)) << 56) | \
      (((uint64_t) (val) &                      \
    (uint64_t) MB_INT64_CONSTANT(0x000000000000ff00U)) << 40) | \
      (((uint64_t) (val) &                      \
    (uint64_t) MB_INT64_CONSTANT(0x0000000000ff0000U)) << 24) | \
      (((uint64_t) (val) &                      \
    (uint64_t) MB_INT64_CONSTANT(0x00000000ff000000U)) <<  8) | \
      (((uint64_t) (val) &                      \
    (uint64_t) MB_INT64_CONSTANT(0x000000ff00000000U)) >>  8) | \
      (((uint64_t) (val) &                      \
    (uint64_t) MB_INT64_CONSTANT(0x0000ff0000000000U)) >> 24) | \
      (((uint64_t) (val) &                      \
    (uint64_t) MB_INT64_CONSTANT(0x00ff000000000000U)) >> 40) | \
      (((uint64_t) (val) &                      \
    (uint64_t) MB_INT64_CONSTANT(0xff00000000000000U)) >> 56)))

#ifdef HOST_IS_BIG_ENDIAN
  #define MB_UINT64_SWAP_LE_BE(val) (MB_UINT64_SWAP_LE_BE_CONSTANT(val))
  #define MB_UINT64_TO_BE(val)     (val)
  #define MB_UINT64_TO_LE(val)     (MB_UINT64_SWAP_LE_BE (val))
#else
  #define MB_UINT64_SWAP_LE_BE(val) (MB_UINT64_SWAP_LE_BE_CONSTANT(val))
  #define MB_UINT64_TO_BE(val)     (MB_UINT64_SWAP_LE_BE (val))
  #define MB_UINT64_TO_LE(val)      (val)
#endif

#define MB_UINT64_FROM_BE(val)              (MB_UINT64_TO_BE(val))
#define MB_UINT64_FROM_LE(val)              (MB_UINT64_TO_LE(val))
#define MB_ENCODE_DOUBLE_TO_UINT64BE(val)    (MB_UINT64_TO_BE   (*(uint64_t*)(val)))
#define MB_DECODE_DOUBLE_FROM_UINT64BE(val)  (MB_UINT64_FROM_BE (*(uint64_t*)(val)))

/*
 * Basic bit swapping functions
 */
#define MB_UINT16_SWAP_BYTES(val)   ((uint16_t) ( \
        (((uint16_t) (val) & (uint16_t) 0x00ffU) << 8) |  \
        (((uint16_t) (val) & (uint16_t) 0xff00U) >> 8)))

#define MB_UINT32_SWAP_BYTES(val)   ((uint32_t) (     \
        (((uint32_t) (val) & (uint32_t) 0x000000ffU) << 24) | \
        (((uint32_t) (val) & (uint32_t) 0x0000ff00U) <<  8) | \
        (((uint32_t) (val) & (uint32_t) 0x00ff0000U) >>  8) | \
        (((uint32_t) (val) & (uint32_t) 0xff000000U) >> 24)))

#if defined (__x86_64__)
# define MB_INT64_CONSTANT(val)  (val##L)
# define MB_UINT64_FORMAT "lu"
# define MB_XINT64_FORMAT "lx"
# define MB_INT64_FORMAT "l"
#else
# define MB_INT64_CONSTANT(val)  (val##LL)
# define MB_UINT64_FORMAT "llu"
# define MB_XINT64_FORMAT "llx"
# define MB_INT64_FORMAT "ll"
#endif

#define MB_UINT64_SWAP_BYTES(val)   ((uint64_t) (                   \
      (((uint64_t) (val) &                      \
    (uint64_t) MB_INT64_CONSTANT(0x00000000000000ffU)) << 56) | \
      (((uint64_t) (val) &                      \
    (uint64_t) MB_INT64_CONSTANT(0x000000000000ff00U)) << 40) | \
      (((uint64_t) (val) &                      \
    (uint64_t) MB_INT64_CONSTANT(0x0000000000ff0000U)) << 24) | \
      (((uint64_t) (val) &                      \
    (uint64_t) MB_INT64_CONSTANT(0x00000000ff000000U)) <<  8) | \
      (((uint64_t) (val) &                      \
    (uint64_t) MB_INT64_CONSTANT(0x000000ff00000000U)) >>  8) | \
      (((uint64_t) (val) &                      \
    (uint64_t) MB_INT64_CONSTANT(0x0000ff0000000000U)) >> 24) | \
      (((uint64_t) (val) &                      \
    (uint64_t) MB_INT64_CONSTANT(0x00ff000000000000U)) >> 40) | \
      (((uint64_t) (val) &                      \
    (uint64_t) MB_INT64_CONSTANT(0xff00000000000000U)) >> 56)))

#include <string.h>
#define DEFAULT_MESSAGE_BUFFER_SIZE 255
/*
 * We reserve 5 bytes at the beginning of the buffer
 * The first byte is a bitset which is used to
 * to tell if the buffer is big or little endian
 * The 4 following bytes is for an uint32_t which
 * may be used to store the buffer size
 */
#define MB_RESERVED_BYTES 5

/* BEGIN_C_DECLS should be used at the beginning of your declarations,
   so that C++ compilers don't mangle their names.  Use END_C_DECLS at
   the end of C declarations. */
#undef BEGIN_C_DECLS
#undef END_C_DECLS
#ifdef __cplusplus
#  define BEGIN_C_DECLS extern "C" {
#  define END_C_DECLS }
#else
# define BEGIN_C_DECLS /* empty */
# define END_C_DECLS /* empty */
#endif

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
typedef struct MsgBuffer {

    /** The buffer itself */
    uint8_t* buffer;
    /** The provisioned buffer size */
    uint32_t bufferMaxSize;
    /** Endianess toggle */
    bool_t bufferHasMyEndianness;
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
} MsgBuffer_t;

BEGIN_C_DECLS

/**
 * Return true if the host is BidEndian
 */
MB_EXPORT bool_t MB_HostIsBigEndian();

/**
 * Return true if the host is LittleEndian
 */
MB_EXPORT bool_t MB_HostIsLittleEndian();

/**
 * Show n bytes of data content in hexa on stdout.
 */
MB_EXPORT void MB_show(void* data, uint32_t n);


/**
 * Default message buffer ructor.
 * The default message buffer size is DEFAULT_MESSAGE_BUFFER_SIZE.
 * @param[in,out] msg pointer to the message buffer structure
 */
MB_EXPORT
void
MB_create(MsgBuffer_t* msg);

/**
 * Constructor with size.
 * Build a MessageBuffer with provisioned bufSize buffer.
 * @param[in,out] msg pointer to the message buffer structure
 * @param[in] bufferMaxSize, the [initial] maximum size of the buffer
 */
MB_EXPORT
void
MB_createWithSize(MsgBuffer_t* msg, uint32_t bufferMaxSize);

/**
 * Destructor
 * @param[in,out] msg pointer to the message buffer structure
 */
MB_EXPORT
void
MB_destroy(MsgBuffer_t* msg);

/**
 * Return the current buffer size (in bytes).
 * This is the size in bytes
 * of the element that were written to the buffer.
 * @return the current buffer size
 */
MB_EXPORT
uint32_t
MB_size(MsgBuffer_t* msg);

/**
 * Return the current maximum buffer size (in bytes).
 * This is the size of the allocated buffer.
 * @return the current buffer maximum size
 */
MB_EXPORT
uint32_t
MB_maxSize(MsgBuffer_t* msg);

/**
 * Assume the buffer is big endian.
 */
MB_EXPORT
void
MB_assumeBufferIsBigEndian(MsgBuffer_t* msg);

/**
 * Assume the buffer is little endian.
 */
MB_EXPORT
void
MB_assumeBufferIsLittleEndian(MsgBuffer_t* msg);

/**
 * Reset buffer state
 * This method should be called if ones want
 * to re-use a buffer in order to avoid reallocation.
 */
MB_EXPORT
void
MB_reset(MsgBuffer_t* msg);

/**
 * Seek buffer in order to write at specified place
 * Will set the write pointer to the seeked offset.
 */
MB_EXPORT
void MB_seek_write(MsgBuffer_t* msg, uint32_t offset);

/**
 * Resize the current maximum buffer size (in bytes).
 * This is the size of the allocated buffer.
 * @param[in] the new buffer maximum size
 * @return the new buffer maximum size
 */
MB_EXPORT
uint32_t MB_resize(MsgBuffer_t* msg, uint32_t newSize);

/**
 * Assume that the underlying buffer has
 * the specified size.
 * This may be used after ones get raw pointer
 * to the underlying buffer and has written to the buffer
 * using memcpy/recv.
 * @param[in] size the assumed size
 */
MB_EXPORT
void MB_assumeSize(MsgBuffer_t* msg, uint32_t size);

/**
 * Assume that the underlying buffer has the size
 * specified by the reserved bytes header.
 * The method will checked the endianity of the buffer
 * from the first reserved byte and then assume that
 * the buffer has size specified by the following 4 bytes.
 * A reallocation of the underlying memory buffer will
 * be done if necessary.
 */
MB_EXPORT
void MB_assumeSizeFromReservedBytes(MsgBuffer_t* msg);

#define MB_DECLARE_SIGNED(type)                \
        MB_EXPORT int32_t                     \
        MB_write_##type##s(MsgBuffer_t* msg,  type##_t* data, uint32_t n);\
\
MB_EXPORT int32_t                     \
MB_read_##type##s(MsgBuffer_t* msg, type##_t* data, uint32_t n);

#define MB_DECLARE_SINGLE_READ_WRITE(type,suffix)     \
        MB_DECLARE_SINGLE_READ_WRITE_(type,type##suffix)

#define MB_DECLARE_SINGLE_READ_WRITE2(type)     \
        MB_DECLARE_SINGLE_READ_WRITE_(type,type)

#define MB_DECLARE_SINGLE_READ_WRITE_(type,datatype)     \
        MB_EXPORT int32_t                     \
        MB_write_##type(MsgBuffer_t* msg,  datatype data);\
\
MB_EXPORT int32_t                     \
MB_read_##type(MsgBuffer_t* msg, datatype* data) ;\
\
MB_EXPORT datatype MB_get_##type(MsgBuffer_t* msg);

MB_EXPORT
int32_t
MB_write_uint8s(MsgBuffer_t* msg,  uint8_t* data, uint32_t n);

MB_EXPORT
int32_t
MB_read_uint8s(MsgBuffer_t* msg, uint8_t* data, uint32_t n);

MB_DECLARE_SINGLE_READ_WRITE(uint8,_t)
MB_DECLARE_SIGNED(int8)
MB_DECLARE_SINGLE_READ_WRITE(int8,_t)

MB_EXPORT
int32_t
MB_write_chars(MsgBuffer_t* msg,  char* data, uint32_t n);

MB_EXPORT
int32_t
MB_read_chars(MsgBuffer_t* msg, char* data, uint32_t n);
MB_DECLARE_SINGLE_READ_WRITE2(char)

#define MB_write_bytes  MB_write_chars
#define MB_read_bytes   MB_read_chars
#define MB_write_byte   MB_write_char
#define MB_read_byte    MB_read_char

MB_EXPORT
int32_t
MB_write_uint16s(MsgBuffer_t* msg,  uint16_t* data, uint32_t n);

MB_EXPORT
int32_t
MB_read_uint16s(MsgBuffer_t* msg, uint16_t* data, uint32_t n);

MB_DECLARE_SINGLE_READ_WRITE(uint16,_t)
MB_DECLARE_SIGNED(int16)
MB_DECLARE_SINGLE_READ_WRITE(int16,_t)

MB_EXPORT
int32_t
MB_write_uint32s(MsgBuffer_t* msg,  uint32_t* data, uint32_t n);

MB_EXPORT
int32_t
MB_read_uint32s(MsgBuffer_t* msg, uint32_t* data, uint32_t n);

MB_DECLARE_SINGLE_READ_WRITE(uint32,_t)
MB_DECLARE_SIGNED(int32)
MB_DECLARE_SINGLE_READ_WRITE(int32,_t)

MB_EXPORT
int32_t
MB_write_uint64s(MsgBuffer_t* msg,  uint64_t* data, uint32_t n);

MB_EXPORT
int32_t
MB_read_uint64s(MsgBuffer_t* msg, uint64_t* data, uint32_t n);

MB_DECLARE_SINGLE_READ_WRITE(uint64,_t)
MB_DECLARE_SIGNED(int64)
MB_DECLARE_SINGLE_READ_WRITE(int64,_t)

MB_EXPORT
int32_t
MB_write_floats(MsgBuffer_t* msg,  float* data, uint32_t n);

MB_EXPORT
int32_t
MB_read_floats(MsgBuffer_t* msg, float* data, uint32_t n);

MB_DECLARE_SINGLE_READ_WRITE2(float)

MB_EXPORT
int32_t
MB_write_doubles(MsgBuffer_t* msg,  double* data, uint32_t n);

MB_EXPORT
int32_t
MB_read_doubles(MsgBuffer_t* msg, double* data, uint32_t n);

MB_DECLARE_SINGLE_READ_WRITE2(double)

MB_EXPORT
int32_t
MB_write_string(MsgBuffer_t* msg, char* str);

MB_EXPORT
void
MB_read_string(MsgBuffer_t* msg, char* str, uint32_t slen);

MB_EXPORT
int32_t
MB_write_bool(MsgBuffer_t* msg, bool_t toggle);

MB_EXPORT
int32_t
MB_read_bool(MsgBuffer_t* msg, bool_t* toggle);

/**
 * Update the reserved bytes area.
 * @post the reserved bytes are up-to-date.
 */
MB_EXPORT
void MB_updateReservedBytes(MsgBuffer_t* msg);

/*
 * Pseudo index operator.
 * This will be used in send/receive socket call.
 * @return address of the underlying buffer + offset.
 */
MB_EXPORT
void* MB_get(MsgBuffer_t* msg, uint32_t offset);

END_C_DECLS
#endif
