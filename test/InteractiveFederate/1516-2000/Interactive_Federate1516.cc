

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <sstream>

#include <RTI/certiRTI1516.h>
#include <RTI/RTI1516.h>
#include <RTI/Enums.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/RTI1516fedTime.h>




using namespace std;

namespace rti1516
{
  class RTIambassador;
}

//Logical Time
std::auto_ptr <
  rti1516::LogicalTimeFactory >
  temps =
  rti1516::LogicalTimeFactoryFactory::makeLogicalTimeFactory (L"CERTI");
std::auto_ptr < rti1516::LogicalTime > tps (temps->makeLogicalTime ());

//Variables
bool
  timeRegulation = false;
bool
  timeConstrained = false;
rti1516::InteractionClassHandle MsgHandle;
rti1516::InteractionClassHandle MsgRHandle;
rti1516::ParameterHandle MsgDataHandle;
rti1516::ParameterHandle MsgDataRHandle;
rti1516::ObjectClassHandle TestObjectClassHandle;
rti1516::ObjectClassHandle TestObjectClassRHandle;
rti1516::AttributeHandle DataHandle;
rti1516::AttributeHandle DataRHandle;
rti1516::ObjectInstanceHandle TestObjectClassInstanceHandle;
rti1516::ObjectInstanceHandle TestObjectClassRInstanceHandle;


//Classe Federe_Interactif
class
  Federe_Interactif:
  public
  rti1516::NullFederateAmbassador
{

public:


  Federe_Interactif ()
  throw ();
  ~
  Federe_Interactif ()
  throw ();

  void
  callTICK ();
  void
  callGetObjClassHandle ();
  void
  callGetObjClassName ();
  void
  callGetAttrHandle ();
  void
  callGetAttrHandleName ();
  void
  callGetIntClassHandle ();
  void
  callGetIntClassName ();
  void
  callGetParaHandle ();
  void
  callGetParaHandleName ();
  void
  callGetObjInstanceHandle ();
  void
  callGetObjInstanceHandleName ();
  void
  callambF ();
  void
  callCFE ();
  void
  callDFE ();
  void
  callJFE (char *name);
  void
  callRFE ();
  void
  callPOCA ();
  void
  callUOC ();
  void
  callUOCA ();
  void
  callPIC ();
  void
  callUIC ();
  void
  callSOCA ();
  void
  callUsubOC ();
  void
  callUsubOCA ();
  void
  callSIC ();
  void
  callUsubIC ();
  void
  callROI ();
  void
  callROIWN (char *name); 
  void
  callUAV ();
  void
  callSI ();
  void
  callETR ();
  void
  callETC ();
  void
  callTAR ();
  void
  callNMR ();
  void
  callCIOT ();


  // CALLBACKS
  // Object management
  void
  discoverObjectInstance (rti1516::ObjectInstanceHandle theObject,
			  rti1516::ObjectClassHandle theObjectClass,
			  std::wstring const &theObjectInstanceName)
  throw (rti1516::CouldNotDiscover,
	 rti1516::ObjectClassNotKnown, rti1516::FederateInternalError)
  {
    wcout << endl << "     <----- discoverObjectInstance name =  " <<
      theObjectInstanceName << endl;
  }

  void
  receiveInteraction (rti1516::InteractionClassHandle theInteraction,
		      rti1516::ParameterHandleValueMap const
		      &theParameterValues,
		      rti1516::VariableLengthData const &theUserSuppliedTag,
		      rti1516::OrderType sentOrdering,
		      rti1516::TransportationType theTransport,
		      rti1516::LogicalTime const &theTime,
		      rti1516::OrderType receivedOrdering,
		      rti1516::MessageRetractionHandle theHandle)
  throw (rti1516::InteractionClassNotRecognized,
	 rti1516::InteractionParameterNotRecognized,
	 rti1516::InvalidLogicalTime, rti1516::FederateInternalError)
  {
    const void *
      tag_data;
    long unsigned int
      tag_size;
    const void *
      value_data;
    rti1516::ParameterHandleValueMap::const_iterator it;

    tag_size = theUserSuppliedTag.size ();
    tag_data = theUserSuppliedTag.data ();
    wcout << endl << "     <----- receiveInteraction timestamp = " <<
      theTime.toString () << " tag = " << (char *) tag_data << endl;

    for (it = theParameterValues.begin (); it != theParameterValues.end ();
	 ++it)
      {
	//value_data = theParameterValues[MsgDataRHandle];
	value_data = (it->second).data ();
	wcout << "            " << (it->first).
	  toString () << " <---> " << (char *) value_data << endl;
      }
  }


  void
  receiveInteraction (rti1516::InteractionClassHandle theInteraction,
		      rti1516::ParameterHandleValueMap const
		      &theParameterValues,
		      rti1516::VariableLengthData const &theUserSuppliedTag,
		      rti1516::OrderType sentOrdering,
		      rti1516::TransportationType theTransport)
  throw (rti1516::InteractionClassNotRecognized,
	 rti1516::InteractionParameterNotRecognized,
	 rti1516::FederateInternalError)
  {
    const void *
      tag_data;
    long unsigned int
      tag_size;
    const void *
      value_data;
    rti1516::ParameterHandleValueMap::const_iterator it;

    tag_size = theUserSuppliedTag.size ();
    tag_data = theUserSuppliedTag.data ();
    wcout << endl << "     <----- receiveInteraction tag = " << (char *)
      tag_data << endl;

    for (it = theParameterValues.begin (); it != theParameterValues.end ();
	 ++it)
      {
	//value_data = theParameterValues[MsgDataHandle];
	value_data = (it->second).data ();
	wcout << "            " << (it->first).
	  toString () << " <---> " << (char *) value_data << endl;
      }
  }

  void
  reflectAttributeValues (rti1516::ObjectInstanceHandle theObject,
			  rti1516::AttributeHandleValueMap const
			  &theAttributes,
			  rti1516::VariableLengthData const
			  &theUserSuppliedTag,
			  rti1516::OrderType sentOrdering,
			  rti1516::TransportationType theTransport,
			  rti1516::LogicalTime const &theTime,
			  rti1516::OrderType receivedOrdering,
			  rti1516::MessageRetractionHandle theHandle)
  throw (rti1516::ObjectInstanceNotKnown, rti1516::AttributeNotRecognized,
	 rti1516::AttributeNotSubscribed, rti1516::FederateInternalError)
  {

    const void *
      tag_data;
    double
      d;
    rti1516::AttributeHandleValueMap::const_iterator it;

    wcout << endl << "     <----- reflectAttributeValues timestamp = " <<
      theTime.toString ();
    tag_data = theUserSuppliedTag.data ();
    wcout << " tag = " << (char *) tag_data << endl;
    for (it = theAttributes.begin (); it != theAttributes.end (); ++it)
      {
	if (it->first == DataHandle)
	  {
	    memcpy (&d, (it->second).data (), (it->second).size ());
	    wcout << "                for the handle " << (it->
							   first).toString ()
	      << " value = " << d << endl;
	  }
      }
  }

  void
  reflectAttributeValues (rti1516::ObjectInstanceHandle theObject,
			  rti1516::AttributeHandleValueMap const
			  &theAttributes,
			  rti1516::VariableLengthData const
			  &theUserSuppliedTag,
			  rti1516::OrderType sentOrdering,
			  rti1516::TransportationType theTransport)
  throw (rti1516::ObjectInstanceNotKnown, rti1516::AttributeNotRecognized,
	 rti1516::AttributeNotSubscribed, rti1516::FederateInternalError)
  {
    const void *
      tag_data;
    double
      d;
    rti1516::AttributeHandleValueMap::const_iterator it;

    tag_data = theUserSuppliedTag.data ();
    wcout << endl << "     <----- reflectAttributeValues tag = "
      << (char *) tag_data;
    for (it = theAttributes.begin (); it != theAttributes.end (); ++it)
      {
	if (it->first == DataRHandle)
	  {
	    memcpy (&d, (it->second).data (), (it->second).size ());
	    wcout << " for the handle " << (it->first).toString ()
	      << " value = " << d << endl;
	  }
      }

  }

  // Time Management
  void
  timeRegulationEnabled (rti1516::LogicalTime const &theTime)
  throw (rti1516::InvalidLogicalTime,
	 rti1516::NoRequestToEnableTimeRegulationWasPending,
	 rti1516::FederateInternalError)
  {
    wcout << endl << "callback timeRegulationEnabled pour l'instant " <<
      theTime.toString () << endl;
    timeRegulation = true;
  }


  void
  timeConstrainedEnabled (rti1516::LogicalTime const &theTime)
  throw (rti1516::InvalidLogicalTime,
	 rti1516::NoRequestToEnableTimeConstrainedWasPending,
	 rti1516::FederateInternalError)
  {
    wcout << endl << "callback timeRegulationConstrained pour l'instant " <<
      theTime.toString () << endl;
    timeConstrained = true;
  }



  void
  timeAdvanceGrant (rti1516::LogicalTime const &theTime)
  throw (rti1516::InvalidLogicalTime,
	 rti1516::JoinedFederateIsNotInTimeAdvancingState,
	 rti1516::FederateInternalError)
  {
    this->theTime = theTime;
    wcout << endl << "t=" << theTime.toString () <<
      " : <----- timeAdvanceGrant(" << theTime.toString () << ")" << endl <<
      endl;
  }




private:
  rti1516::RTIambassador * rtiAmb;
  rti1516::FederateHandle fedHandle;
  RTI1516fedTime
    theTime;
  RTI1516fedTimeInterval
    lookahead;
};




