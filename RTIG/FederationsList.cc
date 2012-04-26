// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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
// $Id: FederationsList.cc,v 3.77 2012/04/26 07:49:35 erk Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "FederationsList.hh"

using std::endl ;

namespace certi {
namespace rtig {

static PrettyDebug D("FEDERATIONSLIST", __FILE__);
static PrettyDebug G("GENDOC",__FILE__);

// ----------------------------------------------------------------------------
// Constructor
FederationsList::FederationsList(SocketServer &server, AuditFile &audit)
    : socketServer(server),
      auditFile(audit),
      verboseLevel(0)
{
}

// ----------------------------------------------------------------------------
// Destructor
FederationsList::~FederationsList()
{
    if (!_handleFederationMap.empty())
        D.Out(pdError, "ListeFederation not empty at destruction time.");

    for (HandleFederationMap::const_iterator i = _handleFederationMap.begin(); i != _handleFederationMap.end(); ++i) {
        delete i->second;
    }
}

// ----------------------------------------------------------------------------
//! Sets Class Relevance Advisory Switch
void
FederationsList::setClassRelevanceAdvisorySwitch(Handle federationHandle,
                                		 FederateHandle federate)
    throw (FederationExecutionDoesNotExist,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    // It may throw a bunch of exceptions.
    federation->setClassRelevanceAdvisorySwitch(federate);
}

// ----------------------------------------------------------------------------
//! Clears Class Relevance Advisory Switch
void
FederationsList::unsetClassRelevanceAdvisorySwitch(Handle federationHandle,
                                		 FederateHandle federate)
    throw (FederationExecutionDoesNotExist,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    // It may throw a bunch of exceptions.
    federation->unsetClassRelevanceAdvisorySwitch(federate);
}

// ----------------------------------------------------------------------------
//! Sets Interaction Relevance Advisory Switch
void
FederationsList::setInteractionRelevanceAdvisorySwitch(Handle federationHandle,
                                		 FederateHandle federate)
    throw (FederationExecutionDoesNotExist,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    // It may throw a bunch of exceptions.
    federation->setInteractionRelevanceAdvisorySwitch(federate);
}

// ----------------------------------------------------------------------------
//! Clears Interaction Relevance Advisory Switch
void
FederationsList::unsetInteractionRelevanceAdvisorySwitch(Handle federationHandle,
                                		 FederateHandle federate)
    throw (FederationExecutionDoesNotExist,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    // It may throw a bunch of exceptions.
    federation->unsetInteractionRelevanceAdvisorySwitch(federate);
}

// ----------------------------------------------------------------------------
//! Sets Attribute Relevance Advisory Switch
void
FederationsList::setAttributeRelevanceAdvisorySwitch(Handle federationHandle,
                                		 FederateHandle federate)
    throw (FederationExecutionDoesNotExist,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    // It may throw a bunch of exceptions.
    federation->setAttributeRelevanceAdvisorySwitch(federate);
}

// ----------------------------------------------------------------------------
//! Clears Attribute Relevance Advisory Switch
void
FederationsList::unsetAttributeRelevanceAdvisorySwitch(Handle federationHandle,
                                		 FederateHandle federate)
    throw (FederationExecutionDoesNotExist,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    // It may throw a bunch of exceptions.
    federation->unsetAttributeRelevanceAdvisorySwitch(federate);
}

// ----------------------------------------------------------------------------
//! Sets Attribute Scope Advisory Switch
void
FederationsList::setAttributeScopeAdvisorySwitch(Handle federationHandle,
                                		 FederateHandle federate)
    throw (FederationExecutionDoesNotExist,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    // It may throw a bunch of exceptions.
    federation->setAttributeScopeAdvisorySwitch(federate);
}

// ----------------------------------------------------------------------------
//! Clears Attribute Scope Advisory Switch
void
FederationsList::unsetAttributeScopeAdvisorySwitch(Handle federationHandle,
                                		 FederateHandle federate)
    throw (FederationExecutionDoesNotExist,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    // It may throw a bunch of exceptions.
    federation->unsetAttributeScopeAdvisorySwitch(federate);
}

// ----------------------------------------------------------------------------
//! Adds a constrained federate to federation.
void
FederationsList::addConstrained(Handle federationHandle,
                                FederateHandle federate)
    throw (FederationExecutionDoesNotExist,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    // It may throw a bunch of exceptions.
    federation->addConstrained(federate);
}

// ----------------------------------------------------------------------------
/*! Creates a new Federate if its name is unique, and returns its new
  Federate Handle. Also sends Null messages from all other federates
  to initialize its LBTS, and finally a RequestPause message if the
  Federation is already paused.
*/
FederateHandle
FederationsList::addFederate(Handle federationHandle,
                             const std::string& name,
                             SocketTCP *tcp_link,
                             NM_Join_Federation_Execution& objectModelData)
    throw (FederationExecutionDoesNotExist,
           FederateAlreadyExecutionMember,
           MemoryExhausted,
           RTIinternalError)
{
    G.Out(pdGendoc,"enter FederationsList::addFederate");

    // It may throw FederationExecutionDoesNotExist
    // Return  federation address giving its handle
    Federation *federation = searchFederation(federationHandle);

    // It may raise a bunch of exceptions
    // adding the federate and return its handle
    FederateHandle federate = federation->add(name, tcp_link);

    federation->getFOM(objectModelData);

    G.Out(pdGendoc,"exit FederationsList::addFederate");

    return federate ;
}

Federation*
FederationsList::searchFederation(Handle federationHandle)
    throw (FederationExecutionDoesNotExist)
{
    HandleFederationMap::const_iterator i = _handleFederationMap.find(federationHandle);
    if (i != _handleFederationMap.end()) {
		return i->second;
    }

    D.Out(pdExcept, "Unknown Federation Handle %d.", federationHandle);
    throw FederationExecutionDoesNotExist("Bad Federation Handle.");
}

// ----------------------------------------------------------------------------
// createFederation
#ifdef FEDERATION_USES_MULTICAST
/** createFederation (with FEDERATION_USES_MULTICAST defined)
/   @param name Federation name
    @param handle Federation handle
    @param mc_link
*/
void FederationsList::createFederation(const std::string& name,
                                       Handle federationHandle,
                                       SocketMC *mc_link)
#else
/** createFederation (with FEDERATION_USES_MULTICAST not defined)
    @param name Federation name
    @param handle Federation handle
    @param FEDid execution id. of the federation (i.e. file name)
*/
    void FederationsList::createFederation(const std::string& name,
                                           Handle federationHandle,
                                           const std::string& FEDid)
#endif
    throw (FederationExecutionAlreadyExists,
           CouldNotOpenFED,
           ErrorReadingFED,
           MemoryExhausted,
           SecurityError,
           RTIinternalError)
{
    G.Out(pdGendoc,"enter FederationsList::createFederation");
    auditFile << ", Handle : " << federationHandle ;
    if (name.empty()) throw RTIinternalError("Invalid Federation Name.");

    // It should throw FederationExecutionDoesNotExist.
    try {
        getFederationHandle(name);
        D.Out(pdExcept, "Federation %s already present.", name.c_str());
        G.Out(pdGendoc,"exit  FederationsList::createFederation on exception");
        throw FederationExecutionAlreadyExists(name);
    }
    catch (FederationExecutionDoesNotExist &e) {
        D.Out(pdDebug,
              "CreerFederation catches FederationExecutionDoesNotExist.");
    }

    Federation *federation = NULL ;
#ifdef FEDERATION_USES_MULTICAST
    federation = new Federation(name, federationHandle, socketServer, auditFile, mc_link, verboseLevel);
#else
    try {
        federation = new Federation(name, federationHandle, socketServer, auditFile, FEDid,verboseLevel);
        D.Out(pdDebug,"new Federation created.");
        }
    catch (CouldNotOpenFED& e) {
        D.Out(pdInit, "Federation constructor : Could not open FED file.");
        G.Out(pdGendoc,"exit FederationsList::createFederation on exception CouldNotOpenFED");
        throw CouldNotOpenFED(e._reason);
        }
    catch (ErrorReadingFED &e) {
        D.Out(pdInit, "Federation constructor : Could not read FED file (maybe incorrect).");
        G.Out(pdGendoc,"exit FederationsList::createFederation on exception ErrorReadingFED");
        throw ErrorReadingFED(e._reason);
        }


#endif
    if (federation == NULL)
        throw MemoryExhausted("No memory left for new Federation.");

    _handleFederationMap[federationHandle] = federation;
    D.Out(pdInit, "New Federation created with Handle %d.", federationHandle);

    G.Out(pdGendoc,"exit FederationsList::createFederation");
}

// ----------------------------------------------------------------------------
// createRegulator
void
FederationsList::createRegulator(Handle federationHandle,
                                 FederateHandle federate,
                                 FederationTime time)
    throw (FederationExecutionDoesNotExist,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    // It may throw a bunch of exceptions.
    federation->addRegulator(federate, time);
}

// ----------------------------------------------------------------------------
// destroyObject with time
void
FederationsList::destroyObject(Handle federationHandle,
			       FederateHandle federate,
			       ObjectHandle id,
			       FederationTime theTime,
			       const std::string& tag)
    throw (FederateNotExecutionMember,
           FederationExecutionDoesNotExist,
           DeletePrivilegeNotHeld,
           ObjectNotKnown,
           SaveInProgress,
           RestoreInProgress,
	   InvalidFederationTime,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    federation->deleteObject(federate, id, theTime, tag);
}

// ----------------------------------------------------------------------------
// destroyObject without time
void
FederationsList::destroyObject(Handle federationHandle,
                               FederateHandle federate,
                               ObjectHandle id,
                               const std::string& tag)
    throw (FederateNotExecutionMember,
           FederationExecutionDoesNotExist,
           DeletePrivilegeNotHeld,
           ObjectNotKnown,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    federation->deleteObject(federate, id, tag);
}

// ----------------------------------------------------------------------------
/*! Return the Handle of the Federation named "name" if it is found in the
  FederationList, else throw FederationExecutionDoesNotExist.
*/
Handle
FederationsList::getFederationHandle(const std::string& name)
    throw (FederationExecutionDoesNotExist)
{
    G.Out(pdGendoc,"enter FederationsList::getFederationHandle");

    for (HandleFederationMap::const_iterator i = _handleFederationMap.begin(); i != _handleFederationMap.end(); ++i) {
        if (i->second->getName() == name) {
            G.Out(pdGendoc,"exit  FederationsList::getFederationHandle");
            return i->second->getHandle();
        }
    }
    G.Out(pdGendoc,"exit  FederationsList::getFederationHandle on exception");

    D.Out(pdDebug, "getFederationHandle throws FederationExecutionDoesNotExist.");
    throw FederationExecutionDoesNotExist(name);
}

// ----------------------------------------------------------------------------
// info
#ifdef FEDERATION_USES_MULTICAST
void FederationsList::info(Handle federationHandle,
                           int &nb_federates,
                           int &nb_regulators,
                           bool &is_syncing,
                           SocketMC* &comm_mc)
#else
    void FederationsList::info(Handle federationHandle,
                               int &nb_federates,
                               int &nb_regulators,
                               bool &is_syncing)
#endif
    throw (FederationExecutionDoesNotExist, RTIinternalError)
{
    G.Out(pdGendoc,"enter FederationsList::info");

    // It may throw FederationExecutionNotFound
    Federation *federation = searchFederation(federationHandle);

    nb_federates  = federation->getNbFederates();
    nb_regulators = federation->getNbRegulators();
    is_syncing    = federation->isSynchronizing();
#ifdef FEDERATION_USES_MULTICAST
    comm_mc = federation->MCLink ;
#endif
    G.Out(pdGendoc,"exit  FederationsList::info");
}

// ----------------------------------------------------------------------------
// registerObject
ObjectHandle
FederationsList::registerObject(Handle federationHandle,
                                FederateHandle federate,
                                ObjectClassHandle object_class,
                                ObjectName_t name)
    throw (FederateNotExecutionMember,
           FederateNotPublishing,
           ObjectAlreadyRegistered,
           ObjectClassNotDefined,
           ObjectClassNotPublished,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    D.Out(pdTrace, "federationHandle = %d, federate = %d.", federationHandle, federate);

    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    D.Out(pdTrace, "theObjectClass = %d, name = %s.", object_class, name.c_str());
    return(federation->registerObject(federate, object_class, name));
}


void
FederationsList::updateRegulator(FederationHandle federationHandle,
                                 FederateHandle federate,
                                 FederationTime time,
                                 bool anonymous)
    throw (FederationExecutionDoesNotExist,
           FederateNotExecutionMember,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    federation->updateRegulator(federate, time, anonymous);
}

// ----------------------------------------------------------------------------
// updateAttribute with time
void
FederationsList::updateAttribute(Handle federationHandle,
                                 FederateHandle federate,
                                 ObjectHandle id,
                                 const std::vector <AttributeHandle> &attributes,
                                 const std::vector <AttributeValue_t> &values,
                                 uint16_t list_size,
                                 FederationTime time,
                                 const std::string& tag)
    throw (FederateNotExecutionMember,
           FederationExecutionDoesNotExist,
           ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    federation->updateAttributeValues(federate, id, attributes, values,
                                      list_size, time, tag);
}

// ----------------------------------------------------------------------------
// updateAttribute without time
void
FederationsList::updateAttribute(Handle federationHandle,
                                 FederateHandle federate,
                                 ObjectHandle id,
                                 const std::vector <AttributeHandle> &attributes,
                                 const std::vector <AttributeValue_t> &values,
                                 uint16_t list_size,
                                 const std::string& tag)
    throw (FederateNotExecutionMember,
           FederationExecutionDoesNotExist,
           ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    federation->updateAttributeValues(federate, id, attributes, values,
                                      list_size, tag);
}
// ----------------------------------------------------------------------------
// updateParameter with time
void
FederationsList::updateParameter(Handle federationHandle,
                                 FederateHandle federate,
                                 InteractionClassHandle interaction,
                                 const std::vector <ParameterHandle> &parameters,
                                 const std::vector <ParameterValue_t> &values,
                                 uint16_t list_size,
                                 FederationTime time,
                                 RegionHandle region,
                                 const std::string& tag)
    throw (FederateNotExecutionMember,
           FederateNotPublishing,
           FederationExecutionDoesNotExist,
           InteractionClassNotDefined,
           InteractionParameterNotDefined,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    G.Out(pdGendoc,"enter FederationsList::updateParameter with time");

    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    federation->broadcastInteraction(federate, interaction, parameters, values,
                                     list_size, time, region, tag);

    G.Out(pdGendoc,"exit FederationsList::updateParameter with time");

}

// ----------------------------------------------------------------------------
// updateParameter without time
void
FederationsList::updateParameter(Handle federationHandle,
                                 FederateHandle federate,
                                 InteractionClassHandle interaction,
                                 const std::vector <ParameterHandle> &parameters,
                                 const std::vector <ParameterValue_t> &values,
                                 uint16_t list_size,
                                 RegionHandle region,
                                 const std::string& tag)
    throw (FederateNotExecutionMember,
           FederateNotPublishing,
           FederationExecutionDoesNotExist,
           InteractionClassNotDefined,
           InteractionParameterNotDefined,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    G.Out(pdGendoc,"enter FederationsList::updateParameter without time");
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    federation->broadcastInteraction(federate, interaction, parameters, values,
                                     list_size, region, tag);

    G.Out(pdGendoc,"exit FederationsList::updateParameter without time");
}
// ----------------------------------------------------------------------------
/*! Called by processRegisterSynchronization and
  processSynchronizationAchieved.
*/
void
FederationsList::manageSynchronization(Handle federationHandle,
                                       FederateHandle federate,
                                       bool state,
                                       const std::string& label,
                                       const std::string& tag)
    throw (FederationAlreadyPaused,
           FederationNotPaused,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{

    G.Out(pdGendoc,"enter FederationsList::manageSynchronization for all federates");

    // It may throw FederationExecutionDoesNotExist
    Federation *federation = searchFederation(federationHandle);

    // It may throw a bunch of exceptions.
    if (state)
        federation->registerSynchronization(federate, label, tag);
    else
        federation->unregisterSynchronization(federate, label);

    G.Out(pdGendoc,"exit  FederationsList::manageSynchronization for all federates");

}

// ----------------------------------------------------------------------------
void
FederationsList::manageSynchronization(Handle federationHandle,
                                       FederateHandle federate,
                                       bool state,
                                       const std::string& label,
                                       const std::string& tag,
                                       unsigned short federate_setSize,
                                       const std::vector <FederateHandle> &federate_set)
    throw (FederationAlreadyPaused,
           FederationNotPaused,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    G.Out(pdGendoc,"enter FederationsList::manageSynchronization with federates set");

    // It may throw FederationExecutionDoesNotExist
    Federation *federation = searchFederation(federationHandle);

    // state true means register else unregister
    // It may throw a bunch of exceptions.
    if (state)
        federation->registerSynchronization(federate, label, tag,
                                            (unsigned short)federate_setSize, federate_set);
    else
        federation->unregisterSynchronization(federate, label);

    G.Out(pdGendoc,"exit  FederationsList::manageSynchronization with federates set");
}

// ----------------------------------------------------------------------------
//! Called by processRegisterSynchronization.
void
FederationsList::broadcastSynchronization(Handle federationHandle,
                                          FederateHandle federate,
                                          const std::string& label,
                                          const std::string& tag)
    throw (FederationExecutionDoesNotExist,
           RTIinternalError)
{
    G.Out(pdGendoc,"enter FederationsList::broadcastSynchronization");

    // It may throw FederationExecutionDoesNotExist
    Federation *federation = searchFederation(federationHandle);

    federation->broadcastSynchronization(federate, label, tag);

    G.Out(pdGendoc,"exit  FederationsList::broadcastSynchronization");
}

// ----------------------------------------------------------------------------
//! Called by processRegisterSynchronization.
// Broadcast only on the federates into a set
void
FederationsList::broadcastSynchronization(Handle federationHandle,
                                          FederateHandle federate,
                                          const std::string& label,
                                          const std::string& tag,
                                          unsigned short federate_setSize,
                                          const std::vector <FederateHandle> &federate_set)
    throw (FederationExecutionDoesNotExist,
           RTIinternalError)
{
    G.Out(pdGendoc,"enter FederationsList::broadcastSynchronization onto a federate set");

    // It may throw FederationExecutionDoesNotExist
    Federation *federation = searchFederation(federationHandle);

    federation->broadcastSynchronization(federate, label, tag, federate_setSize, federate_set);

    G.Out(pdGendoc,"exit  FederationsList::broadcastSynchronization onto a federate set");
}


// ----------------------------------------------------------------------------
// publishInteraction
void
FederationsList::publishInteraction(Handle federationHandle,
                                    FederateHandle federate,
                                    InteractionClassHandle interaction,
                                    bool pub)
    throw (InteractionClassNotDefined,
           FederationExecutionDoesNotExist,
           FederateNotExecutionMember,
           SaveInProgress,
           SecurityError,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    federation->publishInteraction(federate, interaction, pub);
}

// ----------------------------------------------------------------------------
// publishObject
void
FederationsList::publishObject(Handle federationHandle,
                               FederateHandle federate,
                               ObjectClassHandle object_class,
                               const std::vector <AttributeHandle> &attributes,
                               bool pub)
    throw (ObjectClassNotDefined,
           AttributeNotDefined,
           FederationExecutionDoesNotExist,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           SecurityError,
           RTIinternalError)

{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    federation->publishObject(federate, object_class, attributes, pub);
}

// ----------------------------------------------------------------------------
// subscribeInteraction
void
FederationsList::subscribeInteraction(Handle federationHandle,
                                      FederateHandle federate,
                                      InteractionClassHandle interaction,
                                      bool sub)
    throw (InteractionClassNotDefined,
           FederationExecutionDoesNotExist,
           FederateNotExecutionMember,
           SaveInProgress,
           SecurityError,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    federation->subscribeInteraction(federate, interaction, sub);
}

// ----------------------------------------------------------------------------
void
FederationsList::subscribeObject(Handle federationHandle,
                                 FederateHandle federate,
                                 ObjectClassHandle object_class,
                                 const std::vector <AttributeHandle> &attributes)
    throw (ObjectClassNotDefined,
           AttributeNotDefined,
           FederationExecutionDoesNotExist,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           SecurityError,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    federation->subscribeObject(federate, object_class, attributes);
}

// ----------------------------------------------------------------------------
// removeConstrained
void
FederationsList::removeConstrained(Handle federationHandle,
                                   FederateHandle federate)
    throw (FederationExecutionDoesNotExist,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    // It may throw a bunch of exceptions.
    federation->removeConstrained(federate);
}

// ----------------------------------------------------------------------------
// destroyFederation
void
FederationsList::destroyFederation(Handle federationHandle)
    throw (FederatesCurrentlyJoined,
           FederationExecutionDoesNotExist,
           RTIinternalError)
{
    G.Out(pdGendoc,"enter FederationsList::destroyFederation");

    // It may throw :
    // FederationExecutionDoesNotExist during search federation
    Federation *federation = searchFederation(federationHandle);

    // It may throw FederatesCurrentlyJoined if federation not empty (in empty)
    if (federation->empty()) {
        _handleFederationMap.erase(_handleFederationMap.find(federationHandle));
        delete federation ;
    }
    G.Out(pdGendoc,"exit FederationsList::destroyFederation");
}

// ----------------------------------------------------------------------------
// remove
void
FederationsList::remove(Handle federationHandle, FederateHandle federate)
    throw (FederationExecutionDoesNotExist,
           FederateOwnsAttributes,
           FederateNotExecutionMember,
           RTIinternalError)
{
    G.Out(pdGendoc,"enter FederationsList::remove");

    // It may throw FederationExecutionDoesNotExist
    Federation *federation = searchFederation(federationHandle);

    // It may throw FederateOwnsAttributes or FederateNotExecutionMember
    federation->remove(federate);

    G.Out(pdGendoc,"exit FederationsList::remove");
}

// ----------------------------------------------------------------------------
// removeRegulator
void
FederationsList::removeRegulator(Handle federationHandle,
                                 FederateHandle federate)
    throw (FederationExecutionDoesNotExist,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    federation->removeRegulator(federate);
}

// ----------------------------------------------------------------------------
/*! This Method tries to remove all references to this Federate in the
  Federation. To be used when a Federate is supposed to have crashed.
*/
void
FederationsList::killFederate(Handle federationHandle, FederateHandle federate)
    throw ()
{
    try {
        // It may throw FederationExecutionDoesNotExist.
        Federation *federation = searchFederation(federationHandle);
        federation->kill(federate);
    }
    catch (Exception &e) {
        return ;
    }
}

// ----------------------------------------------------------------------------
// isOwner
bool
FederationsList::isOwner(Handle federationHandle,
                         FederateHandle federate,
                         ObjectHandle id,
                         AttributeHandle attribute)
    throw (FederateNotExecutionMember,
           ObjectNotKnown,
           AttributeNotDefined,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    D.Out(pdDebug, "Owner of Attribute %u of Object %u .", attribute, id);

    return federation->isOwner(federate, id, attribute);
}

// ----------------------------------------------------------------------------
// searchOwner
void
FederationsList::searchOwner(Handle federationHandle,
                             FederateHandle federate,
                             ObjectHandle id,
                             AttributeHandle attribute)
    throw (FederateNotExecutionMember,
           ObjectNotKnown,
           AttributeNotDefined,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    D.Out(pdDebug, "Owner of Attribute %u of Object %u .", attribute, id);

    federation->queryAttributeOwnership(federate, id, attribute);
}

// ----------------------------------------------------------------------------
// negotiateDivestiture
void
FederationsList::negotiateDivestiture(Handle federationHandle,
                                      FederateHandle federate,
                                      ObjectHandle id,
                                      const std::vector <AttributeHandle> &attributes,
                                      uint16_t list_size,
                                      const std::string& tag)
    throw (FederateNotExecutionMember,
           ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           AttributeAlreadyBeingDivested,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    D.Out(pdDebug, "NegotiatedAttributeOwnershipDivestiture of Object %u.", id);

    federation->negotiateDivestiture(federate, id, attributes, list_size, tag);
}

// ----------------------------------------------------------------------------
// acquireIfAvailable
void
FederationsList::acquireIfAvailable(Handle federationHandle,
                                    FederateHandle federate,
                                    ObjectHandle id,
                                    const std::vector <AttributeHandle> &attributes,
                                    uint16_t list_size)
    throw (ObjectNotKnown,
           ObjectClassNotPublished,
           AttributeNotDefined,
           AttributeNotPublished,
           FederateOwnsAttributes,
           AttributeAlreadyBeingAcquired,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    federation->acquireIfAvailable(federate, id, attributes, list_size);
}

// ----------------------------------------------------------------------------
// divest
void
FederationsList::divest(Handle federationHandle,
                        FederateHandle federate,
                        ObjectHandle id,
                        const std::vector <AttributeHandle> &attributes,
                        uint16_t list_size)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);
    federation->divest(federate, id, attributes, list_size);
}

// ----------------------------------------------------------------------------
// acquire
void
FederationsList::acquire(Handle federationHandle,
                         FederateHandle federate,
                         ObjectHandle id,
                         const std::vector <AttributeHandle> &attributes,
                         uint16_t list_size,
                         const std::string& tag)
    throw (ObjectNotKnown,
           ObjectClassNotPublished,
           AttributeNotDefined,
           AttributeNotPublished,
           FederateOwnsAttributes,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);
    D.Out(pdDebug, "attributeOwnershipAcquisition of Object %u .", id);
    federation->acquire(federate, id, attributes, list_size, tag);
}

