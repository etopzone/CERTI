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
// $Id: FederationManagement.cc,v 3.5 2003/02/19 15:45:22 breholee Exp $
// ----------------------------------------------------------------------------

#include "FederationManagement.hh"

namespace certi {
namespace rtia {

static pdCDebug D("RTIA_FM", "(RTIA FM) ");

// ------------------------
// -- FederationManagement --
// ------------------------

FederationManagement::FederationManagement(Communications *GC)
{
    comm = GC ;
    tm = NULL ;

    _numero_federation = 0 ;
    federate = 0 ;

    _fin_execution = RTI_FALSE ;

    _est_createur_federation = RTI_FALSE ;
    _est_membre_federation = RTI_FALSE ;

    _est_pause = RTI_FALSE ;
    _RTI_demande_pause = RTI_FALSE ;
    _RTI_demande_fin_pause = RTI_FALSE ;

    _nom_federation[0] = 0 ;
    _nom_federe[0] = 0 ;
    _label_pause[0] = 0 ;
}


// -------------------------
// -- ~FederationManagement --
// -------------------------

FederationManagement::~FederationManagement()
{
    TypeException e ;

    if (_est_membre_federation) {
        resignFederationExecution(DELETE_OBJECTS, e);
    }

    // BUG: On devrait pouvoir quitter une federation que l'on a cree
    // sans la detruire en partant.
    // a-t-on avertit les autres federes de notre depart et donc
    // de la destruction de notre objet par RemoveObject(HARZI)
    // car le Remove Object ne diffuse pas le message
    if (_est_createur_federation) {
        printf("RTIA: Je reste actif pour detruire la federation...\n");

        destroyFederationExecution(_nom_federation, e);
        while (e != e_NO_EXCEPTION)
            {
                sleep(1);
                destroyFederationExecution(_nom_federation, e);
            }
        printf("RTIA: Federation detruite.\n");
    }
}

// ----------------------------------------------------------------------------
//! createFederationExecution.
void
FederationManagement::
createFederationExecution(const char *theName,
                          TypeException &e)
{
    NetworkMessage requete, reponse ;

    D.Out(pdInit, "Creating Federation %s.", theName);

    e = e_NO_EXCEPTION ;

    // BUG: pour l'instant, le federe ne peut creer q'une federation
    if (_est_createur_federation || _est_membre_federation)
        e = e_RTIinternalError ;

    if (e == e_NO_EXCEPTION) {
        requete.type = m_CREATE_FEDERATION_EXECUTION ;
        strcpy(requete.federationName, theName);

        comm->sendMessage(&requete);

        comm->waitMessage(&reponse, m_CREATE_FEDERATION_EXECUTION,
                          federate);

        if (reponse.exception == e_NO_EXCEPTION) {
            strcpy(_nom_federation, theName);
            _numero_federation = reponse.federation ;
            _est_createur_federation = RTI_TRUE ;
            D.Out(pdInit, "est createur");
        }
        else {
            e = reponse.exception ;
            D.Out(pdInit, "deja cree");
        }
    }
}


// ----------------------------------------------------------------------------
//! destroyFederationExecution.
void
FederationManagement::
destroyFederationExecution(const char *theName,
                           TypeException &e)
{
    NetworkMessage requete, reponse ;

    D.Out(pdInit, "Destroy Federation %s.", theName);

    e = e_NO_EXCEPTION ;

    // BUG: On devrait pouvoir detruire une federation meme si on est
    // pas le createur.
    if (!_est_createur_federation || strcmp(theName, _nom_federation))
        e = e_FederationExecutionDoesNotExist ;

    if (e == e_NO_EXCEPTION) {
        requete.type = m_DESTROY_FEDERATION_EXECUTION ;
        requete.federation = _numero_federation ;
        requete.federate = federate ;
        strcpy(requete.federationName, theName);

        printf("EMISSION DE LA REQUETE\n");
        comm->sendMessage(&requete);
        printf("ATTENTE DE LA REPONSE\n");
        comm->waitMessage(&reponse,
                          m_DESTROY_FEDERATION_EXECUTION,
                          federate);

        if (reponse.exception == e_NO_EXCEPTION) {
            _nom_federation[0] = 0 ;
            _numero_federation = 0 ;
            _est_createur_federation = RTI_FALSE ;
            _fin_execution = RTI_TRUE ;
        }
        else
            e = reponse.exception ;
    }
}

// ----------------------------------------------------------------------------
//! joinFederationExecution.
FederateHandle
FederationManagement::
joinFederationExecution(const char *Federate,
                        const char *Federation,
                        TypeException &e)
{
    NetworkMessage requete, reponse ;
    int i, nb ;

    D.Out(pdInit, "Join Federation %s as %s.", Federation, Federate);

    e = e_NO_EXCEPTION ;

    if (_est_createur_federation && strcmp(Federation, _nom_federation))
        e = e_RTIinternalError ;

    if (_est_membre_federation)
        e = e_FederateAlreadyExecutionMember ;

    if (e == e_NO_EXCEPTION) {
        requete.type = m_JOIN_FEDERATION_EXECUTION ;
        strcpy(requete.federationName, Federation);
        strcpy(requete.federateName, Federate);

        requete.bestEffortAddress = comm->getAddress();
        requete.bestEffortPeer = comm->getPort();

        comm->sendMessage(&requete);

        // On attend la reponse du RTIG(de n'importe quel federe)
        comm->waitMessage(&reponse, m_JOIN_FEDERATION_EXECUTION, 0);

        // Si c'est positif, cette reponse contient le nombre de regulateurs.
        // On attend alors un message NULL de chacun d'eux.
        if (reponse.exception == e_NO_EXCEPTION) {
            strcpy(_nom_federation, Federation);
            strcpy(_nom_federe, Federate);

            _numero_federation = reponse.federation ;
            federate = reponse.federate ;
            tm->MyFederateNumber = reponse.federate ;

#ifdef FEDERATION_USES_MULTICAST
            // creation du socket pour la communication best-effort
            comm->CreerSocketMC(reponse.AdresseMulticast, MC_PORT);
#endif

            nb = reponse.numberOfRegulators ;
            for (i=0 ; i<nb ; i++) {
                comm->waitMessage(&reponse, m_MESSAGE_NULL, 0);
                assert(tm != NULL);
                tm->insert(reponse.federate, reponse.date);
            }

            _est_membre_federation = RTI_TRUE ;
            return(federate);
        }
        else
            e = reponse.exception ;
    }

    return(0);
}

// -------------------------------
// -- resignFederationExecution --
// -------------------------------

void
FederationManagement::resignFederationExecution(ResignAction,
                                                TypeException &e)
{
    NetworkMessage msg ;
    TypeException exception = e_NO_EXCEPTION ;

    e = e_NO_EXCEPTION ;

    D.Out(pdInit, "Resign Federation.");

    if (!_est_membre_federation)
        e = e_FederateNotExecutionMember ;

    if (e == e_NO_EXCEPTION) {
        tm->StopperAvanceTemps();

        if (tm->requestRegulateurState() == RTI_TRUE)
            tm->setTimeRegulating(RTI_FALSE, exception);

        msg.type = m_RESIGN_FEDERATION_EXECUTION ;
        msg.federation = _numero_federation ;
        msg.federate = federate ;

        comm->sendMessage(&msg);

        _est_membre_federation = RTI_FALSE ;
        _numero_federation = 0 ;
        federate = 0 ;

        // BUG: Voir DestroyFederation ou ~GF.
        if (!_est_createur_federation)
            _fin_execution = RTI_TRUE ;

    }
}

// ----------------------------------------------------------------------------
//! requestPause.
void
FederationManagement::requestPause(const char *label, TypeException &e)
{
    NetworkMessage req, rep ;

    D.Out(pdProtocol, "RequestPause, _est_pause = %d.", _est_pause);

    assert(label != NULL);

    e = e_NO_EXCEPTION ;

    if (_est_pause)
        e = e_FederationAlreadyPaused ;

    if (!_est_membre_federation)
        e = e_FederateNotExecutionMember ;

    if (e == e_NO_EXCEPTION) {
        req.type = m_REQUEST_PAUSE ;
        strcpy(req.label, label);
        req.federation = _numero_federation ;
        req.federate = federate ;

        comm->sendMessage(&req);

        comm->waitMessage(&rep, m_REQUEST_PAUSE, federate);

        if (rep.exception == e_NO_EXCEPTION) {
            assert(_est_pause == RTI_FALSE);
            assert(_RTI_demande_pause == RTI_FALSE);
            assert(_RTI_demande_fin_pause == RTI_FALSE);

            _est_pause = RTI_TRUE ;
            strcpy(_label_pause, label);
        }
        else
            e = rep.exception ;
    }
}

// ----------------------------------------------------------------------------
//! pauseAchieved.
void
FederationManagement::pauseAchieved(const char *label, TypeException &e)
{
    assert(label != NULL);

    e = e_NO_EXCEPTION ;

    D.Out(pdProtocol, "PauseAchieved, _est_pause = %d.", _est_pause);

    if (!_RTI_demande_pause)
        e = e_NoPauseRequested ;

    // si label different du label de la pause
    if (strcmp(_label_pause, label))
        e = e_UnknownLabel ;

    if (!_est_membre_federation)
        e = e_FederateNotExecutionMember ;

    if (e == e_NO_EXCEPTION) {
        assert(_RTI_demande_pause == RTI_TRUE);
        assert(_est_pause == RTI_FALSE);
        assert(_RTI_demande_fin_pause == RTI_FALSE);

        _RTI_demande_pause = RTI_FALSE ;

        _est_pause = RTI_TRUE ;
    }
}

// ----------------------------------------------------------------------------
//! requestResume.
void
FederationManagement::requestResume(const char *label, TypeException &e)
{
    NetworkMessage req ;

    D.Out(pdProtocol, "RequestResume, _est_pause = %d.", _est_pause);

    assert(label != NULL);

    e = e_NO_EXCEPTION ;

    if (!_est_pause)
        e = e_FederationNotPaused ;

    if (!_est_membre_federation)
        e = e_FederateNotExecutionMember ;

    if (e == e_NO_EXCEPTION) {
        assert(_est_pause == RTI_TRUE);
        assert(_RTI_demande_pause == RTI_FALSE);
        assert(_RTI_demande_fin_pause == RTI_FALSE);

        req.type = m_REQUEST_RESUME ;
        strcpy(req.label, label);

        req.federation = _numero_federation ;
        req.federate = federate ;

        comm->sendMessage(&req);

        _est_pause = RTI_FALSE ;
        strcpy(_label_pause, label);
    }
}

// ----------------------------------------------------------------------------
//! resumeAchieved
void
FederationManagement::resumeAchieved(TypeException &e)
{
    D.Out(pdProtocol, "ResumeAchieved, _est_pause = %d.", _est_pause);

    e = e_NO_EXCEPTION ;

    if (!_RTI_demande_fin_pause)
        e = e_NoResumeRequested ;

    if (!_est_membre_federation)
        e = e_FederateNotExecutionMember ;

    if (e == e_NO_EXCEPTION) {
        assert(_est_pause == RTI_TRUE);
        assert(_RTI_demande_pause == RTI_FALSE);
        assert(_RTI_demande_fin_pause == RTI_TRUE);

        _RTI_demande_fin_pause = RTI_FALSE ;
        _est_pause = RTI_FALSE ;
    }
}

// ----------------------------------------------------------------------------
//! initiatePause.
void
FederationManagement::initiatePause(const char *label)
{
    Message req, rep ;

    D.Out(pdProtocol, "InitiatePause, _est_pause = %d.", _est_pause);

    assert(label != NULL);

    req.type = INITIATE_PAUSE ;
    req.setLabel(label);

    comm->requestFederateService(&req, &rep);

    // BUG: A remplacer par if (exception) throw RTIinternalError.
    assert(rep.exception == e_NO_EXCEPTION);

    if (rep.exception == e_NO_EXCEPTION) {
        assert(_est_pause == RTI_FALSE);
        assert(_RTI_demande_pause == RTI_FALSE);
        assert(_RTI_demande_fin_pause == RTI_FALSE);

        _RTI_demande_pause = RTI_TRUE ;
        strcpy(_label_pause, label);
    }
}

// ----------------------------------------------------------------------------
//! initiateResume.
void
FederationManagement::initiateResume(const char *label)
{
    Message req, rep ;

    D.Out(pdProtocol, "InitiateResume, _est_pause = %d.", _est_pause);

    assert(label != NULL);

    req.type = INITIATE_RESUME ;
    req.setLabel(label);

    comm->requestFederateService(&req, &rep);
    // BUG: A remplacer par if (exception) throw RTIinternalError.
    assert(rep.exception == e_NO_EXCEPTION);

    if (rep.exception == e_NO_EXCEPTION) {
        assert(_est_pause == RTI_TRUE);
        assert(_RTI_demande_pause == RTI_FALSE);
        assert(_RTI_demande_fin_pause == RTI_FALSE);

        _RTI_demande_fin_pause = RTI_TRUE ;
        strcpy(_label_pause, label);
    }
}

}} // namespace certi/rtia

// $Id: FederationManagement.cc,v 3.5 2003/02/19 15:45:22 breholee Exp $
