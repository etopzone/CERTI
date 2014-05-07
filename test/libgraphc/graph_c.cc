// ---------------------------------------------------------------------------
// libgraphc - X11 drawing library
// Copyright (C) 2002, 2003  ONERA
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// ---------------------------------------------------------------------------



#include <config.h>

#ifndef X_DISPLAY_MISSING

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

#include "graph_c.hh"

#include <X11/bitmaps/cntr_ptr>
#include <X11/bitmaps/dot>
#include <X11/bitmaps/dropbar8>
#include <X11/bitmaps/left_ptr>
#include <X11/bitmaps/right_ptr>
#include <X11/bitmaps/menu16>
#include <X11/bitmaps/opendotMask>
#include <X11/bitmaps/star>
#include <X11/bitmaps/xlogo16>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*---------------------------------------------------------------------------*/
/*    definition des variables globales                                      */
/*---------------------------------------------------------------------------*/

   static Display       *canal_aff;
   static GC            contexte_graph;
   static Window        nouv_fen;  
   static Pixmap        damier;
   static XEvent        evenem; 
   static XGCValues     val_gc;
   static unsigned long masq_bin; 
   static unsigned long masq_attrib;
   static XSetWindowAttributes xswa;
   static XColor        couleu;   /* composante RVB ,donne la valeur de pixel
				     en retour */
   static XColor        exact;    /* definition RVB exacte souhaitee */
   static Status        code_ret;             
   static Colormap      palette;  /* id de la palette de couleurs par defaut
				     de l ecran */ 
   static unsigned long carrplan;
   static unsigned long cavplan; 
   static const char         * nom_coul;
   static mode_remp     b; 
   static style         c;
//   static policecar     d;   
//   static taillecar     t;
   static KeySym        touche;  
   static Font          fonte;
   static Cursor        curseur;
  
  
  /*---definition des styles de pointilles-----*/
  /*===========================================*/

   static char dots1 [] = {5,5};    /* 5 pixels lumineux puis 5 eteints */
   static char dots2 [] = {20,10};
   static char dots3 [] = {20,2};

/*---------------------------------------------------------------------------*/
/*    definition des procedures et des fonctions globales                    */
/*---------------------------------------------------------------------------*/

   static const char * rech(couleur a);
   static void rempli(mode_remp b);  
   static void lignestyle(style c); 
   static void choixfonte(policecar d,taillecar t);
 
/*---------------------------------------------------------------------------*/
/*    body des procedures et des fonctions globales                          */
/*---------------------------------------------------------------------------*/
               
 /*----recherche chaine de caracteres identifiant la couleur-----*/
 /*==============================================================*/

static const char * rech(couleur a)   {

  const char * nom_coub;
  
  switch(a) {
  case BLACK:   nom_coub = "Black";   break;
  case RED:     nom_coub = "Red";     break;
  case GREEN:   nom_coub = "Green";   break;
  case BLUE:    nom_coub = "Blue";    break;
  case GRAY:    nom_coub = "Gray";    break;
  case CYAN:    nom_coub = "Cyan";    break;
  case YELLOW:  nom_coub = "Yellow";  break;
  case MAGENTA: nom_coub = "Magenta"; break;
  case BROWN:   nom_coub = "Brown";   break;
  case ORANGE:  nom_coub = "Orange";  break;
  case PINK:    nom_coub = "Pink";    break;
  case VIOLET:  nom_coub = "Violet";  break;
  case WHITE:   nom_coub = "White";   break;
  default:      nom_coub = "Black"; break;
  }
  
  return nom_coub;
}        
/*---------------------------------------------------------------------------*/

 /*-----affectation du mode de remplissage au contexte graphique---*/
 /*================================================================*/
                                   
 static void rempli(mode_remp b)  {    
              
     int i;
     i = 0;

     if(b == COUL_UNIE) 
     XSetFillStyle(canal_aff,
                    contexte_graph,
                    FillSolid) ; 
     
 
     if(b == MOTIF1)  {
     damier = XCreateBitmapFromData(canal_aff,
                                     nouv_fen,
(const char *)cntr_ptr_bits,
                                     cntr_ptr_width,
                                     cntr_ptr_height);  
     i = 1; }
   
     if(b == MOTIF2)  {
     damier = XCreateBitmapFromData(canal_aff,
                                     nouv_fen,
(const char *)dot_bits,
                                     dot_width,
                                     dot_height); 
     i = 1; } 

     if(b == MOTIF3)  {
     damier = XCreateBitmapFromData(canal_aff,
                                     nouv_fen,
(const char *)dropbar8_bits,
                                     dropbar8_width,
                                     dropbar8_height);  
     i = 1; }

     if(b == MOTIF4)   {
     damier = XCreateBitmapFromData(canal_aff,
                                     nouv_fen,
(const char *)left_ptr_bits,
                                     left_ptr_width,
                                     left_ptr_height); 
     i = 1; }

     if(b == MOTIF5)   {
     damier = XCreateBitmapFromData(canal_aff,
                                     nouv_fen,
(const char *)right_ptr_bits,
                                     right_ptr_width,
                                     right_ptr_height);    
     i = 1; }
   
     if(b == MOTIF6)   {
     damier = XCreateBitmapFromData(canal_aff,
                                     nouv_fen,
(const char *)menu16_bits,
                                     menu16_width,
                                     menu16_height);   
     i = 1; }

     if(b == MOTIF7)   {
     damier = XCreateBitmapFromData(canal_aff,
                                     nouv_fen,
(const char *)opendotMask_bits,
                                     opendotMask_width,
                                     opendotMask_height);  
     i = 1; }

     if(b == MOTIF8)   {
     damier = XCreateBitmapFromData(canal_aff,
                                     nouv_fen,
(const char *)star_bits,
                                     star_width,
                                     star_height);  
     i = 1; } 

     if(b == MOTIF9)   {
     damier = XCreateBitmapFromData(canal_aff,
                                     nouv_fen,
(const char *)xlogo16_bits,
                                     xlogo16_width,
                                     xlogo16_height);   
     i = 1; }
              
                     
     if(i == 1) {
     XSetStipple(canal_aff,
                  contexte_graph,
                  damier);
     XSetFillStyle(canal_aff,
                    contexte_graph,
                    FillOpaqueStippled); }
     

} 
/*---------------------------------------------------------------------------*/

 /*---affectation de la police de caractere au contexte graphique--*/
 /*================================================================*/
                                   
 static void choixfonte(policecar d, taillecar t)  {      
     
   if(t == TAILLE8) {
     if(d == COURIER) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-courier-medium-r-normal--8-80-75-75-m-50-iso8859-1");
              
     if(d == COURIER_GRAS) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-courier-bold-r-normal--8-80-75-75-m-50-iso8859-1");

     if(d == COURIER_OBLIQUE)
       fonte = XLoadFont(canal_aff,
                       "-adobe-courier-medium-o-normal--8-80-75-75-m-50-iso8859-1");

     if(d == COURIER_GRAS_OBLIQUE) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-courier-bold-o-normal--8-80-75-75-m-50-iso8859-1");

     if(d == HELVETICA) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-helvetica-bold-o-normal--8-80-75-75-p-50-iso8859-1"); 

     if(d == TIMES) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-times-bold-i-normal--8-80-75-75-p-47-iso8859-1"); 

     if(d == SYMBOL) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-symbol-medium-r-normal--8-80-75-75-p-51-adobe-fontspecific"); 
   }   

   if(t == TAILLE10) {
     if(d == COURIER) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-courier-medium-r-normal--10-100-75-75-m-60-iso8859-1");
              
     if(d == COURIER_GRAS) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-courier-bold-r-normal--10-100-75-75-m-60-iso8859-1");
     
     if(d == COURIER_OBLIQUE)
       fonte = XLoadFont(canal_aff,
			 "-adobe-courier-medium-o-normal--10-100-75-75-m-60-iso8859-1");
     
     if(d == COURIER_GRAS_OBLIQUE) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-courier-bold-o-normal--10-100-75-75-m-60-iso8859-1");
     
     if(d == HELVETICA) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-helvetica-bold-o-normal--10-100-75-75-p-60-iso8859-1"); 
     
     if(d == TIMES) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-times-bold-i-normal--10-100-75-75-p-61-iso8859-1"); 
     
     if(d == SYMBOL) 
     fonte = XLoadFont(canal_aff,
                       "-adobe-symbol-medium-r-normal--10-100-75-75-p-61-adobe-fontspecific"); 
   }
      
   if(t == TAILLE12) {
     if(d == COURIER) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-courier-medium-r-normal--12-120-75-75-m-70-iso8859-1");
     
     if(d == COURIER_GRAS) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-courier-bold-r-normal--12-120-75-75-m-70-iso8859-1");
     
     if(d == COURIER_OBLIQUE)        
       fonte = XLoadFont(canal_aff,
			 "-adobe-courier-medium-o-normal--12-120-75-75-m-70-iso8859-1");
     
     if(d == COURIER_GRAS_OBLIQUE) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-courier-bold-o-normal--12-120-75-75-m-70-iso8859-1");
     
     if(d == HELVETICA) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-helvetica-bold-o-normal--12-120-75-75-p-69-iso8859-1"); 
     
     if(d == TIMES) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-times-bold-i-normal--12-120-75-75-p-68-iso8859-1"); 
     
     if(d == SYMBOL) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-symbol-medium-r-normal--12-120-75-75-p-74-adobe-fontspecific"); 
   }
   
   if(t == TAILLE14) {
     if(d == COURIER) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-courier-medium-r-normal--14-140-75-75-m-90-iso8859-1");
     
     if(d == COURIER_GRAS) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-courier-bold-r-normal--14-140-75-75-m-90-iso8859-1");
     
     if(d == COURIER_OBLIQUE)
       fonte = XLoadFont(canal_aff,
			 "-adobe-courier-medium-o-normal--14-140-75-75-m-90-iso8859-1");
     
     if(d == COURIER_GRAS_OBLIQUE) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-courier-bold-o-normal--14-140-75-75-m-90-iso8859-1");
     
     if(d == HELVETICA) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-helvetica-bold-o-normal--14-140-75-75-p-82-iso8859-1"); 
     
     if(d == TIMES) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-times-bold-i-normal--14-140-75-75-p-77-iso8859-1"); 
     
     if(d == SYMBOL) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-symbol-medium-r-normal--14-140-75-75-p-85-adobe-fontspecific"); 
   }
   
   if(t == TAILLE18) {
     if(d == COURIER) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-courier-medium-r-normal--18-180-75-75-m-110-iso8859-1");
     
     if(d == COURIER_GRAS) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-courier-bold-r-normal--18-180-75-75-m-110-iso8859-1");

     if(d == COURIER_OBLIQUE)
       fonte = XLoadFont(canal_aff,
			 "-adobe-courier-medium-o-normal--18-180-75-75-m-110-iso8859-1");

     if(d == COURIER_GRAS_OBLIQUE) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-courier-bold-o-normal--18-180-75-75-m-110-iso8859-1");
     
     if(d == HELVETICA) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-helvetica-bold-o-normal--18-180-75-75-p-104-iso8859-1"); 
     
     if(d == TIMES) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-times-bold-i-normal--18-180-75-75-p-98-iso8859-1"); 
     
     if(d == SYMBOL) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-symbol-medium-r-normal--18-180-75-75-p-107-adobe-fontspecific"); 
   }
   
   if(t == TAILLE24) {
     if(d == COURIER) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-courier-medium-r-normal--24-240-75-75-m-150-iso8859-1");
     
     if(d == COURIER_GRAS) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-courier-bold-r-normal--24-240-75-75-m-150-iso8859-1");
     
     if(d == COURIER_OBLIQUE)
       fonte = XLoadFont(canal_aff,
			 "-adobe-courier-medium-o-normal--24-240-75-75-m-150-iso8859-1");
     
     if(d == COURIER_GRAS_OBLIQUE) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-courier-bold-o-normal--24-240-75-75-m-150-iso8859-1");
     
     if(d == HELVETICA) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-helvetica-bold-o-normal--24-240-75-75-p-138-iso8859-1"); 
     
     if(d == TIMES) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-times-bold-i-normal--24-240-75-75-p-128-iso8859-1"); 
     
     if(d == SYMBOL) 
       fonte = XLoadFont(canal_aff,
			 "-adobe-symbol-medium-r-normal--24-240-75-75-p-142-adobe-fontspecific"); 
   }
   
   
   XSetFont(canal_aff,
	     contexte_graph,
	     fonte);
}

