#include <gtest/gtest.h>

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

#include <libHLA/MessageBuffer.hh>

using ::std::cout;
using ::std::endl;

using libhla::MessageBuffer;

TEST(MessageBufferTest, TestRW_uint8)
{
    MessageBuffer msgBuf;
    
    uint8_t initial(232), destination(0), destination_alt(0);
    
    msgBuf.write_uint8(initial);
    msgBuf.write_uint8(initial);
    
    msgBuf.read_uint8(&destination);
    
    ASSERT_EQ(initial, destination);
    
    destination_alt = msgBuf.read_uint8();
    
    ASSERT_EQ(initial, destination_alt);
}

TEST(MessageBufferTest, TestRW_int8)
{
    MessageBuffer msgBuf;
    
    int8_t initial(-125), destination(0), destination_alt(0);
    
    msgBuf.write_int8(initial);
    msgBuf.write_int8(initial);
    
    msgBuf.read_int8(&destination);
    
    ASSERT_EQ(initial, destination);
    
    destination_alt = msgBuf.read_int8();
    
    ASSERT_EQ(initial, destination_alt);
}

TEST(MessageBufferTest, TestRW_uint16)
{
    MessageBuffer msgBuf;
    
    uint16_t initial(0xFFAA), destination(0), destination_alt(0);
    
    msgBuf.write_uint16(initial);
    msgBuf.write_uint16(initial);
    
    msgBuf.read_uint16(&destination);
    
    ASSERT_EQ(initial, destination);
    
    destination_alt = msgBuf.read_uint16();
    
    ASSERT_EQ(initial, destination_alt);
}

TEST(MessageBufferTest, TestRW_int16)
{
    MessageBuffer msgBuf;
    
    int16_t initial(-576), destination(0), destination_alt(0);
    
    msgBuf.write_int16(initial);
    msgBuf.write_int16(initial);
    
    msgBuf.read_int16(&destination);
    
    ASSERT_EQ(initial, destination);
    
    destination_alt = msgBuf.read_int16();
    
    ASSERT_EQ(initial, destination_alt);
}

TEST(MessageBufferTest, TestRW_uint32)
{
    MessageBuffer msgBuf;
    
    uint32_t initial(0xFFAAEEBB), destination(0), destination_alt(0);
    
    msgBuf.write_uint32(initial);
    msgBuf.write_uint32(initial);
    
    msgBuf.read_uint32(&destination);
    
    ASSERT_EQ(initial, destination);
    
    destination_alt = msgBuf.read_uint32();
    
    ASSERT_EQ(initial, destination_alt);
}

TEST(MessageBufferTest, TestRW_int32)
{
    MessageBuffer msgBuf;
    
    int32_t initial(-9999), destination(0), destination_alt(0);
    
    msgBuf.write_int32(initial);
    msgBuf.write_int32(initial);
    
    msgBuf.read_int32(&destination);
    
    ASSERT_EQ(initial, destination);
    
    destination_alt = msgBuf.read_int32();
    
    ASSERT_EQ(initial, destination_alt);
}

TEST(MessageBufferTest, TestRW_uint64)
{
    MessageBuffer msgBuf;
    
    uint64_t initial(0xFFEEDDCC), destination(0), destination_alt(0);
    
    msgBuf.write_uint64(initial);
    msgBuf.write_uint64(initial);
    
    msgBuf.read_uint64(&destination);
    
    ASSERT_EQ(initial, destination);
    
    destination_alt = msgBuf.read_uint64();
    
    ASSERT_EQ(initial, destination_alt);
}

TEST(MessageBufferTest, TestRW_int64)
{
    MessageBuffer msgBuf;
    
    int64_t initial(-1000000000), destination(0), destination_alt(0);
    
    msgBuf.write_int64(initial);
    msgBuf.write_int64(initial);
    
    msgBuf.read_int64(&destination);
    
    ASSERT_EQ(initial, destination);
    
    destination_alt = msgBuf.read_int64();
    
    ASSERT_EQ(initial, destination_alt);
}

TEST(MessageBufferTest, TestRW_float)
{
    MessageBuffer msgBuf;
    
    float initial(3.1415927f), destination(0), destination_alt(0);
    
    msgBuf.write_float(initial);
    msgBuf.write_float(initial);
    
    msgBuf.read_float(&destination);
    
    ASSERT_EQ(initial, destination);
    
    destination_alt = msgBuf.read_float();
    
    ASSERT_EQ(initial, destination_alt);
}

TEST(MessageBufferTest, TestRW_double)
{
    MessageBuffer msgBuf;
    
    double initial(2.7182818), destination(0), destination_alt(0);
    
    msgBuf.write_double(initial);
    msgBuf.write_double(initial);
    
    msgBuf.read_double(&destination);
    
    ASSERT_EQ(initial, destination);
    
    destination_alt = msgBuf.read_double();
    
    ASSERT_EQ(initial, destination_alt);
}