// ----------------------------------------------------------------------------
// cancelNegotiatedAttributeOwnershipDivestiture
void
FederationsList::cancelDivestiture(Handle federationHandle,
                                   FederateHandle federate,
                                   ObjectHandle id,
                                   const std::vector <AttributeHandle> &attributes,
                                   uint16_t list_size)
    throw (ObjectNotKnown, AttributeNotDefined, AttributeNotOwned,
           AttributeDivestitureWasNotRequested, FederateNotExecutionMember,
           SaveInProgress, RestoreInProgress, RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    D.Out(pdDebug,
          "cancelNegotiatedAttributeOwnershipDivestiture of Object %u .", id);

    federation->cancelDivestiture(federate, id, attributes, list_size);
}

// ----------------------------------------------------------------------------
// respondRelease
AttributeHandleSet*
FederationsList::respondRelease(Handle federationHandle,
                                FederateHandle federate,
                                ObjectHandle id,
                                const std::vector <AttributeHandle> &attributes,
                                uint16_t list_size)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           FederateWasNotAskedToReleaseAttribute,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    D.Out(pdDebug, "AttributeOwnershipRealeaseResponse of Object %u .", id);

    return federation->respondRelease(federate, id, attributes, list_size);
}

// ----------------------------------------------------------------------------
// cancelAcquisition
void
FederationsList::cancelAcquisition(Handle federationHandle,
                                   FederateHandle federate,
                                   ObjectHandle id,
                                   const std::vector <AttributeHandle> &attributes,
                                   uint16_t list_size)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeAlreadyOwned,
           AttributeAcquisitionWasNotRequested,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederationExecutionDoesNotExist.
    Federation *federation = searchFederation(federationHandle);

    D.Out(pdDebug, "cancelAttributeOwnershipAcquisition of Object %u .", id);

    federation->cancelAcquisition(federate, id, attributes, list_size);
}

