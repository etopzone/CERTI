#include <gtest/gtest.h>

#include <iostream>

#include <libHLA/MsgBuffer.h>

using ::std::cout;
using ::std::endl;

TEST(MsgBufferTest, TestRW_uint8)
{
    MsgBuffer_t MsgBuf;
    MB_create(&MsgBuf);

    uint8_t initial(232), destination(0), destination_alt(0);

    MB_write_uint8(&MsgBuf, initial);
    MB_write_uint8(&MsgBuf, initial);

    MB_read_uint8(&MsgBuf, &destination);

    ASSERT_EQ(initial, destination);

    destination_alt = MB_get_uint8(&MsgBuf);

    ASSERT_EQ(initial, destination_alt);

    MB_destroy(&MsgBuf);
}

TEST(MsgBufferTest, TestRW_int8)
{
    MsgBuffer_t MsgBuf;
    MB_create(&MsgBuf);

    int8_t initial(-125), destination(0), destination_alt(0);

    MB_write_int8(&MsgBuf, initial);
    MB_write_int8(&MsgBuf, initial);

    MB_read_int8(&MsgBuf, &destination);

    ASSERT_EQ(initial, destination);

    destination_alt = MB_get_int8(&MsgBuf);

    ASSERT_EQ(initial, destination_alt);

    MB_destroy(&MsgBuf);
}

TEST(MsgBufferTest, TestRW_uint16)
{
    MsgBuffer_t MsgBuf;
    MB_create(&MsgBuf);

    uint16_t initial(0xFFAA), destination(0), destination_alt(0);

    MB_write_uint16(&MsgBuf, initial);
    MB_write_uint16(&MsgBuf, initial);

    MB_read_uint16(&MsgBuf, &destination);

    ASSERT_EQ(initial, destination);

    destination_alt = MB_get_uint16(&MsgBuf);

    ASSERT_EQ(initial, destination_alt);

    MB_destroy(&MsgBuf);
}

TEST(MsgBufferTest, TestRW_int16)
{
    MsgBuffer_t MsgBuf;
    MB_create(&MsgBuf);

    int16_t initial(-576), destination(0), destination_alt(0);

    MB_write_int16(&MsgBuf, initial);
    MB_write_int16(&MsgBuf, initial);

    MB_read_int16(&MsgBuf, &destination);

    ASSERT_EQ(initial, destination);

    destination_alt = MB_get_int16(&MsgBuf);

    ASSERT_EQ(initial, destination_alt);

    MB_destroy(&MsgBuf);
}

TEST(MsgBufferTest, TestRW_uint32)
{
    MsgBuffer_t MsgBuf;
    MB_create(&MsgBuf);

    uint32_t initial(0xFFAAEEBB), destination(0), destination_alt(0);

    MB_write_uint32(&MsgBuf, initial);
    MB_write_uint32(&MsgBuf, initial);

    MB_read_uint32(&MsgBuf, &destination);

    ASSERT_EQ(initial, destination);

    destination_alt = MB_get_uint32(&MsgBuf);

    ASSERT_EQ(initial, destination_alt);

    MB_destroy(&MsgBuf);
}

TEST(MsgBufferTest, TestRW_int32)
{
    MsgBuffer_t MsgBuf;
    MB_create(&MsgBuf);

    int32_t initial(-9999), destination(0), destination_alt(0);

    MB_write_int32(&MsgBuf, initial);
    MB_write_int32(&MsgBuf, initial);

    MB_read_int32(&MsgBuf, &destination);

    ASSERT_EQ(initial, destination);

    destination_alt = MB_get_int32(&MsgBuf);

    ASSERT_EQ(initial, destination_alt);

    MB_destroy(&MsgBuf);
}

