#include <RTI.hh>
#include <fedtime.hh>
#include <NullFederateAmbassador.hh>

#include <stdlib.h>
#include <iostream>
#include <memory>
#include <string>
#include <string.h>
using namespace std;


// VARIABLES GLOBALES


RTIfedTime* temps = new RTIfedTime(0.0);
RTIfedTime* lookahead = new RTIfedTime(1.0);
RTIfedTime ft;

bool timeRegulation = false;
bool timeConstrained = false;

class Federe_Interactif : public RTI::RTIambassador, public NullFederateAmbassador
{
public:
  
  RTI::ObjectClassHandle Federation_InteractiveID;
  RTI::AttributeHandle numbID;
  RTI::ObjectHandle objInstID_numb;

  int numb;

  Federe_Interactif() {}
  
  virtual ~Federe_Interactif() throw (RTI::FederateInternalError) {}
  
private:
  // Federation Management

  // Declaration Management
  void startRegistrationForObjectClass(RTI::ObjectClassHandle oca)
      throw (RTI::ObjectClassNotPublished,
         RTI::FederateInternalError)
    {
	  cout << "startRegistrationForObjectClass - ObjectClassHandle = " << oca  << endl;
    }
  
  // Object Management
  void discoverObjectInstance(RTI::ObjectHandle theObject, RTI::ObjectClassHandle theObjectClass, const char *theObjectName)
    throw ( RTI::CouldNotDiscover,
	    RTI::ObjectClassNotKnown,
	    RTI::FederateInternalError) 
   {
	  cout << "discoverObjectInstance -<" << theObjectName << ">- ObjectHandle = "
		   << theObject << " now wait for the other federate updateAttributeValues" << endl;
	  objInstID_numb = theObject;

   } 
   
  void reflectAttributeValues(RTI::ObjectHandle theObject, const RTI::AttributeHandleValuePairSet& theAttributes, const char *theTag)
      throw ( RTI::ObjectNotKnown,
	      RTI::AttributeNotKnown,
	      RTI::FederateOwnsAttributes,
	      RTI::FederateInternalError) 
    {
	  RTI::ULong length;
	  char *attrValue;
  
	  for (unsigned int i=0; i<theAttributes.size(); i++) 
	  {

            RTI::AttributeHandle handle = theAttributes.getHandle(i);
            length = theAttributes.getValueLength(i);
            attrValue = new char[length];
            theAttributes.getValue(i, attrValue, length);
 
	    cout << "reflectAttributeValues, the number = " << *(reinterpret_cast<int*>(attrValue)) << endl;

            delete[] attrValue;
	  }
    }
  void receiveInteraction (
            RTI::InteractionClassHandle       theInteraction,
      const RTI::ParameterHandleValuePairSet& theParameters,
      const RTI::FedTime&                     theTime,
      const char                             *theTag,
            RTI::EventRetractionHandle        theHandle)
    throw (
      RTI::InteractionClassNotKnown,
      RTI::InteractionParameterNotKnown,
      RTI::InvalidFederationTime,
      RTI::FederateInternalError)
    {
      std::cout << std::endl << "t=" << ((RTIfedTime&)theTime).getTime() << " : <----- receiveInteraction(" << ((RTIfedTime&)theTime).getTime() << ")" << theTag << std::endl;
    }

  void receiveInteraction (
            RTI::InteractionClassHandle       theInteraction,
      const RTI::ParameterHandleValuePairSet& theParameters,
      const char                             *theTag)
    throw (
      RTI::InteractionClassNotKnown,
      RTI::InteractionParameterNotKnown,
      RTI::FederateInternalError)
    {
      std::cout << std::endl << "     <----- receiveInteraction" << std::endl;
    }

