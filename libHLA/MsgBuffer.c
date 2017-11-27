/* ----------------------------------------------------------------------------
 * CERTI - HLA RunTime Infrastructure
 * Copyright (C) 2002-2005 ONERA
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 *
 * ----------------------------------------------------------------------------*/

#include "MsgBuffer.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MB_IMPLEMENT_SIGNED(type)                                                                                      \
    int32_t MB_write_##type##s(MsgBuffer_t* msg, type##_t* data, uint32_t n)                                           \
    {                                                                                                                  \
        return MB_write_u##type##s(msg, (u##type##_t*) (data), n);                                                     \
    }                                                                                                                  \
                                                                                                                       \
    int32_t MB_read_##type##s(MsgBuffer_t* msg, type##_t* data, uint32_t n)                                            \
    {                                                                                                                  \
        return MB_read_u##type##s(msg, (u##type##_t*) (data), n);                                                      \
    }

#define MB_IMPLEMENT_SINGLE_READ_WRITE(type, suffix) MB_IMPLEMENT_SINGLE_READ_WRITE_(type, type##suffix)

#define MB_IMPLEMENT_SINGLE_READ_WRITE2(type) MB_IMPLEMENT_SINGLE_READ_WRITE_(type, type)

#define MB_IMPLEMENT_SINGLE_READ_WRITE_(type, datatype)                                                                \
    int32_t MB_write_##type(MsgBuffer_t* msg, datatype data)                                                           \
    {                                                                                                                  \
        return MB_write_##type##s(msg, &data, 1);                                                                      \
    }                                                                                                                  \
                                                                                                                       \
    int32_t MB_read_##type(MsgBuffer_t* msg, datatype* data)                                                           \
    {                                                                                                                  \
        return MB_read_##type##s(msg, data, 1);                                                                        \
    }                                                                                                                  \
                                                                                                                       \
    datatype MB_get_##type(MsgBuffer_t* msg)                                                                           \
    {                                                                                                                  \
        datatype retval;                                                                                               \
        MB_read_##type##s(msg, &retval, 1);                                                                            \
        return retval;                                                                                                 \
    }

MB_IMPLEMENT_SINGLE_READ_WRITE(uint8, _t)
MB_IMPLEMENT_SIGNED(int8)
MB_IMPLEMENT_SINGLE_READ_WRITE(int8, _t)
MB_IMPLEMENT_SINGLE_READ_WRITE2(char)
MB_IMPLEMENT_SINGLE_READ_WRITE(uint16, _t)
MB_IMPLEMENT_SIGNED(int16)
MB_IMPLEMENT_SINGLE_READ_WRITE(int16, _t)
MB_IMPLEMENT_SINGLE_READ_WRITE(uint32, _t)
MB_IMPLEMENT_SIGNED(int32)
MB_IMPLEMENT_SINGLE_READ_WRITE(int32, _t)
MB_IMPLEMENT_SINGLE_READ_WRITE(uint64, _t)
MB_IMPLEMENT_SIGNED(int64)
MB_IMPLEMENT_SINGLE_READ_WRITE(int64, _t)
MB_IMPLEMENT_SINGLE_READ_WRITE2(float)
MB_IMPLEMENT_SINGLE_READ_WRITE2(double)

int32_t MB_write_chars(MsgBuffer_t* msg, char* data, uint32_t n)
{
    return MB_write_uint8s(msg, (uint8_t*) (data), n);
}

int32_t MB_read_chars(MsgBuffer_t* msg, char* data, uint32_t n)
{
    return MB_read_uint8s(msg, (uint8_t*) (data), n);
}

int32_t MB_write_bool(MsgBuffer_t* msg, bool_t toggle)
{
    if (toggle) {
        return MB_write_uint8(msg, 1);
    }
    else {
        return MB_write_uint8(msg, 0);
    }
}

int32_t MB_read_bool(MsgBuffer_t* msg, bool_t* toggle)
{
    return MB_read_uint8(msg, toggle);
}

static void MB_initialize(MsgBuffer_t* msg)
{
    assert(msg);
    msg->buffer = NULL;
    msg->bufferMaxSize = DEFAULT_MESSAGE_BUFFER_SIZE;
    msg->bufferHasMyEndianness = bTRUE;
    msg->writeOffset = MB_RESERVED_BYTES;
    msg->readOffset = MB_RESERVED_BYTES;
} /* end of MB_initialize() */