//méthode
Federe_Interactif::Federe_Interactif ()throw ():rtiAmb (0), fedHandle (), theTime (0.0),
lookahead (1.0)
{

}

//méthode
Federe_Interactif::~Federe_Interactif ()throw ()
{
  delete rtiAmb;
}


//Prototypes
void print_menu (void);


Federe_Interactif *myInteractifFederate = new Federe_Interactif ();


int
main (int argc, char **argv)
{

  int i = 0;			//compteur
  float d;
  char c;			//entrée clavier (préambule automatique)
  string commande;		//comande entrée pour le test

  if (argc != 2)
    {
      cout << "Vous devez donner en parametre le nom du federe" << endl;
      exit (1);
    }

  while (1)
    {
      cout << "Voulez-vous un preambule automatique [y/n]" << endl;
      cin >> c;
      if (c == 'y' || c == 'n')
	break;
      cout << "Vous devez entrer 'y' ou 'n'" << endl;
    }

  if (c == 'y')
    {
	  // AMBASSADOR CREATION
      myInteractifFederate->callambF ();

      //CREER
      myInteractifFederate->callCFE ();

      //REJOINDRE
      myInteractifFederate->callJFE (argv[1]);

      //PUBLIER INTERACTION
      myInteractifFederate->callGetIntClassHandle ();
      myInteractifFederate->callGetParaHandle ();
      myInteractifFederate->callPIC ();

      //SOUSCRIRE INTERACTION
      myInteractifFederate->callSIC ();

      //PUBLIER CLASSE D'OBJET
      myInteractifFederate->callGetObjClassHandle ();
      myInteractifFederate->callGetAttrHandle ();
      myInteractifFederate->callPOCA ();
      myInteractifFederate->callROI ();

      //SOUSCRIRE CLASSE D'OBJET      
      myInteractifFederate->callSOCA ();

      //DECLARER REGULATEUR ET CONTRAINT
      myInteractifFederate->callETR ();
      myInteractifFederate->callETC ();
    }

  //MENU PRINCIPAL
  while (1)
    {
      i++;
      cout << endl << "Choisissez une action : " << endl;
      cin >> commande;

      if (commande == "q" || commande == "quit")
	break;
      else if (commande == "h" || commande == "help")
	print_menu ();
      else if (commande == "t")
	myInteractifFederate->callTICK ();	//*
      else if (commande == "getoch")
	myInteractifFederate->callGetObjClassHandle ();
      else if (commande == "getocn")
	myInteractifFederate->callGetObjClassName ();
      else if (commande == "getah")
	myInteractifFederate->callGetAttrHandle ();
      else if (commande == "getahn")
	myInteractifFederate->callGetAttrHandleName ();
      else if (commande == "getich")
	myInteractifFederate->callGetIntClassHandle ();
      else if (commande == "geticn")
	myInteractifFederate->callGetIntClassName ();
      else if (commande == "getph")
	myInteractifFederate->callGetParaHandle ();
      else if (commande == "getphn")
	myInteractifFederate->callGetParaHandleName ();
      else if (commande == "getoih")
	myInteractifFederate->callGetObjInstanceHandle ();
      else if (commande == "getoihn")
	myInteractifFederate->callGetObjInstanceHandleName ();
      else if (commande == "amb")
	myInteractifFederate->callambF ();
      else if (commande == "cfe")
	myInteractifFederate->callCFE ();
      else if (commande == "dfe")
	myInteractifFederate->callDFE ();
      else if (commande == "jfe")
	myInteractifFederate->callJFE (argv[1]);
      else if (commande == "rfe")
	myInteractifFederate->callRFE ();
      else if (commande == "poca")
	myInteractifFederate->callPOCA ();
      else if (commande == "uoc")
	myInteractifFederate->callUOC ();
      else if (commande == "uoca")
	myInteractifFederate->callUOCA ();
      else if (commande == "pic")
	myInteractifFederate->callPIC ();
      else if (commande == "uic")
	myInteractifFederate->callUIC ();
      else if (commande == "soca")
	myInteractifFederate->callSOCA ();
      else if (commande == "usuboc")
	myInteractifFederate->callUsubOC ();
      else if (commande == "usuboca")
	myInteractifFederate->callUsubOCA ();
      else if (commande == "sic")
	myInteractifFederate->callSIC ();
      else if (commande == "usubic")
	myInteractifFederate->callUsubIC ();
      else if (commande == "roi")
	myInteractifFederate->callROI ();
      else if (commande == "roiwn")
	myInteractifFederate->callROIWN (argv[1]);	
      else if (commande == "uav")
	myInteractifFederate->callUAV ();
      else if (commande == "si")
	myInteractifFederate->callSI ();
      else if (commande == "etr")
	myInteractifFederate->callETR ();
      else if (commande == "etc")
	myInteractifFederate->callETC ();
      else if (commande == "tar")
	myInteractifFederate->callTAR ();
      //else if (commande=="tara") myInteractifFederate->callTARA();
      else if (commande == "nmr")
	myInteractifFederate->callNMR ();
      //else if (commande=="nera") myInteractifFederate->callNERA();
      //else if (commande=="ead") myInteractifFederate->callEAD();
      //else if (commande=="dad") myInteractifFederate->callDAD();
      //else if (commande=="qlbts") myInteractifFederate->callQLBTS();
      //else if (commande=="qmnet") myInteractifFederate->callQMNET();
      //else if (commande=="ml") myInteractifFederate->callML();
      //else if (commande=="ql") myInteractifFederate->callQL();
      //else if (commande=="ciot") myInteractifFederate->callCIOT();
      else
	cout << " commande inconnue " << endl;
    }
  exit (1);
}

