// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
//
// This file is part of Billard
//
// Billard is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// Billard is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
// ----------------------------------------------------------------------------

#include <config.h>

#include "Ball.hh"
#include "graph_c.hh"

#include <cmath>

using namespace std ;

// ----------------------------------------------------------------------------
/** Ball constructor
    \param h HLA object handle
 */
Ball::Ball(RTI::ObjectHandle h)
    : x(-10.0), y(-10.0), dx(3.0), dy(3.0), radius(10.0), active(false), ID(h)
{
#ifndef X_DISPLAY_MISSING
    color = BLACK ;
#endif
}

// ----------------------------------------------------------------------------
//! Displays the Ball on the right place in window.
void
Ball::display()
{
#ifndef X_DISPLAY_MISSING
    cercler disque ;
    point centre ;

    centre.X = (int) x ;
    centre.Y = (int) y ;
    disque = Definecr(centre, (int) radius, COUL_UNIE,
		      active ? (couleur) color : GRAY);
    Drawcr(disque);
#endif
}

// ----------------------------------------------------------------------------
//! Clear the Ball from window.
void
Ball::erase()
{
#ifndef X_DISPLAY_MISSING
    cercler disque ;
    point centre ;

    centre.X = (int)x ;
    centre.Y = (int)y ;
    disque = Definecr(centre, (int) radius, COUL_UNIE, WHITE);
    Drawcr(disque);
#endif
}

// ----------------------------------------------------------------------------
//! Set new values for x and y, based on dx and dy.
void
Ball::move()
{
    x += dx ;
    y += dy ;
}

// ----------------------------------------------------------------------------
//! Put the Ball at a determined location.
void
Ball::setPosition(float xx, float yy)
{
    x = xx ;
    y = yy ;
}

// ----------------------------------------------------------------------------
//! Modify direction (x/y)
void
Ball::setDirection(float dxx, float dyy)
{
    dx = dxx ;
    dy = dyy ;
}

// ----------------------------------------------------------------------------
//! Detects and take into account collisions occured with window borders.
void
Ball::checkBorderCollision(float largeur, float hauteur)
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
/** Detects and take into account collisions occured with another Ball
    \param ab Pointer to the other ball
 */
bool
Ball::checkBallCollision(Ball *ab)
{
    return std::sqrt((x + dx - ab->x) * (x + dx - ab->x) +
		     (y + dy - ab->y) * (y + dy - ab->y)) <= 2 * radius;
}

// ----------------------------------------------------------------------------
//! Initialize with a seed
void
Ball::init(int seed)
{
    x = radius + seed * 60 + 3 ;
    y = radius + seed * 20 ;

    if (seed % 2) dx = -dx ;

    display();
}

// ----------------------------------------------------------------------------
//! Initialize with coordinates
void
Ball::init(int x_, int y_)
{
    x = x_ ;
    y = y_ ;

    if ((int) x % 2 == 1) dx = -dx ;

    display();
}
