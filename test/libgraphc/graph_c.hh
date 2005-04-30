// ---------------------------------------------------------------------------
// libgraphc - X11 drawing library
// Copyright (C) 2002, 2003  ONERA
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: graph_c.hh,v 3.1 2005/04/30 14:46:54 breholee Exp $ 
// ---------------------------------------------------------------------------

#ifndef X_DISPLAY_MISSING

#ifndef GRAPH_C_HH
#define GRAPH_C_HH

#include <config.h>

/*--------------------------------------------------------------------------*/
/*---definition des attributs graphiques------------------------------------*/
/*--------------------------------------------------------------------------*/ 

   typedef  enum {PLEIN,
                  POINTILLE1,
                  POINTILLE2,
                  POINTILLE3}  style;

   typedef  enum {BLACK,
                  RED,
                  GREEN,
                  BLUE,                                                        
                  GRAY,
                  CYAN,
                  YELLOW,
                  MAGENTA,
                  BROWN,
                  ORANGE,
                  PINK,
                  VIOLET,
                  WHITE}       couleur;  
                                               
   typedef  enum {COURIER,
                  COURIER_GRAS,
                  COURIER_OBLIQUE,
                  COURIER_GRAS_OBLIQUE,
                  HELVETICA, 
                  TIMES,
                  SYMBOL}       policecar;  

   typedef  enum {TAILLE8,
                  TAILLE10,
                  TAILLE12,
                  TAILLE14,
                  TAILLE18,
                  TAILLE24}     taillecar; 


   typedef  enum {COUL_UNIE,
                  MOTIF1,
                  MOTIF2,
                  MOTIF3,
                  MOTIF4,
                  MOTIF5,
                  MOTIF6,
                  MOTIF7,
                  MOTIF8,
                  MOTIF9}      mode_remp;
                                        
/*--------------------------------------------------------------------------*/
/*---definition des types structures----------------------------------------*/
/*--------------------------------------------------------------------------*/

   typedef   struct {short X;
                     short Y;}          point;   
                                       
   typedef   struct {point  TAB[100];}  tpoint;    

   typedef   struct {char   TAC[100];}  tchar;   

   typedef   struct {char   TAD[50];}   tcla1;
   
   typedef   struct {char   TAE[50];}   tcla2;
                           
           /*===============================*/
           /*       figures simples         */
           /*  coordonnees mode origine     */
           /*===============================*/       
     

   typedef   struct {point          POI_COORD;                   /* point */
                     couleur        POI_COULEUR;} pointt;

   typedef   struct {point          LIG_DEBUT;                   /* ligne */
                     point          LIG_FIN;
                     style          LIG_STYLE;
                     couleur        LIG_COULEUR;} ligne;
                                           
   typedef   struct {point          REC_DEBUT;                   /* rectangle*/
                     unsigned long  REC_LARGEUR;
                     unsigned long  REC_HAUTEUR;
                     style          REC_STYLE;
                     couleur        REC_COULEUR;} rectangle;

   typedef   struct {point          CER_CENTRE;                  /* cercle */
                     unsigned long  CER_RAYON;
                     style          CER_STYLE;
                     couleur        CER_COULEUR;} cercle; 

   typedef   struct {int            POL_NSOMMETS;                /* polygone */
                     style          POL_STYLE;   
                     couleur        POL_COULEUR; 
                     point          POL_TABPOINTS[100];} polygone;    

   typedef   struct {point          ARC_DEBUTARC;                /* arc simple */
                     unsigned long  ARC_LARGEUR;
                     unsigned long  ARC_HAUTEUR;
                     int            ARC_ANGLEDROIT;
                     int            ARC_ANGLEGAUCHE;
                     style          ARC_STYLE;
                     couleur        ARC_COULEUR;} arc;   

     
           /*==================================*/
           /*       figures pleines            */
           /*  coordonnees mode origine        */
           /*  fermeture de l arc par 2 rayons */ 
           /*  remplissage du polygone par la  */
           /*  regle pair/impair               */
           /*==================================*/       

                                                      
   typedef   struct {point          PREC_DEBUT;                 /* rectangle */
                     unsigned long  PREC_LARGEUR;
                     unsigned long  PREC_HAUTEUR;
                     mode_remp      PREC_REMPLI;
                     couleur        PREC_COULEUR;} rectangler;   
                                                               
   typedef   struct {point          PCER_CENTRE;                /* cercle  */
                     unsigned long  PCER_RAYON;
                     mode_remp      PCER_REMPLI;
                     couleur        PCER_COULEUR;} cercler;
                        
   typedef   struct {int            PPOL_NSOMMETS;              /* polygone */ 
                     mode_remp      PPOL_REMPLI;      
                     couleur        PPOL_COULEUR;   
                     point          PPOL_TABPOINTS[100];} polygoner;    
                                                              
   typedef   struct {point          PARC_DEBUTARC;              /* arc simple*/
                     unsigned long  PARC_LARGEUR;      
                     unsigned long  PARC_HAUTEUR;
                     int            PARC_ANGLEDROIT;
                     int            PARC_ANGLEGAUCHE;
                     mode_remp      PARC_REMPLI;
                     couleur        PARC_COULEUR;} arcr;   
   

           /*===============================*/
           /*     chaines de caracteres     */
           /*===============================*/


   typedef   struct {point          TEX_DEBUTCHAINE;
                     policecar      TEX_POLICE;  
                     taillecar      TEX_TAILLE;
                     couleur        TEX_COULEUR;
                     char           TEX_CHAINE[100];} texte;

 