// ----------------------------------------------------------------------------
long
FederationsList::createRegion(Handle federationHandle,
                              FederateHandle federate,
                              SpaceHandle space,
                              long nb_extents)
    throw (SpaceNotDefined, InvalidExtents, FederateNotExecutionMember,
           SaveInProgress, RestoreInProgress, RTIinternalError)
{
    Federation *federation = searchFederation(federationHandle);

    Debug(D, pdDebug) << "Create " << nb_extents << "-extent Region "
               << "in space " << space << endl ;

    return federation->createRegion(federate, space, nb_extents);
}

// ----------------------------------------------------------------------------
void
FederationsList::modifyRegion(Handle federationHandle,
			      FederateHandle federate,
			      RegionHandle region,
			      const std::vector<Extent> &extents)
    throw (InvalidExtents, SaveInProgress, RestoreInProgress, RTIinternalError)
{
    Federation *federation = searchFederation(federationHandle);

    Debug(D, pdDebug) << "Modify region #" << region << endl ;
    federation->modifyRegion(federate, region, extents);
}

// ----------------------------------------------------------------------------
void
FederationsList::deleteRegion(Handle federationHandle,
                              FederateHandle federate,
                              long region)
    throw (RegionNotKnown, RegionInUse, FederateNotExecutionMember,
           SaveInProgress, RestoreInProgress, RTIinternalError)
{
    Federation *federation = searchFederation(federationHandle);

    Debug(D, pdDebug) << "Delete region " << region << endl ;

    federation->deleteRegion(federate, region);
}

