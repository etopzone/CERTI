// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
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
// $Id: main.cc,v 3.2 2002/12/11 00:47:33 breholee Exp $
// ---------------------------------------------------------------------------

#include "RTIA.hh"

using namespace certi ;
using namespace rtia ;

// We'd better use a pointer than a static object, but static objects
// are freed automatically when a Kill signal is received !
RTIA rtia; 

extern "C" void SignalHandler(int Signal);

// ----------
// -- Main --
// ----------

int main(void)
{  
  signal(SIGINT,  SignalHandler);
  signal(SIGPIPE, SignalHandler);

  try {
    ::rtia.execute();
  }
  catch(Exception &e) {
    printf("\nRTIA has thrown %s exception.\n", e._name);
    if(e._reason != NULL)
      printf("Reason: %s\n", e._reason);
  }
}

// -------------------
// -- SignalHandler --
// -------------------

void SignalHandler(int Signal)
{ 
  int pid = getpid();

  printf("\nRTIA: Recu signal %d. Killing myself softly.\n", Signal);
  kill(SIGKILL,pid);

}

// EOF $Id: main.cc,v 3.2 2002/12/11 00:47:33 breholee Exp $
