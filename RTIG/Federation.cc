// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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
// $Id: Federation.cc,v 3.16 2003/04/23 17:24:08 breholee Exp $
// ----------------------------------------------------------------------------

#include "Federation.hh"

namespace certi {

class XmlParser ;

namespace rtig {

static pdCDebug D("FEDERATION", "(Fed.tion) - ");

// ----------------------------------------------------------------------------
//! Constructor
/*! Allocates memory the Name's storage, and read its FED file to store the
  result in RootObj.
*/
#ifdef FEDERATION_USES_MULTICAST
Federation::Federation(const char *federation_name,
                       FederationHandle federation_handle,
                       SocketServer *socket_server,
                       AuditFile *audit_server,
                       SocketMC *mc_link)
#else
    Federation::Federation(const char *federation_name,
                           FederationHandle federation_handle,
                           SocketServer *socket_server,
                           AuditFile *audit_server)
#endif
    throw (CouldNotOpenRID, ErrorReadingRID, MemoryExhausted, SecurityError,
           RTIinternalError)
    : list<Federate *>(), saveInProgress(false), saveStatus(true),
      restoreInProgress(false)
{
    fedparser::FedParser *fed_reader ;
    char file_name[MAX_FEDERATION_NAME_LENGTH + 5] ;

#ifdef FEDERATION_USES_MULTICAST // -----------------
    // Initialize Multicast
    if (mc_link == NULL) {
        D.Out(pdExcept, "Null Multicast socket for new Federation.");
        throw RTIinternalError("NULL Multicast socket for new Federation.");
    }

    D.Out(pdInit, "New Federation %d will use Multicast.", federation_handle);
    MCLink = mc_link ;
#endif // FEDERATION_USES_MULTICAST // --------------

    // Flags
    saveInProgress = false ;
    restoreInProgress = false ;

    // Allocates Name
    if ((federation_name == 0) || (federation_handle == 0))
        throw RTIinternalError("Null init parameter in Federation creation.");

    if (strlen(federation_name) > MAX_FEDERATION_NAME_LENGTH)
        throw RTIinternalError("Federation name too long.");

    name = strdup(federation_name);
    if (name == 0)
        throw MemoryExhausted("No memory left for Federation Name.");

    // Default Attribute values
    handle = federation_handle ;

    nextObjectId = (ObjectHandle) 1 ;
    nextFederateHandle = (FederateHandle) 1 ;

    D.Out(pdInit, "New Federation created with Handle %d, now reading FOM.",
          handle);

    // Initialize the Security Server.
    server = new SecurityServer(socket_server, audit_server, handle);
    if (server == NULL) throw RTIinternalError();

    // Read FOM File to initialize Root Object.
    root = new RootObject(server);

    if (root == 0)
        throw MemoryExhausted("No memory left for Federation Root Object.");

    cout << "New federation: " << name << endl ;
    cout << "Looking for .fed file... " ;

    string filename = string(name) + ".fed" ;
    ifstream *fdd = new ifstream(filename.c_str());

    if (fdd->is_open()) {

        cout << "yes" << endl ;
        fed_reader = new fedparser::FedParser(root);
        if (fed_reader == 0)
            throw MemoryExhausted("No memory left to read FED file.");

        server->Audit->addToLinef(", Fed File : %s", filename.c_str());

        try {
            fed_reader->readFile(filename.c_str());
        }
        catch (Exception *e) {
            delete fed_reader ;
            delete server ;
            server = NULL ;
            delete root ;
            root = NULL ;
            throw e ;
        }

        delete fed_reader ;
        delete fdd ;

        // Retrieve the FED file last modification time(for Audit)
        struct stat StatBuffer ;
        char *MTimeBuffer ;

        if (stat(file_name, &StatBuffer) == 0) {
            MTimeBuffer = ctime(&StatBuffer.st_mtime);
            MTimeBuffer[strlen(MTimeBuffer) - 1] = 0 ; // Remove trailing \n
            server->Audit->addToLinef("(Last modified %s)", MTimeBuffer);
        }
        else
            server->Audit->addToLinef("(could not retrieve last modif time, "
                                      "errno %d).", errno);
    }
    else {
        cout << "no" << endl ;
        if (XmlParser::exists()) {
            cout << "Looking for .xml file... " ;

            filename = string(name) + ".xml" ;
            fdd = new ifstream(filename.c_str());

            if (fdd->is_open()) {

                cout << "yes" << endl ;

                XmlParser *parser = new XmlParser(root);
                server->Audit->addToLinef(", XML File : %s", filename.c_str());

                try {
                    parser->parse(filename);
                }
                catch (Exception *e) {
                    delete parser ;
                    delete server ;
                    server = NULL ;
                    delete root ;
                    root = NULL ;
                    throw e ;
                }
                delete parser ;
                delete fdd ;
            }
            else
                cout << "no" << endl ;
        }
    }
}

// ----------------------------------------------------------------------------
// Destructor
Federation::~Federation()
{
    D.Out(pdInit, "Destroying Federation %d...", handle);

    // If there are Federates, delete them all!
    for (list<Federate *>::const_iterator i = begin(); i != end(); i++) {
        delete(*i);
    }
    clear();

    // Free local allocations
    free(name);
    delete root ;

    delete server ;

#ifdef FEDERATION_USES_MULTICAST
    FermerConnexion(mc_link);
    delete mc_link ;
#endif

}

// ----------------------------------------------------------------------------
// Get attributes
int
Federation::getNbFederates(void) const
{
    return size();
}

// ----------------------------------------------------------------------------
//! Return true if federation is being synchronized.
bool
Federation::isSynchronizing(void) const
{
    return !synchronizationLabels.empty();
}

// ----------------------------------------------------------------------------
FederationHandle
Federation::getHandle(void) const
{
    return handle ;
}

// ----------------------------------------------------------------------------
//! getName.
const char *
Federation::getName(void) const
{
    return name ;
}

int
Federation::getNbRegulators(void) const
{
    return regulators.size();
}

// ----------------------------------------------------------------------------
/*! Add the Federate to the Federation, and return its new federate handle.
  MAX_FEDERATE is the maximum number of federates per federation. Also send
  Null messages from all others federates to initialize its LBTS, and
  finally a RequestPause message if the Federation is already paused.
*/
FederateHandle
Federation::add(const char *federate_name, SocketTCP *tcp_link)
    throw (FederateAlreadyExecutionMember, MemoryExhausted, RTIinternalError)
{
    if (federate_name == 0) {
        D.Out(pdExcept, "Tried to add a NULL named federate.");
        throw RTIinternalError("Tried to add NULL federate to federation.");
    }

    if (size() >= MAX_FEDERATE) {
        D.Out(pdExcept, "Federation %d has too many Federates.", handle);
        throw RTIinternalError("Too many federates in federation.");
    }

    try {
        this->getByName(federate_name);
        throw FederateAlreadyExecutionMember();
    }
    catch (FederateNotExecutionMember &e) {
        // Nothing to do.
    }

    FederateHandle federate_handle = getNewHandle();

    Federate *federate = new Federate(federate_name, federate_handle);

    push_front(federate);

    D.Out(pdInit, "Federate %d joined Federation %d.", federate_handle, handle);

    // Donner au nouveau federe un dernier message nul
    // de chaque federe(i) contenant son heure logique h(i)
    // pour que le nouveau puisse calculer son LBTS.
    NetworkMessage message ;
    try {
        for (int i = 1 ; i <= regulators.size(); i++) {
            message.type = m_MESSAGE_NULL ;
            message.federation = handle ;

            regulators.get(i, message.federate, message.date);

            D.Out(pdTerm, "Sending NULL message(type %d) from %d to new federate.",
                  message.type, message.federate);

            message.write(tcp_link);
        }

        // If federation is synchronizing, put federate in same state.
        if (isSynchronizing()) {
            message.type = m_ANNOUNCE_SYNCHRONIZATION_POINT ;
            message.federate = federate_handle ;
            message.federation = handle ;

            std::map<const char *, const char *>::const_iterator i ;
            i = synchronizationLabels.begin();
            for (; i != synchronizationLabels.end(); i++) {
                message.setLabel((*i).first);
                message.setTag((*i).second);
                D.Out(pdTerm, "Sending synchronization message %s (type %d)"
                      " to the new Federate.", (*i).first, message.type);

                message.write(tcp_link);
                federate->addSynchronizationLabel((*i).first);
            }
        }
    }
    catch (NetworkError) {
        throw RTIinternalError("Network Error while initializing federate.");
    }

    return federate_handle ;
}

// ----------------------------------------------------------------------------
// Set Federate's attribute IsConstrained to true.
/*! FIXME: name/ merge with removeConstrained.
 */
void
Federation::addConstrained(FederateHandle federate_handle)
    throw (FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember
    Federate *federate = getByHandle(federate_handle);

    if (federate->isConstrained()) {
        D.Out(pdExcept, "Federate %d already constrained.", federate_handle);
        throw RTIinternalError("Time Regulating already enabled.");
    }

    federate->setConstrained(true);
    D.Out(pdTerm, "Federation %d: Federate %d is now constrained.",
          handle, federate_handle);
}

// ----------------------------------------------------------------------------
//! Add the Federate to the Regulators List.
/*! Check if it's already present, but not if the Time 'theTime' is allowed
  or not.
*/
void
Federation::addRegulator(FederateHandle federate_handle, FederationTime time)
    throw (FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember
    Federate *federate = getByHandle(federate_handle);

    // It may throw RTIinternalError if Federate was not regulators.
    regulators.insert(federate_handle, time);
    federate->setRegulator(true);

    D.Out(pdTerm, "Federation %d: Federate %d is now a regulator(Time=%f).",
          handle, federate_handle, time);

    NetworkMessage msg ;
    msg.type = m_SET_TIME_REGULATING ;
    msg.exception = e_NO_EXCEPTION ;
    msg.federation = handle ;
    msg.federate = federate_handle ;
    msg.regulator = RTI_TRUE ;
    msg.date = time ;

    this->broadcastAnyMessage(&msg, 0);
}

// ----------------------------------------------------------------------------
//! Broadcast 'msg' to all Federate except the one whose Handle is 'Except'.
void
Federation::broadcastAnyMessage(NetworkMessage *msg,
                                FederateHandle except_federate)
{
    Socket *socket = NULL ;

    // Broadcast the message 'msg' to all Federates in the Federation
    // except to Federate whose Handle is 'Except_Federate'.
    for (list<Federate *>::const_iterator i = begin(); i != end(); i++) {
        if ((*i)->getHandle() != except_federate) {
            try {
#ifdef HLA_USES_UDP
                socket = server->getSocketLink((*i)->getHandle(), BEST_EFFORT);
#else
                socket = server->getSocketLink((*i)->getHandle());
#endif
                msg->write(socket);
            }
            catch (RTIinternalError &e) {
                D[pdExcept] << "Reference to a killed Federate while "
                            << "broadcasting." << endl ;
            }
            catch (NetworkError &e) {
                D.Out(pdExcept, "Network error while broadcasting, ignoring.");
            }
        }
    }

    // BUG: If except = 0, could use Multicast.
}

// ----------------------------------------------------------------------------
//! broadcastInteraction

void
Federation::broadcastInteraction(FederateHandle federate_handle,
                                 InteractionClassHandle interaction,
                                 ParameterHandle *parameter_handles,
                                 ParameterValue *parameter_values,
                                 UShort list_size,
                                 FederationTime time,
                                 const char *tag)
    throw (FederateNotExecutionMember,
           FederateNotPublishing,
           InteractionClassNotDefined,
           InteractionParameterNotDefined,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate_handle);