TEST(MsgBufferTest, TestRW_uint64)
{
    MsgBuffer_t MsgBuf;
    MB_create(&MsgBuf);

    uint64_t initial(0xFFEEDDCC), destination(0), destination_alt(0);

    MB_write_uint64(&MsgBuf, initial);
    MB_write_uint64(&MsgBuf, initial);

    MB_read_uint64(&MsgBuf, &destination);

    ASSERT_EQ(initial, destination);

    destination_alt = MB_get_uint64(&MsgBuf);

    ASSERT_EQ(initial, destination_alt);

    MB_destroy(&MsgBuf);
}

TEST(MsgBufferTest, TestRW_int64)
{
    MsgBuffer_t MsgBuf;
    MB_create(&MsgBuf);

    int64_t initial(-1000000000), destination(0), destination_alt(0);

    MB_write_int64(&MsgBuf, initial);
    MB_write_int64(&MsgBuf, initial);

    MB_read_int64(&MsgBuf, &destination);

    ASSERT_EQ(initial, destination);

    destination_alt = MB_get_int64(&MsgBuf);

    ASSERT_EQ(initial, destination_alt);

    MB_destroy(&MsgBuf);
}

TEST(MsgBufferTest, TestRW_float)
{
    MsgBuffer_t MsgBuf;
    MB_create(&MsgBuf);

    float initial(3.1415927f), destination(0), destination_alt(0);

    MB_write_float(&MsgBuf, initial);
    MB_write_float(&MsgBuf, initial);

    MB_read_float(&MsgBuf, &destination);

    ASSERT_EQ(initial, destination);

    destination_alt = MB_get_float(&MsgBuf);

    ASSERT_EQ(initial, destination_alt);

    MB_destroy(&MsgBuf);
}

TEST(MsgBufferTest, TestRW_double)
{
    MsgBuffer_t MsgBuf;
    MB_create(&MsgBuf);

    double initial(2.7182818), destination(0), destination_alt(0);

    MB_write_double(&MsgBuf, initial);
    MB_write_double(&MsgBuf, initial);

    MB_read_double(&MsgBuf, &destination);

    ASSERT_EQ(initial, destination);

    destination_alt = MB_get_double(&MsgBuf);

    ASSERT_EQ(initial, destination_alt);

    MB_destroy(&MsgBuf);
}

TEST(MsgBufferTest, TestRW_boolTrue)
{
    MsgBuffer_t MsgBuf;
    MB_create(&MsgBuf);

    bool_t initial(true), destination(false);

    MB_write_bool(&MsgBuf, initial);

    MB_read_bool(&MsgBuf, &destination);

    ASSERT_EQ(initial, destination);

    MB_destroy(&MsgBuf);
}

TEST(MsgBufferTest, TestRW_boolFalse)
{
    MsgBuffer_t MsgBuf;
    MB_create(&MsgBuf);

    bool_t initial(false), destination(true);

    MB_write_bool(&MsgBuf, initial);

    MB_read_bool(&MsgBuf, &destination);

    ASSERT_EQ(initial, destination);

    MB_destroy(&MsgBuf);
}

TEST(MsgBufferTest, TestRW_cString)
{
    MsgBuffer_t MsgBuf;
    MB_create(&MsgBuf);

    const char* initial = "a C style string";
    char* destination;

    uint32_t initialSize(0), destinationSize(0);

    initialSize = strlen(initial);

    MB_write_uint32(&MsgBuf, initialSize);
    MB_write_bytes(&MsgBuf, const_cast<char*>(initial), initialSize);

    MB_read_uint32(&MsgBuf, &destinationSize);

    ASSERT_EQ(initialSize, destinationSize);

    destination = new char[destinationSize + 1];
    memset(destination, '\0', destinationSize + 1);
    MB_read_bytes(&MsgBuf, destination, destinationSize);

    ASSERT_STREQ(initial, destination);

    MB_destroy(&MsgBuf);
}