/*--------------------------------------------------------------------------*/
/*  definitions des fonctions et des procedures                             */
/*         pour les figures simples                                         */
/*--------------------------------------------------------------------------*/
                            
  /*---module POINT---*/
  /*==================*/

   extern pointt   Definex(point   COORD,
                                   couleur WCOULEUR);

   extern void     Drawx(pointt  P);
   extern void     Change_attribx(pointt  *P,     
                                   couleur WCOULEUR); 
   extern void     Undrawx(pointt  P);  
   extern void     Movex(pointt  *P,
                                   point   COORD);  
   extern void     Duplicx(pointt  P,
                                   pointt  *PD,
                                   point   COORD);

   extern point    Coordx(pointt  P);
   extern couleur  Coulx(pointt  P);     

  
   /*---module LIGNE---*/ 
   /*==================*/


   extern ligne    Definel(point         DEBUT,
                                   point         FIN,
                                   style         WSTYLE,
                                   couleur       WCOULEUR);

   extern void     Drawl(ligne         L);
   extern void     Change_attribl(ligne         *L,
                                   style         WSTYLE,
                                   couleur       WCOULEUR); 
   extern void     Undrawl(ligne         L); 
   extern void     Movel(ligne         *L,
                                   short         TRANSX,
                                   short         TRANSY);
   extern void     Duplicl(ligne         L,
                                   ligne         *LD,
                                   short         TRANSX,
                                   short         TRANSY);
                                               
   extern point    Debutl(ligne         L);
   extern point    Finl(ligne         L);
   extern style    Stylel(ligne         L);
   extern couleur  Coull(ligne         L);
                                 
       
   /*---module RECTANGLE---*/ 
   /*======================*/


   extern rectangle     Definer(point         DEBUT,
                                        unsigned long LARGEUR,
                                        unsigned long HAUTEUR,
                                        style         WSTYLE,
                                        couleur       WCOULEUR);

   extern void          Drawr(rectangle     R);
   extern void          Change_attribr(rectangle     *R,
                                        style         WSTYLE,
                                        couleur       WCOULEUR); 
   extern void          Undrawr(rectangle     R); 
   extern void          Mover(rectangle     *R, 
                                        point         DEBUT); 
   extern void          Duplicr(rectangle     R,
                                        rectangle     *RD,
                                        point         DEBUT);

   extern point         Debutr(rectangle     R);
   extern unsigned long Largr(rectangle     R);
   extern unsigned long Hautr(rectangle     R);
   extern style         Styler(rectangle     R);
   extern couleur       Coulr(rectangle     R);

                              
    /*----module CERCLE----*/
    /*=====================*/


    extern cercle          Definec(point          CENTRE,
                                           unsigned long  RAYON,
                                           style          WSTYLE,
                                           couleur        WCOULEUR);
                          
    extern void            Drawc(cercle         C);
    extern void            Change_attribc(cercle         *C,
                                           style          WSTYLE,  
                                           couleur        WCOULEUR);
    extern void            Undrawc(cercle         C); 
    extern void            Movec(cercle         *C,
                                           point          CENTRE);
    extern void            Duplicc(cercle         C,
                                           cercle         *CD,
                                           point          CENTRE);
                                
    extern point           Centrec(cercle         C);
    extern unsigned long   Rayonc(cercle         C);
    extern style           Stylec(cercle         C);
    extern couleur         Coulc(cercle         C);

    

   /*----module POLYGONE----*/ 
   /*=======================*/


    extern polygone       Definep(int            NSOMMETS, 
                                          style          WSTYLE,
                                          couleur        WCOULEUR, 
                                          point          TABPOINTS[100]);  

    extern void           Drawp(polygone       P);
    extern void           Change_attribp(polygone       *P,
                                          style          WSTYLE,
                                          couleur        WCOULEUR);  
    extern void           Undrawp(polygone       P);
    extern void           Movep(polygone        *P,
                                          short           TRANSX,
                                          short           TRANSY);
    extern void           Duplicp(polygone        P,
                                          polygone        *LP,
                                          short           TRANSX,
                                          short           TRANSY);
                                        
    extern int            Nsommetsp(polygone       P);
    extern style          Stylep(polygone       P);
    extern couleur        Coulp(polygone       P); 
    extern tpoint         Tpoints(polygone       P);  
                    

   /*----module ARC-----*/
   /*===================*/

    extern arc             Definea(point          DEBUTARC,
                                           unsigned long  LARGEUR,
                                           unsigned long  HAUTEUR,
                                           int            ANGLEDROIT,
                                           int            ANGLEGAUCHE,
                                           style          WSTYLE,
                                           couleur        WCOULEUR);
                         
    extern void            Drawa(arc            A);
    extern void            Change_attriba(arc            *A,
                                           style          WSTYLE,
                                           couleur        WCOULEUR); 
    extern void            Undrawa(arc            A);
    extern void            Movea(arc            *A,
                                           point          DEBUTARC); 
    extern void            Duplica(arc            A,
                                           arc            *AD,
                                           point          DEBUTARC);
                         
    extern point           Debutarca(arc            A); 
    extern unsigned long   Largeura(arc            A);
    extern unsigned long   Hauteura(arc            A);
    extern int             Angledroita(arc            A);
    extern int             Anglegauchea(arc            A);
    extern style           Stylea(arc            A);
    extern couleur         Coula(arc            A);     
                          
               