// ----------------------------------------------------------------------------
void
FederationsList::associateRegion(Handle federationHandle,
				 FederateHandle federate,
				 ObjectHandle object,
				 RegionHandle region,
				 unsigned short nb_attributes,
				 const std::vector <AttributeHandle> &attributes)
	throw (RegionInUse, FederateNotExecutionMember, SaveInProgress,
	       RestoreInProgress, RTIinternalError)
{
    Debug(D, pdDebug) << "Associate region " << region << " for updates." << endl ;

    Federation *federation = searchFederation(federationHandle);

    federation->associateRegion(federate, object, region, nb_attributes, attributes);
}

// ----------------------------------------------------------------------------
void
FederationsList::unassociateRegion(Handle federationHandle,
				   FederateHandle federate,
				   ObjectHandle object,
				   RegionHandle region)
	throw (RegionInUse, FederateNotExecutionMember, SaveInProgress,
	       RestoreInProgress, RTIinternalError)
{
    Federation *federation = searchFederation(federationHandle);

    Debug(D, pdDebug) << "Unassociate region for updates " << region << endl ;

    federation->unassociateRegion(federate, object, region);
}

// ----------------------------------------------------------------------------
void
FederationsList::subscribeAttributesWR(Handle federationHandle,
				       FederateHandle federate,
				       ObjectClassHandle object_class,
				       RegionHandle region,
				       unsigned short nb,
				       const std::vector <AttributeHandle> &attributes)
    throw (FederateNotExecutionMember, SaveInProgress, RestoreInProgress,
	   RTIinternalError)
{
    Federation *federation = searchFederation(federationHandle);

    Debug(D, pdDebug) << " Subscribe attributes with region " << region << endl ;

    federation->subscribeAttributesWR(federate, object_class, region, nb, attributes);
}

