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
// $Id: bille.hh,v 3.1 2002/12/11 00:47:34 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef BILLE_HH
#define BILLE_HH

#include <config.h>

class CBille
{
public:
  CBille();
  
  float rayon;
  float dx, dy;
  float olddx, olddy;
  float x, y;
  float color;

        
  unsigned long ID; // id de l'objet HLA
  
  void Afficher();
  void Effacer();
  void Deplacer();
  void Positionner(float, float);
  void Direction(float, float);
  void CollisionBords(float, float);
  int Collision(CBille *);

  void Initialiser(int Graine);
  // Positionne la bille en fonction de Graine puis l'affiche.

};

class CBoule : public CBille
{
public:
  int Color;

  CBoule();

  // Same as CBille::Afficher but uses Color.
  void Afficher();
};
#endif