static void MB_error(MsgBuffer_t* msg, const char* funName, uint32_t sz, uint32_t acsz)
{
    fprintf(stderr,
            "%s::invalid access of size <%d> inside a buffer of accessible size <%d> (writeOffset=%d,readOffset=%d)\n",
            funName,
            sz,
            acsz,
            msg->writeOffset,
            msg->readOffset);
    exit(EXIT_FAILURE);
} /* end of MB_error */

/**
 * Reallocate the underlying buffer to size n.
 * Reallocation will be done only if it is necessary.
 */
static void MB_reallocate(MsgBuffer_t* msg, uint32_t n);

/**
 * Update reserved bytes in order to indicate
 * the actual size of the buffer.
 */
static void MB_setSizeInReservedBytes(MsgBuffer_t* msg, uint32_t n);

bool_t MB_HostIsBigEndian()
{
#ifdef HOST_IS_BIG_ENDIAN
    return bTRUE;
#else
    return bFALSE;
#endif
}

bool_t MB_HostIsLittleEndian()
{
#ifdef HOST_IS_BIG_ENDIAN
    return bFALSE;
#else
    return bTRUE;
#endif
}

void MB_show(void* data, uint32_t n)
{
    uint32_t i;
    uint8_t* u8data = (uint8_t*) (data);
    for (i = 0; i < n; ++i) {
        if ((0 == (i % 2)) && (i > 0)) {
            printf(" ");
        }
        printf("%02X", u8data[i]);
    }
}

void MB_create(MsgBuffer_t* msg)
{
    MB_initialize(msg);
    MB_reallocate(msg, DEFAULT_MESSAGE_BUFFER_SIZE);
    memset(msg->buffer, 0, msg->bufferMaxSize);
} /* end of MB_MessageBuffer() */

void MB_createWithSize(MsgBuffer_t* msg, uint32_t bufferMaxSize)
{
    MB_initialize(msg);
    MB_reallocate(msg, bufferMaxSize);
    memset(msg->buffer, 0, msg->bufferMaxSize);
} /* end of MB_MessageBuffer(uint32_t) */

/*
 * FIXME we may put a COMPILED FLAG here in order
 * to prevent reallocation beside the first one (when buffer==NULL)
 */
void MB_reallocate(MsgBuffer_t* msg, uint32_t n)
{
    uint8_t* oldbuf = msg->buffer;
    /* (re)allocation is done iff
	 *  - buffer is NULL (never allocated)
	 *  or
	 *  - requested size exceed current bufferMaxSize
	 */
    if ((NULL == msg->buffer) || (n > msg->bufferMaxSize)) {
        /* FIXME should try/catch for alloc error
		 * ands wrap-up inside RTIinternalError
		 */
        msg->buffer = calloc(n, sizeof(uint8_t));
        msg->bufferMaxSize = n;
        /*
		 * If oldbuf wasn't null then copy
		 * oldbuf in the new buf
		 * the writeOffset should be valid.
		 */
        if (NULL != oldbuf) {
            memcpy(msg->buffer, oldbuf, msg->writeOffset);
            free(oldbuf);
            oldbuf = NULL;
        }
        else {
            MB_updateReservedBytes(msg);
        }
    }
} /* end of MB_MessageBuffer(MsgBuffer_t*, uint32_t) */

void MB_destroy(MsgBuffer_t* msg)
{
    if (NULL != msg->buffer) {
        free(msg->buffer);
        msg->buffer = NULL;
    }
} /* end of MB_~MessageBuffer() */

uint32_t MB_size(MsgBuffer_t* msg)
{
    return msg->writeOffset;
}

uint32_t MB_maxSize(MsgBuffer_t* msg)
{
    return msg->bufferMaxSize;
}

void MB_assumeBufferIsBigEndian(MsgBuffer_t* msg)
{
    msg->bufferHasMyEndianness = MB_HostIsBigEndian();
    msg->buffer[0] = 0x01;
} /* end of MB_assumeBufferIsBigEndian() */

