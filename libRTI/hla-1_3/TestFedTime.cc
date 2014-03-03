// ----------------------------------------------------------------------------
// TestFedTime.cc - Module test for the libFedTime
// Copyright (C) 2008  Eric Noulard <eric.noulard@gmail.com>
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
// $Id: TestFedTime.cc,v 1.1 2014/03/03 15:18:23 erk Exp $
// ----------------------------------------------------------------------------

#include <iostream>
#include <sstream>
#include <cstdlib>

#include "certi.hh"
#include "fedtime.hh"

#define BUFLEN 12
#define PRINTBUFLEN 50
std::string
show(const void* data, uint32_t n) {
   uint32_t            i;
   std::stringstream msg;

   const uint8_t* u8data = reinterpret_cast<const uint8_t*>(data);
   msg << "0x";
   for (i=0;i<n;++i) {
	   if ((0==(i%2))&&(i>0)) {
	   		   msg << " ";
	   }
	   msg << std::uppercase << std::hex << (int)u8data[i];
   }
   return msg.str();
}

int
main(int argc, char* argv[]) {
	int result = EXIT_SUCCESS;
	char buffer[BUFLEN];
	char printBuffer[PRINTBUFLEN];

	std::cout << "Host byte-order: "
#ifdef HOST_IS_BIG_ENDIAN
	<< "big-endian" << std::endl;
#else
	<< "little-endian" << std::endl;
#endif
	RTI::FedTime*  time = RTI::FedTimeFactory::makeZero();
	std::cout << "PrintableLength   = " << time->getPrintableLength() <<std::endl;
	if (time->getPrintableLength() > (PRINTBUFLEN+1)) {
		std::cerr << "Cannot print value " << PRINTBUFLEN+1 << " < "<< time->getPrintableLength() << std::endl;
		exit(EXIT_FAILURE);
	}
	time->getPrintableString(printBuffer);
	std::cout << "Zero              = " << printBuffer <<std::endl;
	time->setEpsilon();time->getPrintableString(printBuffer);
	std::cout << "Epsilon           = " << printBuffer<<std::endl;
	time->setPositiveInfinity();time->getPrintableString(printBuffer);
	std::cout << "Positive Infinity = " << printBuffer<<std::endl;
	time->setZero();
	RTIfedTime pi(RTI::Double(3.14159));
	*time = pi;
	time->getPrintableString(printBuffer);
	std::cout << "Pi                = " << printBuffer<<std::endl;
	double pid = pi.getTime();
	std::cout << "      raw value   = " << show(&pid,sizeof(pid)) << std::endl;
	std::cout << "Trying to encode time..." <<std::endl;
	std::cout << "  encoded length  = " << time->encodedLength() <<std::endl;
	if (time->encodedLength()<(BUFLEN+1)) {
		time->encode(buffer);
	} else {
	    std::cerr << "Cannot encode :"<< (BUFLEN+1) << "<" << time->encodedLength() << std::endl;
	    return EXIT_FAILURE;
	}
	std::cout << "  encoded value   = " << show(buffer,time->encodedLength()) << std::endl;
	std::cout << "Trying to decode time..." <<std::endl;
	RTI::FedTime*  time2 = RTI::FedTimeFactory::decode(buffer);
	time2->getPrintableString(printBuffer);
	std::cout << "  decoded value   = " << printBuffer << std::endl;
	return result;
}

// $Id: TestFedTime.cc,v 1.1 2014/03/03 15:18:23 erk Exp $