//Menu d'aide
void
print_menu (void)
{
  bool menu = true;
  while (menu)
    {
      int choix;
      cout << "\t0- Quitter" << endl;
      cout << "\t1- Commandes utilitaires" << endl;
      cout << "\t2- Federation Management" << endl;
      cout << "\t3- Declaration Management" << endl;
      cout << "\t4- Object Management" << endl;
      cout << "\t5- Time Management\n" << endl;
      cout << "votre choix : ";
      if (!(cin >> choix))
	{
	  cin.clear ();
	  cin.ignore ();
	  choix = 10;
	}
      cout << "\n\n-------------------------------------\n";

      switch (choix)
	{
	case 0:
	  menu = false;
	  break;

	case 1:
	  cout << "\t1- Commandes utilitaires\n" << endl;
	  cout << "t       => Evoke Callback (tick)" << endl;
	  cout << "getoch  => Get Object Class Handle" << endl;
	  cout << "getocn  => Get Object Class Name" << endl;
	  cout << "getah   => Get Attribute Handle" << endl;
	  cout << "getahn  => Get Attribute Handle Name" << endl;
	  cout << "getich  => Get Interaction Class Handle" << endl;
	  cout << "geticn  => Get Interaction Class Name" << endl;
	  cout << "getph   => Get Parameter Handle" << endl;
	  cout << "getphn  => Get Parameter Handle Name" << endl;
	  cout << "getoih  => Get Object Instance Handle" << endl;
	  cout << "getoihn => Get Object Instance Handle Name\n\n" << endl;
	  break;

	case 2:
	  cout << "\t2- Federation Management\n" << endl;
	  cout << "amb => create ambassador" << endl;
	  cout << "cfe => create federation execution" << endl;
	  cout << "dfe => destroy federation execution" << endl;
	  cout << "jfe => join federation execution" << endl;
	  cout << "rfe => resign federation execution\n\n" << endl;
	  break;

	case 3:
	  cout << "\t3- Declaration Management\n" << endl;
	  cout << "poca    => Publish Object Class Attributes" << endl;
	  cout << "uoc     => Unpublish Object Class" << endl;
	  cout << "uoca    => Unpublish Object Class Attributes" << endl;
	  cout << "pic     => Publish Interaction Class" << endl;
	  cout << "usuboc  => Unsubscribe Object Class" << endl;
	  cout << "usuboca => Unsubscribe Object Class Attributes" << endl;
	  cout << "usubic  => Unsubscribe Interaction Class" << endl;
	  cout << "sic     => Suscribe Interaction Class\n\n" << endl;
	  break;

	case 4:
	  cout << "\t4- Object Management\n" << endl;
	  cout << "si  => Send Interaction" << endl;
	  cout << "roi => Register Object Instance" << endl;
	  cout << "roiwn => Register Object Instance with a name" << endl;	  
	  cout << "uav => Update Attribute Values\n\n" << endl;
	  break;

	case 5:
	  cout << "\t5- Time Management\n" << endl;
	  cout << "etc => Enable Time Constrained" << endl;
	  cout << "etr => Enable Time Regulator" << endl;
	  cout << "tar => Time Advance Request" << endl;
	  cout << "ner => Next Event Request" << endl;
	  cout << ".. => ..." << endl;
	  break;

	default:
	  cout << "\tEntrée invalide\n" << endl;
	  break;


	}			//fin switch
    }				//fin while
}				//fin print_menu


/*************************************************************************************************
********************************** COMMANDES UTILITAIRE ******************************************
*************************************************************************************************/

//evokeCallback (TICK)
void
Federe_Interactif::callTICK ()
{

  //wcout << endl << "t=" << tps->toString() << " : -----> tick" << endl;
  wcout << endl << "t=" << this->theTime.
    toString () << " : -----> tick" << endl;
  rtiAmb->evokeCallback (0.1);
  //rtiAmb->evokeMultipleCallbacks(0.0, 1.0);
}

//getObjectClassHandle
void
Federe_Interactif::callGetObjClassHandle ()
{
  cout << "=>Get Object Class Handle(s)" << endl;

  try
  {
    TestObjectClassHandle = rtiAmb->getObjectClassHandle (L"TestObjectClass");

    if (TestObjectClassHandle.isValid ())
      std::cout << "* Got class handle\n";
    else
      std::cout << "* Fail: Got invalid class handle\n" << endl;
  }

  catch (rti1516::Exception & e)
  {
    std::wcout << L"Error:" << e.what () << std::endl;
  }

  try
  {
    TestObjectClassRHandle =
      rtiAmb->getObjectClassHandle (L"TestObjectClassR");

    if (TestObjectClassRHandle.isValid ())
      std::cout << "* Got class handle\n" << endl;
    else
      std::cout << "* Fail: Got invalid class handle\n" << endl;
  }

  catch (rti1516::Exception & e)
  {
    std::wcout << L"Error:" << e.what () << std::endl;
  }
}