TEST(MsgBufferTest, RevertEndianness)
{
    MsgBuffer_t MsgBuf;
    MB_create(&MsgBuf);

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

    MB_write_uint8(&MsgBuf, u8);
    cout << "        Written <";
    MB_show(&u8, 1);
    cout << ">" << endl;
    MB_write_uint16(&MsgBuf, u16);
    cout << "        Written <";
    MB_show(&u16, 2);
    cout << ">" << endl;
    MB_write_uint32(&MsgBuf, u32);
    cout << "        Written <";
    MB_show(&u32, 4);
    cout << ">" << endl;
    MB_write_uint64(&MsgBuf, u64);
    cout << "        Written <";
    MB_show(&u64, 8);
    cout << ">" << endl;
    MB_write_int8(&MsgBuf, i8);
    cout << "        Written <";
    MB_show(&i8, 1);
    cout << ">" << endl;
    MB_write_int16(&MsgBuf, i16);
    cout << "        Written <";
    MB_show(&i16, 2);
    cout << ">" << endl;
    MB_write_int32(&MsgBuf, i32);
    cout << "        Written <";
    MB_show(&i32, 4);
    cout << ">" << endl;
    MB_write_int64(&MsgBuf, i64);
    cout << "        Written <";
    MB_show(&i64, 8);
    cout << ">" << endl;
    MB_write_float(&MsgBuf, f32);
    cout << "        Written <";
    MB_show(&f32, 4);
    cout << ">" << endl;
    MB_write_double(&MsgBuf, d64);
    cout << "        Written <";
    MB_show(&d64, 8);
    cout << ">" << endl;

    cout << "    ...and assume it is ";
    if (MB_HostIsBigEndian()) {
        cout << "Little Endian";
        MB_assumeBufferIsLittleEndian(&MsgBuf);
        cout << " whereas the host was big" << endl;
    }
    if (MB_HostIsLittleEndian()) {
        cout << "Big Endian";
        MB_assumeBufferIsBigEndian(&MsgBuf);
        cout << " whereas the host was little" << endl;
    }
    MB_read_uint8(&MsgBuf, &vu8);
    cout << "        Read  <";
    MB_show(&vu8, 1);
    cout << ">" << endl;
    MB_read_uint16(&MsgBuf, &vu16);
    cout << "        Read <";
    MB_show(&vu16, 2);
    cout << ">" << endl;
    MB_read_uint32(&MsgBuf, &vu32);
    cout << "        Read <";
    MB_show(&vu32, 4);
    cout << ">" << endl;
    MB_read_uint64(&MsgBuf, &vu64);
    cout << "        Read <";
    MB_show(&vu64, 8);
    cout << ">" << endl;
    MB_read_int8(&MsgBuf, &vi8);
    cout << "        Read <";
    MB_show(&vi8, 1);
    cout << ">" << endl;
    MB_read_int16(&MsgBuf, &vi16);
    cout << "        Read <";
    MB_show(&vi16, 2);
    cout << ">" << endl;
    MB_read_int32(&MsgBuf, &vi32);
    cout << "        Read <";
    MB_show(&vi32, 4);
    cout << ">" << endl;
    MB_read_int64(&MsgBuf, &vi64);
    cout << "        Read <";
    MB_show(&vi64, 8);
    cout << ">" << endl;
    MB_read_float(&MsgBuf, &vf32);
    cout << "        Read <";
    MB_show(&vf32, 4);
    cout << ">" << endl;
    MB_read_double(&MsgBuf, &vd64);
    cout << "        Read <";
    MB_show(&vd64, 8);
    cout << ">" << endl;

    MB_destroy(&MsgBuf);
}

