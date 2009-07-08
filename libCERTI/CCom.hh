// **** DEBUT ENTETE *********************************************************
//
//                    Copyright 1997 ONERA
//
//
//_NOM : hla/test/test_msg/CSockServer.hh
//
//_VERSION : 1.1
//
//_HISTORIQUE :
//  1.1  Creation par Thomas Hautesserres (hautesse@cert.fr).
//
//_DESCRIPTION : Classe CSockServer, qui simule un serveur recevant des 
//   CMessageReseau d'une autre classe CSockClient, a travers une connexion
//   TCP encapsulee dans la classe CSocketTCP (fichier ../util/sockTCP.hh).
//
//_CONTEXTE : Test de performance des differentes implementations des
//   CMessageReseau et des CSocket*.
//
//_CATEGORIE : Composant de l'implementation de HLA dans le cadre de SAIDA
//
//_SYNOPSIS :
//
//_ENTREE : 
//
//_SORTIE : 
//
//_MODIFIEE :  
//
//_LOCALE : 
// 
//_DIAGNOSTIC : 
//
//_BUGS CONNUS : ?
//
//_VOIR AUSSI : le fichier README.
//
// **** FIN ENTETE ***********************************************************

#ifndef CCOM_HH
#define CCOM_HH

#include "setup.hh"
#include "SocketTCP.hh"
#include "NetworkMessage.hh"
#include "SocketServer.hh"

using namespace certi ;

class CCom {

public:

  // --------------------------------
  // -- Constructor and Destructor --
  // --------------------------------

  // The first parameter is the function called when NULL messages are
  // received, and the second is called when UAV messages are received.
  CCom  (ProcessFunction *NullProcess = NULL,
	 ProcessFunction *UAVProcess  = NULL) ;
  
  virtual ~CCom () ;


  // ------------------
  // -- Main Methods --  
  // ------------------

  // Return the number Messages received (of all types)
  virtual long Main (unsigned int Time=0)=0;

  // Should be called whenever a signal is caught (stop the main loop)
  virtual void SignalHandler (int Sig)=0;

private:

  // ------------------------
  // -- Private Attributes --
  // ------------------------
  
  char EndLoop;
  
};

  // } // end namespace 

#endif // CCOM_HH