//getObjectClassName
void
Federe_Interactif::callGetObjClassName ()
{
  cout << "=>Get Object Class Name(s)" << endl;

  try
  {
    std::wstring getClassName (L"TestObjectClass");

    std::wstring returnedClassName =
      rtiAmb->getObjectClassName (TestObjectClassHandle);


    if (getClassName != returnedClassName)
      {
	std::wcerr <<
	  L"* Fail: Returned classname does not match expected classname" <<
	  std::endl;
      }
    else
      {
	std::wcerr << L"* Got expected classname : ";
	std::wcerr << returnedClassName << endl;
      }
  }
  catch (rti1516::Exception & e)
  {
    std::cout << "* Erreur" << std::endl;
  }

  try
  {
    std::wstring getClassName (L"TestObjectClassR");

    std::wstring returnedClassName =
      rtiAmb->getObjectClassName (TestObjectClassRHandle);


    if (getClassName != returnedClassName)
      {
	std::wcerr <<
	  L"* Fail: Returned classname does not match expected classname" <<
	  std::endl;
      }
    else
      {
	std::wcerr << L"* Got expected classname : ";
	std::wcerr << returnedClassName << endl;
      }
  }
  catch (rti1516::Exception & e)
  {
    std::cout << "* Erreur" << std::endl;
  }
}

//getAttributeHandle
void
Federe_Interactif::callGetAttrHandle ()
{
  cout << "=>getAttributeHandle(s)" << endl;
  try
  {
    DataHandle = rtiAmb->getAttributeHandle (TestObjectClassHandle, L"Data");

    if (DataHandle.isValid ())
      {
	cout << "* Got attribute handle" << endl;
      }

    else
      {
	cout << "* Fail: Got invalid class handle\n" << endl;
      }
  }
  catch (rti1516::Exception & e)
  {
    std::cout << "* Erreur" << std::endl;
  }

  try
  {
    DataRHandle =
      rtiAmb->getAttributeHandle (TestObjectClassRHandle, L"DataR");

    if (DataHandle.isValid ())
      {
	cout << "* Got attribute handle\n" << endl;
      }

    else
      {
	cout << "* Fail: Got invalid class handle\n" << endl;
      }
  }
  catch (rti1516::Exception & e)
  {
    std::cout << "* Erreur" << std::endl;
  }
}

//getAttributeHandleName
void
Federe_Interactif::callGetAttrHandleName ()
{
  cout << "=>getAttributeHandleName(s)" << endl;
  try
  {
    std::wstring orgAttrName (L"Data");

    std::wstring getattrName =
      rtiAmb->getAttributeName (TestObjectClassHandle, DataHandle);

    if (orgAttrName == getattrName)
      {
	cout << "* Got attribute handle name : ";
	std::wcerr << getattrName << endl;
      }

    else
      {
	cout << "* Got invalib attribute handle name" << endl;
      }
  }
  catch (rti1516::Exception & e)
  {
    std::cout << "Erreur" << std::endl;
  }

  try
  {
    std::wstring orgAttrName (L"DataR");

    std::wstring getattrName =
      rtiAmb->getAttributeName (TestObjectClassRHandle, DataRHandle);

    if (orgAttrName == getattrName)
      {
	cout << "* Got attribute handle name : ";
	std::wcerr << getattrName << endl;
      }

    else
      {
	cout << "* Got invalib attribute handle name" << endl;
      }
  }
  catch (rti1516::Exception & e)
  {
    std::cout << "Erreur" << std::endl;
  }
}

//getInteractionClassHandle
void
Federe_Interactif::callGetIntClassHandle ()
{
  cout << "=>getInteractionClassHandle(s)" << endl;
  try
  {
    MsgHandle = rtiAmb->getInteractionClassHandle (L"Msg");

    if (MsgHandle.isValid ())
      {
	cout << "* Got interaction class handle\n" << endl;
      }

    else
      {
	cout << "* Got invalid interaction class handle\n" << endl;
      }

  }
  catch (rti1516::Exception & e)
  {
    std::cout << "Erreur" << std::endl;
  }
  try
  {
    MsgRHandle = rtiAmb->getInteractionClassHandle (L"MsgR");

    if (MsgRHandle.isValid ())
      {
	cout << "* Got interaction class handle\n" << endl;
      }

    else
      {
	cout << "* Got invalid interaction class handle\n" << endl;
      }

  }
  catch (rti1516::Exception & e)
  {
    std::cout << "Erreur" << std::endl;
  }
}

//getInteractionClassName
void
Federe_Interactif::callGetIntClassName ()
{
  cout << "=>getInteractionClassName(s)" << endl;
  try
  {
    std::wstring orgMsgName (L"Msg");

    std::wstring getMsgName = rtiAmb->getInteractionClassName (MsgHandle);

    if (orgMsgName == getMsgName)
      {
	cout << "* Got interaction class name : ";
	std::wcerr << getMsgName << endl;
      }

    else
      {
	cout << "* Got invalid interaction class name" << endl;
      }

  }
  catch (rti1516::Exception & e)
  {
    std::cout << "* Erreur" << std::endl;
  }

  try
  {
    std::wstring orgMsgName (L"MsgR");

    std::wstring getMsgName = rtiAmb->getInteractionClassName (MsgRHandle);

    if (orgMsgName == getMsgName)
      {
	cout << "* Got interaction class name : ";
	std::wcerr << getMsgName << endl;
      }

    else
      {
	cout << "* Got invalid interaction class name" << endl;
      }
  }
  catch (rti1516::Exception & e)
  {
    std::cout << "* Erreur" << std::endl;
  }
}

//getParameterHandle
void
Federe_Interactif::callGetParaHandle ()
{
  cout << "=>getParameterHandle(s)" << endl;
  try
  {

    MsgDataHandle = rtiAmb->getParameterHandle (MsgHandle, L"MsgData");

    if (MsgDataHandle.isValid ())
      {
	cout << "* Got parameter handle\n" << endl;
      }

    else
      {
	cout << "* Got invalid parameter handle\n" << endl;
      }

  }
  catch (rti1516::Exception & e)
  {
    std::cout << "* Erreur" << std::endl;
  }

  try
  {

    MsgDataRHandle = rtiAmb->getParameterHandle (MsgRHandle, L"MsgDataR");

    if (MsgDataRHandle.isValid ())
      {
	cout << "* Got parameter handle\n" << endl;
      }

    else
      {
	cout << "* Got invalid parameter handle\n" << endl;
      }

  }
  catch (rti1516::Exception & e)
  {
    std::cout << "* Erreur" << std::endl;
  }

}

//getParameterHandleName
void
Federe_Interactif::callGetParaHandleName ()
{
  cout << "=>getParameterHandleName(s)" << endl;
  try
  {
    std::wstring orgParamName (L"MsgData");

    std::wstring getParamName =
      rtiAmb->getParameterName (MsgHandle, MsgDataHandle);

    if (orgParamName == getParamName)
      {
	cout << "* Got parameter handle name : ";
	std::wcerr << getParamName << endl;
      }

    else
      {
	cout << "* Got invalid parameter handle name " << endl;

      }
  }
  catch (rti1516::Exception & e)
  {
    std::cout << "* Erreur" << std::endl;
  }
  try
  {
    std::wstring orgParamName (L"MsgDataR");

    std::wstring getParamName =
      rtiAmb->getParameterName (MsgRHandle, MsgDataRHandle);

    if (orgParamName == getParamName)
      {
	cout << "* Got parameter handle name : ";
	std::wcerr << getParamName << endl;
      }

    else
      {
	cout << "* Got invalid parameter handle name " << endl;

      }
  }
  catch (rti1516::Exception & e)
  {
    std::cout << "* Erreur" << std::endl;
  }

}

