// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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
// $Id: bille.cc,v 3.3 2003/02/19 17:20:28 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>

#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "bille.hh"
#include "constants.hh"

#ifdef TEST_USES_GRAPHICS
#include "graph_c.hh"
#endif

using namespace std ;

// ----------------------------------------------------------------------------
//! CBille constructor.
CBille::CBille(void)
{
    x = -1.0 ;
    y = -1.0 ;
    dx = 3.0 ;
    dy = 3.0 ;
    rayon = 10.0 ;
#ifdef TEST_USES_GRAPHICS
    color = BLACK ;
#endif
}

// ----------------------------------------------------------------------------
//! CBoule constructor.
CBoule::CBoule(void) : CBille()
{
#ifdef TEST_USES_GRAPHICS
    Color = RED ;
#endif
}

// ----------------------------------------------------------------------------
//! Displays the 'bille' on the right place in window.
void
CBille::Afficher(void)
{
#ifdef TEST_USES_GRAPHICS
    cercler disque ;
    point centre ;

    centre.X = (int)x ;
    centre.Y = (int)y ;
    disque = Definecr(centre, (int)rayon, COUL_UNIE, (couleur) color);
    Drawcr(disque);
#endif
}

// ----------------------------------------------------------------------------
//! Displays the 'boule' on the right place in window.
void
CBoule::Afficher(void)
{
#ifdef TEST_USES_GRAPHICS
    cercler disque ;
    point centre ;

    centre.X = (int)x ;
    centre.Y = (int)y ;
    disque = Definecr(centre, (int)rayon, COUL_UNIE, (couleur) Color);
    Drawcr(disque);
#endif
}

// ----------------------------------------------------------------------------
//! Clear the 'bille' from window.
void
CBille::Effacer(void)
{
#ifdef TEST_USES_GRAPHICS
    cercler disque ;
    point centre ;

    centre.X = (int)x ;
    centre.Y = (int)y ;
    disque = Definecr(centre, (int)rayon, COUL_UNIE, WHITE);
    Drawcr(disque);
#endif
}

// ----------------------------------------------------------------------------
//! Determine new values for x and y, based on dx and dy.
void
CBille::Deplacer(void)
{
    x += dx ;
    y += dy ;
#ifdef ECHO_COORD
    printf("[%04f ; %04f]\r", x, y);
    fflush(stdout);
#endif
}

// ----------------------------------------------------------------------------
//! Put the 'bille' at a determined location.
void
CBille::Positionner(float xx, float yy)
{
    this->x = xx ;
    this->y = yy ;
}

// ----------------------------------------------------------------------------
//! modify dx and dy directions.
void CBille::Direction(float dxx, float dyy)
{
    dx = dxx ;
    dy = dyy ;
}

// ----------------------------------------------------------------------------
//! Detects and take into account collisions occured with window borders.
void
CBille::CollisionBords(float largeur, float hauteur)
{
    // si collision avec le bord de gauche ou avec le bord de droite
    if ((x < rayon) || (x > largeur - rayon)) {
        dx = -dx ;
    }

    // si collision avec le bord du haut ou avec le bord du bas
    if ((y < rayon) || (y > hauteur - rayon)) {
        dy = -dy ;
    }
}

// ----------------------------------------------------------------------------
/*! Detects and take into account collisions occured with another 'bille' from
  window.

  ab : autre bille.
*/
int
CBille::Collision(CBille *ab)
{
    // distance entre les centres des deux billes
    float distance ;

    distance = sqrt((x+dx-ab->x)*(x+dx-ab->x) +(y+dy-ab->y)*(y+dy-ab->y));

    // detecter collision
    if (distance <= 2 * rayon) {
        return 1 ;
    }
    else
        return 0 ;
}

// ----------------------------------------------------------------------------
//! Initialiser
void
CBille::Initialiser(int Graine)
{
    x = rayon + (float) Graine * 60 + 3 ;
    y = rayon + (float) Graine * 20 ;

    if ((Graine) % 2)
        dx = -dx ;

    Afficher();
}

// $Id: bille.cc,v 3.3 2003/02/19 17:20:28 breholee Exp $