/*---------------------------------------------------------------------------*/
   
 /*-----affectation du style de ligne au contexte graphique---*/
 /*===========================================================*/
                                   
 static void lignestyle(style c)  {      

     if(c == PLEIN)
     XSetLineAttributes(canal_aff,
                         contexte_graph,
                         5,
                         LineSolid,
                         CapButt,
                         JoinMiter);

     if(c == POINTILLE1)  
     { XSetDashes(canal_aff,
                   contexte_graph,
                   0,
                   dots1,
                   2);
     XSetLineAttributes(canal_aff,
                         contexte_graph,
                         5,
                         LineOnOffDash,
                         CapButt,
                         JoinMiter); };
     
     if(c == POINTILLE2)
     { XSetDashes(canal_aff,
                   contexte_graph,
                   0,
                   dots2,
                   2);
     XSetLineAttributes(canal_aff,
                         contexte_graph,
                         5,
                         LineOnOffDash,
                         CapButt,
                         JoinMiter); };

     if(c == POINTILLE3)
     { XSetDashes(canal_aff,
                   contexte_graph,
                   0,
                   dots3,
                   2);
     XSetLineAttributes(canal_aff,
                         contexte_graph,
                         5,
                         LineOnOffDash,
                         CapButt,
                         JoinMiter); };

}  

/*---------------------------------------------------------------------------*/

/*==================================================*/
/*      corps des fonctions et des procedures       */
/*               figures simples                    */
/*==================================================*/  


              
/*---module POINT-----*/ 
/*====================*/

pointt Definex(point    COORD,
                couleur  WCOULEUR) {

       pointt Pointloc;
       Pointloc.POI_COORD   = COORD;
       Pointloc.POI_COULEUR = WCOULEUR;
       return  Pointloc;
  } 
     
void Drawx(pointt P)  {
      
     nom_coul =  rech(P.POI_COULEUR);
     code_ret = XAllocNamedColor(canal_aff,
                                  palette,
                                  nom_coul,
                                  &couleu,
                                  &exact); 

     masq_bin = GCForeground | GCFillStyle; 
     val_gc.foreground = couleu.pixel; 
     val_gc.fill_style = FillSolid;   
     XChangeGC(canal_aff,
                contexte_graph,
                masq_bin,
                &val_gc);
  
     XDrawPoint(canal_aff,
                nouv_fen,
                contexte_graph,
                P.POI_COORD.X,
                P.POI_COORD.Y);
     XFlush(canal_aff);
  }  
  
void Undrawx(pointt P)  { 
                            
     masq_bin = GCForeground | GCFillStyle; 
     val_gc.foreground = carrplan;
     val_gc.fill_style = FillSolid;    
     XChangeGC(canal_aff,
                contexte_graph,
                masq_bin,
                &val_gc);
  
     XDrawPoint(canal_aff,
                nouv_fen,
                contexte_graph,
                P.POI_COORD.X,
                P.POI_COORD.Y);
     XFlush(canal_aff);        
          
  }  

void Change_attribx(pointt   *P,
                     couleur  WCOULEUR)  { 
                       
      Undrawx(*P);
      P->POI_COULEUR   = WCOULEUR; 
      Drawx(*P);
  }

void Movex(pointt *P, 
            point  COORD) {

      Undrawx(*P); 
      P->POI_COORD = COORD;
      Drawx(*P);
}
       
void Duplicx(pointt P,
              pointt *PD, 
              point  COORD) {
      
      PD->POI_COORD  = COORD;   
      PD->POI_COULEUR  = P.POI_COULEUR;  
      Definex(PD->POI_COORD,
               PD->POI_COULEUR);
      Drawx(*PD);

}