//getObjectInstanceHandle
void
Federe_Interactif::callGetObjInstanceHandle ()
{
  cout << "=>getObjectInstanceHandle(s)" << endl;

  std::wstring objName1 (L"TestObject1");
  std::wstring objName2 (L"TestObject2");

  try
  {

	rti1516::ObjectInstanceHandle returnedObjHandle =
	  rtiAmb->getObjectInstanceHandle (objName1);

	if (returnedObjHandle.isValid ())
	  {
	    cout << "* Got object handle with the name " ;
	    std::wcerr << objName1 << endl;
	  }
	else
	  {
	    cout << "* Got invalid object handle " << endl;
	  }

  }

  catch (rti1516::Exception & e)
  {
    std::cout << "* Erreur" << std::endl;
  }
}

//getObjectInstanceName
void
Federe_Interactif::callGetObjInstanceHandleName ()
{
  cout << "=>getObjectInstanceName" << endl;

  std::wstring objName1 (L"TestObject1");
  std::wstring objName2 (L"TestObject2");

  try
  {

	std::wstring returnedObjName =
	  rtiAmb->getObjectInstanceName (TestObjectClassInstanceHandle);

	if (objName1 == returnedObjName)
	  {
	    cout << "* Got object handle name : ";
	    std::wcerr << returnedObjName << endl;
	  }
	else
	  {
	    cout << "* Got invalid object handle name " << endl;
	  }

  }
  catch (rti1516::Exception & e)
  {
    std::cout << "* Erreur" << std::endl;
  }
  
  
 try
  {

	std::wstring returnedObjName =
	  rtiAmb->getObjectInstanceName (TestObjectClassRInstanceHandle);

	if (objName2 == returnedObjName)
	  {
	    cout << "* Got object handle name : ";
	    std::wcerr << returnedObjName << endl;
	  }
	else
	  {
	    cout << "* Got invalid object handle name " << endl;
	  }

  }
  catch (rti1516::Exception & e)
  {
    std::cout << "* Erreur" << std::endl;
  } 
  
}

/*************************************************************************************************
********************************* FEDERARION MANAGEMENT ******************************************
*************************************************************************************************/

//createAmbassador
void
Federe_Interactif::callambF ()
{
  bool test = true;



  std::auto_ptr < rti1516::RTIambassadorFactory >
    rtiAmbFact (new rti1516::RTIambassadorFactory ());
  std::vector < std::wstring > constructorArgs;

  cout << "=>create RTI Ambassador" << endl;
  try
  {
    std::auto_ptr < rti1516::RTIambassador >
      rtiAmbP (rtiAmbFact->createRTIambassador (constructorArgs));
    rtiAmb = rtiAmbP.release ();
  }
  catch (rti1516::RTIinternalError & e)
  {
    test = false;
  }
  catch (rti1516::Exception & e)
  {
    test = false;
  }

  if (test)
    {
      cout << "* Ambassador created" << endl << endl;
    }
  else
    {
      cout << "\t->createAmbassador" << endl;
      std::cout << "* Error creating ambassador" << std::endl;
    }
}

//createFederationExecution
void
Federe_Interactif::callCFE ()
{
  cout << "=>createFederationExecution" << endl;

  bool test = true;

  try
  {
    rtiAmb->createFederationExecution (L"Federation_Interactive",
				       L"Certi-Test-02.xml");
  }

  catch (rti1516::Exception & e)
  {
    test = false;
    std::wcerr << L"* Error : " << e.what () << std::endl;
  }

  if (test)
    std::cout << "* Federation creee\n" << std::endl;
  else
    std::cout << "* Federation non creee\n" << std::endl;
}

//deleteFederationExecution
void
Federe_Interactif::callDFE ()
{
  cout << "=>deleteFederationExecution" << endl;
  int test = 0;
  try
  {
    rtiAmb->destroyFederationExecution (L"Federation_Interactive");
  }
  catch (rti1516::FederationExecutionDoesNotExist & e)
  {
    test = 1;
  }
  catch (rti1516::Exception & e)
  {
    test = 2;
  }


  if (test == 0)
    cout << "* Federation detruite\n" << endl;
  else if (test == 1)
    cout << "* No federation to destroy\n" << endl;
  else
    cout << "* Error destroying federation\n" << endl;
}

// joinFederationExecution
void
Federe_Interactif::callJFE (char *name)
{
  cout << "=>joinFederationExecution" << endl;
  bool test = true;

  // char => wstring
  std::wstring wname (name, name + strlen (name));

  try
  {
    rtiAmb->joinFederationExecution (wname, L"Federation_Interactive",
				     *myInteractifFederate);
  }
  catch (rti1516::Exception & e)
  {
    test = false;
  }

  if (test)
    std::cout << "* Federation rejointe\n" << std::endl;
  else
    std::cout << "* Federation non rejointe\n" << std::endl;

}

// resignFederationExecution
void
Federe_Interactif::callRFE ()
{
  cout << "=>resignFederationExecution" << endl;
  bool test = true;
  try
  {
    rtiAmb->
      resignFederationExecution (rti1516::CANCEL_THEN_DELETE_THEN_DIVEST);
  }
  catch (rti1516::Exception & e)
  {
    test = false;
  }

  if (test)
    std::cout << "* federation quittee\n" << std::endl;
  else
    std::cout << "* federation non quittee\n" << std::endl;

}

/*************************************************************************************************
********************************* DECLARATION MANAGEMENT *****************************************
*************************************************************************************************/

// publishObjectClassAttributes
void
Federe_Interactif::callPOCA ()
{
  cout << "=>publishObjectClassAttributes" << endl;
  bool test = true;

  rti1516::AttributeHandleSet classAttributes;
  classAttributes.insert (DataHandle);

  try
  {
    rtiAmb->publishObjectClassAttributes (TestObjectClassHandle,
					  classAttributes);
  }

  catch (rti1516::Exception & e)
  {
    test = false;
  }

  rti1516::AttributeHandleSet classRAttributes;
  classRAttributes.insert (DataRHandle);

  try
  {
    rtiAmb->publishObjectClassAttributes (TestObjectClassRHandle,
					  classRAttributes);
  }

  catch (rti1516::Exception & e)
  {
    test = false;
  }

  if (test)
    std::cout << "* Object classes attributes published\n" << std::endl;
  else
    std::cout << "* can't publish object class attributes\n" << std::endl;
}