  void provideAttributeValueUpdate(RTI::ObjectHandle oh, const RTI::AttributeHandleSet &ahs)
     throw (RTI::ObjectNotKnown,
           RTI::AttributeNotKnown,
           RTI::AttributeNotOwned,
           RTI::FederateInternalError)
     {
	   cout << "provideAttributeValueUpdate, ObjectHandle = " << oh << endl;
	   for (unsigned int i=0; i<ahs.size(); i++)
	     {
	       cout << "  attributeHandle = " << ahs.getHandle(i) << endl;
	     }
      }

  // Ownership Management

  // Time Management
  void timeRegulationEnabled(const RTI::FedTime& theTime)
    throw (
      RTI::InvalidFederationTime,
      RTI::EnableTimeRegulationWasNotPending,
      RTI::FederateInternalError)
  {
     std::cout << std::endl << "callback timeRegulationEnabled pour l'instant " <<
               ((RTIfedTime&)theTime).getTime() << std::endl;
     timeRegulation = true;
  }

  void timeConstrainedEnabled(const RTI::FedTime& theTime)
    throw (
      RTI::InvalidFederationTime,
      RTI::EnableTimeConstrainedWasNotPending,
      RTI::FederateInternalError)
  {
     std::cout << std::endl << "callback timeRegulationConstrained pour l'instant " <<
               ((RTIfedTime&)theTime).getTime() << std::endl;
     timeConstrained = true;
  }

 void timeAdvanceGrant (const RTI::FedTime& theTime)
  throw (
    RTI::InvalidFederationTime,
    RTI::TimeAdvanceWasNotInProgress,
    RTI::FederateInternalError)
  {
    *(temps) = theTime;
    std::cout << std::endl << "t=" << ((RTIfedTime&)theTime).getTime() << " : <----- timeAdvanceGrant(" << ((RTIfedTime&)theTime).getTime() << ")"<< std::endl << std::endl;
  }
};

void print_menu (void);
void enter_integer (void);
void callTICK (void);
void callGOCH(void);
void callGAH(void);
void callCFE (void);
void callJFE (char * name);
void callRFE (void);
void callDFE (void);
void callPIC (void);
void callSIC (void);
void callPOC (void);
void callSOCA (void);
void callSI (void);
void callROI (void);
void callUAV (void);
void callRCAVU (void);
void callROAVU (void);
void callETR(void);
void callETC(void);
void callTAR(void);
void callTARA(void);
void callNER(void);
void callNERA(void);
void callEAD(void);
void callDAD(void);
void callQLBTS(void);
void callQMNET(void);
void callML(void);
void callQL(void);
void callCIOT(void);


//VARIABLE
Federe_Interactif* myFedere_Interactif = new Federe_Interactif();