/*--------------------------------------------------------------------------*/
/*  definitions des fonctions et des procedures                             */
/*         pour les figures pleines                                         */
/*--------------------------------------------------------------------------*/
    

   /*---module RECTANGLE---*/ 
   /*======================*/


   extern rectangler    Definerr(point         DEBUT,
                                         unsigned long LARGEUR,
                                         unsigned long HAUTEUR, 
                                         mode_remp     REMPLI,
                                         couleur       WCOULEUR);

   extern void          Drawrr(rectangler    PR);
   extern void          Change_attribrr(rectangler    *PR, 
                                         mode_remp     REMPLI,
                                         couleur       WCOULEUR);  
   extern void          Undrawrr(rectangler    PR);
   extern void          Moverr(rectangler    *PR, 
                                         point         DEBUT);  
   extern void          Duplicrr(rectangler    PR,
                                         rectangler    *PRD,
                                         point         DEBUT);
                              
   extern point         Debutrr(rectangler    PR);
   extern unsigned long Largrr(rectangler    PR);
   extern unsigned long Hautrr(rectangler    PR); 
   extern mode_remp     Remprr(rectangler    PR);
   extern couleur       Coulrr(rectangler    PR);

   
    /*----module CERCLE----*/ 
    /*=====================*/


    extern cercler         Definecr(point          CENTRE,
                                            unsigned long  RAYON, 
                                            mode_remp      REMPLI,
                                            couleur        WCOULEUR);
    
    extern void            Drawcr(cercler        PC); 
    extern void            Change_attribcr(cercler        *PC,
                                            mode_remp      REMPLI,
                                            couleur        WCOULEUR); 
    extern void            Undrawcr(cercler        PC); 
    extern void            Movecr(cercler        *PC,
                                            point          CENTRE);  
    extern void            Dupliccr(cercler        PC,
                                            cercler        *PCD,
                                            point          CENTRE);
                                                           
    extern point           Centrecr(cercler        PC);
    extern unsigned long   Rayoncr(cercler        PC); 
    extern mode_remp       Rempcr(cercler        PC);
    extern couleur         Coulcr(cercler        PC); 
  


   /*----module POLYGONE----*/ 
   /*=======================*/

                       
    extern polygoner      Definepr(int            NSOMMETS, 
                                           mode_remp      REMPLI,
                                           couleur        WCOULEUR, 
                                           point          TABPOINTS[100]);  

    extern void           Drawpr(polygoner      PP);
    extern void           Change_attribpr(polygoner      *PP,
                                           mode_remp      REMPLI,
                                           couleur        WCOULEUR);  
    extern void           Undrawpr(polygoner      PP);
    extern void           Movepr(polygoner      *PP,
                                          short           TRANSX,
                                          short           TRANSY);
    extern void           Duplicpr(polygoner      PP,
                                           polygoner      *PPD,
                                           short          TRANSX,
                                           short          TRANSY);

    extern int            Nsommetspr(polygoner      PP);
    extern mode_remp      Remppr(polygoner      PP);
    extern couleur        Coulpr(polygoner      PP); 
    extern tpoint         Tpointsr(polygoner      PP);  
                    

   /*----module ARC-----*/
   /*===================*/

    extern arcr            Definear(point          DEBUTARC,
                                            unsigned long  LARGEUR,
                                            unsigned long  HAUTEUR,
                                            int            ANGLEDROIT,
                                            int            ANGLEGAUCHE,
                                            mode_remp      REMPLI,
                                            couleur        WCOULEUR);

    extern void            Drawar(arcr           PA);
    extern void            Change_attribar(arcr           *PA,
                                            mode_remp      REMPLI,
                                            couleur        WCOULEUR); 
    extern void            Undrawar(arcr           PA); 
    extern void            Movear(arcr           *PA,
                                            point          DEBUTARC);
    extern void            Duplicar(arcr           PA,
                                            arcr           *PAD,
                                            point          DEBUTARC);
                                 
    extern point           Debutarcar(arcr           PA); 
    extern unsigned long   Largeurar(arcr           PA);
    extern unsigned long   Hauteurar(arcr           PA);
    extern int             Angledroitar(arcr           PA);
    extern int             Anglegauchear(arcr           PA); 
    extern mode_remp       Rempar(arcr           PA);
    extern couleur         Coular(arcr           PA);     
                                                 