    root->Interactions->broadcastInteraction(federate_handle,
                                             interaction,
                                             parameter_handles,
                                             parameter_values,
                                             list_size,
                                             time,
                                             tag);
    D.Out(pdRequest, "Federation %d: Broadcasted Interaction %d from Federate "
          "%d nb params %d.", handle, interaction, federate_handle, list_size);
    for (int i=0 ; i < list_size ; i++)
        D.Out(pdRequest,
              " Param %d Value %s",
              parameter_handles[i],
              parameter_values[i]);
}

// ----------------------------------------------------------------------------
// deleteObject

void
Federation::deleteObject(FederateHandle federate,
                         ObjectHandle id,
                         const char *tag)
    throw (FederateNotExecutionMember,
           DeletePrivilegeNotHeld,
           ObjectNotKnown,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    D.Out(pdRegister, "Federation %d: Federate %d destroys object %d.",
          this->handle, federate, id);

    root->deleteObjectInstance(federate, id, tag);
}

// ----------------------------------------------------------------------------
//! registerSynchronization.
void
Federation::registerSynchronization(FederateHandle federate,
                                    const char *label,
                                    const char *tag)
    throw (FederateNotExecutionMember,
           FederationAlreadyPaused,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    this->check(federate); // It may throw FederateNotExecutionMember.

    if ((label == NULL) || (strlen(label) > MAX_USER_TAG_LENGTH))
        throw RTIinternalError("Bad pause label(null or too long).");

    // Verify label does not already exists
    std::map<const char *, const char *>::const_iterator i ;
    i = synchronizationLabels.begin();
    for (; i != synchronizationLabels.end(); i++) {
        if (!strcmp((*i).first, label)) {
            throw FederationAlreadyPaused(); // Label already pending.
        }
    }

    // If not already in pending labels, insert to list.
    synchronizationLabels.insert(pair<const char *, const char *>(strdup(label),
                                                                  strdup(tag)));

    // Add label to each federate (may throw RTIinternalError).
    list<Federate *>::iterator j ;
    for (j = begin(); j != end(); j++) {
        (*j)->addSynchronizationLabel(label);
    }

    D[pdTerm] << "Federation " << handle << " is now synchronizing for label "
              << label << endl ;
}

// ----------------------------------------------------------------------------
void
Federation::broadcastSynchronization(FederateHandle federate,
                                     const char *label,
                                     const char *tag)
    throw (RTIinternalError)
{
    this->check(federate); // It may throw FederateNotExecutionMember.

    if ((label == NULL) || (strlen(label) > MAX_USER_TAG_LENGTH))
        throw RTIinternalError("Bad pause label(null or too long).");

    // broadcast announceSynchronizationPoint() to all federates in federation.
    NetworkMessage msg ;
    msg.type = m_ANNOUNCE_SYNCHRONIZATION_POINT ;
    msg.federate = federate ;
    msg.federation = handle ;
    msg.setLabel(label);
    msg.setTag(tag);

    broadcastAnyMessage(&msg, 0);
}

// ----------------------------------------------------------------------------
//! the_time is not managed yet.
void
Federation::requestFederationSave(FederateHandle the_federate,
                                  const char *the_label,
                                  FederationTime the_time)
    throw (FederateNotExecutionMember,
           SaveInProgress)
{
    check(the_federate);

    if (saveInProgress)
        throw SaveInProgress("Already in saving state.");

    for (list<Federate *>::iterator j = begin(); j != end(); j++) {
        (*j)->setSaving(true);
    }

    saveStatus = true ;
    saveInProgress = true ;

    NetworkMessage msg ;
    msg.type = m_INITIATE_FEDERATE_SAVE ;
    msg.federate = the_federate ;
    msg.federation = handle ;
    msg.setLabel(the_label);

    broadcastAnyMessage(&msg, 0);
}

// ----------------------------------------------------------------------------
/*! Received from a federate to inform a save has been received and is being
  processed.
  Maybe, a timeout should be set to determine if federate still alive.
*/
void
Federation::federateSaveBegun(FederateHandle the_federate)
    throw (FederateNotExecutionMember)
{
    check(the_federate);
}

// ----------------------------------------------------------------------------
void
Federation::federateSaveStatus(FederateHandle the_federate, bool the_status)
    throw (FederateNotExecutionMember)
{
    Federate * federate = getByHandle(the_federate);
    federate->setSaving(false);

    if (!the_status)
        saveStatus = false ;

    // Verify that all federates save ended (complete or not).
    for (list<Federate *>::iterator j = begin(); j != end(); j++) {
        if ((*j)->isSaving())
            return ;
    }

    // Send end save message.
    NetworkMessage msg ;

    msg.type = saveStatus ? m_FEDERATION_SAVED : m_FEDERATION_NOT_SAVED ;

    msg.federate = the_federate ;
    msg.federation = handle ;

    broadcastAnyMessage(&msg, 0);

    // Reinitialize state.
    saveStatus = true ;
    saveInProgress = false ;
}

// ----------------------------------------------------------------------------
//! Return a pointer of the Federate whose Handle is theHandle, if found.
Federate*
Federation::getByHandle(FederateHandle federate_handle) const
    throw (FederateNotExecutionMember)
{
    for (list<Federate *>::const_iterator i = begin(); i != end(); i++) {
        if ((*i)->getHandle() == federate_handle)
            return (*i);
    }

    throw FederateNotExecutionMember("Federate Handle not found.");
}

// ----------------------------------------------------------------------------
//! Return a pointer of the Federate whose Name is theName, if found.
Federate*
Federation::getByName(const char *federate_name) const
    throw (FederateNotExecutionMember)
{
    for (list<Federate *>::const_iterator i = begin(); i != end(); i++) {
        if (strcmp((*i)->getName(), federate_name) == 0)
            return (*i);
    }

    throw FederateNotExecutionMember("Federate Name not found.");
}

// ----------------------------------------------------------------------------
//! Return a brand new Federate Handle.
/*! Throw an exception if there are no more handles left(MAX_FEDERATEHANDLE).
 */
FederateHandle
Federation::getNewHandle(void)
    throw (RTIinternalError)
{
    if (nextFederateHandle > MAX_FEDERATEHANDLE) {
        D.Out(pdExcept,
              "Federation %d: Max Federate Handle count reached.", handle);
        throw RTIinternalError("Max Federate Handle reached.");
    }

    FederateHandle new_handle = nextFederateHandle ;
    nextFederateHandle++ ;
    return new_handle ;
}

// ----------------------------------------------------------------------------
// empty
/*! Return true if there are no Federates left in the Federation, else throw
  FederatesCurrentlyJoined.
*/
bool
Federation::empty(void) const
    throw (FederatesCurrentlyJoined)
{
    if (list<Federate *>::empty())
        return true ;
    else
        throw FederatesCurrentlyJoined();
}

// ----------------------------------------------------------------------------
// check
/*! Return true if the Federate is part of the Federation, else throw an
  exception.
*/
bool
Federation::check(FederateHandle federate_handle) const
    throw (FederateNotExecutionMember)
{
    for (list<Federate *>::const_iterator i = begin(); i != end(); i++) {
        if ((*i)->getHandle() == federate_handle)
            return true ;
    }

    throw FederateNotExecutionMember();
}

// ----------------------------------------------------------------------------
// killFederate
/*! This Method tries to remove all references to this Federate in the
  Federation. To be used when a Federate is supposed to have crashed.
*/
void
Federation::kill(FederateHandle federate)
    throw ()
{
    // NOTE: Connection to the federate is already closed.

    D.Out(pdInit, "Killing Federate %d.", federate);

    // is regulator ?
    try {
        removeRegulator(federate);
        D.Out(pdInit, "Regulator Federate %d removed...", federate);
    } catch (Exception &e) {}

    // is constrained ?
    try {
        removeConstrained(federate);
        D.Out(pdInit, "Constrained Federate %d removed...", federate);
    } catch (Exception &e) {}

    // Remove references to this federate in root object
    root->killFederate(federate);
    D[pdTrace] << "Federate " << federate << " removed from the Root Object "
               << endl ;

    // delete from federations list
    try {
        remove(federate);
        D.Out(pdInit, "Federate %d removed...", federate);
    } catch (Exception &e) {}
}

// ----------------------------------------------------------------------------
// publishInteraction

void
Federation::publishInteraction(FederateHandle federate,
                               InteractionClassHandle interaction,
                               bool pub)
    throw (InteractionClassNotDefined,
           FederateNotExecutionMember,
           SaveInProgress,
           SecurityError,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    // It may throw InteractionClassNotDefined
    root->Interactions->publish(federate, interaction, pub);
    D.Out(pdRequest,
          "Federation %d: Federate %d has(un)published Interaction %d.",
          handle, federate, interaction);
}

// ----------------------------------------------------------------------------
// publishObject

void
Federation::publishObject(FederateHandle federate,
                          ObjectClassHandle object,
                          AttributeHandle *attributes,
                          UShort list_size,
                          bool pub)
    throw (ObjectClassNotDefined,
           AttributeNotDefined,
           FederateNotExecutionMember,
           SaveInProgress,
           SecurityError,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    // It may throw *NotDefined
    root->ObjectClasses->publish(federate, object, attributes, list_size, pub);

    D.Out(pdRegister,
          "Federation %d: Federate %d(un)publishes %d attrib. of ObjClass %d.",
          handle, federate, list_size, object);
}

// ----------------------------------------------------------------------------
// registerObject

ObjectHandle
Federation::registerObject(FederateHandle federate,
                           ObjectClassHandle class_handle,
                           ObjectName object_name)
    throw (FederateNotExecutionMember,
           FederateNotPublishing,
           ObjectAlreadyRegistered,
           ObjectClassNotDefined,
           ObjectClassNotPublished,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // Allocate new ID.
    if (nextObjectId > MAX_OBJECTID) {
        D.Out(pdError, "RTIinternalError : nextObjectId = %d, MAX = %d.",
              nextObjectId, MAX_OBJECTID);
        throw RTIinternalError("Too many IDs requested.");
    }

    ObjectHandle new_id = nextObjectId ;
    nextObjectId++ ;

    D.Out(pdRegister,
          "Federation %d: Federate %d registering Object %d of Class %d.",
          handle, federate, new_id, class_handle);

    // Register Object.
    root->registerObjectInstance(federate, class_handle, new_id, object_name);
    D.Out(pdDebug, "suite");
    return new_id ;
}

// ----------------------------------------------------------------------------
// removeFederate
/*! BUG: Currently does not check if Federate owns attributes. The Federate
  Object is deleted.
*/
void
Federation::remove(FederateHandle federate_handle)
    throw (FederateOwnsAttributes, FederateNotExecutionMember)
{
    for (list<Federate *>::iterator i = begin(); i != end(); i++) {
        if ((*i)->getHandle() == federate_handle) {
            // BUG: RemoveFederate: Should see if Federate owns attributes
            delete (*i);
            erase(i);

            D.Out(pdInit, "Federation %d: Removed Federate %d.", handle,
                  federate_handle);
            return ;
        }
    }

    D.Out(pdExcept, "Federation %d could not remove unknown federate %d.",
          handle, federate_handle);
    throw FederateNotExecutionMember();
}

// ----------------------------------------------------------------------------
//! Set Federate's IsConstrained to false.
void
Federation::removeConstrained(FederateHandle federate_handle)
    throw (FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember
    Federate *federate = getByHandle(federate_handle);

    if (federate->isConstrained() == false) {
        D.Out(pdExcept, "Federate %d was not constrained.", federate_handle);
        throw RTIinternalError("Time constrained not enabled.");
    }

    federate->setConstrained(false);
    D.Out(pdTerm, "Federation %d: Federate %d is not constrained anymore.",
          handle, federate_handle);
}

// ----------------------------------------------------------------------------
//! Remove the Federate from the Regulators' list.
void
Federation::removeRegulator(FederateHandle federate_handle)
    throw (FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember
    Federate *federate = getByHandle(federate_handle);

    // It may throw RTIinternalError if Federate was not regulators.
    regulators.remove(federate_handle);

    federate->setRegulator(false);

    D.Out(pdTerm, "Federation %d: Federate %d is not a regulator anymore.",
          handle, federate_handle);

    NetworkMessage msg ;
    msg.type = m_SET_TIME_REGULATING ;
    msg.exception = e_NO_EXCEPTION ;
    msg.federation = handle ;
    msg.federate = federate_handle ;
    msg.regulator = RTI_FALSE ;
    msg.date = 0 ;

    broadcastAnyMessage(&msg, 0);
}

// ----------------------------------------------------------------------------
//! Return 'IDCount' unique IDs, ranging from FirstID to LastID.
void
Federation::requestId(ObjectHandlecount id_count,
                      ObjectHandle &first_id,
                      ObjectHandle &last_id)
    throw (TooManyIDsRequested)
{
    // BUG: Should note a security threat for this service.

    if (nextObjectId + id_count - 1 > MAX_OBJECTID)
        throw TooManyIDsRequested();

    first_id = nextObjectId ;
    last_id = nextObjectId + id_count - 1 ;

    D.Out(pdInit, "Federation %d returns %d new IDs starting at %d.",
          handle, id_count, first_id);

    nextObjectId = last_id + 1 ;
}

// ----------------------------------------------------------------------------
//! unregisterSynchronization.
void
Federation::unregisterSynchronization(FederateHandle federate_handle,
                                      const char *label)
    throw (FederateNotExecutionMember,
           FederationNotPaused,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    this->check(federate_handle); // It may throw FederateNotExecutionMember.

    if ((label == NULL) || (strlen(label) > MAX_USER_TAG_LENGTH))
        throw RTIinternalError("Bad pause label(null or too long).");

    // Set federate synchronized on this label.
    Federate *federate = getByHandle(federate_handle);
    federate->removeSynchronizationLabel(label);

    // Test in every federate is synchronized. Otherwise, quit method.
    list<Federate *>::iterator j ;
    for (j = begin(); j != end(); j++) {
        if ((*j)->isSynchronizationLabel(label) == true)
            return ;
    }

    // All federates from federation has called synchronizationPointAchieved.

    D.Out(pdTerm, "Federation %d is not Paused anymore.", handle);
    // Remove label from federation list.
    std::map<const char *, const char *>::iterator i ;
    i = synchronizationLabels.begin();
    for (; i != synchronizationLabels.end(); i++) {
        if (!strcmp((*i).first, label)) {
            delete (*i).first ;
            delete (*i).second ;
            synchronizationLabels.erase(i);
            break ;
        }
    }

    // send a federationSynchronized().
    NetworkMessage msg ;
    msg.type = m_FEDERATION_SYNCHRONIZED ;
    msg.exception = e_NO_EXCEPTION ;
    msg.federation = handle ;
    msg.federate = federate_handle ;
    msg.setLabel(label);

    broadcastAnyMessage(&msg, 0);

    D.Out(pdTerm, "Federation %d is synchronized on %s.", handle, label);
}

// ----------------------------------------------------------------------------
// subscribeInteraction

void
Federation::subscribeInteraction(FederateHandle federate,
                                 InteractionClassHandle interaction,
                                 bool sub)
    throw (InteractionClassNotDefined,
           FederateNotExecutionMember,
           SaveInProgress,
           SecurityError,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    // It may throw *NotDefined
    root->Interactions->subscribe(federate, interaction, sub);
    D.Out(pdRegister,
          "Federation %d: Federate %d(un)subscribes to Interaction %d.",
          handle, federate, interaction);
}

// ----------------------------------------------------------------------------
// subscribeObject

void
Federation::subscribeObject(FederateHandle federate,
                            ObjectClassHandle object,
                            AttributeHandle *attributes,
                            UShort list_size,
                            bool sub)
    throw (ObjectClassNotDefined,
           AttributeNotDefined,
           FederateNotExecutionMember,
           SaveInProgress,
           SecurityError,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    // It may throw *NotDefined
    root->ObjectClasses->subscribe(federate, object, attributes, list_size, sub);
    D.Out(pdRegister,
          "Federation %d: Federate %d(un)sub. to %d attrib. of ObjClass %d.",
          handle, federate, list_size, object);
}

// ----------------------------------------------------------------------------
// updateAttributeValues

void
Federation::updateAttributeValues(FederateHandle federate,
                                  ObjectHandle id,
                                  AttributeHandle *attributes,
                                  AttributeValue *values,
                                  UShort list_size,
                                  FederationTime time,
                                  const char *tag)
    throw (FederateNotExecutionMember,
           ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    // It may throw *NotDefined
    root->ObjectClasses->updateAttributeValues(federate, id, attributes, values,
                                               list_size, time, tag);

    D.Out(pdRegister,
          "Federation %d: Federate %d updated attributes of Object %d.",
          handle, federate, id);
}

// ----------------------------------------------------------------------------
//! Update the current time of a regulator federate.
void
Federation::updateRegulator(FederateHandle federate_handle,
                            FederationTime time)
    throw (FederateNotExecutionMember,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember
    Federate *federate = getByHandle(federate_handle);

    if (federate->isRegulator() == false) {
        D.Out(pdExcept, "Federate %d is not a regulator.", federate_handle);
        throw RTIinternalError("Time regulation not enabled.");
    }

    D.Out(pdTerm, "Federation %d: Federate %d's new time is %f.",
          handle, federate_handle, time);

    regulators.update(federate_handle, time);

    NetworkMessage msg ;
    msg.type = m_MESSAGE_NULL ;
    msg.exception = e_NO_EXCEPTION ;
    msg.federation = handle ;
    msg.federate = federate_handle ;
    msg.date = time ;

    broadcastAnyMessage(&msg, federate_handle);
}

// ----------------------------------------------------------------------------
// isOwner (isAttributeOwnedByFederate)
bool
Federation::isOwner(FederateHandle federate,
                    ObjectHandle id,
                    AttributeHandle attribute)
    throw (FederateNotExecutionMember,
           ObjectNotKnown,
           AttributeNotDefined,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    D.Out(pdDebug, "Owner of Object %u Atrribute %u", id, attribute);

    // It may throw *NotDefined
    return root->objects->isAttributeOwnedByFederate(id, attribute, federate);
}

// ----------------------------------------------------------------------------
// queryAttributeOwnership

void
Federation::queryAttributeOwnership(FederateHandle federate,
                                    ObjectHandle id,
                                    AttributeHandle attribute)
    throw (FederateNotExecutionMember,
           ObjectNotKnown,
           AttributeNotDefined,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    D.Out(pdDebug, "Owner of Object %u Atrribute %u", id, attribute);

    // It may throw *NotDefined
    root->ObjectClasses->queryAttributeOwnership(id, attribute, federate);
}

// ----------------------------------------------------------------------------
// negotiateDivestiture (negotiatedAttributeOwnershipDivestiture)

void
Federation::negotiateDivestiture(FederateHandle federate,
                                 ObjectHandle id,
                                 AttributeHandle *attribs,
                                 UShort list_size,
                                 const char *tag)
    throw (FederateNotExecutionMember,
           ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           AttributeAlreadyBeingDivested,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    // It may throw *NotDefined
    root->ObjectClasses->negotiatedAttributeOwnershipDivestiture(federate,
                                                                 id,
                                                                 attribs,
                                                                 list_size,
                                                                 tag);
}

// ----------------------------------------------------------------------------
// acquireIfAvailable (attributeOwnershipAcquisitionIfAvailable)

void
Federation::acquireIfAvailable(FederateHandle federate,
                               ObjectHandle id,
                               AttributeHandle *attribs,
                               UShort list_size)
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
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    // It may throw *NotDefined
    root->ObjectClasses->attributeOwnershipAcquisitionIfAvailable(federate,
                                                                  id,
                                                                  attribs,
                                                                  list_size);
}

