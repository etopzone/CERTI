// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002  ONERA
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
// $Id: main.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

#include "RTIG.hh"

using namespace certi ;
using namespace rtig ;

static RTIG rtip;

// -------------------
// -- SignalHandler --
// -------------------

extern "C" void SignalHandler(int sig)
{ 
  rtip.signalHandler(sig);

  // Catch signal again.
  signal(sig, SignalHandler);
}

// ----------
// -- MAIN --
// ----------

int main()
{  
  printf("CERTI " VERSION " - Copyright 2002 ONERA\n");
  printf("This is free software; see the source for copying conditions.  "
	 "There is NO\n");
  printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A "
	 "PARTICULAR PURPOSE.\n\n");

  signal(SIGINT,  SignalHandler);
  signal(SIGPIPE, SignalHandler);

  rtip.execute();

  printf("\nRTIG exiting.\n");
  exit(EXIT_SUCCESS);
}

// $Id: main.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $
