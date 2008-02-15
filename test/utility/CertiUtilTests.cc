// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This file is part of CERTI
//
// CERTI is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// ----------------------------------------------------------------------------
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include "config.h"
#include "PrettyDebug.hh"
#include "MessageBuffer.hh"
#include <cassert>
#include <string>

#include "Clock.hh"
void clockTests(certi::Clock& aClock) {
	uint64_t tick1;
	uint64_t tick2;
	cout << "Testing clock <" << aClock.getName() << "> BEGIN..." <<endl; 
	cout << "    Clock resolution is: " << aClock.getResolution() << " nano-seconds" <<endl;
	tick1 = aClock.getCurrentTicksValue();
	tick2 = aClock.getCurrentTicksValue();
	cout << "    Tick1 = " << tick1 << endl;
	cout << "    Tick2 = " << tick2 << endl;
	cout << "    Delta = " << aClock.getDeltaNanoSecond(tick1,tick2) << " nano-seconds" <<endl;
	cout << "Testing clock <" << aClock.getName() << "> END." <<endl; 
}
#ifdef HAVE_TSC_CLOCK
#include "TSCClock.hh"
#endif
#ifdef HAVE_POSIX_CLOCK
#include "PosixClock.hh"
#endif

void messageBufferTests(certi::MessageBuffer& MsgBuf) {
	std::string    stdstr = "a std:string";
	const char*   str = "a char* string";
	uint8_t  u8   =  232;
	int8_t   i8   = -125;
	uint16_t u16  = 0xFFAA;
	int16_t  i16  = -576;
	uint32_t u32  = 0xFFAAEEBB;
	int32_t  i32  = -9999;
	uint64_t u64  = 0xFFEEDDCC;
	int64_t  i64  = -1000000000;
	float    f32  = 3.1415927;
	double   d64  = 2.7182818;
	char*    vstr = NULL;
	std::string  vstdstr;
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
	cout << "Testing MessageBuffer class BEGIN..."<<endl;
	cout << "    Current (Default) MessageBuffer MaxSize               = "<< MsgBuf.maxSize() <<endl;
	cout << "    Current (initially void) MessageBuffer size = "<< MsgBuf.size()<<endl;
	cout << "    Encoding to buffer..." <<endl;
	cout << "    bytes string = " << str << endl;
	MsgBuf.write_uint32(strlen(str));
	MsgBuf.write_bytes(str,strlen(str));
	MsgBuf.write_uint8(u8);
	MsgBuf.write_uint16(u16);
	MsgBuf.write_uint32(u32);
	MsgBuf.write_uint64(u64);
	MsgBuf.write_int8(i8);
	MsgBuf.write_int16(i16);
	MsgBuf.write_int32(i32);
	MsgBuf.write_int64(i64);
	MsgBuf.write_float(f32);
	MsgBuf.write_double(d64);
	cout << "    std::string = " << stdstr << endl;
	MsgBuf.write_string(stdstr);
	cout << "    Current MessageBuffer size                  = "<< MsgBuf.size()<<endl;
	cout << "    Decoding from buffer..." <<endl;
	MsgBuf.read_uint32(&vu32);
	vstr = new char[vu32+1];
	memset(vstr,'\0',vu32+1);
	MsgBuf.read_bytes(vstr,vu32);
	cout << "    bytes string = " << vstr << endl; assert(0==strcmp(str,vstr));
	MsgBuf.read_uint8(&vu8); assert(vu8==u8);
	MsgBuf.read_uint16(&vu16); assert(vu16==u16);
	MsgBuf.read_uint32(&vu32); assert(vu32==u32);
	MsgBuf.read_uint64(&vu64); assert(vu64==u64);
	MsgBuf.read_int8(&vi8); assert(vi8==i8);
	MsgBuf.read_int16(&vi16); assert(vi16==i16);
	MsgBuf.read_int32(&vi32); assert(vi32==i32);
	MsgBuf.read_int64(&vi64); assert(vi64==i64);
	MsgBuf.read_float(&vf32); assert(vf32==f32);
	MsgBuf.read_double(&vd64); assert(vd64==d64);	
	vstdstr  = MsgBuf.read_string(); assert(vstdstr==stdstr);
	cout << "    std::string = " << vstdstr << endl;
	cout << "    All encoded/decoded values are equal." << endl;
	cout << "    Now reset buffer." << endl;
	MsgBuf.resetBuffer();
	cout << "    Re-write on buffer..." << endl;
	MsgBuf.write_uint8(u8);  cout << "        Written <"; certi::MessageBuffer::show(&u8,1); cout << ">" << endl;
	MsgBuf.write_uint16(u16);cout << "        Written <"; certi::MessageBuffer::show(&u16,2); cout << ">" << endl;
	MsgBuf.write_uint32(u32);cout << "        Written <"; certi::MessageBuffer::show(&u32,4); cout << ">" << endl;
	MsgBuf.write_uint64(u64);cout << "        Written <"; certi::MessageBuffer::show(&u64,8); cout << ">" << endl;
	MsgBuf.write_int8(i8);cout << "        Written <"; certi::MessageBuffer::show(&i8,1); cout << ">" << endl;
	MsgBuf.write_int16(i16);cout << "        Written <"; certi::MessageBuffer::show(&i16,2); cout << ">" << endl;
	MsgBuf.write_int32(i32);cout << "        Written <"; certi::MessageBuffer::show(&i32,4); cout << ">" << endl;
	MsgBuf.write_int64(i64);cout << "        Written <"; certi::MessageBuffer::show(&i64,8); cout << ">" << endl;
	MsgBuf.write_float(f32);cout << "        Written <"; certi::MessageBuffer::show(&f32,4); cout << ">" << endl;
	MsgBuf.write_double(d64);cout << "        Written <"; certi::MessageBuffer::show(&d64,8); cout << ">" << endl;

	cout << "    ...and assume it is ";    
	if (certi::MessageBuffer::HostIsBigEndian()) {
		cout << "Little Endian";
		MsgBuf.assumeBufferIsLittleEndian();
		cout << " whereas the host was big" << endl;
	}
	if (certi::MessageBuffer::HostIsLittleEndian()) {
		cout << "Big Endian";
		MsgBuf.assumeBufferIsBigEndian();
		cout << " whereas the host was little" << endl;
	}
	MsgBuf.read_uint8(&vu8);   cout << "        Read  <"; certi::MessageBuffer::show(&vu8,1); cout << ">" << endl;
	MsgBuf.read_uint16(&vu16); cout << "        Read <"; certi::MessageBuffer::show(&vu16,2); cout << ">" << endl;
	MsgBuf.read_uint32(&vu32); cout << "        Read <"; certi::MessageBuffer::show(&vu32,4); cout << ">" << endl;
	MsgBuf.read_uint64(&vu64); cout << "        Read <"; certi::MessageBuffer::show(&vu64,8); cout << ">" << endl;
	MsgBuf.read_int8(&vi8);    cout << "        Read <"; certi::MessageBuffer::show(&vi8,1); cout << ">" << endl;
	MsgBuf.read_int16(&vi16);  cout << "        Read <"; certi::MessageBuffer::show(&vi16,2); cout << ">" << endl;
	MsgBuf.read_int32(&vi32);  cout << "        Read <"; certi::MessageBuffer::show(&vi32,4); cout << ">" << endl;
	MsgBuf.read_int64(&vi64);  cout << "        Read <"; certi::MessageBuffer::show(&vi64,8); cout << ">" << endl;
	MsgBuf.read_float(&vf32);  cout << "        Read <"; certi::MessageBuffer::show(&vf32,4); cout << ">" << endl;
	MsgBuf.read_double(&vd64); cout << "        Read <"; certi::MessageBuffer::show(&vd64,8); cout << ">" << endl;
	delete[] vstr;
	
	cout << "    Trying to overload the buffer..." <<endl;
	cout << "    Current (Default) MessageBuffer MaxSize               = "<< MsgBuf.maxSize() <<endl;
	u32 = MsgBuf.maxSize()*2;
	vstr = new char[u32+1];
	vstr[u32]='\0';
	memset(vstr,'A',u32);	
	MsgBuf.write_uint32(u32);
	MsgBuf.write_chars(vstr,u32);	
	cout << "    Written char* is " << vstr << endl;
	delete[] vstr;
	cout << "    Current           MessageBuffer MaxSize               = "<< MsgBuf.maxSize() <<endl;
	vstdstr = MsgBuf.read_string();
	cout << "    Read String is   " << vstdstr << endl;
	cout << "Testing MessageBuffer class END."<<endl;
} /* end of messageBufferTests */

int
main(int argc, char **argv)
{	
#ifdef HAVE_POSIX_CLOCK	
	certi::PosixClock posixClk;
#endif
#ifdef HAVE_TSC_CLOCK	
	certi::TSCClock   tscClk;
#endif
	certi::MessageBuffer MsgBuf;

	cout << "CERTI Utility Tests->BEGIN..."<< endl ;

	cout << "    Host is ";
	if (certi::MessageBuffer::HostIsBigEndian()) {
		cout << "Big Endian";
	}
	if (certi::MessageBuffer::HostIsLittleEndian()) {
		cout << "Little Endian";
	}
	cout << endl;	
	messageBufferTests(MsgBuf);
#ifdef HAVE_TSC_CLOCK
	clockTests(tscClk);
#endif
#ifdef HAVE_POSIX_CLOCK
	clockTests(posixClk);
#endif
	cout << "CERTI Utility Test->END." <<endl;

	return 0;
}