void MB_assumeBufferIsLittleEndian(MsgBuffer_t* msg)
{
    msg->bufferHasMyEndianness = MB_HostIsLittleEndian();
    msg->buffer[0] = 0x00;
} /* end of MB_assumeBufferIsLittleEndian() */

void MB_reset(MsgBuffer_t* msg)
{
    msg->bufferHasMyEndianness = bTRUE;
    msg->writeOffset = MB_RESERVED_BYTES;
    msg->readOffset = MB_RESERVED_BYTES;
    MB_updateReservedBytes(msg);
} /* MB_resetBuffer() */

uint32_t MB_resize(MsgBuffer_t* msg, uint32_t newSize)
{
    MB_reallocate(msg, newSize);
    return msg->bufferMaxSize;
}

void MB_assumeSize(MsgBuffer_t* msg, uint32_t size)
{
    /* This is done in order to overflow
	 * buffer max size but this may well be
	 * an error (FIXME should throw an exception ?)
	 */
    if (size <= msg->bufferMaxSize) {
        msg->writeOffset = size;
    }
    else {
        msg->writeOffset = msg->bufferMaxSize;
    }
}

void MB_assumeSizeFromReservedBytes(MsgBuffer_t* msg)
{
    uint32_t toBeAssumedSize;
    /* verify endianity from reserved byte 0 */
    if (msg->buffer[0] == 0x01) {
        MB_assumeBufferIsBigEndian(msg);
    }
    else {
        MB_assumeBufferIsLittleEndian(msg);
    }
    /* read size from reserved bytes 1..4 */
    msg->readOffset = 1;
    toBeAssumedSize = MB_get_uint32(msg);
    /* reallocation may be needed */
    MB_reallocate(msg, toBeAssumedSize);
    MB_assumeSize(msg, toBeAssumedSize);
} /* end of assumeSizeFromReservedBytes */

void MB_setSizeInReservedBytes(MsgBuffer_t* msg, uint32_t n)
{
    uint32_t oldWR_Offset;
    /* backup write Offset */
    oldWR_Offset = msg->writeOffset;
    /* update size in reserved bytes */
    msg->writeOffset = 1;
    MB_write_uint32(msg, n);
    /* restore writeOffset */
    msg->writeOffset = oldWR_Offset;
} /* end of setSizeInReservedBytes */

int32_t MB_write_uint8s(MsgBuffer_t* msg, uint8_t* data, uint32_t n)
{
    if (n >= (msg->bufferMaxSize - msg->writeOffset)) {
        /* reallocate buffer on-demand */
        MB_reallocate(msg,
                      msg->bufferMaxSize + (n - (msg->bufferMaxSize - msg->writeOffset)) + DEFAULT_MESSAGE_BUFFER_SIZE);
    }
    /* copy data */
    memcpy(msg->buffer + msg->writeOffset, data, n);
    /* update write offset */
    msg->writeOffset += n;
    return (msg->writeOffset - n);
} /* end of MB_write_uint8s(uint8_t*, uint32_t) */

int32_t MB_read_uint8s(MsgBuffer_t* msg, uint8_t* data, uint32_t n)
{
    if (n + msg->readOffset > msg->writeOffset) {
        MB_error(msg, "MB_read_uint8s", n, msg->writeOffset - msg->readOffset);
    }

    memcpy(data, msg->buffer + msg->readOffset, n);
    msg->readOffset += n;
    return (msg->readOffset - n);
} /* end of MB_read_uint8s(uint8_t*, uint32_t) */

int32_t MB_write_uint16s(MsgBuffer_t* msg, uint16_t* data, uint32_t n)
{
    uint32_t i;
    uint16_t an_uint16;

    if ((2 * n) >= (msg->bufferMaxSize - msg->writeOffset)) {
        /* reallocate buffer on-demand */
        MB_reallocate(
            msg, msg->bufferMaxSize + (2 * n) - (msg->bufferMaxSize - msg->writeOffset) + DEFAULT_MESSAGE_BUFFER_SIZE);
    }

    /* do not swap byte if it is not necessary */
    if (msg->bufferHasMyEndianness) {
        memcpy(msg->buffer + msg->writeOffset, data, 2 * n);
        msg->writeOffset += 2 * n;
    }
    else {
        for (i = 0; i < n; ++i) {
            an_uint16 = MB_UINT16_SWAP_BYTES(data[i]);
            memcpy(msg->buffer + msg->writeOffset, &an_uint16, 2);
            msg->writeOffset += 2;
        }
    }
    return (msg->writeOffset - 2 * n);
} /* end of MB_write_uint16s(uint16_t*, uint32_t) */