//unpublishObjectClass
void
Federe_Interactif::callUOC ()
{
  cout << "=>unpublishObjectClass" << endl;
  bool test = true;

  rti1516::ObjectClassHandle classHandle =
    rtiAmb->getObjectClassHandle (L"TestObjectClass");
  cout << "\t-> getObjectClassHandle\n" << endl;

  try
  {
    rtiAmb->unpublishObjectClass (classHandle);
  }
  catch (rti1516::Exception & e)
  {
    test = false;
  }

  if (test)
    std::cout << "* Object class unpublished\n" << std::endl;
  else
    std::cout << "* can't unpublish object class\n" << std::endl;
}

//unpublishObjectClassAttributes
//PB A REVOIR !!!
void
Federe_Interactif::callUOCA ()
{
  cout << "=>unpublishObjectClassAttributes" << endl;
  bool test = true;

  rti1516::ObjectClassHandle classHandle =
    rtiAmb->getObjectClassHandle (L"TestObjectClass");
  cout << "\t-> getObjectClassHandle" << endl;
  rti1516::AttributeHandle attrHandle =
    rtiAmb->getAttributeHandle (classHandle, L"Data");
  cout << "\t-> getAttributeHandle\n" << endl;
  rti1516::AttributeHandleSet classAttributes;
  classAttributes.insert (attrHandle);

  try
  {
    rtiAmb->unpublishObjectClassAttributes (classHandle, classAttributes);
  }
  catch (rti1516::Exception & e)
  {
    test = false;
  }

  if (test)
    std::cout << "* Object class attributes unpublished\n" << std::endl;
  else
    std::cout << "* can't unpublish object class attributes\n" << std::endl;
}

// publishInteractionClass
void
Federe_Interactif::callPIC ()
{
  cout << "=>publishInteractionClass(s)" << endl;
  bool test = true;
  try
  {
    rtiAmb->publishInteractionClass (MsgHandle);
  }

  catch (rti1516::Exception & e)
  {
    test = false;
  }
  try
  {
    rtiAmb->publishInteractionClass (MsgRHandle);
  }

  catch (rti1516::Exception & e)
  {
    test = false;
  }

  if (test)
    std::cout << "* interactions publiees\n" << std::endl;
  else
    std::cout << "* interaction non publiee\n" << std::endl;
}

// unpublishInteractionClass
void
Federe_Interactif::callUIC ()
{
  cout << "=>unpublishInteractionClass" << endl;
  bool test = true;
  rti1516::InteractionClassHandle msgHandle =
    rtiAmb->getInteractionClassHandle (L"Msg");
  cout << "\t-> getInteractionClassHandle\n" << endl;

  try
  {
    rtiAmb->unpublishInteractionClass (msgHandle);
  }
  catch (rti1516::Exception & e)
  {
    test = false;
  }

  if (test)
    std::cout << "* Interaction class unpublished" << std::endl;
  else
    std::cout << "* can't unpublish interaction class" << std::endl;
}

// subscribeObjectClassAttributes
void
Federe_Interactif::callSOCA ()
{
  cout << "=>subscribeObjectClassAttributes" << endl;
  bool test = true;

  rti1516::AttributeHandleSet classAttributes;

  classAttributes.insert (DataHandle);

  try
  {
    rtiAmb->subscribeObjectClassAttributes (TestObjectClassHandle,
					    classAttributes);
  }
  catch (rti1516::Exception & e)
  {
    test = false;
  }

  rti1516::AttributeHandleSet classRAttributes;

  classRAttributes.insert (DataHandle);

  try
  {
    rtiAmb->subscribeObjectClassAttributes (TestObjectClassRHandle,
					    classRAttributes);
  }
  catch (rti1516::Exception & e)
  {
    test = false;
  }

  if (test)
    std::cout << "* Object class attributes subscribed\n" << std::endl;
  else
    std::cout << "* can't subscribe object class attributes\n" << std::endl;
}

// unsubscribeObjectClass
void
Federe_Interactif::callUsubOC ()
{
  cout << "=>unsubscribeObjectClass" << endl;
  bool test = true;

  rti1516::ObjectClassHandle classHandle =
    rtiAmb->getObjectClassHandle (L"TestObjectClass");
  cout << "\t-> getObjectClassHandle\n" << endl;

  try
  {
    rtiAmb->unsubscribeObjectClass (classHandle);
  }
  catch (rti1516::Exception & e)
  {
    test = false;
  }

  if (test)
    std::cout << "* Object class unsubscribed\n" << std::endl;
  else
    std::cout << "* can't unsubscribe object class\n" << std::endl;
}

// unsubscribeObjectClassAttributes
void
Federe_Interactif::callUsubOCA ()
{
  cout << "=>unsubscribeObjectClassAttributes" << endl;
  bool test = true;

  rti1516::AttributeHandleSet classAttributes;
  rti1516::ObjectClassHandle classHandle =
    rtiAmb->getObjectClassHandle (L"TestObjectClass");
  cout << "\t-> getObjectClassHandle" << endl;
  rti1516::AttributeHandle attrHandle =
    rtiAmb->getAttributeHandle (classHandle, L"Data");
  cout << "\t-> getObjectClassHandle\n" << endl;
  classAttributes.insert (attrHandle);

  try
  {
    rtiAmb->unsubscribeObjectClassAttributes (classHandle, classAttributes);
  }
  catch (rti1516::Exception & e)
  {
    test = false;
  }

  if (test)
    std::cout << "* object class attributes unsubscribed\n" << std::endl;
  else
    std::cout << "* can't unsubscribe object class attributes\n" << std::endl;
}

// subscribeInteractionClass
void
Federe_Interactif::callSIC ()
{
  cout << "=>subscribeInteractionClass(s)" << endl;
  bool test = true;

  try
  {
    rtiAmb->subscribeInteractionClass (MsgHandle);
  }

  catch (rti1516::Exception & e)
  {
    test = false;
  }

  try
  {
    rtiAmb->subscribeInteractionClass (MsgRHandle);
  }

  catch (rti1516::Exception & e)
  {
    test = false;
  }

  if (test)
    std::cout << "* interactions souscrites\n" << std::endl;
  else
    std::cout << "* interaction non souscrite\n" << std::endl;
}

// unsubscribeInteractionClass
void
Federe_Interactif::callUsubIC ()
{
  cout << "=>unsubscribeInteractionClass" << endl;
  bool test = true;

  rti1516::InteractionClassHandle msgHandle =
    rtiAmb->getInteractionClassHandle (L"Msg");
  cout << "\t-> getInteractionClassHandle\n" << endl;

  try
  {
    rtiAmb->unsubscribeInteractionClass (msgHandle);
  }
  catch (rti1516::Exception & e)
  {
    test = false;
  }

  if (test)
    std::cout << "* Interaction class unsubscribed\n" << std::endl;
  else
    std::cout << "* can't unsubscribe interaction class\n" << std::endl;
}

/*************************************************************************************************
************************************ OBJECT MANAGEMENT *******************************************
*************************************************************************************************/