// ----------------------------------------------------------------------------
void
FederationsList::unsubscribeAttributesWR(Handle federationHandle,
					 FederateHandle federate,
					 ObjectClassHandle object_class,
					 RegionHandle region)
    throw (FederateNotExecutionMember, SaveInProgress, RestoreInProgress,
	   RTIinternalError)
{
    Federation *federation = searchFederation(federationHandle);

    Debug(D, pdDebug) << "Unsubscribe attributes with region " << region << endl ;

    federation->unsubscribeAttributesWR(federate, object_class, region);
}

// ----------------------------------------------------------------------------
void
FederationsList::subscribeInteractionWR(Handle federationHandle,
					FederateHandle federate,
					InteractionClassHandle ic,
					RegionHandle region)
    throw (FederateNotExecutionMember, SaveInProgress, RestoreInProgress,
	   RTIinternalError)
{
    Federation *federation = searchFederation(federationHandle);

    Debug(D, pdDebug) << "Subscribe interaction with region " << region << endl ;

    federation->subscribeInteractionWR(federate, ic, region);
}

// ----------------------------------------------------------------------------
void
FederationsList::unsubscribeInteractionWR(Handle federationHandle,
					  FederateHandle federate,
					  InteractionClassHandle ic,
					  RegionHandle region)
    throw (FederateNotExecutionMember, SaveInProgress, RestoreInProgress,
	   RTIinternalError)
{
    Federation *federation = searchFederation(federationHandle);

    Debug(D, pdDebug) << "Unsubscribe interaction with region " << region << endl ;

    federation->unsubscribeInteractionWR(federate, ic, region);
}