int32_t MB_read_uint16s(MsgBuffer_t* msg, uint16_t* data, uint32_t n)
{
    uint32_t i;
    uint16_t an_uint16;

    if (2 * n + msg->readOffset > msg->writeOffset) {
        MB_error(msg, "MB_read_uint16s", n, msg->writeOffset - msg->readOffset);
    }

    /* do not swap byte if it is not necessary */
    if (msg->bufferHasMyEndianness) {
        memcpy(data, msg->buffer + msg->readOffset, 2 * n);
        msg->readOffset += 2 * n;
    }
    else {
        for (i = 0; i < n; ++i) {
            memcpy(&an_uint16, msg->buffer + msg->readOffset, 2);
            data[i] = MB_UINT16_SWAP_BYTES(an_uint16);
            msg->readOffset += 2;
        }
    }

    return (msg->readOffset - 2 * n);
} /* end of MB_read_uint16s(uint16_t*, uint32_t) */

int32_t MB_write_uint32s(MsgBuffer_t* msg, uint32_t* data, uint32_t n)
{
    uint32_t i;
    uint32_t an_uint32;

    if ((4 * n) >= (msg->bufferMaxSize - msg->writeOffset)) {
        /* reallocate buffer on-demand */
        MB_reallocate(
            msg, msg->bufferMaxSize + (4 * n) - (msg->bufferMaxSize - msg->writeOffset) + DEFAULT_MESSAGE_BUFFER_SIZE);
    }

    /* do not swap byte if it is not necessary */
    if (msg->bufferHasMyEndianness) {
        memcpy(msg->buffer + msg->writeOffset, data, 4 * n);
        msg->writeOffset += 4 * n;
    }
    else {
        for (i = 0; i < n; ++i) {
            an_uint32 = MB_UINT32_SWAP_BYTES(data[i]);
            memcpy(msg->buffer + msg->writeOffset, &an_uint32, 4);
            msg->writeOffset += 4;
        }
    }
    return (msg->writeOffset - 4 * n);
} /* end of MB_write_uint32s */

int32_t MB_read_uint32s(MsgBuffer_t* msg, uint32_t* data, uint32_t n)
{
    uint32_t i;
    uint32_t an_uint32;

    if (4 * n + msg->readOffset > msg->writeOffset) {
        MB_error(msg, "MB_read_uint32s", n, msg->writeOffset - msg->readOffset);
    }

    /* do not swap byte if it is not necessary */
    if (msg->bufferHasMyEndianness) {
        memcpy(data, msg->buffer + msg->readOffset, 4 * n);
        msg->readOffset += 4 * n;
    }
    else {
        for (i = 0; i < n; ++i) {
            memcpy(&an_uint32, msg->buffer + msg->readOffset, 4);
            data[i] = MB_UINT32_SWAP_BYTES(an_uint32);
            msg->readOffset += 4;
        }
    }
    return (msg->readOffset - 4 * n);
} /* end of MB_read_uint32s */

int32_t MB_write_uint64s(MsgBuffer_t* msg, uint64_t* data, uint32_t n)
{
    uint32_t i;
    union deux32 {
        uint32_t ui32[2];
        uint64_t ui64;
    } a_deux32;
    uint32_t an_uint32;

    if ((8 * n) >= (msg->bufferMaxSize - msg->writeOffset)) {
        /* reallocate buffer on-demand */
        MB_reallocate(
            msg, msg->bufferMaxSize + (8 * n) - (msg->bufferMaxSize - msg->writeOffset) + DEFAULT_MESSAGE_BUFFER_SIZE);
    }

    /* do not swap byte if it is not necessary */
    if (msg->bufferHasMyEndianness) {
        memcpy(msg->buffer + msg->writeOffset, data, 8 * n);
        msg->writeOffset += 8 * n;
    }
    else {
        for (i = 0; i < n; ++i) {
            a_deux32.ui64 = data[i];
            an_uint32 = MB_UINT32_SWAP_BYTES(a_deux32.ui32[1]);
            memcpy(msg->buffer + msg->writeOffset, &an_uint32, 4);
            msg->writeOffset += 4;
            an_uint32 = MB_UINT32_SWAP_BYTES(a_deux32.ui32[0]);
            memcpy(msg->buffer + msg->writeOffset, &an_uint32, 4);
            msg->writeOffset += 4;
        }
    }
    return (msg->writeOffset - 8 * n);
} /* end of MB_write_uint64s */