point Coordx(pointt P)  {
    
       return P.POI_COORD;
  } 
                     
couleur Coulx(pointt P)  {
     
       return P.POI_COULEUR;
  }                                            

/*---------------------------------------------------------------------------*/

/*---module LIGNE-----*/ 
/*====================*/

ligne Definel(point    DEBUT,
               point    FIN,
               style    WSTYLE,
               couleur  WCOULEUR) {

       ligne Ligloc;
       Ligloc.LIG_DEBUT     = DEBUT;
       Ligloc.LIG_FIN       = FIN;
       Ligloc.LIG_STYLE     = WSTYLE; 
       Ligloc.LIG_COULEUR   = WCOULEUR; 
       return  Ligloc;
  } 
   
void Drawl(ligne L)  { 
                        
     nom_coul = rech(L.LIG_COULEUR) ;       
     code_ret = XAllocNamedColor(canal_aff,
                                  palette,
                                  nom_coul,
                                  &couleu,
                                  &exact); 

     masq_bin = GCLineStyle | GCForeground | GCFillStyle; 
     val_gc.line_style = LineSolid; 
     val_gc.foreground = couleu.pixel;
     val_gc.fill_style = FillSolid; 
     XChangeGC(canal_aff,
                contexte_graph,
                masq_bin,
                &val_gc); 

     c = L.LIG_STYLE;
     lignestyle(c);

     XDrawLine(canal_aff,
               nouv_fen,
               contexte_graph,
               L.LIG_DEBUT.X,
               L.LIG_DEBUT.Y,
               L.LIG_FIN.X,
               L.LIG_FIN.Y);
     XFlush(canal_aff);
  }  
   
void Undrawl(ligne L)  { 
                            
     masq_bin = GCForeground | GCLineStyle | GCFillStyle; 
     val_gc.foreground = carrplan; 
     val_gc.line_style = LineSolid;
     val_gc.fill_style = FillSolid;  
     XChangeGC(canal_aff,
                contexte_graph,
                masq_bin,
                &val_gc);
  
     XDrawLine(canal_aff,
               nouv_fen,
               contexte_graph,
               L.LIG_DEBUT.X,
               L.LIG_DEBUT.Y,
               L.LIG_FIN.X,
               L.LIG_FIN.Y);
     XFlush(canal_aff);
  }  

void Change_attribl(ligne   *L,
                     style   WSTYLE,
                     couleur WCOULEUR)  { 
      Undrawl(*L);     
      L->LIG_STYLE     = WSTYLE;        
      L->LIG_COULEUR   = WCOULEUR;  
      Drawl(*L);
  }                       

void Movel(ligne *L, 
            short TRANSX,
            short TRANSY) {

      Undrawl(*L); 
      L->LIG_DEBUT.X  =(L->LIG_DEBUT).X + TRANSX;
      L->LIG_DEBUT.Y  =(L->LIG_DEBUT).Y + TRANSY;
      L->LIG_FIN.X    =(L->LIG_FIN).X   + TRANSX;
      L->LIG_FIN.Y    =(L->LIG_FIN).Y   + TRANSY;
      Drawl(*L);
}
 
void Duplicl(ligne         L,
              ligne         *LD,
              short         TRANSX,
              short         TRANSY) {
       
      LD->LIG_DEBUT.X  =(L.LIG_DEBUT).X + TRANSX;
      LD->LIG_DEBUT.Y  =(L.LIG_DEBUT).Y + TRANSY;
      LD->LIG_FIN.X    =(L.LIG_FIN).X   + TRANSX;
      LD->LIG_FIN.Y    =(L.LIG_FIN).Y   + TRANSY;
      LD->LIG_STYLE    = L.LIG_STYLE;
      LD->LIG_COULEUR  = L.LIG_COULEUR;  
      Definel(LD->LIG_DEBUT,
               LD->LIG_FIN,
               LD->LIG_STYLE,
               LD->LIG_COULEUR);
      Drawl(*LD);

}

point Debutl(ligne L)  {
    
       return L.LIG_DEBUT;
  } 
                     
point Finl(ligne L)  {
   
       return L.LIG_FIN;
  }

style Stylel(ligne L)  {
   
       return L.LIG_STYLE;
  }

couleur Coull(ligne L)  {
     
       return L.LIG_COULEUR;
  }  
                                 
/*---------------------------------------------------------------------------*/

/*---module RECTANGLE-----*/ 
/*=======================*/

rectangle  Definer(point         DEBUT,
                    unsigned long LARGEUR,
                    unsigned long HAUTEUR,
                    style         WSTYLE,
                    couleur       WCOULEUR) {

       rectangle Recloc;
       Recloc.REC_DEBUT     = DEBUT;
       Recloc.REC_LARGEUR   = LARGEUR;
       Recloc.REC_HAUTEUR   = HAUTEUR;
       Recloc.REC_STYLE     = WSTYLE; 
       Recloc.REC_COULEUR   = WCOULEUR; 
       return  Recloc;
  } 

void Drawr(rectangle R)  { 
     
     nom_coul = rech(R.REC_COULEUR);
     code_ret = XAllocNamedColor(canal_aff,
                                  palette,
                                  nom_coul,
                                  &couleu,
                                  &exact); 
           
     masq_bin = GCLineStyle | GCForeground | GCFillStyle; 
     val_gc.line_style = LineSolid; 
     val_gc.foreground = couleu.pixel; 
     val_gc.fill_style = FillSolid;   
     XChangeGC(canal_aff,
                contexte_graph,
                masq_bin,
                &val_gc);  
 
     c = R.REC_STYLE;
     lignestyle(c);

     XDrawRectangle(canal_aff,
                    nouv_fen,
                    contexte_graph,
                    R.REC_DEBUT.X,
                    R.REC_DEBUT.Y,
                    R.REC_LARGEUR,
                    R.REC_HAUTEUR);
     XFlush(canal_aff);
  }  
   
void Undrawr(rectangle R)  { 
                            
     masq_bin = GCForeground | GCLineStyle | GCFillStyle; 
     val_gc.line_style = LineSolid;
     val_gc.foreground = carrplan; 
     val_gc.fill_style = FillSolid;   
     XChangeGC(canal_aff,
                contexte_graph,
                masq_bin,
                &val_gc);
  
     XDrawRectangle(canal_aff,
                    nouv_fen,
                    contexte_graph,
                    R.REC_DEBUT.X,
                    R.REC_DEBUT.Y,
                    R.REC_LARGEUR,
                    R.REC_HAUTEUR);
     XFlush(canal_aff);
  }  


void Change_attribr(rectangle  *R,
                     style      WSTYLE,
                     couleur    WCOULEUR)  { 
      Undrawr(*R);
      R->REC_STYLE    = WSTYLE;        
      R->REC_COULEUR  = WCOULEUR; 
      Drawr(*R);
  }

void Mover(rectangle *R, 
            point     DEBUT) {

      Undrawr(*R);  
      R->REC_DEBUT = DEBUT;
      Drawr(*R);
}
 
void Duplicr(rectangle     R,
              rectangle     *RD,
              point         DEBUT) {
                          
        
      RD->REC_DEBUT    = DEBUT;
      RD->REC_LARGEUR  = R.REC_LARGEUR; 
      RD->REC_HAUTEUR  = R.REC_HAUTEUR;
      RD->REC_STYLE    = R.REC_STYLE;
      RD->REC_COULEUR  = R.REC_COULEUR;  
      Definer(RD->REC_DEBUT,
               RD->REC_LARGEUR,
               RD->REC_HAUTEUR,
               RD->REC_STYLE,
               RD->REC_COULEUR);
      Drawr(*RD);

}

point Debutr(rectangle R)  {
    
       return R.REC_DEBUT;
  } 
                     
unsigned long Largr(rectangle R)  {
   
       return R.REC_LARGEUR;
  }
   
unsigned long Hautr(rectangle R)  {

       return R.REC_HAUTEUR;
  }

