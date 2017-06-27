#include <gtest/gtest.h>

#include <iostream>

#include <libHLA/HLAtypesIEEE1516.hh>

using ::std::cout;
using ::std::endl;
using ::std::stringstream;

using namespace libhla;

namespace {
static constexpr int field_a = 0;
static constexpr int field_b = 1;
static constexpr int field_c = 2;
}

template <class T, typename V>
void testType(const V& value, const std::string& expected)
{
    stringstream result;

    ::libhla::HLAdata<T> data;

    (*data) = value;
    data.print(result);

    ASSERT_EQ(expected, result.str());
}

TEST(HLATypesTest, TestLittleEndianInt16)
{
    testType<HLAinteger16LE>(1, "0000:  01 00\n");
    testType<HLAinteger16LE>(255, "0000:  ff 00\n");
    testType<HLAinteger16LE>(256, "0000:  00 01\n");
}

TEST(HLATypesTest, TestBigEndianInt16)
{
    testType<HLAinteger16BE>(1, "0000:  00 01\n");
    testType<HLAinteger16BE>(255, "0000:  00 ff\n");
    testType<HLAinteger16BE>(256, "0000:  01 00\n");
}

TEST(HLATypesTest, TestLittleEndianInt32)
{
    testType<HLAinteger32LE>(-255, "0000:  01 ff ff ff\n");
}

TEST(HLATypesTest, TestBigEndianInt32)
{
    testType<HLAinteger32BE>(-255, "0000:  ff ff ff 01\n");
}

TEST(HLATypesTest, TestLittleEndianInt64)
{
    testType<HLAinteger64LE>(256, "0000:  00 01 00 00 00 00 00 00\n");
}

TEST(HLATypesTest, TestBigEndianInt64)
{
    testType<HLAinteger64BE>(256, "0000:  00 00 00 00 00 00 01 00\n");
}

TEST(HLATypesTest, TestLittleEndianFloat32)
{
    testType<HLAfloat32LE>(10.0, "0000:  00 00 20 41\n");
}

TEST(HLATypesTest, TestBigEndianFloat32)
{
    testType<HLAfloat32BE>(10.0, "0000:  41 20 00 00\n");
}

TEST(HLATypesTest, TestLittleEndianFloat64)
{
    testType<HLAfloat64LE>(10.0, "0000:  00 00 00 00 00 00 24 40\n");
}

TEST(HLATypesTest, TestBigEndianFloat64)
{
    testType<HLAfloat64BE>(10.0, "0000:  40 24 00 00 00 00 00 00\n");
}

TEST(HLATypesTest, TestLittleEndianOP)
{
    testType<HLAoctetPairLE>(0xABCD, "0000:  cd ab\n");
}

TEST(HLATypesTest, TestBigEndianOP)
{
    testType<HLAoctetPairBE>(0xABCD, "0000:  ab cd\n");
}

TEST(HLATypesTest, Test2)
{
    // clang-format off
    using TA = HLAfixedRecord<HLAfixedField<field_a, HLAoctet,
                              HLAfixedField<field_b, HLAboolean,
                              HLAfixedField<field_c, HLAfloat64BE
                              >>>>;
    // clang-format on

    HLAdata<TA> A;
    (*A).field<field_a>() = 'A';
    (*A).field<field_b>() = HLAtrue;
    (*A).field<field_c>() = 3.14;

    std::stringstream result;
    A.print(result);

    ASSERT_EQ("0000:  41 00 00 00 00 00 00 01 40 09 1e b8 51 eb 85 1f\n", result.str());
}

TEST(HLATypesTest, Test3)

// TODO: Add support for HLAvariantRecord

// IEEE 1516.2, Section 4.12.9.3
{
    // clang-format off
    using TA = HLAfixedRecord<HLAfixedField<field_a, HLAinteger32BE,
                              HLAfixedField<field_b, HLAoctet>>>;
    // clang-format on

    using TB = HLAfixedArray<TA, 2>;

    HLAdata<TB> B;
    (*B)[0].field<field_b>() = '0';
    (*B)[1].field<field_a>() = 42;
    (*B)[1].field<field_b>() = '1';

    std::stringstream result;
    B.print(result);

    ASSERT_EQ("0000:  00 00 00 00 30 00 00 00 00 00 00 2a 31\n", result.str());
}

TEST(HLATypesTest, Test4)
// IEEE 1516.2, Section 4.12.9.4
{
    using TA = HLAvariableArray<HLAfloat64BE>;

    HLAdata<TA> A;
    (*A).set_size(1);
    (*A)[0] = 3.14;

    std::stringstream result;
    A.print(result);

    ASSERT_EQ("0000:  00 00 00 01 00 00 00 00 40 09 1e b8 51 eb 85 1f\n", result.str());
}

TEST(HLATypesTest, Test5)
{
    // clang-format off
    using TA = HLAvariableArray<HLAfixedRecord<HLAfixedField<field_a, HLAvariableArray<HLAfloat64LE>,
                                               HLAfixedField<field_b, HLAvariableArray<HLAinteger32LE>>>>>;
    // clang-format on

    HLAdata<TA> A;
    (*A).set_size(1);
    (*A)[0].field<field_a>().set_size(1);
    (*A)[0].field<field_a>()[0] = 3.14;
    (*A)[0].field<field_b>().set_size(1);
    (*A)[0].field<field_b>()[0] = 7;

    (*A)[0].field<field_a>().set_size(2);
    (*A)[0].field<field_a>()[1] = 2.718;

    std::stringstream result;
    A.print(result);

    ASSERT_EQ("0000:  00 00 00 01 00 00 00 00 00 00 00 02 00 00 00 00\n"
              "0010:  1f 85 eb 51 b8 1e 09 40 58 39 b4 c8 76 be 05 40\n"
              "0020:  00 00 00 01 07 00 00 00\n",
              result.str());
}

TEST(HLATypesTest, Test6)
{
    HLAdata<HLAASCIIstring> A;
    *A = "UFO";

    std::stringstream result;
    A.print(result);

    ASSERT_EQ("0000:  00 00 00 03 55 46 4f\n", result.str());
}

TEST(HLATypesTest, Test7)
{
    enum test7_enum { ENUM_0 = 0, ENUM_1 = 1, ENUM_2 = 2 };

    typedef HLAenumeratedType<test7_enum, HLAoctet> TA;

    HLAdata<TA> A;
    *A = ENUM_2;

    std::stringstream result;
    A.print(result);

    ASSERT_EQ("0000:  02\n", result.str());
}
