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
// $Id: constants.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef TH_CONSTANTS_HH
#define TH_CONSTANTS_HH

// ------------------------------------
// -- Do your own customization here --
// ------------------------------------

// Undef these constants to make benchmark tests. It will not use the
// graphic part / coord display
#ifdef WITH_X11
#define TEST_USES_GRAPHICS
#endif // WITH_X11
#define ECHO_COORD

// ---------------------------------------
// -- Stop your changes after this line --
// ---------------------------------------

#define MAX_COUNT 5     // Ticking the RTI

// NB: Definitions in this file must be coherent with the .fed file.

// Classes
#define CLA_BILLE "Bille"
#define CLA_BOULE "Boule"

// Attributes
#define ATT_POSITION_X "PositionX"
#define ATT_POSITION_Y "PositionY"
#define ATT_COLOR      "Color"

// Interactions
#define INT_BING       "Bing"

// Parameters
#define PAR_DX "DX"
#define PAR_DY "DY"
#define PAR_BOUL "BoulNum"

// Types
#define TYP_FLOAT "float"
#define TYP_INT   "int"

#endif // TH_CONSTANTS_HH