int32_t MB_read_uint64s(MsgBuffer_t* msg, uint64_t* data, uint32_t n)
{
    uint32_t i;
    union deux32 {
        uint32_t ui32[2];
        uint64_t ui64;
    } a_deux32;
    uint32_t an_uint32;

    if (8 * n + msg->readOffset > msg->writeOffset) {
        MB_error(msg, "read_uint64s", n, msg->writeOffset - msg->readOffset);
    }

    /* do not swap byte if it is not necessary */
    if (msg->bufferHasMyEndianness) {
        memcpy(data, msg->buffer + msg->readOffset, 8 * n);
        msg->readOffset += 8 * n;
    }
    else {
        for (i = 0; i < n; ++i) {
            memcpy(&an_uint32, msg->buffer + msg->readOffset, 4);
            a_deux32.ui32[1] = MB_UINT32_SWAP_BYTES(an_uint32);
            msg->readOffset += 4;

            memcpy(&an_uint32, msg->buffer + msg->readOffset, 4);
            a_deux32.ui32[0] = MB_UINT32_SWAP_BYTES(an_uint32);
            msg->readOffset += 4;

            data[i] = a_deux32.ui64;
        }
    }
    return (msg->readOffset - 8 * n);
}

int32_t MB_write_floats(MsgBuffer_t* msg, float* data, uint32_t n)
{
    uint32_t* data32;
    data32 = (uint32_t*) (data);
    return MB_write_uint32s(msg, data32, n);
}

int32_t MB_read_floats(MsgBuffer_t* msg, float* data, uint32_t n)
{
    uint32_t* data32;
    data32 = (uint32_t*) (data);
    return MB_read_uint32s(msg, data32, n);
}

int32_t MB_write_doubles(MsgBuffer_t* msg, double* data, uint32_t n)
{
    uint64_t* data64;
    data64 = (uint64_t*) (data);
    return MB_write_uint64s(msg, data64, n);
}

int32_t MB_read_doubles(MsgBuffer_t* msg, double* data, uint32_t n)
{
    uint64_t* data64;
    data64 = (uint64_t*) (data);
    return MB_read_uint64s(msg, data64, n);
}

int32_t MB_write_string(MsgBuffer_t* msg, char* str)
{
    MB_write_int32(msg, strlen(str));
    return MB_write_chars(msg, str, strlen(str));
} /* end of MB_write_string */

void MB_read_string(MsgBuffer_t* msg, char* str, uint32_t slen)
{
    int32_t len = MB_get_int32(msg);

    if (len + msg->readOffset > msg->writeOffset) {
        MB_error(msg, "MB_read_string", len, msg->writeOffset - msg->readOffset);
    }

    if (slen > len) {
        strncpy(str, (char*) (msg->buffer + msg->readOffset), len);
        /* we should terminate the C string */
        str[len] = '\0';
    }
    else {
        MB_error(msg, "MB_read_string NOT enough output storage", len, slen);
    }
    msg->readOffset += len;
} /* end of MB_read_string */

void MB_updateReservedBytes(MsgBuffer_t* msg)
{
    /* set up buffer endianess */
    if ((MB_HostIsBigEndian() && msg->bufferHasMyEndianness)
        || (MB_HostIsLittleEndian() && !msg->bufferHasMyEndianness)) {
        msg->buffer[0] = 0x01;
    }
    else {
        msg->buffer[0] = 0x00;
    }
    /* set up size */
    MB_setSizeInReservedBytes(msg, MB_size(msg));
} /* end of updateReservedBytes */

void* MB_get(MsgBuffer_t* msg, uint32_t offset)
{
    return msg->buffer + offset;
}