/*---------------------------------------------------------------------------*/
/*  definitions des fonctions et des procedures                              */
/*         pour les chaines de caracteres                                    */
/*---------------------------------------------------------------------------*/

    /*----module TEXTE-----*/
    /*=====================*/


    extern texte      Definet(point      DEBUTCHAINE,
                                      policecar  POLICE,
                                      taillecar  TAILLE,
                                      couleur    WCOULEUR,
                                      char       CHAINE[100]);

     extern void      Drawt(texte      T);
     extern void      Change_attribt(texte      *T,
                                      policecar  POLICE,
                                      taillecar  TAILLE,
                                      couleur    WCOULEUR); 
     extern void      Undrawt(texte      T);
     extern void      Movet(texte      *T,
                                      point      DEBUTCHAINE);
     extern void      Duplict(texte      T,
                                      texte      *TD,
                                      point      DEBUTCHAINE);


     extern point     Debchainet(texte      T);
     extern policecar Policet(texte      T);  
     extern taillecar Taillet(texte      T);
     extern couleur   Coult(texte      T);
     extern tchar     Chainet(texte      T);    

/*---------------------------------------------------------------------------*/
/*  definitions des fonctions et des procedures                              */
/*         pour la creation d une fenetre X WINDOW                           */
/*---------------------------------------------------------------------------*/
                                           
                                                 
     /*---initialisation session graphique----*/ 
     /*=======================================*/

     extern void InitGraphe(int      X,
                             int      Y, 
                             unsigned WIDHT,
                             unsigned HEIGHT);
                                                  

     /*---fermeture session graphique-----*/
     /*===================================*/

     extern void CloseGraphe();     


     /*---effacement d une fenetre X WINDOW----*/
     /*========================================*/
                             
     extern void Clear(); 

/*---------------------------------------------------------------------------*/
/*  definitions des fonctions et des procedures                              */
/*         pour l interface clavier-souris                                   */
/*---------------------------------------------------------------------------*/
 
     /*---appui d'une touche du clavier----*/
     /*====================================*/

     extern int Keypressed();  

     /*---retour caractere frappe au clavier avec affichage----*/
     /*========================================================*/
                                             
     extern tcla1 retourkey1(point COORD);       


     /*---retour caractere frappe au clavier sans affichage----*/
     /*========================================================*/
                                             
     extern tcla2 retourkey2();       

     /*---appui d'un bouton de la souris---*/
     /*====================================*/

     extern int Buttonpressed();
     
     /*---positionnement du curseur---*/
     /*===============================*/
     
     extern void PointCursor(point COORD);
     
void ModeXOR();
void ModeNORMAL();

#endif // GRAPH_C_HH
#endif // X_DISPLAY_MISSING

// $Id: graph_c.hh,v 3.1 2005/04/30 14:46:54 breholee Exp $