TEST(MessageBufferTest, TestRW_boolTrue)
{
    MessageBuffer msgBuf;
    
    bool initial(true), destination(false);
    
    msgBuf.write_bool(initial);
    
    destination = msgBuf.read_bool();
    
    ASSERT_EQ(initial, destination);
}

TEST(MessageBufferTest, TestRW_boolFalse)
{
    MessageBuffer msgBuf;
    
    bool initial(false), destination(true);
    
    msgBuf.write_bool(initial);
    
    destination = msgBuf.read_bool();
    
    ASSERT_EQ(initial, destination);
}

TEST(MessageBufferTest, TestRW_cString)
{
    MessageBuffer msgBuf;
    
    const char* initial = "a C style string";
    char* destination;
    
    uint32_t initialSize(0), destinationSize(0);
    
    initialSize = strlen(initial);
    
    msgBuf.write_uint32(initialSize);
    msgBuf.write_bytes(initial, initialSize);
    
    msgBuf.read_uint32(&destinationSize);
    
    ASSERT_EQ(initialSize, destinationSize);
    
    destination = new char[destinationSize + 1];
    memset(destination, '\0', destinationSize + 1);
    msgBuf.read_bytes(destination, destinationSize);
    
    ASSERT_STREQ(initial, destination);
}

TEST(MessageBufferTest, TestRW_stdString)
{
    MessageBuffer msgBuf;
    
    std::string initial("a std:string"), destination("");
    
    msgBuf.write_string(initial);
    
    destination = msgBuf.read_string();
    
    ASSERT_EQ(initial, destination);
}

TEST(MessageBufferTest, RevertEndianness)
{
    MessageBuffer MsgBuf;
    
    cout << "    Re-write on buffer..." << endl;
    
    uint8_t u8 = 232;
    int8_t i8 = -125;
    uint16_t u16 = 0xFFAA;
    int16_t i16 = -576;
    uint32_t u32 = 0xFFAAEEBB;
    int32_t i32 = -9999;
    uint64_t u64 = 0xFFEEDDCC;
    int64_t i64 = -1000000000;
    float f32 = 3.1415927f;
    double d64 = 2.7182818;
    
    uint8_t vu8 = 0;
    int8_t vi8 = 0;
    uint16_t vu16 = 0;
    int16_t vi16 = 0;
    uint32_t vu32 = 0;
    int32_t vi32 = 0;
    uint64_t vu64 = 0;
    int64_t vi64 = 0;
    float vf32 = 0.0;
    double vd64 = 0.0;
    
    MsgBuf.write_uint8(u8);
    cout << "        Written <";
    MessageBuffer::show(&u8, 1);
    cout << ">" << endl;
    MsgBuf.write_uint16(u16);
    cout << "        Written <";
    MessageBuffer::show(&u16, 2);
    cout << ">" << endl;
    MsgBuf.write_uint32(u32);
    cout << "        Written <";
    MessageBuffer::show(&u32, 4);
    cout << ">" << endl;
    MsgBuf.write_uint64(u64);
    cout << "        Written <";
    MessageBuffer::show(&u64, 8);
    cout << ">" << endl;
    MsgBuf.write_int8(i8);
    cout << "        Written <";
    MessageBuffer::show(&i8, 1);
    cout << ">" << endl;
    MsgBuf.write_int16(i16);
    cout << "        Written <";
    MessageBuffer::show(&i16, 2);
    cout << ">" << endl;
    MsgBuf.write_int32(i32);
    cout << "        Written <";
    MessageBuffer::show(&i32, 4);
    cout << ">" << endl;
    MsgBuf.write_int64(i64);
    cout << "        Written <";
    MessageBuffer::show(&i64, 8);
    cout << ">" << endl;
    MsgBuf.write_float(f32);
    cout << "        Written <";
    MessageBuffer::show(&f32, 4);
    cout << ">" << endl;
    MsgBuf.write_double(d64);
    cout << "        Written <";
    MessageBuffer::show(&d64, 8);
    cout << ">" << endl;
    
    cout << "    ...and assume it is ";
    if (MessageBuffer::HostIsBigEndian()) {
        cout << "Little Endian";
        MsgBuf.assumeBufferIsLittleEndian();
        cout << " whereas the host was big" << endl;
    }
    if (MessageBuffer::HostIsLittleEndian()) {
        cout << "Big Endian";
        MsgBuf.assumeBufferIsBigEndian();
        cout << " whereas the host was little" << endl;
    }
    MsgBuf.read_uint8(&vu8);
    cout << "        Read  <";
    MessageBuffer::show(&vu8, 1);
    cout << ">" << endl;
    MsgBuf.read_uint16(&vu16);
    cout << "        Read <";
    MessageBuffer::show(&vu16, 2);
    cout << ">" << endl;
    MsgBuf.read_uint32(&vu32);
    cout << "        Read <";
    MessageBuffer::show(&vu32, 4);
    cout << ">" << endl;
    MsgBuf.read_uint64(&vu64);
    cout << "        Read <";
    MessageBuffer::show(&vu64, 8);
    cout << ">" << endl;
    MsgBuf.read_int8(&vi8);
    cout << "        Read <";
    MessageBuffer::show(&vi8, 1);
    cout << ">" << endl;
    MsgBuf.read_int16(&vi16);
    cout << "        Read <";
    MessageBuffer::show(&vi16, 2);
    cout << ">" << endl;
    MsgBuf.read_int32(&vi32);
    cout << "        Read <";
    MessageBuffer::show(&vi32, 4);
    cout << ">" << endl;
    MsgBuf.read_int64(&vi64);
    cout << "        Read <";
    MessageBuffer::show(&vi64, 8);
    cout << ">" << endl;
    MsgBuf.read_float(&vf32);
    cout << "        Read <";
    MessageBuffer::show(&vf32, 4);
    cout << ">" << endl;
    MsgBuf.read_double(&vd64);
    cout << "        Read <";
    MessageBuffer::show(&vd64, 8);
    cout << ">" << endl;
}