// ----------------------------------------------------------------------------
// registerObjectWithRegion
ObjectHandle FederationsList::registerObjectWithRegion(Handle federationHandle,
						       FederateHandle federate,
						       ObjectClassHandle handle,
						       ObjectName_t tag,
						       RegionHandle region,
						       int nb,
						       const std::vector <AttributeHandle> &attrs)
	throw (ObjectClassNotDefined, ObjectClassNotPublished,
	       AttributeNotDefined, AttributeNotPublished, RegionNotKnown,
	       InvalidRegionContext, ObjectAlreadyRegistered,
	       FederateNotExecutionMember, SaveInProgress, RestoreInProgress,
	       RTIinternalError)
{
    Federation *federation = searchFederation(federationHandle);

    return federation->registerObjectWithRegion(federate, handle, tag, region, nb, attrs);
}

// ----------------------------------------------------------------------------
// requestFederationSave with time
void
FederationsList::requestFederationSave(Handle federationHandle,
                                       FederateHandle the_federate,
                                       const std::string& the_label,
                                       FederationTime the_time)
{
    G.Out(pdGendoc,"enter FederationsList::requestFederationSave with time");

    // It may throw FederationExecutionDoesNotExist
    Federation *federation = searchFederation(federationHandle);

    federation->requestFederationSave(the_federate, the_label, the_time);

    G.Out(pdGendoc,"exit  FederationsList::requestFederationSave with time");
}

