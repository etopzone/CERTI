// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI
//
// CERTI is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// $Id: main.cc,v 3.2 2003/01/14 17:17:33 breholee Exp $
// ---------------------------------------------------------------------------

#include "RTIG.hh"

using namespace certi ;
using namespace rtig ;

static RTIG* rtip ;

// -------------------
// -- SignalHandler --
// -------------------

extern "C" void SignalHandler(int sig)
{ 
  rtip->signalHandler(sig);

  // Catch signal again.
  signal(sig, SignalHandler);
}

// ----------
// -- MAIN --
// ----------

int main(int argc, char *argv[])
{  
    gengetopt_args_info args_info;
    if(cmdline_parser(argc, argv, &args_info) != 0) exit(EXIT_FAILURE) ;

    printf("CERTI " VERSION " - Copyright 2002, 2003  ONERA\n");
    printf("This is free software; see the source for copying conditions. "
           "There is NO\nwarranty; not even for MERCHANTABILITY or FITNESS "
           "FOR A PARTICULAR PURPOSE.\n\n");

    signal(SIGINT,  SignalHandler);
    signal(SIGPIPE, SignalHandler);
    
    rtip = new RTIG();
    rtip->execute();
    delete rtip ;

    cout << "RTIG exiting." << endl ;
    exit(EXIT_SUCCESS);
}

// $Id: main.cc,v 3.2 2003/01/14 17:17:33 breholee Exp $