style Styler(rectangle R)  {
   
       return R.REC_STYLE;
  }

couleur Coulr(rectangle R)  {
     
       return R.REC_COULEUR;
  }  

/*---------------------------------------------------------------------------*/

/*---module CERCLE-----*/ 
/*=====================*/

cercle Definec(point          CENTRE,
                unsigned long  RAYON,
                style          WSTYLE,
                couleur        WCOULEUR)   {

      cercle  Cerloc;
      Cerloc.CER_CENTRE  = CENTRE;
      Cerloc.CER_RAYON   = RAYON;
      Cerloc.CER_STYLE   = WSTYLE;
      Cerloc.CER_COULEUR = WCOULEUR; 
      return   Cerloc;
  } 

void Drawc(cercle C)  {  
      
      nom_coul = rech(C.CER_COULEUR);
      code_ret = XAllocNamedColor(canal_aff,
                                   palette,
                                   nom_coul,
                                   &couleu,
                                   &exact); 
                                            
      masq_bin = GCLineStyle | GCForeground | GCFillStyle; 
      val_gc.line_style = LineSolid; 
      val_gc.foreground = couleu.pixel;
      val_gc.fill_style = FillSolid;   
      XChangeGC(canal_aff,
                 contexte_graph,
                 masq_bin,
                 &val_gc);
                                                             
      c = C.CER_STYLE;
      lignestyle(c);

      XDrawArc(canal_aff,
               nouv_fen,
               contexte_graph,
               C.CER_CENTRE.X-C.CER_RAYON,
               C.CER_CENTRE.Y-C.CER_RAYON,
               C.CER_RAYON*2,
               C.CER_RAYON*2,
               0,
               360*64);
      XFlush(canal_aff); 
  }
     
void Undrawc(cercle C)  { 
                            
     masq_bin = GCForeground | GCLineStyle | GCFillStyle; 
     val_gc.foreground = carrplan; 
     val_gc.line_style = LineSolid;
     val_gc.fill_style = FillSolid;   
     XChangeGC(canal_aff,
                contexte_graph,
                masq_bin,
                &val_gc);
       
     XDrawArc(canal_aff,
              nouv_fen,
              contexte_graph,
              C.CER_CENTRE.X-C.CER_RAYON,
              C.CER_CENTRE.Y-C.CER_RAYON,
              C.CER_RAYON*2,
              C.CER_RAYON*2,
              0,
              360*64);
     XFlush(canal_aff);
  }  
  

void Change_attribc(cercle  *C,
                     style   WSTYLE,
                     couleur WCOULEUR)  {
      
       Undrawc(*C);             
       C->CER_STYLE    = WSTYLE;
       C->CER_COULEUR  = WCOULEUR; 
       Drawc(*C);
  }
 
void Movec(cercle *C, 
            point  CENTRE) {

      Undrawc(*C); 
      C->CER_CENTRE = CENTRE;
      Drawc(*C);
}
 

void Duplicc(cercle     C,
              cercle     *CD,
              point      CENTRE) {

      CD->CER_CENTRE   = CENTRE; 
      CD->CER_RAYON    = C.CER_RAYON;
      CD->CER_STYLE    = C.CER_STYLE;
      CD->CER_COULEUR  = C.CER_COULEUR;  
      Definec(CD->CER_CENTRE,
               CD->CER_RAYON,
               CD->CER_STYLE,
               CD->CER_COULEUR);
      Drawc(*CD);

}

point Centrec(cercle C)  {
    
       return C.CER_CENTRE;
  } 
                     
unsigned long Rayonc(cercle C)  {
    
       return C.CER_RAYON;
  }

style Stylec(cercle C)  {
    
       return C.CER_STYLE;
  }

couleur Coulc(cercle C)  {
    
       return C.CER_COULEUR;
  }  
           
/*---------------------------------------------------------------------------*/

/*-----module POLYGONE------*/
/*==========================*/

                                         
polygone Definep(int            NSOMMETS,
                  style          WSTYLE,
                  couleur        WCOULEUR, 
                  point          TABPOINTS[100]) {      
                                    

      polygone polloc; 
      int i;

      for(i=0; i<NSOMMETS; i++)                   
      polloc.POL_TABPOINTS[i] = TABPOINTS[i];      
      polloc.POL_NSOMMETS  = NSOMMETS;
      polloc.POL_STYLE     = WSTYLE;
      polloc.POL_COULEUR   = WCOULEUR; 
      return polloc;
} 

void Drawp(polygone P)  { 
      
      nom_coul = rech(P.POL_COULEUR);
      code_ret = XAllocNamedColor(canal_aff,
                                   palette,
                                   nom_coul,
                                   &couleu,
                                   &exact); 

      masq_bin = GCLineStyle | GCForeground | GCFillStyle; 
      val_gc.line_style = LineSolid; 
      val_gc.foreground = couleu.pixel;
      val_gc.fill_style = FillSolid;   
      XChangeGC(canal_aff,
                 contexte_graph,
                 masq_bin,
                 &val_gc);   
      
      c = P.POL_STYLE;
      lignestyle(c);
  
      XDrawLines(canal_aff,
                 nouv_fen,
                 contexte_graph, 
(XPoint *)P.POL_TABPOINTS,
                 P.POL_NSOMMETS,       
                 CoordModeOrigin); 
      XFlush(canal_aff);                                    
  }

  
void Undrawp(polygone P)  { 
       
      masq_bin = GCForeground | GCLineStyle | GCFillStyle; 
      val_gc.foreground = carrplan;
      val_gc.line_style = LineSolid;
      val_gc.fill_style = FillSolid;   
      XChangeGC(canal_aff,
                 contexte_graph,
                 masq_bin,
                 &val_gc);
  
      XDrawLines(canal_aff,
                 nouv_fen,
                 contexte_graph, 
(XPoint *)P.POL_TABPOINTS,
                 P.POL_NSOMMETS,       
                 CoordModeOrigin); 
      XFlush(canal_aff);                                    
  }


void Change_attribp(polygone *P,
                     style    WSTYLE,
                     couleur  WCOULEUR)  {
      
       Undrawp(*P);
       P->POL_STYLE    = WSTYLE;
       P->POL_COULEUR  = WCOULEUR;   
       Drawp(*P);
  }            


void Movep(polygone *P, 
            short    TRANSX,
            short    TRANSY) { 

      int i;
      Undrawp(*P); 

      for(i=0; i<P->POL_NSOMMETS; i++)   {                
      P->POL_TABPOINTS[i].X =(P->POL_TABPOINTS[i]).X + TRANSX; 
      P->POL_TABPOINTS[i].Y =(P->POL_TABPOINTS[i]).Y + TRANSY; }
      Drawp(*P);
}


void Duplicp(polygone         P,
              polygone         *PD,
              short            TRANSX,
              short            TRANSY) {
                                            
      int i;

      for(i=0; i<P.POL_NSOMMETS; i++)   {                
      PD->POL_TABPOINTS[i].X =(P.POL_TABPOINTS[i]).X + TRANSX; 
      PD->POL_TABPOINTS[i].Y =(P.POL_TABPOINTS[i]).Y + TRANSY; }
      PD->POL_STYLE    = P.POL_STYLE;
      PD->POL_COULEUR  = P.POL_COULEUR;  
      PD->POL_NSOMMETS = P.POL_NSOMMETS;
      Definep(PD->POL_NSOMMETS,
               PD->POL_STYLE,
               PD->POL_COULEUR,
               PD->POL_TABPOINTS);
      Drawp(*PD);

}

int   Nsommetsp(polygone P)  {

       return P.POL_NSOMMETS;
  }

    
style Stylep(polygone P)   {
    
       return P.POL_STYLE;
  }

couleur Coulp(polygone P)  {
    
       return P.POL_COULEUR;
  }
 
tpoint Tpoints(polygone P) {
                           
        tpoint t1;
        int i;
        for(i=0; i< P.POL_NSOMMETS; i++)
        t1.TAB[i] = P.POL_TABPOINTS[i];
        return t1;
  }

