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
// $Id: List.hh,v 3.2 2002/12/11 00:47:33 breholee Exp $
// ---------------------------------------------------------------------------

#ifndef _CERTI_LIST_HH
#define _CERTI_LIST_HH

#include <iostream>

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "RTItypes.hh"

namespace certi {

// description d'une cellule de la liste
struct cellule
{
  void *valeur;
  cellule *suivant;
};


// specification de la classe liste generique
class LISTEGEN
{
public:
  LISTEGEN();
  LISTEGEN(LISTEGEN &);
  ~LISTEGEN();

  void *Ieme(unsigned int pos);
  void Inserer(unsigned int pos, void *);
  void Supprimer(unsigned int pos);
  LISTEGEN & operator =(const LISTEGEN &);

protected:
  int       lg;
  cellule * _debut;
  int       _i_courant;
  cellule * _cell_courante;

  void PositionnerCelluleCourante(int i);
};



// specification de la classe liste
template <class T>
class List : public LISTEGEN
{
public:
  inline unsigned int getLength(void) {return lg;}
  T Ieme(int i) {return((T) LISTEGEN::Ieme(i));}
  void Inserer(int i, T adr) {LISTEGEN::Inserer(i,adr);}
  void Rechercher(Boolean &trouve, int &rang, Boolean(*test)(T))
  {
    T tampon;

    trouve = RTI_FALSE;
    for(unsigned int i=1; i<=lg && !trouve; i++)
      {
	tampon = Ieme(i);
	if(test(tampon))
	  {
	    trouve = RTI_TRUE;
	    rang = i;
	  }
      }
  }
};
}

#endif // _CERTI_LIST_HH

// EOF $Id: List.hh,v 3.2 2002/12/11 00:47:33 breholee Exp $