int main(int argc, char **argv)
{

  int i;
  float d;
  char c, entree;
  string commande;

  if (argc!=2)
  {
    cout <<"Vous devez donner en parametre le nom du federe" << endl;
    exit(1);
  }

  while(1)
  {
    cout <<"Voulez-vous un preambule automatique [y/n]" << endl;
    scanf("%c", &c);
    scanf("%c", &entree);
    if (c=='y' || c=='n') break;
    cout <<"Vous devez entrer 'y' ou 'n'" << endl;
  }


  if (c=='y')
  {
    //CREER
    callCFE ();

    //REJOINDRE
    callJFE (argv[1]);

    //PUBLIER INTERACTION
    callPIC ();

    //SOUSCRIRE INTERACTION
    callSIC ();

    //PUBLIER CLASSE D'OBJET
    callGOCH ();
    callGAH ();
    callPOC ();

    // SOUSCRIRE CLASSE D'OBJET
    callSOCA ();

    //DECLARER REGULATEUR ET CONTRAINT
    callETR ();
    callETC ();

    //LIVRAISON ASYNCHRONE DES MESSAGES NON ESTAMPILLES
    callEAD ();
  }

  //MENU PRINCIPAL

  while(1)
  {
      cout << endl <<"Choisissez une action : " << endl;
      cin >> commande;

      if (commande=="q" || commande=="quit") break;
      else if (commande=="h" || commande=="help") print_menu();
      else if (commande=="enter_integer") enter_integer();
      else if (commande=="t") callTICK();
      else if (commande=="goch") callGOCH();
      else if (commande=="gah") callGAH();
      else if (commande=="cfe") callCFE();
      else if (commande=="dfe") callDFE();
      else if (commande=="jfe") callJFE(argv[1]);
      else if (commande=="rfe") callRFE();
      else if (commande=="pic") callPIC();
      else if (commande=="sic") callSIC();
      else if (commande=="poc") callPOC();
      else if (commande=="soca") callSOCA();
      else if (commande=="si") callSI();
      else if (commande=="roi") callROI();
      else if (commande=="uav") callUAV();
      else if (commande=="rcavu") callRCAVU();
      else if (commande=="roavu") callROAVU();
      else if (commande=="etr") callETR();
      else if (commande=="etc") callETC();
      else if (commande=="tar") callTAR();
      else if (commande=="tara") callTARA();
      else if (commande=="ner") callNER();
      else if (commande=="nera") callNERA();
      else if (commande=="ead") callEAD();
      else if (commande=="dad") callDAD();
      else if (commande=="qlbts") callQLBTS();
      else if (commande=="qmnet") callQMNET();
      else if (commande=="ml") callML();
      else if (commande=="ql") callQL();
      else if (commande=="ciot") callCIOT();
      else cout << " commande inconnue " << endl;
  }

  exit(1);
}

void print_menu (void) {
   cout << "liste de commandes utilitaires :" << endl;
   cout << "q enter_integer" << endl;
   cout << "liste de commandes support services :" << endl;
   cout << "t goch gah" << endl;
   cout << "liste de commandes Federation Management :" << endl;
   cout << "cfe dfe jfe rfe" << endl;
   cout << "liste de commandes Declaration Management :" << endl;
   cout << "pic sic poc soca" << endl;
   cout << "liste de commandes Object Management :" << endl;
   cout << "si roi uav rcavu ocavu" << endl;
   cout << "liste de commandes Time Management :" << endl;
   cout << "etr etc tar tara ner nera ead dad qlbts qmnet ml ql ciot" << endl;

}

void enter_integer (void) {
    std::cout  << std::endl << " Enter an integer : ";
    scanf("%d", &(myFedere_Interactif->numb));
}