/*---------------------------------------------------------------------------*/

/*----module ARC-----*/ 
/*===================*/
                    
arc Definea(point          DEBUTARC, 
             unsigned long  LARGEUR,
             unsigned long  HAUTEUR,
             int            ANGLEDROIT,
             int            ANGLEGAUCHE,
             style          WSTYLE,
             couleur        WCOULEUR)  {
 
      arc arcloc;
      arcloc.ARC_DEBUTARC    = DEBUTARC;
      arcloc.ARC_LARGEUR     = LARGEUR;
      arcloc.ARC_HAUTEUR     = HAUTEUR;
      arcloc.ARC_ANGLEDROIT  = ANGLEDROIT;
      arcloc.ARC_ANGLEGAUCHE = ANGLEGAUCHE;
      arcloc.ARC_STYLE       = WSTYLE;
      arcloc.ARC_COULEUR     = WCOULEUR;
      return  arcloc;
  } 
                     

void Drawa(arc A) {   
      
      nom_coul = rech(A.ARC_COULEUR);
      code_ret = XAllocNamedColor(canal_aff,
                                   palette,
                                   nom_coul,
                                   &couleu,
                                   &exact); 

      masq_bin = GCLineStyle | GCForeground; 
      val_gc.line_style = LineSolid; 
      val_gc.foreground = couleu.pixel;   
      XChangeGC(canal_aff,
                 contexte_graph,
                 masq_bin,
                 &val_gc);   

      c = A.ARC_STYLE;
      lignestyle(c);

      XDrawArc(canal_aff,
               nouv_fen,
               contexte_graph,
               A.ARC_DEBUTARC.X,
               A.ARC_DEBUTARC.Y,
               A.ARC_LARGEUR,
               A.ARC_HAUTEUR,
               A.ARC_ANGLEDROIT*64,
               A.ARC_ANGLEGAUCHE*64); 
      XFlush(canal_aff);
  }                      

void Undrawa(arc A) {  
      
      masq_bin = GCForeground | GCLineStyle | GCFillStyle; 
      val_gc.foreground = carrplan;  
      val_gc.line_style = LineSolid; 
      val_gc.fill_style = FillSolid;
      XChangeGC(canal_aff,
                 contexte_graph,
                 masq_bin,
                 &val_gc);

      XDrawArc(canal_aff,
               nouv_fen,
               contexte_graph,
               A.ARC_DEBUTARC.X,
               A.ARC_DEBUTARC.Y,
               A.ARC_LARGEUR,
               A.ARC_HAUTEUR,
               A.ARC_ANGLEDROIT*64,
               A.ARC_ANGLEGAUCHE*64); 
      XFlush(canal_aff);
  }                      


void Change_attriba(arc     *A,
                     style   WSTYLE,
                     couleur WCOULEUR)  {
       
       Undrawa(*A);
       A->ARC_STYLE    = WSTYLE;
       A->ARC_COULEUR  = WCOULEUR; 
       Drawa(*A); 
  }

void Movea(arc *A, 
            point DEBUTARC) {

      Undrawa(*A); 
      A->ARC_DEBUTARC = DEBUTARC;
      Drawa(*A);
}


void Duplica(arc        A,
              arc        *AD,
              point      DEBUTARC) {

      AD->ARC_DEBUTARC    = DEBUTARC; 
      AD->ARC_LARGEUR     = A.ARC_LARGEUR;
      AD->ARC_HAUTEUR     = A.ARC_HAUTEUR;
      AD->ARC_ANGLEDROIT  = A.ARC_ANGLEDROIT;
      AD->ARC_ANGLEGAUCHE = A.ARC_ANGLEGAUCHE;
      AD->ARC_STYLE       = A.ARC_STYLE;
      AD->ARC_COULEUR     = A.ARC_COULEUR;  
      Definea(AD->ARC_DEBUTARC,
               AD->ARC_LARGEUR,
               AD->ARC_HAUTEUR,
               AD->ARC_ANGLEDROIT,
               AD->ARC_ANGLEGAUCHE,
               AD->ARC_STYLE,
               AD->ARC_COULEUR);
      Drawa(*AD);

}

point Debutarca(arc A)   {

       return A.ARC_DEBUTARC;
  } 
                      
unsigned long Largeura(arc A)  {

       return A.ARC_LARGEUR;
  }

unsigned long Hauteura(arc A)  {  

       return A.ARC_HAUTEUR;
  }

int  Angledroita(arc A)  {

       return A.ARC_ANGLEDROIT;
  }

int  Anglegauchea(arc A)  {

       return A.ARC_ANGLEGAUCHE;
  }

style Stylea(arc A)  {
   
       return A.ARC_STYLE;
  }

couleur Coula(arc A)  {
     
       return A.ARC_COULEUR;
  }  
          
/*--------------------------------------------------------------------------*/ 

/*==================================================*/
/*      corps des fonctions et des procedures       */
/*               figures pleines                    */
/*==================================================*/
 
/*---module RECTANGLE-----*/ 
/*=======================*/

rectangler  Definerr(point         DEBUT,
                      unsigned long LARGEUR,
                      unsigned long HAUTEUR,
                      mode_remp     REMPLI,
                      couleur       WCOULEUR) {

       rectangler Recloc;
       Recloc.PREC_DEBUT    = DEBUT;
       Recloc.PREC_LARGEUR  = LARGEUR;
       Recloc.PREC_HAUTEUR  = HAUTEUR; 
       Recloc.PREC_REMPLI   = REMPLI;
       Recloc.PREC_COULEUR  = WCOULEUR; 
       return  Recloc;
  } 

void Drawrr(rectangler PR)  { 
       
     nom_coul = rech(PR.PREC_COULEUR);
     code_ret = XAllocNamedColor(canal_aff,
                                  palette,
                                  nom_coul,
                                  &couleu,
                                  &exact); 

     masq_bin = GCForeground | GCFillStyle; 
     val_gc.foreground = couleu.pixel; 
     val_gc.fill_style = FillSolid;   
     XChangeGC(canal_aff,
                contexte_graph,
                masq_bin,
                &val_gc);
                 
     b = PR.PREC_REMPLI;
     rempli(b);    

     XFillRectangle(canal_aff,
                    nouv_fen,
                    contexte_graph,
                    PR.PREC_DEBUT.X,
                    PR.PREC_DEBUT.Y,
                    PR.PREC_LARGEUR,
                    PR.PREC_HAUTEUR);
     XFlush(canal_aff);
  }  
   
void Undrawrr(rectangler PR)  { 
                            
     masq_bin = GCForeground | GCFillStyle; 
     val_gc.foreground = carrplan; 
     val_gc.fill_style = FillSolid;   
     XChangeGC(canal_aff,
                contexte_graph,
                masq_bin,
                &val_gc);
  
     XFillRectangle(canal_aff,
                    nouv_fen,
                    contexte_graph,
                    PR.PREC_DEBUT.X,
                    PR.PREC_DEBUT.Y,
                    PR.PREC_LARGEUR,
                    PR.PREC_HAUTEUR);
     XFlush(canal_aff);
  }  


void Change_attribrr(rectangler  *PR, 
                      mode_remp   REMPLI,
                      couleur     WCOULEUR)  { 
      
      Undrawrr(*PR); 
      PR->PREC_REMPLI   = REMPLI;
      PR->PREC_COULEUR  = WCOULEUR;
      Drawrr(*PR); 
  }

void Moverr(rectangler *PR, 
             point      DEBUT) {

      Undrawrr(*PR); 
      PR->PREC_DEBUT = DEBUT;
      Drawrr(*PR);
}


void Duplicrr(rectangler     PR,
               rectangler     *PRD,
               point          DEBUT) {
             
      PRD->PREC_DEBUT    = DEBUT; 
      PRD->PREC_LARGEUR  = PR.PREC_LARGEUR; 
      PRD->PREC_HAUTEUR  = PR.PREC_HAUTEUR;
      PRD->PREC_REMPLI   = PR.PREC_REMPLI;
      PRD->PREC_COULEUR  = PR.PREC_COULEUR;  
      Definerr(PRD->PREC_DEBUT,
                PRD->PREC_LARGEUR,
                PRD->PREC_HAUTEUR,
                PRD->PREC_REMPLI,
                PRD->PREC_COULEUR);
      Drawrr(*PRD);

}

