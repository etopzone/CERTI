// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003, 2004  ONERA
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
// $Id: Federation.cc,v 3.37.2.1 2004/07/02 12:18:53 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "Federation.hh"

#include "fed.hh"
#include "XmlParser.hh"
#include "ObjectClassAttribute.hh"
#include "PrettyDebug.hh"

#include <map>
#include <fstream>
#include <iostream>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

using std::pair ;
using std::ifstream ;
using std::ios ;
using std::cout ;
using std::endl ;
using std::string ;
using std::list ;
using std::cerr ;
using std::vector ;

// Definitions
#ifdef HAVE_XML
#define ROOT_NODE (const xmlChar*) "rtigSaveData"
#define NODE_FEDERATION (const xmlChar*) "federation"
#define NODE_FEDERATE (const xmlChar*) "federate"
#endif // HAVE_XML

namespace certi {

class XmlParser ;

namespace rtig {

static PrettyDebug D("FEDERATION", __FILE__);

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
                           Handle federation_handle,
                           SocketServer *socket_server,
                           AuditFile *audit_server)
#endif
    throw (CouldNotOpenRID, ErrorReadingRID, MemoryExhausted, SecurityError,
           RTIinternalError)
    : saveInProgress(false), restoreInProgress(false), saveStatus(true),
      restoreStatus(true), verbose(true)
{
    //    fedparser::FedParser *fed_reader ;

#ifdef FEDERATION_USES_MULTICAST // -----------------
    // Initialize Multicast
    if (mc_link == NULL) {
        D.Out(pdExcept, "Null Multicast socket for new Federation.");
        throw RTIinternalError("NULL Multicast socket for new Federation.");
    }

    D.Out(pdInit, "New Federation %d will use Multicast.", federation_handle);
    MCLink = mc_link ;
#endif // FEDERATION_USES_MULTICAST // --------------

    // Allocates Name
    if ((federation_name == 0) || (federation_handle == 0))
        throw RTIinternalError("Null init parameter in Federation creation.");

    if (strlen(federation_name) > MAX_FEDERATION_NAME_LENGTH)
        throw RTIinternalError("Federation name too long.");

    name = strdup(federation_name);

    // Default Attribute values
    handle = federation_handle ;

    D.Out(pdInit, "New Federation created with Handle %d, now reading FOM.",
          handle);

    // Initialize the Security Server.
    server = new SecurityServer(socket_server, audit_server, handle);
    if (server == NULL) throw RTIinternalError();

    // Read FOM File to initialize Root Object.
    root = new RootObject(server);

    cout << "New federation: " << name << endl ;
    cout << "Looking for .fed file... " ;

    string filename = string(name) + ".fed" ;
    ifstream fdd(filename.c_str());

    if (fdd.is_open()) {
	fdd.close();

        cout << "yes" << endl ;
	int err = fedparser::build(filename.c_str(), root, verbose);
	if (err) throw ErrorReadingFED();
	    
        // Retrieve the FED file last modification time(for Audit)
        struct stat StatBuffer ;
        char *MTimeBuffer ;

        if (stat(filename.c_str(), &StatBuffer) == 0) {
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
            fdd.open(filename.c_str());

            if (fdd.is_open()) {
		fdd.close();
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
            }
            else {
                cout << "no" << endl ;
		throw CouldNotOpenFED();
	    }
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
//! Returns the number of federates in federation.
int
Federation::getNbFederates() const
{
    return size();
}

// ----------------------------------------------------------------------------
//! Return true if federation is being synchronized.
bool
Federation::isSynchronizing() const
{
    return !synchronizationLabels.empty();
}

// ----------------------------------------------------------------------------
//! Returns the federation handle.
Handle
Federation::getHandle() const
{
    return handle ;
}

// ----------------------------------------------------------------------------
//! Returns the federation name given in 'Create Federation Execution'.
const char *
Federation::getName() const
{
    return name ;
}

// ----------------------------------------------------------------------------
//! Returns the number of federates regulator.
int
Federation::getNbRegulators() const
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

    FederateHandle federate_handle = federateHandles.provide();
    Federate *federate = new Federate(federate_name, federate_handle);
    push_front(federate);
    D.Out(pdInit, "Federate %d joined Federation %d.", federate_handle, handle);

    // Send, to the newly added federate, a Null message from each regulating
    // federate (i) with their logical time h(i). This permits to calculate
    // its LBTS.
    NetworkMessage message ;
    try {
        for (unsigned int i = 1 ; i <= regulators.size(); ++i) {
            message.type = NetworkMessage::MESSAGE_NULL ;
            message.federation = handle ;

            regulators.get(i, message.federate, message.date);

            D.Out(pdTerm, "Sending NULL message(type %d) from %d to new federate.",
                  message.type, message.federate);

            message.write(tcp_link);
        }

        // If federation is synchronizing, put federate in same state.
        if (isSynchronizing()) {
            message.type = NetworkMessage::ANNOUNCE_SYNCHRONIZATION_POINT ;
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
    msg.type = NetworkMessage::SET_TIME_REGULATING ;
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
/** Removes an object instance from federation.
    @param federate Federate requesting removal
    @param id Object handle
    @param tag Label for this operation
 */
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
    objectHandles.free(id);
}

// ----------------------------------------------------------------------------
//! Add a new synchronization point to federation.
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
/*! Broadcast an 'Announce Synchronization Point' when registering a new
  synchronization point.
*/
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
    msg.type = NetworkMessage::ANNOUNCE_SYNCHRONIZATION_POINT ;
    msg.federate = federate ;
    msg.federation = handle ;
    msg.setLabel(label);
    msg.setTag(tag);

    broadcastAnyMessage(&msg, 0);
}

// ----------------------------------------------------------------------------
//! Request a federation save.
/*! This service puts each federate from federation in saving state.
  !! the_time is not managed yet.
*/
void
Federation::requestFederationSave(FederateHandle the_federate,
                                  const char *the_label,
                                  FederationTime /* the_time */)
    throw (FederateNotExecutionMember, SaveInProgress)
{
    check(the_federate);

    if (saveInProgress)
        throw SaveInProgress("Already in saving state.");

    for (list<Federate *>::iterator j = begin(); j != end(); j++) {
        (*j)->setSaving(true);
    }

    saveStatus = true ;
    saveInProgress = true ;
    saveLabel = the_label ;

    NetworkMessage msg ;
    msg.type = NetworkMessage::INITIATE_FEDERATE_SAVE ;
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
//! Informs that a federate returns a save end message (with success or not!).
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

    // Save RTIG Data for future restoration.
    if (saveStatus) {
        saveStatus = saveXmlData();
    }

    // Send end save message.
    NetworkMessage msg ;

    msg.type = saveStatus ? NetworkMessage::FEDERATION_SAVED : NetworkMessage::FEDERATION_NOT_SAVED ;

    msg.federate = the_federate ;
    msg.federation = handle ;

    broadcastAnyMessage(&msg, 0);

    // Reinitialize state.
    saveStatus = true ;
    saveInProgress = false ;
}

// ----------------------------------------------------------------------------
//! Informs that a federate is requesting a save.
void
Federation::requestFederationRestore(FederateHandle the_federate,
                                     const char *the_label)
    throw (FederateNotExecutionMember)
{
    check(the_federate);

    if (restoreInProgress)
        throw RestoreInProgress("Already in restoring state.");

    Socket * socket ;

    NetworkMessage * msg = new NetworkMessage ;
    msg->federate = the_federate ;
    msg->federation = handle ;
    msg->setLabel(the_label);

    // Informs sending federate of success/failure in restoring.
    // At this point, only verify that file is present.
    bool success = true ;
#ifdef HAVE_XML
    string filename = string(name) + "_" + string(the_label) + ".xcs" ;
    doc = xmlParseFile(filename.c_str());

    // Did libXML manage to parse the file ?
    if (doc == 0) {
        cerr << "XML restore file not parsed successfully" << endl ;
        xmlFreeDoc(doc);
        success = false ;
    }
    if (success) {
        success = restoreXmlData();
    }
#else
    success = false ;
#endif // HAVE_XML

    msg->type = success ?
        NetworkMessage::REQUEST_FEDERATION_RESTORE_SUCCEEDED
        : NetworkMessage::REQUEST_FEDERATION_RESTORE_FAILED ;

    socket = server->getSocketLink(msg->federate);
    msg->write(socket);
    delete msg ;

    // Reading file failed: not restoring !
    if (!success)
        return ;

    // Otherwise...
    list<Federate *>::iterator j ;
    for (j = begin(); j != end(); j++) {
        (*j)->setRestoring(true);
    }
    restoreStatus = true ;
    restoreInProgress = true ;

    // Informs federates a new restore is being done.
    msg = new NetworkMessage ;
    msg->federate = the_federate ;
    msg->federation = handle ;
    msg->type = NetworkMessage::FEDERATION_RESTORE_BEGUN ;

    broadcastAnyMessage(msg, 0);
    delete msg ;

    // For each federate, send an initiateFederateRestore with correct handle.
    msg = new NetworkMessage ;
    msg->federation = handle ;
    msg->setLabel(the_label);
    msg->type = NetworkMessage::INITIATE_FEDERATE_RESTORE ;

    list<Federate *>::const_iterator k ;
    for (k = begin(); k != end(); k++) {
        msg->federate = (*k)->getHandle();

        // send message.
        socket = server->getSocketLink(msg->federate);
        msg->write(socket);
    }
}

// ----------------------------------------------------------------------------
//! Informs that a federate has ended a restore.
/*! If each federate in federation has ended restoring, this service send a
  federation restore status.
*/
void
Federation::federateRestoreStatus(FederateHandle the_federate,
                                  bool the_status)
    throw (FederateNotExecutionMember)
{
    Federate * federate = getByHandle(the_federate);
    federate->setRestoring(false);

    if (!the_status)
        restoreStatus = false ;

    // Verify that all federates save ended (complete or not).
    list<Federate *>::iterator j ;
    for (j = begin(); j != end(); j++) {
        if ((*j)->isRestoring() == true)
            return ;
    }

    // Send end restore message.
    NetworkMessage msg ;

    if (restoreStatus)
        msg.type = NetworkMessage::FEDERATION_RESTORED ;
    else
        msg.type = NetworkMessage::FEDERATION_NOT_RESTORED ;

    msg.federate = the_federate ;
    msg.federation = handle ;

    broadcastAnyMessage(&msg, 0);

    // Reinitialize state.
    restoreStatus = true ;
    restoreInProgress = false ;
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
//! Return true if no federates are in federation.
/*! Return true if there are no Federates left in the Federation, else throw
  FederatesCurrentlyJoined.
*/
bool
Federation::empty() const
    throw (FederatesCurrentlyJoined)
{
    if (list<Federate *>::empty())
        return true ;
    else
        throw FederatesCurrentlyJoined();
}

// ----------------------------------------------------------------------------
//! Return true if federate is in federation.
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
/** Make a federate resign the federation. This function tries to
    remove all references to this federate in the federation. To be used
    when a Federate is supposed to have crashed.
    @param federate Handle of the federate to kill
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
//! Adds a new object instance to federation.
ObjectHandle
Federation::registerObject(FederateHandle federate,
                           ObjectClassHandle class_handle,
                           const char *object_name)
    throw (FederateNotExecutionMember,
           FederateNotPublishing,
           ObjectAlreadyRegistered,
           ObjectClassNotDefined,
           ObjectClassNotPublished,
           SaveInProgress,
           RestoreInProgress,
           RTIinternalError)
{
    ObjectHandle new_id = objectHandles.provide();

    D.Out(pdRegister,
          "Federation %d: Federate %d registering Object %d of Class %d.",
          handle, federate, new_id, class_handle);

    // create a name if necessary
    string strname = "" ;
    strname += object_name ? string(object_name) : "HLA" + new_id ;

    // Register Object.
    root->registerObjectInstance(federate, class_handle, new_id, 
				 strname.c_str());
    return new_id ;
}

// ----------------------------------------------------------------------------
/** Remove a federate.
    @param federate_handle Handle of the federate to remove.
    @bug Currently does not check if Federate owns attributes. The
    Federate Object is deleted.
*/
void
Federation::remove(FederateHandle federate_handle)
    throw (FederateOwnsAttributes, FederateNotExecutionMember)
{
    for (list<Federate *>::iterator i = begin(); i != end(); i++) {
        if ((*i)->getHandle() == federate_handle) {
            // BUG: RemoveFederate: Should see if Federate owns attributes
	    federateHandles.free(federate_handle);
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
    msg.type = NetworkMessage::SET_TIME_REGULATING ;
    msg.exception = e_NO_EXCEPTION ;
    msg.federation = handle ;
    msg.federate = federate_handle ;
    msg.regulator = RTI_FALSE ;
    msg.date = 0 ;

    broadcastAnyMessage(&msg, 0);
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
            // Allocated by strdup().
            free(const_cast<char *>((*i).first));
            free(const_cast<char *>((*i).second));
            synchronizationLabels.erase(i);
            break ;
        }
    }

    // send a federationSynchronized().
    NetworkMessage msg ;
    msg.type = NetworkMessage::FEDERATION_SYNCHRONIZED ;
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
    msg.type = NetworkMessage::MESSAGE_NULL ;
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
    root->objects->queryAttributeOwnership(id, attribute, federate);
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
    root->objects->cancelNegotiatedAttributeOwnershipDivestiture(federate,
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
// modifyRegion
void
Federation::modifyRegion(FederateHandle federate, RegionHandle region,
			 const vector<Extent> &extents)
    throw (RegionNotKnown, InvalidExtents, SaveInProgress, RestoreInProgress,
	   RTIinternalError)
{
    check(federate);
    root->modifyRegion(region, extents);
}

// ----------------------------------------------------------------------------
//! deleteRegion
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

    // TODO: check RegionInUse
    root->deleteRegion(region);
}

// ----------------------------------------------------------------------------
//! associateRegion
void
Federation::associateRegion(FederateHandle federate,
			    ObjectHandle object,
			    RegionHandle the_handle,
			    unsigned short nb,
			    AttributeHandle *attributes)
    throw (RegionNotKnown, SaveInProgress, RestoreInProgress, RTIinternalError)
{
    check(federate);

    RegionImp *region = root->getRegion(the_handle);

    root->getObject(object)->unassociate(region);
	
    for (int i = 0 ; i < nb ; ++i) {
	root->getObjectAttribute(object, attributes[i])->associate(region);
    }    
}

// ----------------------------------------------------------------------------
//! unassociateRegion
void
Federation::unassociateRegion(FederateHandle federate, ObjectHandle object,
			      RegionHandle the_handle)
    throw (RegionNotKnown, SaveInProgress, RestoreInProgress,
	   RTIinternalError)
{
    check(federate);

    RegionImp *region = root->getRegion(the_handle);
    root->getObject(object)->unassociate(region);
}

// ----------------------------------------------------------------------------
//! subscribeAttributesWR
void
Federation::subscribeAttributesWR(FederateHandle federate,
				  ObjectClassHandle c,
				  RegionHandle region_handle,
				  unsigned short nb,
				  AttributeHandle *attributes)
    throw (RegionNotKnown,
	       SaveInProgress,
	       RestoreInProgress,
	       RTIinternalError)
{
    check(federate);

    RegionImp *r = root->getRegion(region_handle);
    
    root->getObjectClass(c)->unsubscribe(federate, r);
    for (int i = 0 ; i < nb ; ++i) {
	root->getObjectClassAttribute(c, attributes[i])->subscribe(federate, r);
    } 
}

// ----------------------------------------------------------------------------
//! unsubscribeAttributesWR
void
Federation::unsubscribeAttributesWR(FederateHandle federate,
				    ObjectClassHandle object_class,
				    RegionHandle region_handle)
	throw (RegionNotKnown,
	       SaveInProgress,
	       RestoreInProgress,
	       RTIinternalError)
{
    check(federate);

    RegionImp *region = root->getRegion(region_handle);

    root->getObjectClass(object_class)->unsubscribe(federate, region);
}

// ----------------------------------------------------------------------------
void
Federation::subscribeInteractionWR(FederateHandle federate,
				   InteractionClassHandle interaction,
				   RegionHandle region_handle)
	throw (RegionNotKnown,
	       SaveInProgress,
	       RestoreInProgress,
	       RTIinternalError)
{
    check(federate);

    RegionImp *region = root->getRegion(region_handle);

    root->getInteractionClass(interaction)->subscribe(federate, region);
}

// ----------------------------------------------------------------------------
void
Federation::unsubscribeInteractionWR(FederateHandle federate,
				     InteractionClassHandle interaction,
				     RegionHandle region_handle)
	throw (RegionNotKnown,
	       SaveInProgress,
	       RestoreInProgress,
	       RTIinternalError)
{
    check(federate);

    RegionImp *region = root->getRegion(region_handle);

    root->getInteractionClass(interaction)->unsubscribe(federate, region);
}

// ----------------------------------------------------------------------------
bool
Federation::restoreXmlData()
{
#ifndef HAVE_XML
    return false ;
#else
    xmlNodePtr cur ;

    cur = xmlDocGetRootElement(doc);
    if (cur == 0) {
        cerr << "XML file is empty" << endl ;
        xmlFreeDoc(doc);
        return false ;
    }

    // Is this root element an ROOT_NODE ?
    if (xmlStrcmp(cur->name, ROOT_NODE)) {
        cerr << "Wrong XML file: not the expected root node" << endl ;
        return false ;
    }

    cur = cur->xmlChildrenNode ;
    if (xmlStrcmp(cur->name, NODE_FEDERATION)) {
        cerr << "Wrong XML file structure" << endl ;
        return false ;
    }

    char *tmp ;
    tmp = (char *)xmlGetProp(cur, (const xmlChar*)"name");
    if (strcmp(name, tmp) != 0) {
        cerr << "Wrong federation name" << endl ;
    }

    cur = cur->xmlChildrenNode ;
    list<Federate *>::iterator i ;
    bool status ;
    while (cur != NULL) {
        if ((!xmlStrcmp(cur->name, NODE_FEDERATE))) {
            for (i = begin(); i != end(); i++) {
                if (!strcmp(
                        (*i)->getName(),
                        (char *) xmlGetProp(cur, (const xmlChar*) "name"))) {
                    // Set federate constrained status
                    if (strcmp("true", (char *)xmlGetProp(
                                   cur, (const xmlChar*)"constrained")) == 0)
                        status = true ;
                    else
                        status = false ;

                    (*i)->setConstrained(status);

                    // Set federate regulating status
                    status = !strcmp("true", (char *) xmlGetProp(
                                         cur, (const xmlChar *) "regulator"));

                    (*i)->setRegulator(status);

                    (*i)->setHandle(
                        strtol((char *) xmlGetProp(
                                   cur, (const xmlChar *) "handle"), 0, 10));
                    break ;
                }
            }
        }
        cur = cur->next ;
    }

    return status ;
#endif // HAVE_XML
}

// ----------------------------------------------------------------------------
bool
Federation::saveXmlData()
{
#ifndef HAVE_XML
    return false ;
#else
    doc = xmlNewDoc((const xmlChar *) "1.0");
    doc->children = xmlNewDocNode(doc, NULL, ROOT_NODE, NULL);

    xmlNodePtr federation ;
    federation = xmlNewChild(doc->children, NULL, NODE_FEDERATION, NULL);

    xmlSetProp(federation, (const xmlChar *) "name", (const xmlChar *) name);

    char t[10] ;
    sprintf(t, "%ld", handle);
    xmlSetProp(federation, (const xmlChar *) "handle", (const xmlChar *) t);

    xmlNodePtr federate ;

    list<Federate *>::iterator i ;
    for (i = begin(); i != end(); i++) {
        federate = xmlNewChild(federation, NULL, NODE_FEDERATE, NULL);

        xmlSetProp(federate,
                   (const xmlChar *)"name",
                   (const xmlChar *)(*i)->getName());

        sprintf(t, "%ld", (*i)->getHandle());
        xmlSetProp(federate, (const xmlChar *)"handle", (const xmlChar *)t);

        xmlSetProp(federate,
                   (const xmlChar *)"constrained", (const xmlChar *)
                   (((*i)->isConstrained()) ? "true" : "false"));
        xmlSetProp(federate, (const xmlChar *)"regulator",
                   (const xmlChar *)(((*i)->isRegulator()) ? "true" : "false"));
    }

    xmlSetDocCompressMode(doc, 9);

    string filename = string(name) + "_" + string(saveLabel) + ".xcs" ;
    xmlSaveFile(filename.c_str(), doc);

    // TODO: tests

    return true ;
#endif // HAVE_XML
}

}} // namespace certi/rtig

// $Id: Federation.cc,v 3.37.2.1 2004/07/02 12:18:53 breholee Exp $