// ----------------------------------------------------------------------------
// divest (unconditionalAttributeOwnershipDivestiture)

void
Federation::divest(FederateHandle federate,
                   ObjectHandle id,
                   AttributeHandle *attrs,
                   UShort list_size)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    // It may throw *NotDefined
    root->ObjectClasses->unconditionalAttributeOwnershipDivestiture(federate,
                                                                    id,
                                                                    attrs,
                                                                    list_size);
}

// ----------------------------------------------------------------------------
// attributeOwnershipAcquisition

void
Federation::acquire(FederateHandle federate,
                    ObjectHandle id,
                    AttributeHandle *attributes,
                    UShort list_size,
                    const char *tag)
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
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    // It may throw *NotDefined
    root->ObjectClasses->attributeOwnershipAcquisition(federate,
                                                       id,
                                                       attributes,
                                                       list_size,
                                                       tag);

    D.Out(pdDebug, "Acquisition on Object %u ", id);
}

// ----------------------------------------------------------------------------
// cancelDivestiture (cancelNegotiatedAttributeOwnershipDivestiture)

void
Federation::cancelDivestiture(FederateHandle federate,
                              ObjectHandle id,
                              AttributeHandle *attributes,
                              UShort list_size)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           AttributeDivestitureWasNotRequested,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    // It may throw *NotDefined
    root->ObjectClasses->
        cancelNegotiatedAttributeOwnershipDivestiture(federate,
                                                      id,
                                                      attributes,
                                                      list_size);

    D.Out(pdDebug, "CancelDivestiture sur Objet %u ", id);
}