point Debutrr(rectangler PR)  {
    
       return PR.PREC_DEBUT;
  } 
                     
unsigned long Largrr(rectangler PR)  {
   
       return PR.PREC_LARGEUR;
  }
   
unsigned long Hautrr(rectangler PR)  {

       return PR.PREC_HAUTEUR;
  }

mode_remp Remprr(rectangler PR)  {
     
       return PR.PREC_REMPLI;
  }  

couleur Coulrr(rectangler PR)  {
     
       return PR.PREC_COULEUR;
  }  

/*---------------------------------------------------------------------------*/

/*---module CERCLE-----*/
/*=====================*/

cercler Definecr(point          CENTRE,
                  unsigned long  RAYON,
                  mode_remp      REMPLI,
                  couleur        WCOULEUR)   {

      cercler  Cerloc;
      Cerloc.PCER_CENTRE    = CENTRE;
      Cerloc.PCER_RAYON     = RAYON; 
      Cerloc.PCER_REMPLI    = REMPLI;
      Cerloc.PCER_COULEUR   = WCOULEUR; 
      return   Cerloc;
  } 

void Drawcr(cercler PC)  {    

      nom_coul = rech(PC.PCER_COULEUR);
      code_ret = XAllocNamedColor(canal_aff,
                                   palette,
                                   nom_coul,
                                   &couleu,
                                   &exact); 

      masq_bin = GCForeground | GCFillStyle; 
      val_gc.foreground = couleu.pixel; 
      val_gc.fill_style = FillSolid;  
      XChangeGC(canal_aff,
                 contexte_graph,
                 masq_bin,
                 &val_gc);  

      b = PC.PCER_REMPLI; 
      rempli(b);  

      XFillArc(canal_aff,
               nouv_fen,
               contexte_graph,
               PC.PCER_CENTRE.X-PC.PCER_RAYON,
               PC.PCER_CENTRE.Y-PC.PCER_RAYON,
               PC.PCER_RAYON*2,
               PC.PCER_RAYON*2,
               0, 
               360*64);
      XFlush(canal_aff); 
  }

void Undrawcr(cercler PC)  {   
       
      masq_bin =  GCForeground | GCFillStyle; 
      val_gc.foreground = carrplan; 
      val_gc.fill_style = FillSolid;  
      XChangeGC(canal_aff,
                 contexte_graph,
                 masq_bin,
                 &val_gc);
      
      XFillArc(canal_aff,
               nouv_fen,
               contexte_graph,
               PC.PCER_CENTRE.X-PC.PCER_RAYON,
               PC.PCER_CENTRE.Y-PC.PCER_RAYON,
               PC.PCER_RAYON*2,
               PC.PCER_RAYON*2,
               0, 
               360*64);
      XFlush(canal_aff); 
  }
     
void Change_attribcr(cercler   *PC,
                      mode_remp REMPLI,
                      couleur   WCOULEUR)  {  

       Undrawcr(*PC);
       PC->PCER_REMPLI   = REMPLI;
       PC->PCER_COULEUR  = WCOULEUR; 
       Drawcr(*PC);
  }

void Movecr(cercler    *PC, 
             point      CENTRE) {

      Undrawcr(*PC); 
      PC->PCER_CENTRE = CENTRE;
      Drawcr(*PC);
}


void Dupliccr(cercler     PC,
               cercler     *PCD,
               point       CENTRE) {

      PCD->PCER_CENTRE   = CENTRE; 
      PCD->PCER_RAYON    = PC.PCER_RAYON;
      PCD->PCER_REMPLI   = PC.PCER_REMPLI;
      PCD->PCER_COULEUR  = PC.PCER_COULEUR;  
      Definecr(PCD->PCER_CENTRE,
                PCD->PCER_RAYON,
                PCD->PCER_REMPLI,
                PCD->PCER_COULEUR);
      Drawcr(*PCD);

}

point Centrecr(cercler PC)  {
    
       return PC.PCER_CENTRE;
  } 
                     
unsigned long Rayoncr(cercler PC)  {
    
       return PC.PCER_RAYON;
  }

mode_remp Rempcr(cercler PC)  {
    
       return PC.PCER_REMPLI;
  } 
 
couleur Coulcr(cercler PC)  {
    
       return PC.PCER_COULEUR;
  }  

/*---------------------------------------------------------------------------*/

/*-----module POLYGONE------*/
/*==========================*/

                                         
polygoner Definepr(int            NSOMMETS,
                    mode_remp      REMPLI,
                    couleur        WCOULEUR, 
                    point          TABPOINTS[100]) {      
                                    

      polygoner polloc; 
      int i;

      for(i=0; i<NSOMMETS; i++)                   
      polloc.PPOL_TABPOINTS[i] = TABPOINTS[i];      
 
      polloc.PPOL_NSOMMETS  = NSOMMETS;
      polloc.PPOL_REMPLI    = REMPLI;
      polloc.PPOL_COULEUR   = WCOULEUR; 
      return polloc;
} 

void Drawpr(polygoner PP)  {

      b = PP.PPOL_REMPLI;
      rempli(b);

      nom_coul = rech(PP.PPOL_COULEUR);
      code_ret = XAllocNamedColor(canal_aff,
                                   palette,
                                   nom_coul,
                                   &couleu, 
                                   &exact); 

      masq_bin = GCForeground; 
      val_gc.foreground = couleu.pixel;   
      XChangeGC(canal_aff,
                 contexte_graph,
                 masq_bin,
                 &val_gc);
                  
      XSetFillRule(canal_aff,
                    contexte_graph,
                    EvenOddRule);
      XFillPolygon(canal_aff,
                   nouv_fen,
                   contexte_graph, 
(XPoint *)PP.PPOL_TABPOINTS,
                   PP.PPOL_NSOMMETS,
                   Complex,      
                   CoordModeOrigin); 
      XFlush(canal_aff);                                    
  }

  
void Undrawpr(polygoner PP)  { 
       
      masq_bin = GCForeground | GCFillStyle; 
      val_gc.foreground = carrplan;  
      val_gc.fill_style = FillSolid; 
      XChangeGC(canal_aff,
                 contexte_graph,
                 masq_bin,
                 &val_gc);

      XSetFillRule(canal_aff,
                    contexte_graph,
                    EvenOddRule);
      XFillPolygon(canal_aff,
                   nouv_fen,
                   contexte_graph, 
(XPoint *)PP.PPOL_TABPOINTS,
                   PP.PPOL_NSOMMETS,
                   Complex,      
                   CoordModeOrigin); 
      XFlush(canal_aff);                   
  }


void Change_attribpr(polygoner *PP, 
                      mode_remp REMPLI,
                      couleur   WCOULEUR)  {
       
       Undrawpr(*PP); 
       PP->PPOL_REMPLI    = REMPLI;
       PP->PPOL_COULEUR   = WCOULEUR;  
       Drawpr(*PP);
  }            
 

void Movepr(polygoner *PP, 
             short     TRANSX,
             short     TRANSY) { 

      int i;
      Undrawpr(*PP); 

      for(i=0; i<PP->PPOL_NSOMMETS; i++)   {                
      PP->PPOL_TABPOINTS[i].X =(PP->PPOL_TABPOINTS[i]).X + TRANSX; 
      PP->PPOL_TABPOINTS[i].Y =(PP->PPOL_TABPOINTS[i]).Y + TRANSY; }
      Drawpr(*PP);
}