// ----------------------------------------------------------------------------
// requestFederationSave without time
void
FederationsList::requestFederationSave(Handle federationHandle,
                                       FederateHandle the_federate,
                                       const std::string& the_label)
{
    G.Out(pdGendoc,"enter FederationsList::requestFederationSave without time");

    // It may throw FederationExecutionDoesNotExist
    Federation *federation = searchFederation(federationHandle);

    federation->requestFederationSave(the_federate, the_label);

    G.Out(pdGendoc,"exit  FederationsList::requestFederationSave without time");
}
// ----------------------------------------------------------------------------
void
FederationsList::federateSaveBegun(Handle federationHandle,
                                   FederateHandle the_federate)
{
    G.Out(pdGendoc,"enter FederationsList::federateSaveBegun");

    // It may throw FederationExecutionDoesNotExist
    Federation *federation = searchFederation(federationHandle);

    federation->federateSaveBegun(the_federate);

    G.Out(pdGendoc,"exit  FederationsList::federateSaveBegun");
}

// ----------------------------------------------------------------------------
void
FederationsList::federateSaveStatus(Handle federationHandle,
                                    FederateHandle the_federate,
                                    bool the_status)
{
    G.Out(pdGendoc,"enter FederationsList::federateSaveStatus");

    // It may throw FederationExecutionDoesNotExist
    Federation *federation = searchFederation(federationHandle);

    federation->federateSaveStatus(the_federate, the_status);

    G.Out(pdGendoc,"exit  FederationsList::federateSaveStatus");
}

