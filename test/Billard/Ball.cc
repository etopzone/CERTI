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
// $Id: Ball.cc,v 3.1 2003/08/06 14:37:47 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>

#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "Ball.hh"

#ifdef TEST_USES_GRAPHICS
#include "graph_c.hh"
#endif

using namespace std ;

// ----------------------------------------------------------------------------
//! CBille constructor.
Ball::Ball(ObjectHandle h)
{
    x = -1.0 ;
    y = -1.0 ;
    dx = 3.0 ;
    dy = 3.0 ;
    radius = 10.0 ;
    ID = h ;
#ifdef TEST_USES_GRAPHICS
    color = BLACK ;
#endif
}

// ----------------------------------------------------------------------------
//! Displays the 'bille' on the right place in window.
void
Ball::display()
{
#ifdef TEST_USES_GRAPHICS
    cercler disque ;
    point centre ;

    centre.X = (int) x ;
    centre.Y = (int) y ;
    disque = Definecr(centre, (int) radius, COUL_UNIE, (couleur) color);
    Drawcr(disque);
#endif
}

// ----------------------------------------------------------------------------
//! Clear the 'bille' from window.
void
Ball::erase()
{
#ifdef TEST_USES_GRAPHICS
    cercler disque ;
    point centre ;

    centre.X = (int)x ;
    centre.Y = (int)y ;
    disque = Definecr(centre, (int)radius, COUL_UNIE, WHITE);
    Drawcr(disque);
#endif
}

// ----------------------------------------------------------------------------
//! Determine new values for x and y, based on dx and dy.
void
Ball::move()
{
    x += dx ;
    y += dy ;
    // #ifdef ECHO_COORD
    // printf("[%04f ; %04f]\r", x, y);
    // fflush(stdout);
    // #endif
}

// ----------------------------------------------------------------------------
//! Put the 'bille' at a determined location.
void
Ball::setPosition(float xx, float yy)
{
    this->x = xx ;
    this->y = yy ;
}

// ----------------------------------------------------------------------------
//! modify dx and dy directions.
void Ball::setDirection(float dxx, float dyy)
{
    dx = dxx ;
    dy = dyy ;
}

// ----------------------------------------------------------------------------
//! Detects and take into account collisions occured with window borders.
void
Ball::collision(float largeur, float hauteur)
{
    // left/right collision
    if ((x < radius) || (x > largeur - radius)) {
        dx = -dx ;
    }

    // top/bottom collision
    if ((y < radius) || (y > hauteur - radius)) {
        dy = -dy ;
    }
}

// ----------------------------------------------------------------------------
/*! Detects and take into account collisions occured with another 'Ball' from
  window.
*/
int
Ball::collision(Ball *ab)
{
    float distance ;

    distance = sqrt((x+dx-ab->x)*(x+dx-ab->x) +(y+dy-ab->y)*(y+dy-ab->y));

    // detecter collision
    if (distance <= 2 * radius) {
        return 1 ;
    }
    else
        return 0 ;
}

// ----------------------------------------------------------------------------
//! init with one int parameter
void
Ball::init(int seed)
{
    x = radius + (float) seed * 60 + 3 ;
    y = radius + (float) seed * 20 ;

    if ((seed) % 2)
        dx = -dx ;

    display();
}

// ----------------------------------------------------------------------------
//! init with coordinates
void
Ball::init(int x_, int y_)
{
    x = x_ ;
    y = y_ ;

    if ((int) x % 2 == 1)
        dx = -dx ;

    display();
}


// $Id: Ball.cc,v 3.1 2003/08/06 14:37:47 breholee Exp $