// SUPPORT SERVICES
//  tick
void callTICK (void) {
    cout << endl << "t=" << temps->getTime() << " : -----> tick" << endl;
    //myFedere_Interactif->tick(0.1, 0.2);
    myFedere_Interactif->tick();
    //myFedere_Interactif->tick2();
}
// getObjectClassHandle
void callGOCH (void) {
    try {
        myFedere_Interactif->Federation_InteractiveID = myFedere_Interactif->getObjectClassHandle("Federation_InteractiveClass");
    } catch ( RTI::Exception &e ) {
        cerr << "RTI exception: " << e._name << " ["
        << (e._reason ? e._reason : "undefined") << "]." << endl;
    } catch ( ... ) {
        cerr << "Error: unknown non-RTI exception." << endl;
    }
    
    std::cout << "ObjectClassHandle = " << myFedere_Interactif->Federation_InteractiveID << std::endl;
}
//  getAttributeHandle
void callGAH (void) {
    try {
        myFedere_Interactif->numbID = myFedere_Interactif->getAttributeHandle("NUMB", myFedere_Interactif->Federation_InteractiveID);
    } catch ( RTI::Exception &e ) {
        cerr << "RTI exception: " << e._name << " ["
        << (e._reason ? e._reason : "undefined") << "]." << endl;
    } catch ( ... ) {
        cerr << "Error: unknown non-RTI exception." << endl;
    }
    std::cout << "AttributeHandle = " << myFedere_Interactif->numbID << std::endl;
}
// FEDERARION MANAGEMENT
// createFederationExecution
void callCFE (void) {
    int test = 1;
    try {
       myFedere_Interactif->createFederationExecution("Federation_Interactive","Interactive_Federation.fed");
    }
    catch (RTI::Exception& e) {
        test = 0;
        std::cout << "catch " << e._name << " reason " << e._reason <<std::endl;
    }
    if (test)
       std::cout << "federation creee" << std::endl;
    else
       std::cout << "federation non creee" << std::endl;
}
// deleteFederationExecution
void callDFE (void) {
    int test = 1;
    try {
       myFedere_Interactif->destroyFederationExecution("Federation_Interactive");
    }
    catch (RTI::Exception& e) {
        test = 0;
        std::cout << "catch " << e._name << " reason " << e._reason <<std::endl;
    }
    if (test)
       std::cout << "federation detruite" << std::endl;
    else
       std::cout << "federation non detruite" << std::endl;
}
// joinFederationExecution
void callJFE (char *name) {
    int test = 1;
    try {
       myFedere_Interactif->joinFederationExecution(name, "Federation_Interactive", myFedere_Interactif);
    }
    catch (RTI::Exception& e) {
        test = 0;
        std::cout << "catch " << e._name << " reason " << e._reason <<std::endl;
    }
    if (test)
       std::cout << "federation rejointe" << std::endl;
    else
       std::cout << "federation non rejointe" << std::endl;
}
// resignFederationExecution
void callRFE (void) {
    int test = 1;
    try {
       myFedere_Interactif->resignFederationExecution(RTI::DELETE_OBJECTS_AND_RELEASE_ATTRIBUTES);
    }
    catch (RTI::Exception& e) {
        test = 0;
        std::cout << "catch " << e._name << " reason " << e._reason <<std::endl;
    }
    if (test)
       std::cout << "federation quittee" << std::endl;
    else
       std::cout << "federation non quittee" << std::endl;
}
// DECLARATION MANAGEMENT
// publishInteractionClass
void callPIC (void) {
    int test = 1;
    try {
       myFedere_Interactif->publishInteractionClass(myFedere_Interactif->getInteractionClassHandle("msg"));
    }
    catch (RTI::Exception& e) {
        test = 0;
        std::cout << "catch " << e._name << " reason " << e._reason <<std::endl;
    }
    if (test)
       std::cout << "interaction publiee" << std::endl;
    else
       std::cout << "interaction non publiee" << std::endl;
}
// subscribeInteractionClass
void callSIC (void) {
    int test = 1;
    try {
       myFedere_Interactif->subscribeInteractionClass(myFedere_Interactif->getInteractionClassHandle("msg"), RTI::RTI_TRUE);
    }
    catch (RTI::Exception& e) {
        test = 0;
        cout << "catch " << e._name << " reason " << e._reason << endl;
    }
    if (test)
       cout << "interaction souscrite" << endl;
    else
       cout << "interaction non souscrite" << endl;
}
// publishObjectClass
void callPOC (void) {
    auto_ptr<RTI::AttributeHandleSet>
    attrnumb(RTI::AttributeHandleSetFactory::create(1));
    attrnumb->add(myFedere_Interactif->numbID);

    try {
        myFedere_Interactif->publishObjectClass(myFedere_Interactif->Federation_InteractiveID, *attrnumb);
    } catch ( RTI::Exception &e ) {
        cerr << "RTI exception: " << e._name << " ["
        << (e._reason ? e._reason : "undefined") << "]." << endl;
    } catch ( ... ) {
        cerr << "Error: unknown non-RTI exception." << endl;
    }
    
    std::cout << "publishObjectClass done, now do suscribeObjectClassAttributes" << std::endl << std::endl;
}
// suscribeObjectClassAttributes
void callSOCA (void) {
    auto_ptr<RTI::AttributeHandleSet>
    attrnumb(RTI::AttributeHandleSetFactory::create(1));
    attrnumb->add(myFedere_Interactif->numbID);
				  
    try {
	myFedere_Interactif->subscribeObjectClassAttributes(myFedere_Interactif->Federation_InteractiveID, *attrnumb);
    } catch ( RTI::Exception &e ) {
	cerr << "RTI exception: " << e._name << " ["
	<< (e._reason ? e._reason : "undefined") << "]." << endl;
    } catch ( ... ) {
	cerr << "Error: unknown non-RTI exception." << endl;
    }
   
    std::cout << "suscribeObjectClassAttributes done, now wait for other federate registerObjectInstance" << std::endl << std::endl;      
}

