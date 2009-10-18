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
// $Id: TestHLAtypes.cc,v 1.7 2009/10/18 19:54:08 gotthardp Exp $
// ----------------------------------------------------------------------------

#include <memory>
#include <sstream>

#include <HLAtypesIEEE1516.hh>

using namespace libhla;

// IEEE 1516.2, Section 4.12.3, Table 23—Basic data representation table
int test0() {
	int retval=0;
	std::stringstream result2b;
	std::stringstream result4b;
	std::stringstream result8b;
	std::string       expected;

	HLAdata<HLAinteger16LE> I16LE;
	HLAdata<HLAinteger32LE> I32LE;
	HLAdata<HLAinteger64LE> I64LE;
	HLAdata<HLAfloat32LE>   F32LE;
	HLAdata<HLAfloat64LE>   F64LE;
	HLAdata<HLAoctetPairLE> OPLE;

	HLAdata<HLAinteger16BE> I16BE;
	HLAdata<HLAinteger32BE> I32BE;
	HLAdata<HLAinteger64BE> I64BE;
	HLAdata<HLAfloat32BE>   F32BE;
	HLAdata<HLAfloat64BE>   F64BE;
	HLAdata<HLAoctetPairBE> OPBE;

	(*I16LE) = 1;
	expected = "0000:  01 00\n";
	I16LE.print(result2b);
	if(result2b.str()!=expected) {
		std::cerr << "test0: <output> does not match expected result" << std::endl
				<< "result="   << result2b.str() << std::endl
				<< "expected=" << expected << std::endl;
		retval+= 1;
	}

	(*I16BE) = 1;
	expected = "0000:  00 01\n";
	result2b.seekp(0);
	I16BE.print(result2b);
	if(result2b.str()!=expected) {
		std::cerr << "test0: <output> does not match expected result" << std::endl
				<< "result="   << result2b.str() << std::endl
				<< "expected=" << expected << std::endl;
		retval+= 1;
	}

	(*I32LE) = -255;
	expected = "0000:  01 ff ff ff\n";
	result4b.seekp(0);
	I32LE.print(result4b);
	if(result4b.str()!=expected) {
		std::cerr << "test0: <output> does not match expected result" << std::endl
				<< "result="   << result4b.str() << std::endl
				<< "expected=" << expected << std::endl;
		retval+= 1;
	}

	(*I32BE) = -255;
	expected = "0000:  ff ff ff 01\n";
	result4b.seekp(0);
	I32BE.print(result4b);
	if(result4b.str()!=expected) {
		std::cerr << "test0: <output> does not match expected result" << std::endl
				<< "result="   << result4b.str() << std::endl
				<< "expected=" << expected << std::endl;
		retval+= 1;
	}

	(*I64LE) = 256;
	expected = "0000:  00 01 00 00 00 00 00 00\n";
	result8b.seekp(0);
	I64LE.print(result8b);
	if(result8b.str()!=expected) {
		std::cerr << "test0: <output> does not match expected result" << std::endl
				<< "result="   << result8b.str() << std::endl
				<< "expected=" << expected << std::endl;
		retval+= 1;
	}

	(*I64BE) = 256;
	expected = "0000:  00 00 00 00 00 00 01 00\n";
	result8b.seekp(0);
	I64BE.print(result8b);
	if(result8b.str()!=expected) {
		std::cerr << "test0: <output> does not match expected result" << std::endl
				<< "result="   << result8b.str() << std::endl
				<< "expected=" << expected << std::endl;
		retval+= 1;
	}

	(*F32LE) = 10.0;
	expected = "0000:  00 00 20 41\n";
	result4b.seekp(0);
	F32LE.print(result4b);
	if(result4b.str()!=expected) {
		std::cerr << "test0: <output> does not match expected result" << std::endl
				<< "result="   << result4b.str() << std::endl
				<< "expected=" << expected << std::endl;
		retval+= 1;
	}

	(*F32BE) = 10.0;
	expected = "0000:  41 20 00 00\n";
	result4b.seekp(0);
	F32BE.print(result4b);
	if(result4b.str()!=expected) {
		std::cerr << "test0: <output> does not match expected result" << std::endl
				<< "result="   << result4b.str() << std::endl
				<< "expected=" << expected << std::endl;
		retval+= 1;
	}

	(*F64LE) = 10.0;
	expected = "0000:  00 00 00 00 00 00 24 40\n";
	result8b.seekp(0);
	F64LE.print(result8b);
	if(result8b.str()!=expected) {
		std::cerr << "test0: <output> does not match expected result" << std::endl
				<< "result="   << result8b.str() << std::endl
				<< "expected=" << expected << std::endl;
		retval+= 1;
	}

	(*F64BE) = 10.0;
	expected = "0000:  40 24 00 00 00 00 00 00\n";
	result8b.seekp(0);
	F64BE.print(result8b);
	if(result8b.str()!=expected) {
		std::cerr << "test0: <output> does not match expected result" << std::endl
				<< "result="   << result8b.str() << std::endl
				<< "expected=" << expected << std::endl;
		retval+= 1;
	}

	(*OPLE) = 0xABCD;
	expected = "0000:  cd ab\n";
	result2b.seekp(0);
	OPLE.print(result2b);
	if(result2b.str()!=expected) {
		std::cerr << "test0: <output> does not match expected result" << std::endl
				<< "result="   << result2b.str() << std::endl
				<< "expected=" << expected << std::endl;
		retval+= 1;
	}

	(*OPBE) = 0xABCD;
	expected = "0000:  ab cd\n";
	result2b.seekp(0);
	OPBE.print(result2b);
	if(result2b.str()!=expected) {
		std::cerr << "test0: <output> does not match expected result" << std::endl
				<< "result="   << result2b.str() << std::endl
				<< "expected=" << expected << std::endl;
		retval+= 1;
	}

	return retval;
} /* end of test0 */

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

enum test7_enum {
	ENUM_0 = 0,
			ENUM_1 = 1,
			ENUM_2 = 2
};

int test7()
{
	typedef HLAenumeratedType<test7_enum, HLAoctet> TA;

	HLAdata<TA> A;
	*A = ENUM_2;

	std::stringstream result;
	A.print(result);

	const char* correct =
			"0000:  02\n";

	if(strcmp(result.str().c_str(), correct) != 0) {
		std::cerr << "test7: <output> does not match expected result" << std::endl
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

	std::cerr << "Host byte-order: "
#ifdef HOST_IS_BIG_ENDIAN
			<< "big-endian" << std::endl;
#else
	<< "little-endian" << std::endl;
#endif
	result += test0();
	result += test1();
	result += test3();
	result += test4();
	result += test5();
	result += test6();
	result += test7();

	return result;
}

// $Id: TestHLAtypes.cc,v 1.7 2009/10/18 19:54:08 gotthardp Exp $