// registerObjectInstance
void
Federe_Interactif::callROI ()
{
  cout << "=>registerObjectInstance(s)" << endl;

  try
  {
    TestObjectClassInstanceHandle =
      rtiAmb->registerObjectInstance (TestObjectClassHandle);
      // the second parameter is optional

    if (TestObjectClassInstanceHandle.isValid ())
      {
	std::cout << "* Object instance registered\n" << std::endl;
      }
    else
      {
	std::cout << "* can't register object instance\n" << std::endl;
      }
  }
  catch (rti1516::Exception & e)
  {
    std::cout << "* error\n" << std::endl;
  }

  try
  {
    TestObjectClassRInstanceHandle =
      rtiAmb->registerObjectInstance (TestObjectClassRHandle);

    if (TestObjectClassRInstanceHandle.isValid ())
      {
	std::cout << "* Object instance registered\n" << std::endl;
      }
    else
      {
	std::cout << "* can't register object instance\n" << std::endl;
      }
  }
  catch (rti1516::Exception & e)
  {
    std::cout << "* error\n" << std::endl;
  }

}

// registerObjectInstance with a name
void
Federe_Interactif::callROIWN (char *name)
{
  cout << "=>registerObjectInstance(s)" << endl;
  
  std::wstringstream stream1;
  stream1 << L"TestObject1_" << name;
  std::wstring objName1 = stream1.str(); 
  
  std::wstringstream stream2;
  stream2 << L"TestObject2_" << name;
  std::wstring objName2 = stream2.str();  

  try
  {
	  
    TestObjectClassInstanceHandle =
      rtiAmb->registerObjectInstance (TestObjectClassHandle, objName1);
      // the second parameter is optional

    if (TestObjectClassInstanceHandle.isValid ())
      {
	std::cout << "* Object instance registered with a name" ;
    std::wcout << objName1 << endl;
      }
    else
      {
	std::cout << "* can't register object instance\n" << std::endl;
      }
  }
  catch (rti1516::Exception & e)
  {
    std::cout << "* error\n" << std::endl;
  }

  try
  {
    TestObjectClassRInstanceHandle =
      rtiAmb->registerObjectInstance (TestObjectClassRHandle, objName2);

    if (TestObjectClassRInstanceHandle.isValid ())
      {
	std::cout << "* Object instance registered with a name" ;
    std::wcout << objName2 << endl;	
      }
    else
      {
	std::cout << "* can't register object instance\n" << std::endl;
      }
  }
  catch (rti1516::Exception & e)
  {
    std::cout << "* error\n" << std::endl;
  }
}

// updateAttributeValues
void
Federe_Interactif::callUAV ()
{
  cout << "=>updateAttributeValues" << endl;
  char choix;
  double d = 1.0;
  double t;

  rti1516::AttributeHandleValueMap attributeValues;
  rti1516::AttributeHandleValueMap attributeRValues;
  rti1516::VariableLengthData theUserSuppliedTag;

  std::string testString ("test uav");
  //rti1516::VariableLengthData varData (testString.c_str (),
  //                                   testString.size () + 1);

  wcout << "Donner la valeur de la donnee (double) : ";
  cin >> d;
  rti1516::VariableLengthData varData (&d, sizeof (d));

  theUserSuppliedTag.setData (testString.c_str (), testString.size () + 1);

  while (1)
    {
      cout << "Avec gestion du temps ?[y/n]" << endl;
      cin >> choix;
      if (choix == 'y' || choix == 'n')
	break;
      cout << "Vous devez entrer 'y' ou 'n'" << endl;
    }

  if (choix == 'y')
    {

      try
      {
	wcout << endl << "t=" << tps->toString () <<
	  " : Donner la valeur de l'estampille voulue : ";
	cin >> t;
	RTI1516fedTime fedTime (t);

	attributeValues.insert (std::pair < rti1516::AttributeHandle,
				rti1516::VariableLengthData > (DataHandle,
							       varData));

	rtiAmb->updateAttributeValues (TestObjectClassInstanceHandle,
				       attributeValues, theUserSuppliedTag,
				       fedTime);
	cout << "* Attribute values updated (with time)" << endl;

      }
      catch (rti1516::Exception & e)
      {
	std::cout << "* error\n" << std::endl;
      }
    }

  else
    {
      attributeRValues.insert (std::pair < rti1516::AttributeHandle,
			       rti1516::VariableLengthData > (DataRHandle,
							      varData));

      try
      {
	rtiAmb->updateAttributeValues (TestObjectClassRInstanceHandle,
				       attributeRValues, theUserSuppliedTag);
	cout << "* Attribute values updated\n" << endl;
      }
      catch (rti1516::Exception & e)
      {
	std::cout << "* error\n" << std::endl;
      }
    }
}


// sendInteraction
void
Federe_Interactif::callSI ()
{
  cout << "=>sendInteraction" << endl;

  double d;
  bool test = true;
  char choix;
  rti1516::ParameterHandleValueMap phvm;
  rti1516::VariableLengthData tag;

  std::string testString ("test string");
  rti1516::VariableLengthData paramValue (testString.c_str (),
					  testString.size () + 1);



  std::string testTag ("test tag");
  tag.setData (testTag.c_str (), testTag.size () + 1);

  while (1)
    {
      cout << "Avec gestion du temps ?[y/n]" << endl;
      cin >> choix;
      if (choix == 'y' || choix == 'n')
	break;
      cout << "Vous devez entrer 'y' ou 'n'" << endl;
    }

  if (choix == 'y')		//SI avec estampille temporelle
    {
      wcout << endl << "t=" << tps->toString () <<
	" : Donner la valeur de l'estampille voulue : ";
      cin >> d;

      RTI1516fedTime fedTime (d);

      //phvm[MsgDataHandle] = paramValue;
      phvm.insert (std::pair < rti1516::ParameterHandle,
		   rti1516::VariableLengthData > (MsgDataHandle, paramValue));

      try
      {
	rtiAmb->sendInteraction (MsgHandle, phvm, tag, fedTime);
      }

      catch (rti1516::Exception & e)
      {
	test = false;
      }

      if (test)
	cout << "* interaction envoyee (estampillee " << d << ")\n" << endl;
      else
	cout << "* interaction non envoyee\n" << endl;

    }

  else				//SI sans estampille temporelle
    {
      //phvm[MsgDataRHandle] = paramValue;
      phvm.insert (std::pair < rti1516::ParameterHandle,
		   rti1516::VariableLengthData > (MsgDataRHandle,
						  paramValue));

      try
      {
	rtiAmb->sendInteraction (MsgRHandle, phvm, tag);
      }

      catch (rti1516::Exception & e)
      {
	test = false;
      }

      if (test)
	cout << "* interaction envoyee\n " << endl;
      else
	cout << "* interaction non envoyee\n" << endl;
    }

}

/*************************************************************************************************
************************************* TIME MANAGEMENT ********************************************
*************************************************************************************************/