TEST(MessageBufferTest, Overload)
{
    MessageBuffer MsgBuf;
    
    cout << "    Current (Default) MessageBuffer MaxSize               = " << MsgBuf.maxSize() << endl;
    MsgBuf.reset();
    uint32_t u32 = MsgBuf.maxSize() * 2;
    char* vstr = new char[u32 + 1];
    vstr[u32] = '\0';
    memset(vstr, 'A', u32);
    MsgBuf.write_uint32(u32);
    MsgBuf.write_chars(vstr, u32);
    cout << "    Written char* is " << vstr << endl;
    u32 = MsgBuf.size();
    delete[] vstr;
    cout << "    Current           MessageBuffer MaxSize               = " << MsgBuf.maxSize() << endl;
    std::string vstdstr = MsgBuf.read_string();
    cout << "    Read String is   " << vstdstr << endl;
    cout << "    Forcibly resize the buffer to " << MsgBuf.maxSize() * 2 << endl;
    MsgBuf.resize(MsgBuf.maxSize() * 2);
    cout << "    Current           MessageBuffer MaxSize               = " << MsgBuf.maxSize() << endl;
    cout << "    Now reset buffer and assume size is                   = " << u32 << endl;
    MsgBuf.reset();
    MsgBuf.assumeSize(u32);
    vstdstr = MsgBuf.read_string();
    cout << "    (re)Read String is   " << vstdstr << endl;
}

TEST(MessageBufferTest, BufferCopyToEmulateNetworkSend)
{
    MessageBuffer MsgBuf;
    MessageBuffer MsgBuf2;
    
    uint8_t u8 = 232;
    uint16_t u16 = 0xFFAA;
    uint32_t u32 = 0xFFAAEEBB;
    
    uint8_t vu8 = 0;
    uint16_t vu16 = 0;
    uint32_t vu32 = 0;
    
    cout << "    preparing a buffer for network send..." << endl;
    MsgBuf.write_uint32(u32);
    MsgBuf.write_uint8(u8);
    MsgBuf.write_uint16(u16);
    
    MsgBuf.updateReservedBytes();
    
    cout << "    now (pseudo) sending a buffer of size " << MsgBuf.size() << " bytes ..." << endl;
    memcpy(MsgBuf2(0), MsgBuf(0), MsgBuf.reservedBytes);
    
    cout << "    MsgBuf = ";
    MsgBuf.show(MsgBuf(0), MsgBuf.reservedBytes);
    cout << endl;
    cout << "    MsgBuf2 = ";
    MsgBuf2.show(MsgBuf(0), MsgBuf2.reservedBytes);
    cout << endl;
    
    cout << "    now (pseudo) receiving buffer header of " << (uint32_t) MsgBuf.reservedBytes << " bytes ..." << endl;
    cout << "    reconstruct receive buffer size from buffer header ..." << endl;
    MsgBuf2.assumeSizeFromReservedBytes();
    
    cout << "    seems to contain " << MsgBuf2.size() << " bytes..." << endl;
    cout << "    now (pseudo) receiving buffer content ..." << endl;
    memcpy(MsgBuf2(MsgBuf.reservedBytes), MsgBuf(MsgBuf.reservedBytes), MsgBuf2.size());
    
    cout << "    reading buffer content..." << endl;
    vu32 = MsgBuf.read_uint32();
    EXPECT_EQ(u32, vu32);
    vu8 = MsgBuf.read_uint8();
    EXPECT_EQ(u8, vu8);
    vu16 = MsgBuf.read_uint16();
    EXPECT_EQ(u16, vu16);
}

#ifdef HOST_IS_BIG_ENDIAN
TEST(MessageBufferTest, BigEndianHost)
{
    EXPECT_TRUE(MessageBuffer::HostIsBigEndian());
    EXPECT_FALSE(MessageBuffer::HostIsLittleEndian());
}
#else
TEST(MessageBufferTest, LittleEndianHost)
{
    EXPECT_TRUE(MessageBuffer::HostIsLittleEndian());
    EXPECT_FALSE(MessageBuffer::HostIsBigEndian());
}
#endif
