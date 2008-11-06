// ----------------------------------------------------------------------------
// TestHLAtypes.cc - Module test for the IEEE 1516.2 compliant datatypes
// Copyright (C) 2008  Petr Gotthard <petr.gotthard@centrum.cz>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1, as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// $Id: TestHLAtypes.cc,v 1.1 2008/11/06 16:18:38 gotthardp Exp $
// ----------------------------------------------------------------------------

#include <memory>
#include <sstream>

#include <HLAtypesIEEE1516.hh>

using namespace libhla;

// IEEE 1516.2, Section 4.12.9.1
int test1()
{
    typedef
      HLAfixedRecord<
#define FIELD_A 0
        HLAfixedField<FIELD_A, HLAoctet,
#define FIELD_B 1
        HLAfixedField<FIELD_B, HLAboolean,
#define FIELD_C 2
        HLAfixedField<FIELD_C, HLAfloat64BE
        > > > > TA;

    HLAdata<TA> A;
    (*A).field<FIELD_A>() = 'A';
    (*A).field<FIELD_B>() = HLAtrue;
    (*A).field<FIELD_C>() = 3.14;

    std::stringstream result;
    A.print(result);

    const char* correct =
        "0000:  41 00 00 00 00 00 00 01 40 09 1e b8 51 eb 85 1f\n";

    if(strcmp(result.str().c_str(), correct) != 0) {
        std::cerr << "test1: <output> does not match expected result" << std::endl
            << result.str() << std::endl << correct << std::endl;
        return 1;
    }
    else {
        std::cout << result.str();
        return 0;
    }
}

// TODO: Add support for HLAvariantRecord

// IEEE 1516.2, Section 4.12.9.3
int test3()
{
    typedef
      HLAfixedRecord<
#define FIELD_A 0
        HLAfixedField<FIELD_A, HLAinteger32BE,
#define FIELD_B 1
        HLAfixedField<FIELD_B, HLAoctet
        > > > TA;

    typedef HLAfixedArray<TA,2> TB;

    HLAdata<TB> B;
    (*B)[0].field<FIELD_B>() = '0';
    (*B)[1].field<FIELD_A>() = 42;
    (*B)[1].field<FIELD_B>() = '1';

    std::stringstream result;
    B.print(result);

    const char* correct =
        "0000:  00 00 00 00 30 00 00 00 00 00 00 2a 31\n";

    if(strcmp(result.str().c_str(), correct) != 0) {
        std::cerr << "test2: <output> does not match expected result" << std::endl
            << result.str() << std::endl << correct << std::endl;
        return 1;
    }
    else {
        std::cout << result.str();
        return 0;
    }
}

// IEEE 1516.2, Section 4.12.9.4
int test4()
{
    typedef HLAvariableArray<HLAfloat64BE> TA;

    HLAdata<TA> A;
    (*A).set_size(1);
    (*A)[0] = 3.14;

    std::stringstream result;
    A.print(result);

    const char* correct =
        "0000:  00 00 00 01 00 00 00 00 40 09 1e b8 51 eb 85 1f\n";

    if(strcmp(result.str().c_str(), correct) != 0) {
        std::cerr << "test4: <output> does not match expected result" << std::endl
            << result.str() << std::endl << correct << std::endl;
        return 1;
    }
    else {
        std::cout << result.str();
        return 0;
    }
}

int test5()
{
    typedef
      HLAvariableArray<
        HLAfixedRecord<
#define FIELD_A 0
          HLAfixedField<FIELD_A, HLAvariableArray<HLAfloat64LE>,
#define FIELD_B 1
          HLAfixedField<FIELD_B, HLAvariableArray<HLAinteger32LE>
          > > > > TA;

    HLAdata<TA> A;
    (*A).set_size(1);
    (*A)[0].field<FIELD_A>().set_size(1);
    (*A)[0].field<FIELD_A>()[0] = 3.14;
    (*A)[0].field<FIELD_B>().set_size(1);
    (*A)[0].field<FIELD_B>()[0] = 7;

    (*A)[0].field<FIELD_A>().set_size(2);
    (*A)[0].field<FIELD_A>()[1] = 2.718;

    std::stringstream result;
    A.print(result);

    const char* correct =
        "0000:  00 00 00 01 00 00 00 00 00 00 00 02 00 00 00 00\n"
        "0010:  1f 85 eb 51 b8 1e 09 40 58 39 b4 c8 76 be 05 40\n"
        "0020:  00 00 00 01 07 00 00 00\n";

    if(strcmp(result.str().c_str(), correct) != 0) {
        std::cerr << "test1: <output> does not match expected result" << std::endl
            << result.str() << std::endl << correct << std::endl;
        return 1;
    }
    else {
        std::cout << result.str();
        return 0;
    }
}

int test6()
{
    HLAdata<HLAASCIIstring> A;
    *A = "UFO";

    std::stringstream result;
    A.print(result);

    const char* correct =
        "0000:  00 00 00 03 55 46 4f\n";

    if(strcmp(result.str().c_str(), correct) != 0) {
        std::cerr << "test6: <output> does not match expected result" << std::endl
            << result.str() << std::endl << correct << std::endl;
        return 1;
    }
    else {
        std::cout << result.str();
        return 0;
    }
}

int main(int argc, char* argv[])
{
    int result = 0;

    result += test1();
    result += test3();
    result += test4();
    result += test5();
    result += test6();

    return result;
}

// $Id: TestHLAtypes.cc,v 1.1 2008/11/06 16:18:38 gotthardp Exp $
