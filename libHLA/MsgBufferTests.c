/* ----------------------------------------------------------------------------
 * CERTI - HLA RunTime Infrastructure
 * Copyright (C) 2002-2005  ONERA
 *
 * This file is part of CERTI
 *
 * CERTI is free software ; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation ; either version 2 of the License, or
 * (at your option) any later version.
 *
 * CERTI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY ; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program ; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * ----------------------------------------------------------------------------*/

#include "MsgBuffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define MYVSTR_SIZE 2048

void messageBufferTests(MsgBuffer_t* MsgBuf) {
    MsgBuffer_t MsgBuf2;
    char*   str = "a char* string";
    char*   stdstr = "a NULL terminated char* string";
    uint8_t  u8   =  232;
    int8_t   i8   = -125;
    uint16_t u16  = 0xFFAA;
    int16_t  i16  = -576;
    uint32_t u32  = 0xFFAAEEBB;
    int32_t  i32  = -9999;
    uint64_t u64  = 0xFFEEDDCC;
    int64_t  i64  = -1000000000;
    float    f32  = 3.1415927f;
    double   d64  = 2.7182818;
    bool_t   trueBool  = bTRUE;
    bool_t   falseBool = bFALSE;
    char*    vstr = NULL;
    char     vstdstr[MYVSTR_SIZE];
    uint8_t  vu8   = 0;
    int8_t   vi8   = 0;
    uint16_t vu16  = 0;
    int16_t  vi16  = 0;
    uint32_t vu32  = 0;
    int32_t  vi32  = 0;
    uint64_t vu64  = 0;
    int64_t  vi64  = 0;
    float    vf32  = 0.0;
    double   vd64  = 0.0;
    bool_t   vtrueBool  = bFALSE;
    bool_t   vfalseBool = bTRUE;
    printf("Testing MsgBuffer structure BEGIN...\n");
    printf("    Current (Default) MsgBuffer MaxSize         = %d\n",MB_maxSize(MsgBuf));
    printf("    Current (initially void) MessageBuffer size = %d\n",MB_size(MsgBuf));
    printf("    Encoding to buffer...\n");
    printf("    bytes string = %s\n",str);
    MB_write_uint32(MsgBuf, strlen(str));
    MB_write_bytes(MsgBuf, str,strlen(str));
    MB_write_uint8(MsgBuf, u8);
    MB_write_uint16(MsgBuf, u16);
    MB_write_uint32(MsgBuf, u32);
    MB_write_uint64(MsgBuf, u64);
    MB_write_int8(MsgBuf, i8);
    MB_write_int16(MsgBuf, i16);
    MB_write_int32(MsgBuf, i32);
    MB_write_int64(MsgBuf, i64);
    MB_write_float(MsgBuf, f32);
    MB_write_double(MsgBuf, d64);
    MB_write_bool(MsgBuf, trueBool);
    MB_write_bool(MsgBuf, falseBool);
    printf("    std::string = %s\n",stdstr);
    MB_write_string(MsgBuf,stdstr);
    printf("    Current MessageBuffer size                  = %d\n",MB_size(MsgBuf));
    printf("    Decoding from buffer...\n");
    MB_read_uint32(MsgBuf,&vu32);
    vstr = malloc(sizeof(char)*(vu32+1));
    memset(vstr,'\0',vu32+1);
    MB_read_bytes(MsgBuf,vstr,vu32);
    printf("    bytes string = %s\n",vstr); assert(0==strcmp(str,vstr));
    MB_read_uint8(MsgBuf,&vu8); assert(vu8==u8);
    MB_read_uint16(MsgBuf,&vu16); assert(vu16==u16);
    MB_read_uint32(MsgBuf,&vu32); assert(vu32==u32);
    MB_read_uint64(MsgBuf,&vu64); assert(vu64==u64);
    MB_read_int8(MsgBuf,&vi8); assert(vi8==i8);
    MB_read_int16(MsgBuf,&vi16); assert(vi16==i16);
    MB_read_int32(MsgBuf,&vi32); assert(vi32==i32);
    MB_read_int64(MsgBuf,&vi64); assert(vi64==i64);
    MB_read_float(MsgBuf,&vf32); assert(vf32==f32);
    MB_read_double(MsgBuf,&vd64); assert(vd64==d64);
    MB_read_bool(MsgBuf,&vtrueBool); assert(trueBool==vtrueBool);
    MB_read_bool(MsgBuf,&vfalseBool); assert(falseBool==vfalseBool);
    MB_read_string(MsgBuf,vstdstr,MYVSTR_SIZE); assert(0==strcmp(vstdstr,stdstr));
    printf("    std::string = %s\n",vstdstr);
    printf("    All encoded/decoded values are equal.\n");
    printf("    Now reset buffer.\n");
    MB_reset(MsgBuf);
    printf("    Re-write on buffer...\n");
    MB_write_uint8(MsgBuf,u8);
    MB_write_uint16(MsgBuf,u16);
    MB_write_uint32(MsgBuf,u32);
    MB_write_uint64(MsgBuf,u64);
    MB_write_int8(MsgBuf,i8);
    MB_write_int16(MsgBuf,i16);
    MB_write_int32(MsgBuf,i32);
    MB_write_int64(MsgBuf,i64);
    MB_write_float(MsgBuf,f32);
    MB_write_double(MsgBuf,d64);
    printf("   Read using alternative method...\n");
    vu8  = MB_get_uint8(MsgBuf); assert(vu8==u8);
    vu16 = MB_get_uint16(MsgBuf); assert(vu16==u16);
    vu32 = MB_get_uint32(MsgBuf); assert(vu32==u32);
    vu64 = MB_get_uint64(MsgBuf); assert(vu64==u64);
    vi8  = MB_get_int8(MsgBuf); assert(vi8==i8);
    vi16 = MB_get_int16(MsgBuf); assert(vi16==i16);
    vi32 = MB_get_int32(MsgBuf); assert(vi32==i32);
    vi64 = MB_get_int64(MsgBuf); assert(vi64==i64);
    vf32 = MB_get_float(MsgBuf); assert(vf32==f32);
    vd64 = MB_get_double(MsgBuf); assert(vd64==d64);
    printf("    All encoded/decoded values are equal.\n");
    printf("    Now reset buffer.\n");
    MB_reset(MsgBuf);
    printf("    Re-write on buffer...\n");
    MB_write_uint8(MsgBuf,u8);  printf("        Written <"); MB_show(&u8,1); printf(">\n");
    MB_write_uint16(MsgBuf,u16);printf("        Written <"); MB_show(&u16,2); printf(">\n");
    MB_write_uint32(MsgBuf,u32);printf("        Written <"); MB_show(&u32,4); printf(">\n");
    MB_write_uint64(MsgBuf,u64);printf("        Written <"); MB_show(&u64,8); printf(">\n");
    MB_write_int8(MsgBuf,i8);printf("        Written <"); MB_show(&i8,1); printf(">\n");
    MB_write_int16(MsgBuf,i16);printf("        Written <"); MB_show(&i16,2); printf(">\n");
    MB_write_int32(MsgBuf,i32);printf("        Written <"); MB_show(&i32,4); printf(">\n");
    MB_write_int64(MsgBuf,i64);printf("        Written <"); MB_show(&i64,8); printf(">\n");
    MB_write_float(MsgBuf,f32);printf("        Written <"); MB_show(&f32,4); printf(">\n");
    MB_write_double(MsgBuf,d64);printf("        Written <"); MB_show(&d64,8); printf(">\n");

    printf("    ...and assume it is ");
    if (MB_HostIsBigEndian()) {
        printf("Little Endian");
        MB_assumeBufferIsLittleEndian(MsgBuf);
        printf(" whereas the host was big\n");
    }
    if (MB_HostIsLittleEndian()) {
        printf("Big Endian");
        MB_assumeBufferIsBigEndian(MsgBuf);
        printf(" whereas the host was little\n");
    }
    MB_read_uint8(MsgBuf,&vu8);   printf("        Read  <"); MB_show(&vu8,1); printf(">\n");
    MB_read_uint16(MsgBuf,&vu16); printf("        Read <"); MB_show(&vu16,2); printf(">\n");
    MB_read_uint32(MsgBuf,&vu32); printf("        Read <"); MB_show(&vu32,4); printf(">\n");
    MB_read_uint64(MsgBuf,&vu64); printf("        Read <"); MB_show(&vu64,8); printf(">\n");
    MB_read_int8(MsgBuf,&vi8);    printf("        Read <"); MB_show(&vi8,1); printf(">\n");
    MB_read_int16(MsgBuf,&vi16);  printf("        Read <"); MB_show(&vi16,2); printf(">\n");
    MB_read_int32(MsgBuf,&vi32);  printf("        Read <"); MB_show(&vi32,4); printf(">\n");
    MB_read_int64(MsgBuf,&vi64);  printf("        Read <"); MB_show(&vi64,8); printf(">\n");
    MB_read_float(MsgBuf,&vf32);  printf("        Read <"); MB_show(&vf32,4); printf(">\n");
    MB_read_double(MsgBuf,&vd64); printf("        Read <"); MB_show(&vd64,8); printf(">\n");
    free(vstr);

    printf("    Trying to overload the buffer...\n");
    printf("    Current (Default) MessageBuffer MaxSize               = %d\n",MB_maxSize(MsgBuf));
    MB_reset(MsgBuf);
    u32 = MB_maxSize(MsgBuf)*2;
    vstr = malloc(sizeof(char)*(u32+1));
    vstr[u32]='\0';
    memset(vstr,'A',u32);
    MB_write_uint32(MsgBuf,u32);
    MB_write_chars(MsgBuf,vstr,u32);
    printf("    Written char* is %s\n",vstr);
    u32 = MB_size(MsgBuf);
    free(vstr);
    printf("    Current           MessageBuffer MaxSize               = %d\n",MB_maxSize(MsgBuf));
    assert(MYVSTR_SIZE>u32);
    MB_read_string(MsgBuf,vstdstr,u32);
    printf("    Read String is   %s\n",vstdstr);
    printf("    Forcibly resize the buffer to %d\n",MB_maxSize(MsgBuf)*2);
    MB_resize(MsgBuf,MB_maxSize(MsgBuf)*2);
    assert(MYVSTR_SIZE>u32);
    printf("    Current           MessageBuffer MaxSize               = %d\n",MB_maxSize(MsgBuf));
    printf("    Now reset buffer and assume size is                   = %d\n",u32);
    MB_reset(MsgBuf);
    MB_assumeSize(MsgBuf,u32);
    MB_read_string(MsgBuf,vstdstr,MYVSTR_SIZE);
    printf("    (re)Read String is   %s\n",vstdstr);

    printf("    preparing a buffer for network send...\n");
    MB_reset(MsgBuf);
    MB_write_uint32(MsgBuf,u32);
    MB_write_uint8(MsgBuf,u8);
    MB_write_uint16(MsgBuf,u16);
    MB_updateReservedBytes(MsgBuf);
    MB_create(&MsgBuf2);
    printf("    now (pseudo) sending a buffer of size %d bytes ...\n",MB_size(MsgBuf));
    memcpy(MB_get(&MsgBuf2,0),MB_get(MsgBuf,0),MB_RESERVED_BYTES);
    printf("    MsgBuf  = ");MB_show(MB_get(MsgBuf,0),MB_RESERVED_BYTES);printf("\n");
    printf("    MsgBuf2 = ");MB_show(MB_get(&MsgBuf2,0),MB_RESERVED_BYTES);printf("\n");
    printf("    now (pseudo) receiving buffer header of %d bytes ...\n",MB_RESERVED_BYTES);
    printf("    reconstruct receive buffer size from buffer header ...\n");
    MB_assumeSizeFromReservedBytes(&MsgBuf2);
    printf("    seems to contain %d bytes...\n",MB_size(&MsgBuf2));
    printf("    now (pseudo) receiving buffer content ...\n");
    memcpy(MB_get(&MsgBuf2,MB_RESERVED_BYTES),MB_get(MsgBuf,MB_RESERVED_BYTES),MB_size(&MsgBuf2));
    printf("    reading buffer content...\n");
    vu32 = MB_get_uint32(&MsgBuf2); assert(vu32==u32);
    vu8  = MB_get_uint8(&MsgBuf2); assert(vu8==u8);
    vu16 = MB_get_uint16(&MsgBuf2); assert(vu16==u16);
    MB_destroy(&MsgBuf2);
    printf("Testing MsgBuffer END.\n");
} /* end of messageBufferTests */

int
main(int argc, char **argv) {

    MsgBuffer_t MsgBuf;

    MB_create(&MsgBuf);
    printf("LibHLA MsgBuffer (pure C language) Tests->BEGIN...\n");

    printf("    Host is ");
    if (MB_HostIsBigEndian()) {
        printf("Big Endian");
    }
    if (MB_HostIsLittleEndian()) {
        printf("Little Endian");
    }
    printf("\n");
    messageBufferTests(&MsgBuf);

    MB_destroy(&MsgBuf);
    printf("LibHLA MsgBuffer  (pure C language) Test->END.\n");
    return 0;
}