TEST(MsgBufferTest, Overload)
{
    MsgBuffer_t MsgBuf;
    MB_create(&MsgBuf);

    cout << "    Current (Default) MessageBuffer MaxSize               = " << MB_maxSize(&MsgBuf) << endl;
    MB_reset(&MsgBuf);
    uint32_t u32 = MB_maxSize(&MsgBuf) * 2;
    char* vstr = new char[u32 + 1];
    vstr[u32] = '\0';
    memset(vstr, 'A', u32);
    MB_write_uint32(&MsgBuf, u32);
    MB_write_chars(&MsgBuf, vstr, u32);
    cout << "    Written char* is " << vstr << endl;
    u32 = MB_size(&MsgBuf);
    delete[] vstr;
    cout << "    Current           MessageBuffer MaxSize               = " << MB_maxSize(&MsgBuf) << endl;
    char bigStr[2048];
    MB_read_string(&MsgBuf, bigStr, u32);
    cout << "    Read String is   " << bigStr << endl;
    cout << "    Forcibly resize the buffer to " << MB_maxSize(&MsgBuf) * 2 << endl;
    MB_resize(&MsgBuf, MB_maxSize(&MsgBuf) * 2);
    cout << "    Current           MessageBuffer MaxSize               = " << MB_maxSize(&MsgBuf) << endl;
    cout << "    Now reset buffer and assume size is                   = " << u32 << endl;
    MB_reset(&MsgBuf);
    MB_assumeSize(&MsgBuf, u32);
    memset(bigStr, '\0', 2048);
    MB_read_string(&MsgBuf, bigStr, u32);
    cout << "    (re)Read String is   " << bigStr << endl;

    MB_destroy(&MsgBuf);
}

TEST(MsgBufferTest, BufferCopyToEmulateNetworkSend)
{
    MsgBuffer_t MsgBuf;
    MB_create(&MsgBuf);
    MsgBuffer_t MsgBuf2;
    MB_create(&MsgBuf2);

    uint8_t u8 = 232;
    uint16_t u16 = 0xFFAA;
    uint32_t u32 = 0xFFAAEEBB;

    uint8_t vu8 = 0;
    uint16_t vu16 = 0;
    uint32_t vu32 = 0;

    cout << "    preparing a buffer for network send..." << endl;
    MB_write_uint32(&MsgBuf, u32);
    MB_write_uint8(&MsgBuf, u8);
    MB_write_uint16(&MsgBuf, u16);

    MB_updateReservedBytes(&MsgBuf);

    cout << "    now (pseudo) sending a buffer of size " << MB_size(&MsgBuf) << " bytes ..." << endl;
    memcpy(MB_get(&MsgBuf2, 0), MB_get(&MsgBuf, 0), MB_RESERVED_BYTES);

    cout << "    MsgBuf = ";
    MB_show(MB_get(&MsgBuf, 0), MB_RESERVED_BYTES);
    cout << endl;
    cout << "    MsgBuf2 = ";
    MB_show(MB_get(&MsgBuf2, 0), MB_RESERVED_BYTES);
    cout << endl;

    cout << "    now (pseudo) receiving buffer header of " << (uint32_t) MB_RESERVED_BYTES << " bytes ..." << endl;
    cout << "    reconstruct receive buffer size from buffer header ..." << endl;
    MB_assumeSizeFromReservedBytes(&MsgBuf2);

    cout << "    seems to contain " << MB_size(&MsgBuf2) << " bytes..." << endl;
    cout << "    now (pseudo) receiving buffer content ..." << endl;
    memcpy(MB_get(&MsgBuf2, MB_RESERVED_BYTES), MB_get(&MsgBuf, MB_RESERVED_BYTES), MB_size(&MsgBuf2));

    cout << "    reading buffer content..." << endl;
    vu32 = MB_get_uint32(&MsgBuf);
    EXPECT_EQ(u32, vu32);
    vu8 = MB_get_uint8(&MsgBuf);
    EXPECT_EQ(u8, vu8);
    vu16 = MB_get_uint16(&MsgBuf);
    EXPECT_EQ(u16, vu16);

    MB_destroy(&MsgBuf);
    MB_destroy(&MsgBuf2);
}

#ifdef HOST_IS_BIG_ENDIAN
TEST(MsgBufferTest, BigEndianHost)
{
    EXPECT_TRUE(MB_HostIsBigEndian());
    EXPECT_FALSE(MB_HostIsLittleEndian());
}
#else
TEST(MsgBufferTest, LittleEndianHost)
{
    EXPECT_TRUE(MB_HostIsLittleEndian());
    EXPECT_FALSE(MB_HostIsBigEndian());
}
#endif