// enableTimeRegulation
void
Federe_Interactif::callETR ()
{
  cout << "=>enableTimeRegulation" << endl;
  try
  {
    rtiAmb->enableTimeRegulation (lookahead);
  }
  catch (rti1516::Exception & e)
  {
    cout << "erreur" << endl;
  }
  while (!timeRegulation)
    {
      rtiAmb->evokeCallback (0.1);
    }
  cout << "* federe regulateur\n" << endl;
}

// enableTimeConstrained
void
Federe_Interactif::callETC ()
{
  cout << "=>enableTimeConstrained" << endl;
  try
  {
    rtiAmb->enableTimeConstrained ();
  }
  catch (rti1516::Exception & e)
  {
    cout << "erreur" << endl;
  }
  while (!timeConstrained)
    {
      rtiAmb->evokeCallback (0.1);

    }
  cout << "* federe contraint\n" << endl;
}

// timeAdvanceRequest
void
Federe_Interactif::callTAR ()
{
  float d;
  bool test = true;
  wcout << endl << "t=" << tps->toString () <<
    " : Donner la date a laquelle vous souhaitez avancer : ";
  cin >> d;
  RTI1516fedTime fedTime (d);
  try
  {
    rtiAmb->timeAdvanceRequest (fedTime);
  }
  catch (rti1516::Exception & e)
  {
    test = false;
  }

  if (test)
    wcout << endl << "t=" << tps->toString () << " timeAdvanceRequest(" << d
      << ")" << endl;
  else
    cout << "timeAdvanceRequest a echoue" << endl;
}

/*// timeAdvanceRequestAvailable
void callTARA (void) {
    float d;
    int test = 1;
    cout << endl << "t=" << temps->getTime() << " : Donner la date a laquelle vous souhaitez avancer : ";
    cin >> d;
    try {
       myFedere_Interactif->timeAdvanceRequestAvailable(*(new RTIfedTime(d)));
    }
    catch (rti1516::Exception& e) {
        test = 0;
        cout << "catch " << e._name << " reason " << e._reason << endl;
    }
    if (test)
       cout << endl << "t=" << temps->getTime() << " timeAdvanceRequestAvailable(" << d << ")" << endl;
    else
       cout << "timeAdvanceRequestAvailable a echoue" << endl;
}*/

// nextEventRequest
void
Federe_Interactif::callNMR (void)
{
  float d;
  bool test = true;
  wcout << endl << "t=" << tps->
    toString () << " : Donner la date a laquelle vous souhaitez avancer : ";
  cin >> d;
  RTI1516fedTime fedTime (d);

  try
  {
    rtiAmb->nextMessageRequest (fedTime);
  }
  catch (rti1516::Exception & e)
  {
    test = false;
  }
  if (test)
    wcout << endl << "t=" << tps->
      toString () << " nextMessageRequest(" << d << ")" << endl;
  else
    wcout << "nextMessageRequest a echoue" << endl;
}

/*// nextEventRequestAvailable
void callNERA (void)
{
    float d;
    int test = 1;
    cout << endl << "t=" << temps->getTime() << " : Donner la date a laquelle vous souhaitez avancer : ";
    cin >> d;
    try {
       myFedere_Interactif->nextEventRequestAvailable(*(new RTIfedTime(d)));
    }
    catch (rti1516::Exception& e) {
        test = 0;
        cout << "catch " << e._name << " reason " << e._reason << endl;
    }
    if (test)
       cout << endl << "t=" << temps->getTime() << " nextEventRequestAvailable(" << d << ")" << endl;
    else
       cout << "nextEventRequestAvailable a echoue" << endl;
}*/

/*// enableAsynchronousDelivery
void callEAD (void)
{
    int test = 1;
    try {
       myFedere_Interactif->enableAsynchronousDelivery();
    }
    catch (rti1516::Exception& e) {
        test = 0;
        cout << "catch " << e._name << " reason " << e._reason << endl;
    }
    if (test)
       cout << "asynchronous delivery enabled" << endl;
    else
       cout << "enableAsynchronousDelivery has failed" << endl;
}*/

/*// disableAsynchronousDelivery
void callDAD (void)
{
    int test = 1;
    try {
       myFedere_Interactif->disableAsynchronousDelivery();
    }
    catch (rti1516::Exception& e) {
        test = 0;
        cout << "catch " << e._name << " reason " << e._reason << endl;
    }
    if (test)
       cout << "asynchronous delivery disabled" << endl;
    else
       cout << "disableAsynchronousDelivery has failed" << endl;
}*/

/*// queryLBTS
void callQLBTS (void)
{
    int test = 1;
    try {
       myFedere_Interactif->queryLBTS(ft);
    }
    catch (rti1516::Exception& e) {
        test = 0;
        cout << "catch " << e._name << " reason " << e._reason << endl;
    }
    if (test)
       cout << endl << "LBTS=" << ft.getTime() << endl;
    else
       cout << "queryLBTS has failed" << endl;
}*/

/*// queryMinNextEventTime
void callQMNET (void)
{
    int test = 1;
    try {
       myFedere_Interactif->queryMinNextEventTime(ft);
    }
    catch (rti1516::Exception& e) {
        test = 0;
        cout << "catch " << e._name << " reason " << e._reason << endl;
    }
    if (test)
       cout << endl << "MinNextEventTime=" << ft.getTime() << endl;
    else
       cout << "queryMinNextEventTime has failed" << endl;
}*/

/*// modifyLookahead
void callML (void)
{
    float d;
    int test = 1;
    cout << endl << "t=" << temps->getTime() << " : Donner la valeur du lookahead voulu : ";
    cin >> d;
    try {
       myFedere_Interactif->modifyLookahead( *(new RTIfedTime(d)) );
    }
    catch (rti1516::Exception& e) {
        test = 0;
        cout << "catch " << e._name << " reason " << e._reason << endl;
    }
    if (test)
       cout << endl << "t=" << temps->getTime() << " modifyLookahead(" << d << " )" << endl;
    else
       cout << "modifyLookahead has failed" << endl;
}*/

/*// queryLookahead
void callQL (void)
{
    int test = 1;
    try {
       myFedere_Interactif->queryLookahead(ft);
    }
    catch (rti1516::Exception& e) {
        test = 0;
        cout << "catch " << e._name << " reason " << e._reason << endl;
    }
    if (test)
       cout << endl << "Lookahead=" << ft.getTime() << endl;
    else
       cout << "queryLookahead has failed" << endl;
}*/

/*// changeInteractionOrderType
void Federe_Interactif::callCIOT ()
{
    int test = 1;
    int type ;
    cout << "ordre RECEIVE (1) ou TIMESTAMP (2) ?" << endl ;
    cin >> type ;

    try
    {
    	rtiAmb->changeInteractionOrderType(rtiAmb->getInteractionClassHandle(L"Msg"), type);
    }

    catch (rti1516::Exception& e)
    {
        test = 0;
    }

    if (test)
       cout << endl << "Nouveau type d'ordre pour l'interaction = " << type << endl;
    else
       cout << "changeInteractionOrderType" << endl;
}*/