// ----------------------------------------------------------------------------
void
FederationsList::requestFederationRestore(Handle federationHandle,
                                          FederateHandle the_federate,
                                          const std::string& the_label)
{
    G.Out(pdGendoc,"enter FederationsList::requestFederationRestore");

    // It may throw FederationExecutionDoesNotExist
    Federation *federation = searchFederation(federationHandle);

    federation->requestFederationRestore(the_federate, the_label);
    G.Out(pdGendoc,"exit  FederationsList::requestFederationRestore");
}

// ----------------------------------------------------------------------------
void
FederationsList::federateRestoreStatus(Handle federationHandle,
                                       FederateHandle the_federate,
                                       bool the_status)
{
    G.Out(pdGendoc,"enter FederationsList::federateRestoreStatus");

    // It may throw FederationExecutionDoesNotExist
    Federation *federation = searchFederation(federationHandle);

    federation->federateRestoreStatus(the_federate, the_status);
    G.Out(pdGendoc,"exit  FederationsList::federateRestoreStatus");
}

// ----------------------------------------------------------------------------
// requestAttribute
FederateHandle
FederationsList::requestObjectOwner(Handle federationHandle,
                                 FederateHandle federate,
                                 ObjectHandle id,
                                 const std::vector <AttributeHandle> &attributes,
                                 uint32_t list_size)
        throw (ObjectNotKnown,
               FederationExecutionDoesNotExist,
               RTIinternalError)
{
    G.Out(pdGendoc,"into FederationsList::requestObjectOwner");

    Federation *federation = searchFederation(federationHandle);

    // It may throw ObjectNotKnown
    return federation->requestObjectOwner(federate, id, attributes, list_size);
}

// ----------------------------------------------------------------------------
// requestClassAttribute
void 
FederationsList::requestClassAttributeValueUpdate(Handle federationHandle,
							    FederateHandle federate,
								ObjectClassHandle classHandle,
								const std::vector <AttributeHandle> &attributes,
								uint32_t list_size)
		throw (ObjectClassNotDefined,
			   FederationExecutionDoesNotExist,
               RTIinternalError)
{
	G.Out(pdGendoc,"into FederationsList::requestClassAttributeValueUpdate");

	Federation *federation = searchFederation(federationHandle);

	federation->requestClassAttributeValueUpdate(federate, classHandle, attributes, list_size);
}

void
FederationsList::reserveObjectInstanceName(Handle federationHandle,
								           FederateHandle the_federate,
								           std::string newObjName)
		throw(IllegalName,
		      FederateNotExecutionMember,
		      SaveInProgress,
		      RestoreInProgress,
		      RTIinternalError)

{
    G.Out(pdGendoc,"enter FederationsList::reserveObjectInstanceName");

    // It may throw FederationExecutionDoesNotExist
    Federation *federation = searchFederation(federationHandle);

    federation->reserveObjectInstanceName(the_federate, newObjName);
    G.Out(pdGendoc,"exit  FederationsList::federateRestoreStatus");
}

bool
FederationsList::handleMessageNullPrime(FederationHandle federation, FederateHandle federate, FederationTime date)
{
   Federation* fed = searchFederation(federation);

   return fed->updateLastNERxForFederate(federate,date);
} /* end of handleMessageNullPrime */

FederationTime
FederationsList::getNullPrimeValue(FederationHandle federation)
{
   Federation* fed = searchFederation(federation);

   return fed->getMinNERx();
} /* end of handleMessageNullPrime */

}} // certi::rtig

// EOF $Id: FederationsList.cc,v 3.77 2012/04/26 07:49:35 erk Exp $