void Duplicpr(polygoner         PP,
               polygoner         *PPD,
               short             TRANSX,
               short             TRANSY) {
                                             
      int i;

      for(i=0; i<PP.PPOL_NSOMMETS; i++)   {                
      PPD->PPOL_TABPOINTS[i].X =(PP.PPOL_TABPOINTS[i]).X + TRANSX; 
      PPD->PPOL_TABPOINTS[i].Y =(PP.PPOL_TABPOINTS[i]).Y + TRANSY; }  
      PPD->PPOL_REMPLI         = PP.PPOL_REMPLI;
      PPD->PPOL_COULEUR        = PP.PPOL_COULEUR;  
      PPD->PPOL_NSOMMETS       = PP.PPOL_NSOMMETS;
      Definepr(PPD->PPOL_NSOMMETS,
                PPD->PPOL_REMPLI,
                PPD->PPOL_COULEUR,
                PPD->PPOL_TABPOINTS);
      Drawpr(*PPD);

}

int   Nsommetspr(polygoner PP)  {

       return PP.PPOL_NSOMMETS;
  }

mode_remp Remppr(polygoner PP)  {
    
       return PP.PPOL_REMPLI;
  }  

couleur Coulpr(polygoner PP)  {
    
       return PP.PPOL_COULEUR;
  }

tpoint Tpointsr(polygoner PP) {
                           
        tpoint t1;
        int i;
        for(i=0; i< PP.PPOL_NSOMMETS; i++)
        t1.TAB[i] = PP.PPOL_TABPOINTS[i];
        return t1;
  }
   
/*---------------------------------------------------------------------------*/

/*----module ARC-----*/ 
/*===================*/
                    
arcr Definear(point          DEBUTARC, 
               unsigned long  LARGEUR,
               unsigned long  HAUTEUR,
               int            ANGLEDROIT,
               int            ANGLEGAUCHE,
               mode_remp      REMPLI,
               couleur        WCOULEUR)  {
 
      arcr arcloc;
      arcloc.PARC_DEBUTARC    = DEBUTARC;
      arcloc.PARC_LARGEUR     = LARGEUR;
      arcloc.PARC_HAUTEUR     = HAUTEUR;
      arcloc.PARC_ANGLEDROIT  = ANGLEDROIT;
      arcloc.PARC_ANGLEGAUCHE = ANGLEGAUCHE;
      arcloc.PARC_REMPLI      = REMPLI;
      arcloc.PARC_COULEUR     = WCOULEUR;
      return  arcloc;
  } 

void Drawar(arcr PA) { 
                         
      b = PA.PARC_REMPLI;
      rempli(b);
                       
      nom_coul = rech(PA.PARC_COULEUR);
      code_ret = XAllocNamedColor(canal_aff,
                                   palette,
                                   nom_coul,
                                   &couleu,
                                   &exact); 

      masq_bin = GCForeground; 
      val_gc.foreground = couleu.pixel;   
      XChangeGC(canal_aff,
                 contexte_graph,
                 masq_bin,
                 &val_gc);

      XFillArc(canal_aff,
               nouv_fen,
               contexte_graph,
               PA.PARC_DEBUTARC.X,
               PA.PARC_DEBUTARC.Y,
               PA.PARC_LARGEUR,
               PA.PARC_HAUTEUR,
               PA.PARC_ANGLEDROIT*64,
               PA.PARC_ANGLEGAUCHE*64); 
      XFlush(canal_aff);
  }                      

void Undrawar(arcr PA) {  
      
      masq_bin = GCForeground | GCFillStyle; 
      val_gc.foreground = carrplan;
      val_gc.fill_style = FillSolid;   
      XChangeGC(canal_aff,
                 contexte_graph,
                 masq_bin,
                 &val_gc);
     
      XFillArc(canal_aff,
               nouv_fen,
               contexte_graph,
               PA.PARC_DEBUTARC.X,
               PA.PARC_DEBUTARC.Y,
               PA.PARC_LARGEUR,
               PA.PARC_HAUTEUR,
               PA.PARC_ANGLEDROIT*64,
               PA.PARC_ANGLEGAUCHE*64); 
      XFlush(canal_aff);
  }                      


void Change_attribar(arcr      *PA, 
                      mode_remp REMPLI,
                      couleur   WCOULEUR)  {
                               
      Undrawar(*PA);
      PA->PARC_REMPLI   = REMPLI;
      PA->PARC_COULEUR  = WCOULEUR; 
      Drawar(*PA); 
  }
                      
void Movear(arcr       *PA, 
             point      DEBUTARC) {

      Undrawar(*PA); 
      PA->PARC_DEBUTARC = DEBUTARC;
      Drawar(*PA);
}
 
void Duplicar(arcr        PA,
               arcr        *PAD,
               point       DEBUTARC) {

      PAD->PARC_DEBUTARC    = DEBUTARC; 
      PAD->PARC_LARGEUR     = PA.PARC_LARGEUR;
      PAD->PARC_HAUTEUR     = PA.PARC_HAUTEUR;
      PAD->PARC_ANGLEDROIT  = PA.PARC_ANGLEDROIT;
      PAD->PARC_ANGLEGAUCHE = PA.PARC_ANGLEGAUCHE;
      PAD->PARC_REMPLI      = PA.PARC_REMPLI;
      PAD->PARC_COULEUR     = PA.PARC_COULEUR;  
      Definear(PAD->PARC_DEBUTARC,
                PAD->PARC_LARGEUR,
                PAD->PARC_HAUTEUR,
                PAD->PARC_ANGLEDROIT,
                PAD->PARC_ANGLEGAUCHE,
                PAD->PARC_REMPLI,
                PAD->PARC_COULEUR);
      Drawar(*PAD);

}

point Debutarcar(arcr PA)   {

       return PA.PARC_DEBUTARC;
  } 
                      
unsigned long Largeurar(arcr PA)  {

       return PA.PARC_LARGEUR;
  }

unsigned long Hauteurar(arcr PA)  {  

       return PA.PARC_HAUTEUR;
  }

int  Angledroitar(arcr PA)  {

       return PA.PARC_ANGLEDROIT;
  }

int  Anglegauchear(arcr PA)  {

       return PA.PARC_ANGLEGAUCHE;
  }

mode_remp Rempar(arcr PA)  {
     
       return PA.PARC_REMPLI;
  } 
 
couleur Coular(arcr PA)  {
     
       return PA.PARC_COULEUR;
  }  

/*--------------------------------------------------------------------------*/ 
     
/*==================================================*/
/*      corps des fonctions et des procedures       */
/*               chaine de caracteres               */
/*==================================================*/
 

/*----module TEXTE-----*/
/*=====================*/

   texte Definet(point          DEBUTCHAINE,
                  policecar      POLICE, 
                  taillecar      TAILLE,
                  couleur        WCOULEUR,
                  char           CHAINE[100])  {
                       
      texte texteloc; 

      strcpy(texteloc.TEX_CHAINE,CHAINE);

      texteloc.TEX_DEBUTCHAINE = DEBUTCHAINE;
      texteloc.TEX_POLICE      = POLICE;
      texteloc.TEX_TAILLE      = TAILLE;
      texteloc.TEX_COULEUR     = WCOULEUR; 
      return   texteloc;
  } 

void Drawt(texte T)  { 
       
      choixfonte(T.TEX_POLICE,T.TEX_TAILLE);   
      nom_coul = rech(T.TEX_COULEUR);
      code_ret = XAllocNamedColor(canal_aff,
                                   palette,
                                   nom_coul,
                                   &couleu,
                                   &exact);    

      masq_bin =  GCForeground | GCFillStyle; 
      val_gc.foreground = couleu.pixel; 
      val_gc.fill_style = FillSolid;  
      
      XChangeGC(canal_aff,
                 contexte_graph,
                 masq_bin,
                 &val_gc);
      XDrawString(canal_aff,
                  nouv_fen,
                  contexte_graph,                   

                  T.TEX_DEBUTCHAINE.X,
                  T.TEX_DEBUTCHAINE.Y,
                  T.TEX_CHAINE,
                  strlen(T.TEX_CHAINE)); 
      XFlush(canal_aff);
  }   
 
