#include <gtest/gtest.h>

#include <cstdlib>
#include <iostream>
#include <sstream>

#include <include/certi.hh>
#include <fedtime.hh>

namespace {
static constexpr int BUFLEN = 12;
static constexpr int PRINTBUFLEN = 50;
}

std::string show(const void* data, uint32_t n)
{
    uint32_t i;
    std::stringstream msg;

    const uint8_t* u8data = reinterpret_cast<const uint8_t*>(data);
    msg << "0x";
    for (i = 0; i < n; ++i) {
        if ((0 == (i % 2)) && (i > 0)) {
            msg << " ";
        }
        msg << std::uppercase << std::hex << (int) u8data[i];
    }
    return msg.str();
}

TEST(FedTimeTest, HostByteOrder)
{
    std::cout << "Host byte-order: "
#ifdef HOST_IS_BIG_ENDIAN
              << "big-endian" << std::endl;
#else
              << "little-endian" << std::endl;
#endif
}

TEST(FedTimeTest, PrintableLength)
{
    RTI::FedTime* time = RTI::FedTimeFactory::makeZero();
    std::cout << "\t\tPrintableLength = " << time->getPrintableLength() << std::endl;

    ASSERT_LT(time->getPrintableLength(), PRINTBUFLEN) << "Cannot print value, use bigger buffers";
}

TEST(FedTimeTest, Zero)
{
    char printBuffer[PRINTBUFLEN];

    RTI::FedTime* time = RTI::FedTimeFactory::makeZero();

    time->getPrintableString(printBuffer);

    ASSERT_STREQ("0", printBuffer);
}

TEST(FedTimeTest, Epsilon)
{
    char printBuffer[PRINTBUFLEN];

    RTI::FedTime* time = RTI::FedTimeFactory::makeZero();

    time->setEpsilon();
    time->getPrintableString(printBuffer);

    // TODO changes between architectures ?
    std::cout << "\t\tEpsilon = " << printBuffer << std::endl;
}

TEST(FedTimeTest, PositiveInfinity)
{
    char printBuffer[PRINTBUFLEN];

    RTI::FedTime* time = RTI::FedTimeFactory::makeZero();

    time->setPositiveInfinity();
    time->getPrintableString(printBuffer);

    // TODO changes between architectures ?
    std::cout << "\t\tPositive Infinity = " << printBuffer << std::endl;
}

TEST(FedTimeTest, Pi)
{
    char printBuffer[PRINTBUFLEN];

    RTI::FedTime* time = RTI::FedTimeFactory::makeZero();

    time->setZero();
    RTIfedTime pi(RTI::Double(3.14159));
    *time = pi;
    time->getPrintableString(printBuffer);

    // TODO changes between architectures ?
    std::cout << "\t\tPi = " << printBuffer << std::endl;

    double pid = pi.getTime();

    // TODO changes between architectures ?
    std::cout << "\t\traw value = " << show(&pid, sizeof(pid)) << std::endl;
}

TEST(FedTimeTest, EncodeThenDecode)
{
    char buffer[BUFLEN];
    char printBuffer[PRINTBUFLEN];

    RTI::FedTime* time = RTI::FedTimeFactory::makeZero();

    RTIfedTime pi(RTI::Double(3.14159));
    *time = pi;

    // TODO changes between architectures ?
    std::cout << "\t\tencoded length = " << time->encodedLength() << std::endl;

    ASSERT_LT(time->encodedLength(), BUFLEN) << "Cannot encode, use bigger buffers";

    time->encode(buffer);

    // TODO changes between architectures ?
    std::cout << "\t\tencoded value = " << show(buffer, time->encodedLength()) << std::endl;

    RTI::FedTime* time2 = RTI::FedTimeFactory::decode(buffer);
    time2->getPrintableString(printBuffer);

    // TODO changes between architectures ?
    std::cout << "\t\tdecoded value = " << printBuffer << std::endl;
}
