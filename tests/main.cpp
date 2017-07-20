#include <gtest/gtest.h>

#ifndef CERTI_TEST
#error CERTI_TEST must be defined for tests
#endif

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