// ----------------------------------------------------------------------------
// respondRelease (attributeOwnershipRealeaseResponse)

AttributeHandleSet*
Federation::respondRelease(FederateHandle federate,
                           ObjectHandle id,
                           AttributeHandle *attributes,
                           UShort list_size)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeNotOwned,
           FederateWasNotAskedToReleaseAttribute,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    D.Out(pdDebug, "RespondRelease on Object %u.", id);

    // It may throw *NotDefined
    return(root->ObjectClasses->attributeOwnershipReleaseResponse(federate,
                                                                  id,
                                                                  attributes,
                                                                  list_size));
}

// ----------------------------------------------------------------------------
// cancelAttributeOwnershipAcquisition

void
Federation::cancelAcquisition(FederateHandle federate,
                              ObjectHandle id,
                              AttributeHandle *attributes,
                              UShort list_size)
    throw (ObjectNotKnown,
           AttributeNotDefined,
           AttributeAlreadyOwned,
           AttributeAcquisitionWasNotRequested,
           FederateNotExecutionMember,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    // It may throw FederateNotExecutionMember.
    this->check(federate);

    D.Out(pdDebug, "CancelAcquisition sur Objet %u ", id);

    // It may throw *NotDefined
    root->ObjectClasses->cancelAttributeOwnershipAcquisition(federate,
                                                             id,
                                                             attributes,
                                                             list_size);
}

// ----------------------------------------------------------------------------
long
Federation::createRegion(FederateHandle federate,
                         SpaceHandle space,
                         long nb_extents)
    throw (SpaceNotDefined, InvalidExtents, SaveInProgress, RestoreInProgress,
           RTIinternalError)
{
    this->check(federate);

    return root->createRegion(space, nb_extents);
}

// ----------------------------------------------------------------------------
void
Federation::deleteRegion(FederateHandle federate,
                         long region)
    throw (RegionNotKnown, RegionInUse, SaveInProgress, RestoreInProgress,
           RTIinternalError)
{
    this->check(federate);

    if (saveInProgress) {
        throw SaveInProgress();
    }
    if (restoreInProgress) {
        throw RestoreInProgress();
    }

    root->deleteRegion(region);
}

}} // namespace certi/rtig

// $Id: Federation.cc,v 3.16 2003/04/23 17:24:08 breholee Exp $

