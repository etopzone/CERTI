// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003  ONERA
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
// $Id: Objects.cc,v 3.1 2003/08/06 14:37:47 breholee Exp $
// ----------------------------------------------------------------------------

#include "Objects.hh"

#include "PrettyDebug.hh"

#include <cassert>

static pdCDebug D("BILLARD", __FILE__ "> ");

using std::string ;
using std::vector ;

// ----------------------------------------------------------------------------
// Constructor
Objects::Objects(RTIambassador &rti_amb, Fed &fed_amb, int xmax, int ymax)
    : rtiamb(rti_amb), fedamb(fed_amb), local(0), XMAX(xmax), YMAX(ymax)
{
}

// ----------------------------------------------------------------------------
// init
void
Objects::init(int seed)
{
    local.init(seed);
}

// ----------------------------------------------------------------------------
// init
void
Objects::init(int x, int y)
{
    local.init(x, y);
}

// ----------------------------------------------------------------------------
// create
void
Objects::create(string federate)
{
    local.ID = fedamb.registerBallInstance(federate.c_str());
}

// ----------------------------------------------------------------------------
// step
void
Objects::step(RTIfedTime &time)
{
    vector<Ball>::iterator it ;

    for (it = remote.begin(); it != remote.end(); ++it) {
        if (it->ID != 0 && local.collision(it)) {

            Ball &ball = *it ;

            fedamb.sendInteraction(local.dx, local.dy, time, it->ID);

            // On prend la vitesse de l'autre sauf dans le cas ou
            // on avait deja la meme. Dans ce cas, on inverse la notre.
            if ((local.dx == it->dx) && (local.dy == it->dy)) {
                local.dx = -local.dx ;
                local.dy = -local.dy ;
            }
            else
                local.setDirection(it->dx, it->dy);

        }
        D.Out(pdTrace, "no collision.");

    }

    // Teste la collision avec le bord
    local.collision(XMAX, YMAX);
    D.Out(pdTrace, "Border collisions...");

    local.x += local.dx ;
    local.y += local.dy ;
}

// ----------------------------------------------------------------------------
// update
void
Objects::update(RTIfedTime &time)
{
    fedamb.sendUpdate(local.x, local.y, local.color, time, local.ID);
    D.Out(pdTrace, "fin tour de boucle.");
}

// ----------------------------------------------------------------------------
// clear
void
Objects::erase()
{
    local.erase();
}
// ----------------------------------------------------------------------------
// display
void
Objects::display()
{
    local.display();
}

// ----------------------------------------------------------------------------
//! Efface tous les objets locaux de la simulation a la date DeletionTime.
void
Objects::destroy(const FedTime& DeletionTime)
{
    try {
        rtiamb.deleteObjectInstance(local.ID, DeletionTime, "DO");
        D.Out(pdRegister, "Local object deleted from federation.");
    }
    catch (Exception &e) {
        D.Out(pdExcept, "**** Exception delete object : %d", &e);
    }
}

// ----------------------------------------------------------------------------
//! receive
void
Objects::receive(ObjectHandle h, int dx, int dy)
{
    if (h == local.ID) {
        local.dx = dx ;
        local.dy = dy ;
    }
}

// ----------------------------------------------------------------------------
//! reflect
// new values for x and y ; if zero, no change
void
Objects::reflect(ObjectHandle h, int x, int y)
{
    vector<Ball>::iterator it ;

    for (it = remote.begin(); it != remote.end(); ++it) {
        if (it->ID == h)
            break ;
    }

    if (it == remote.end())
        D.Out(pdError, "Fed: error, id not found (%d).", h);
    else {
        it->erase();

        float oldx = it->x ;
        it->x = x ;
        it->dx = it->x - oldx ;

        float oldy = it->y ;
        it->y = y ;
        it->dy = it->y - oldy ;

        it->display();
    }
}

// ----------------------------------------------------------------------------
//! remove
// remove object
void
Objects::remove(ObjectHandle h)
{
    vector<Ball>::iterator it ;

    for (it = remote.begin(); it != remote.end(); ++it) {
        if (it->ID == h) {
            it->erase();
            remote.erase(it);
            return ;
        }
    }
}

// ----------------------------------------------------------------------------
//! discover
//
void
Objects::discover(ObjectHandle h)
{
    remote.push_back(Ball(h));
}

// $Id: Objects.cc,v 3.1 2003/08/06 14:37:47 breholee Exp $
