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
// $Id: bille.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include <stdio.h>
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

//
// constructeur de la classe
//
//
CBille::CBille()
{
  x = -1.0;
  y = -1.0;
  dx = 3.0;
  dy = 3.0;
  rayon = 10.0;
#ifdef TEST_USES_GRAPHICS
  color = BLACK;
#endif
}

CBoule::CBoule() : CBille()
{
#ifdef TEST_USES_GRAPHICS
  Color = RED;
#endif
}

//
// afficher la bille a la bonne position dans la fenetre
//
//
void CBille::Afficher()
{
#ifdef TEST_USES_GRAPHICS
  cercler disque;
  point centre;

  centre.X =(int)x;
  centre.Y =(int)y;
  disque = Definecr(centre,(int)rayon, COUL_UNIE,(couleur) color);
  Drawcr(disque);
#endif
}

void CBoule::Afficher()
{
#ifdef TEST_USES_GRAPHICS
  cercler disque;
  point centre;

  centre.X =(int)x;
  centre.Y =(int)y;
  disque = Definecr(centre,(int)rayon, COUL_UNIE,(couleur) Color);
  Drawcr(disque);
#endif
}

//
// effacer la bille de la fenetre
//
//
void CBille::Effacer()
{
#ifdef TEST_USES_GRAPHICS
  cercler disque;
  point centre;

  centre.X =(int)x;
  centre.Y =(int)y;
  disque = Definecr(centre,(int)rayon, COUL_UNIE, WHITE);
  Drawcr(disque);
#endif
}

//
// calculer les nouvelles valeurs de x et y
// en fonction de dx et dy
//
//
void CBille::Deplacer()
{
  x = x + dx;
  y = y + dy;
#ifdef ECHO_COORD
  printf("[%04f;%04f]\r", x, y);
  fflush(stdout);
#endif
}

// 
// positionner la bille a telles coordonnees
//
//
void CBille::Positionner(float xx, float yy)
{
  this->x = xx;
  this->y = yy;
}

//
// modifier les directions dx et dy
//
//
void CBille::Direction(float dxx, float dyy)
{
  dx = dxx;
  dy = dyy;
}

//
// detecter et prendre en compte les collisions avec
// les bords de la fenetre
//
//
void CBille::CollisionBords(float largeur, float hauteur)
{
  // si collision avec le bord de gauche ou avec le bord de droite
  if((x < rayon) ||(x > largeur - rayon))
    { 
      dx = -dx;
    }
  
  // si collision avec le bord du haut ou avec le bord du bas
  if((y < rayon) ||(y > hauteur - rayon))
    { 
      dy = -dy;
    }
}

//
// detecter et prendre en compte une collision avec
// une autre bille de la fenetre
//
// ab : autre bille
//
int CBille::Collision(CBille *ab)
{
  // distance entre les centres des deux billes
  float distance;

  distance = sqrt((x+dx-ab->x)*(x+dx-ab->x) +(y+dy-ab->y)*(y+dy-ab->y) );

  // detecter collision
  if(distance <= 2 * rayon )
  { 
    return 1;
  }
  else
    return 0;
}

// -----------------
// -- Initialiser --
// -----------------

void CBille::Initialiser(int Graine)
{
  x = rayon +(float)Graine*60 + 3;
  y = rayon +(float)Graine*20;

  if((Graine) % 2)
    dx = -dx;
  
  Afficher();
}
