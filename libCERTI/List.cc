// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- 
// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002  ONERA
//
// This file is part of CERTI-libcerti
//
// CERTI-libcerti is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libcerti is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: List.cc,v 3.2 2002/12/11 00:47:33 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "List.hh"

namespace certi {

//------------------------------------------------------------------
LISTEGEN::LISTEGEN()
{
  _debut = NULL;
  lg = 0;
  _i_courant = 0;
  _cell_courante = NULL;
}


//------------------------------------------------------------------
LISTEGEN::LISTEGEN(LISTEGEN &l)
{
  lg = l.lg;
  _debut = l._debut;
  _i_courant = l._i_courant;
  _cell_courante = l._cell_courante;

  if(l.lg != 0)
  {
    for(unsigned int i=1; i<=l.lg; i++)
      Inserer(i, l.Ieme(i));
  }
}


//------------------------------------------------------------------
LISTEGEN::~LISTEGEN()
{
  struct cellule *ptr_courant, *p;

  ptr_courant = _debut;

  while(ptr_courant != NULL)
  {
    p = ptr_courant;
    ptr_courant = ptr_courant->suivant;
    delete p;
  }
}


//------------------------------------------------------------------
void* LISTEGEN::Ieme(unsigned int i)
{
  assert(i>0 && i<=lg);

  PositionnerCelluleCourante(i);
  return _cell_courante->valeur;
}



//------------------------------------------------------------------
void LISTEGEN::Supprimer(unsigned int i)
{
  struct cellule *cell_a_detruire;

  assert(i>0 && i<=lg);

  lg--;

  if(i == 1)
  {
    cell_a_detruire = _debut;
    _debut = _debut->suivant;
    if(lg == 0)
      _i_courant = 0;
    else
      _i_courant = 1;
    _cell_courante = _debut;
  }
  else  // i>1
  {
    PositionnerCelluleCourante(i-1);
    cell_a_detruire = _cell_courante->suivant;
    _cell_courante->suivant = cell_a_detruire->suivant;
  }

  delete cell_a_detruire;
}



//------------------------------------------------------------------
void LISTEGEN::Inserer(unsigned int i, void *e)
{
  struct cellule *p;

  assert(i>0 && i<=lg+1);  // lg+1 pour pouvoir inserer en fin de liste

  p = new cellule;
  p->valeur = e;
  lg++;

  if(i==1)
  {
    p->suivant = _debut;
    _debut = p;
    _i_courant = 1;
    _cell_courante = _debut;
  }
  else  // i>1
  {
    PositionnerCelluleCourante(i-1);
    p->suivant = _cell_courante->suivant;
    _cell_courante->suivant = p;
    _i_courant = i;
    _cell_courante = p;
  }
}



//------------------------------------------------------------------
LISTEGEN & LISTEGEN::operator =(const LISTEGEN &l)
{
  void *tampon;

  if(this != &l)
  {
    lg = l.lg;
    _debut = l._debut;
    _i_courant = l._i_courant;
    _cell_courante = l._cell_courante;

    if(l.lg != 0)
    {
      for(int i=1; i<=l.lg; i++)
      {
        tampon = Ieme(i);
        Inserer(i, tampon);
      }
    }
  }
  return *this;
}



//------------------------------------------------------------------
void LISTEGEN::PositionnerCelluleCourante(int i)
{
  if(i < _i_courant)
  {
    _i_courant = 1;
    _cell_courante = _debut;
  }

  while(_i_courant != i)
  {
    _i_courant++;
    _cell_courante = _cell_courante->suivant;
  }
}

}

// $Id: List.cc,v 3.2 2002/12/11 00:47:33 breholee Exp $