// OBJECT MANAGEMENT
// sendInteraction
void callSI (void) {
    float d;
    int test = 1;
    cout << endl << "t=" << temps->getTime() << " : Donner la valeur de l'estampille voulue : ";
    cin >> d;
    try {
        myFedere_Interactif->sendInteraction(myFedere_Interactif->getInteractionClassHandle("msg"),
                                             *(RTI::ParameterSetFactory::create(0)),
                                             *(new RTIfedTime(d)),
                                             "msg");
    }
    catch (RTI::Exception& e) {
        test = 0;
        cout << "catch " << e._name << " reason " << e._reason << endl;
    }
    if (test)
       cout << "interaction envoyee (estampillee " << d << ")" << endl;
    else
       cout << "interaction non envoyee" << endl;
}
// registerObjectInstance
void callROI (void) {
    try {
        myFedere_Interactif->objInstID_numb = myFedere_Interactif->registerObjectInstance(myFedere_Interactif->Federation_InteractiveID, "numb");
    } catch ( RTI::Exception &e ) {
        cerr << "RTI exception: " << e._name << " ["
        << (e._reason ? e._reason : "undefined") << "]." << endl;
    } catch ( ... ) {
        cerr << "Error: unknown non-RTI exception." << endl;
    }
    std::cout << "registerObjectInstance done, now do timeAdvanceRequest and tick to receive discoverObjectInstance" << myFedere_Interactif->objInstID_numb << std::endl << std::endl;
}
// updateAttributeValues
void callUAV (void) {
    auto_ptr<RTI::AttributeHandleValuePairSet>
    ahvps(RTI::AttributeSetFactory::create(1));
    ahvps -> add(myFedere_Interactif->numbID, (char*)&(myFedere_Interactif->numb), sizeof(int));

    try {
        myFedere_Interactif->updateAttributeValues(myFedere_Interactif->objInstID_numb, *ahvps, "numb");
    } catch ( RTI::Exception &e ) {
        cerr << "RTI exception: " << e._name << " ["
        << (e._reason ? e._reason : "undefined") << "]." << endl;
    } catch ( ... ) {
        cerr << "Error: unknown non-RTI exception." << endl;
    }

    ahvps -> empty ();
    std::cout << "updateAttributeValues done, now tick to receive reflectAttributeValues and display the number" << std::endl << std::endl;
}
// requestClassAttributeValueUpdate
void callRCAVU (void) {
    auto_ptr<RTI::AttributeHandleSet>
    attrnumb(RTI::AttributeHandleSetFactory::create(1));
    attrnumb->add(myFedere_Interactif->numbID);

    try {
        myFedere_Interactif->requestClassAttributeValueUpdate(myFedere_Interactif->Federation_InteractiveID, *attrnumb);
    } catch ( RTI::Exception &e ) {
        cerr << "RTI exception: " << e._name << " ["
        << (e._reason ? e._reason : "undefined") << "]." << endl;
    } catch ( ... ) {
        cerr << "Error: unknown non-RTI exception." << endl;
    }

    attrnumb -> empty ();
    std::cout << "requestClassAttributeValueUpdate done, now tick to receive provideAttributeValueUpdate" << std::endl << std::endl;
}
// requestObjectAttributeValueUpdate
void callROAVU (void) {
    auto_ptr<RTI::AttributeHandleSet>
    attrnumb(RTI::AttributeHandleSetFactory::create(1));
    attrnumb->add(myFedere_Interactif->numbID);

    try {
        myFedere_Interactif->requestObjectAttributeValueUpdate(myFedere_Interactif->objInstID_numb, *attrnumb);
    } catch ( RTI::Exception &e ) {
        cerr << "RTI exception: " << e._name << " ["
        << (e._reason ? e._reason : "undefined") << "]." << endl;
    } catch ( ... ) {
        cerr << "Error: unknown non-RTI exception." << endl;
    }

    attrnumb -> empty ();
    std::cout << "requestObjectAttributeValueUpdate done, now tick to receive provideAttributeValueUpdate" << std::endl << std::endl;
}
// TIME MANAGEMENT
// enableTimeRegulation
void callETR (void) {
    try {
       myFedere_Interactif->enableTimeRegulation(*temps,*lookahead);
    }
    catch (RTI::Exception& e) {
        cout << "catch " << e._name << " reason " << e._reason << endl;
    }
    while (!timeRegulation)
    {
        myFedere_Interactif->tick(0.1, 0.2);
    }
    cout << "federe regulateur" << endl;
}
// enableTimeConstrained
void callETC (void) {
    try {
       myFedere_Interactif->enableTimeConstrained();
    }
    catch (RTI::Exception& e) {
        cout << "catch " << e._name << " reason " << e._reason << endl;
    }
    while (!timeConstrained)
    {
        myFedere_Interactif->tick(0.1, 0.2);

    }
    cout << "federe contraint" << endl;
}
// timeAdvanceRequest
void callTAR (void) {
    float d;
    int test = 1;
    cout << endl << "t=" << temps->getTime() << " : Donner la date a laquelle vous souhaitez avancer : ";
    cin >> d;
    try {
       myFedere_Interactif->timeAdvanceRequest(*(new RTIfedTime(d)));
    }
    catch (RTI::Exception& e) {
        test = 0;
        cout << "catch " << e._name << " reason " << e._reason << endl;
    }
    if (test)
       cout << endl << "t=" << temps->getTime() << " timeAdvanceRequest(" << d << ")" << endl;
    else
       cout << "timeAdvanceRequest a echoue" << endl;
}
// timeAdvanceRequestAvailable
void callTARA (void) {
    float d;
    int test = 1;
    cout << endl << "t=" << temps->getTime() << " : Donner la date a laquelle vous souhaitez avancer : ";
    cin >> d;
    try {
       myFedere_Interactif->timeAdvanceRequestAvailable(*(new RTIfedTime(d)));
    }
    catch (RTI::Exception& e) {
        test = 0;
        cout << "catch " << e._name << " reason " << e._reason << endl;
    }
    if (test)
       cout << endl << "t=" << temps->getTime() << " timeAdvanceRequestAvailable(" << d << ")" << endl;
    else
       cout << "timeAdvanceRequestAvailable a echoue" << endl;
}
// nextEventRequest
void callNER (void) {
    float d;
    int test = 1;
    cout << endl << "t=" << temps->getTime() << " : Donner la date a laquelle vous souhaitez avancer : ";
    cin >> d;
    try {
       myFedere_Interactif->nextEventRequest(*(new RTIfedTime(d)));
    }
    catch (RTI::Exception& e) {
        test = 0;
        cout << "catch " << e._name << " reason " << e._reason << endl;
    }
    if (test)
       cout << endl << "t=" << temps->getTime() << " nextEventRequest(" << d << ")" << endl;
    else
       cout << "nextEventRequest a echoue" << endl;
}
// nextEventRequestAvailable
void callNERA (void) {
    float d;
    int test = 1;
    cout << endl << "t=" << temps->getTime() << " : Donner la date a laquelle vous souhaitez avancer : ";
    cin >> d;
    try {
       myFedere_Interactif->nextEventRequestAvailable(*(new RTIfedTime(d)));
    }
    catch (RTI::Exception& e) {
        test = 0;
        cout << "catch " << e._name << " reason " << e._reason << endl;
    }
    if (test)
       cout << endl << "t=" << temps->getTime() << " nextEventRequestAvailable(" << d << ")" << endl;
    else
       cout << "nextEventRequestAvailable a echoue" << endl;
}
// enableAsynchronousDelivery
void callEAD (void) {
    int test = 1;
    try {
       myFedere_Interactif->enableAsynchronousDelivery();
    }
    catch (RTI::Exception& e) {
        test = 0;
        cout << "catch " << e._name << " reason " << e._reason << endl;
    }
    if (test)
       cout << "asynchronous delivery enabled" << endl;
    else
       cout << "enableAsynchronousDelivery has failed" << endl;
}
// disableAsynchronousDelivery
void callDAD (void) {
    int test = 1;
    try {
       myFedere_Interactif->disableAsynchronousDelivery();
    }
    catch (RTI::Exception& e) {
        test = 0;
        cout << "catch " << e._name << " reason " << e._reason << endl;
    }
    if (test)
       cout << "asynchronous delivery disabled" << endl;
    else
       cout << "disableAsynchronousDelivery has failed" << endl;
}
// queryLBTS
void callQLBTS (void) {
    int test = 1;
    try {
       myFedere_Interactif->queryLBTS(ft);
    }
    catch (RTI::Exception& e) {
        test = 0;
        cout << "catch " << e._name << " reason " << e._reason << endl;
    }
    if (test)
       cout << endl << "LBTS=" << ft.getTime() << endl;
    else
       cout << "queryLBTS has failed" << endl;
}
// queryMinNextEventTime
void callQMNET (void) {
    int test = 1;
    try {
       myFedere_Interactif->queryMinNextEventTime(ft);
    }
    catch (RTI::Exception& e) {
        test = 0;
        cout << "catch " << e._name << " reason " << e._reason << endl;
    }
    if (test)
       cout << endl << "MinNextEventTime=" << ft.getTime() << endl;
    else
       cout << "queryMinNextEventTime has failed" << endl;
}
// modifyLookahead
void callML (void) {
    float d;
    int test = 1;
    cout << endl << "t=" << temps->getTime() << " : Donner la valeur du lookahead voulu : ";
    cin >> d;
    try {
       myFedere_Interactif->modifyLookahead( *(new RTIfedTime(d)) );
    }
    catch (RTI::Exception& e) {
        test = 0;
        cout << "catch " << e._name << " reason " << e._reason << endl;
    }
    if (test)
       cout << endl << "t=" << temps->getTime() << " modifyLookahead(" << d << " )" << endl;
    else
       cout << "modifyLookahead has failed" << endl;
}
// queryLookahead
void callQL (void) {
    int test = 1;
    try {
       myFedere_Interactif->queryLookahead(ft);
    }
    catch (RTI::Exception& e) {
        test = 0;
        cout << "catch " << e._name << " reason " << e._reason << endl;
    }
    if (test)
       cout << endl << "Lookahead=" << ft.getTime() << endl;
    else
       cout << "queryLookahead has failed" << endl;
}
// changeInteractionOrderType
void callCIOT (void) {
    int test = 1;
    int type ;
    cout << "ordre RECEIVE (1) ou TIMESTAMP (2) ?" << endl ;
    cin >> type ;
    try {
       myFedere_Interactif->changeInteractionOrderType(myFedere_Interactif->getInteractionClassHandle("msg"),
                                                       type);
    }
    catch (RTI::Exception& e) {
        test = 0;
        cout << "catch " << e._name << " reason " << e._reason << endl;
    }
    if (test)
       cout << endl << "Nouveau type d'ordre pour l'interaction = " << type << endl;
    else
       cout << "changeInteractionOrderType" << endl;
}