void Undrawt(texte T)  { 
     
      masq_bin =  GCForeground | GCFillStyle; 
      val_gc.foreground = carrplan; 
      val_gc.fill_style = FillSolid;  
           
      XChangeGC(canal_aff,
                 contexte_graph,
                 masq_bin,
                 &val_gc);
      XDrawString(canal_aff,
                  nouv_fen,
                  contexte_graph,
                  T.TEX_DEBUTCHAINE.X,
                  T.TEX_DEBUTCHAINE.Y,
                  T.TEX_CHAINE,
                  strlen(T.TEX_CHAINE)); 
      XFlush(canal_aff);
  }   

void Change_attribt(texte     *T,
                     policecar POLICE, 
                     taillecar TAILLE,
                     couleur   WCOULEUR)  {
       
       Undrawt(*T);
       T->TEX_POLICE    = POLICE; 
       T->TEX_TAILLE    = TAILLE;
       T->TEX_COULEUR   = WCOULEUR; 
       Drawt(*T);
  }


void Movet(texte *T, 
            point DEBUTCHAINE) {

      Undrawt(*T); 
      T->TEX_DEBUTCHAINE = DEBUTCHAINE;
      Drawt(*T);
}


void Duplict(texte      T,
              texte      *TD,
              point      DEBUTCHAINE) {

      TD->TEX_DEBUTCHAINE   = DEBUTCHAINE; 
      TD->TEX_POLICE        = T.TEX_POLICE;
      TD->TEX_TAILLE        = T.TEX_TAILLE;
      TD->TEX_COULEUR       = T.TEX_COULEUR; 
      strcpy(TD->TEX_CHAINE,T.TEX_CHAINE);
      Definet(TD->TEX_DEBUTCHAINE, 
               TD->TEX_POLICE,
               TD->TEX_TAILLE,
               TD->TEX_COULEUR,
               TD->TEX_CHAINE);

      Drawt(*TD);

}

point Debchainet(texte T)  {

       return T.TEX_DEBUTCHAINE;
  }
                     
policecar Policet(texte T)  {    

       return T.TEX_POLICE;     
  }
    
taillecar Taillet(texte T)  {
     
       return T.TEX_TAILLE;
  }

couleur Coult(texte T)   {
  
       return T.TEX_COULEUR;
  }        
 
tchar Chainet(texte T)  {

        tchar t2;
        int i;
        for(i=0; i< 100; i++)
        t2.TAC[i] = T.TEX_CHAINE[i];
        return t2;
  }  

/*----------------------------------------------------------------------------*/
 
/*==================================================*/
/*      corps des fonctions et des procedures       */
/*      initialisation fenetre sous X WINDOW        */
/*==================================================*/

                     
void InitGraphe(int X,
                 int Y,
                 unsigned WIDTH,
                 unsigned HEIGHT)  {

     XSizeHints infos_fen; 
     int ecran;
     //     sleep(5);
     if((canal_aff = XOpenDisplay(0))==NULL)
     {
     	printf("Ne peut se connecter au serveur X. \
     	Verifiez la variable DISPLAY\n");
     	exit(1);
     }
     
     ecran     = DefaultScreen(canal_aff);
     cavplan   = BlackPixel(canal_aff,ecran);
     carrplan  = WhitePixel(canal_aff,ecran);  
     palette   = DefaultColormap(canal_aff,ecran);  
     infos_fen.x = X;
     infos_fen.y = Y;
     infos_fen.width  = WIDTH;
     infos_fen.height = HEIGHT; 
     infos_fen.flags  = PPosition | PSize;    
     masq_attrib = CWOverrideRedirect;
     xswa.override_redirect = True;
     
     nouv_fen = XCreateSimpleWindow(canal_aff,
                                     DefaultRootWindow(canal_aff),
                                     infos_fen.x,
                                     infos_fen.y,
                                     infos_fen.width,
                                     infos_fen.height,
                                     5,
                                     cavplan,
                                     carrplan);  
     XChangeWindowAttributes(canal_aff,
                              nouv_fen,
                              masq_attrib,
                              &xswa);
     XSetWindowBorder(canal_aff,
                       nouv_fen,
                       couleu.pixel);
     contexte_graph = XCreateGC(canal_aff,
                                 nouv_fen,
                                 0,
                                 0);
     XSetForeground(canal_aff,
                     contexte_graph,
                     cavplan);
     XSetBackground(canal_aff,
                     contexte_graph,
                     carrplan);
     curseur = XCreateFontCursor(canal_aff,132);
     XDefineCursor(canal_aff,nouv_fen,curseur);
     
     XSelectInput(canal_aff,
                   nouv_fen,
                   ExposureMask | KeyPressMask | ButtonPressMask);
     XMapRaised(canal_aff,
                 nouv_fen);
     XNextEvent(canal_aff,
                 &evenem);      
 }      
/*---------------------------------------------------------------------------*/

void CloseGraphe() {

     XFreeGC(canal_aff,contexte_graph);
     XDestroyWindow(canal_aff,nouv_fen);
     XCloseDisplay(canal_aff);
 }
/*---------------------------------------------------------------------------*/

void Clear() {

     XClearWindow(canal_aff,nouv_fen);
 }
/*---------------------------------------------------------------------------*/

int Keypressed() {
     
     if(XEventsQueued(canal_aff,QueuedAfterReading) != 0) {  

     XNextEvent(canal_aff,
                 &evenem);   

     if(evenem.type == KeyPress) return 0; 
     
     else return 1; } 
     else return 1;

} 

/*---------------------------------------------------------------------------*/

tcla1 retourkey1(point COORD) {

     int   nbcar;
     tcla1 tret;
     char  frappe[5];
     int   i;
     
     for(i=0; i<=49; i++)
     tret.TAD[i] = ' ';
     
     for(i=1; i<=50; i++) {
     
     if(XEventsQueued(canal_aff,QueuedAfterReading) !=0) {
     
     XNextEvent(canal_aff,
                 &evenem);
                 
     if(evenem.type == KeyPress) {
     nbcar = XLookupString((XKeyEvent *)&evenem,
                           frappe,
                           5,
                           &touche,
                           0);
     XDrawString(canal_aff,
                nouv_fen,
                contexte_graph,
                COORD.X,
                COORD.Y,
                frappe,
                nbcar);
     XFlush(canal_aff);
     COORD.X = COORD.X + 10;
     if(frappe[0] != ' ')
     tret.TAD[i] = frappe [0];
     }
     }
     }
     return tret;
 }

/*---------------------------------------------------------------------------*/

tcla2 retourkey2() {

     /*int   nbcar;*/
     tcla2 tret1;
     char  frappe[5];
     int   i;
     
     for(i=0; i<=49; i++)
     tret1.TAE[i] = ' ';
     
     for(i=1; i<=50; i++) {
     
     if(XEventsQueued(canal_aff,QueuedAfterReading) !=0) {
     
     XNextEvent(canal_aff,
                 &evenem);
                 
     if(evenem.type == KeyPress) {
     /*nbcar*/(void)XLookupString((XKeyEvent *)&evenem,
                           frappe,
                           5,
                           &touche,
                           0);
     XFlush(canal_aff);
     if(frappe[0] != ' ')
     tret1.TAE[i] = frappe [0];
     }
     }
     }
     return tret1;
 }
 
/*---------------------------------------------------------------------------*/

int Buttonpressed() {
     
     if(XEventsQueued(canal_aff,QueuedAfterReading) != 0) {  

     XNextEvent(canal_aff,
                 &evenem);   

     if(evenem.type == ButtonPress) return 0;
                                              
     else return 1; } 
     else return 1;

} 

/*---------------------------------------------------------------------------*/

void PointCursor(point COORD) {

     XWarpPointer(canal_aff,
                   None,
                   nouv_fen,
                   0,
                   0,
                   0,
                   0,
                   COORD.X,
                   COORD.Y);
     XFlush(canal_aff);
     
}

void ModeXOR()
{
	XSetFunction(canal_aff,contexte_graph,GXxor);
}

void ModeNORMAL()
{
	XSetFunction(canal_aff,contexte_graph,GXcopy);
}
#else
void unUsedSymbol() {}

#endif // X_DISPLAY_MISSING

// $Id: graph_c.cc,v 3.9 2014/05/07 12:10:57 erk Exp $
