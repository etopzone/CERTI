// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: Exception.cc,v 3.16 2008/11/25 16:32:14 gotthardp Exp $
// ----------------------------------------------------------------------------

#include "Exception.hh"
#include "certi.hh"

// static members for HLA Exceptions
const char *certi::FederateNotPublishing::_ex = "FederateNotPublishing" ;
const char *certi::FederateNotSubscribing::_ex = "FederateNotSubscribing" ;
const char *certi::InvalidObjectHandle::_ex = "InvalidObjectHandle" ;
const char *certi::SecurityError::_ex = "SecurityError" ;
const char *certi::CouldNotOpenRID::_ex = "CouldNotOpenRID" ;
const char *certi::ErrorReadingRID::_ex = "ErrorReadingRID" ;
const char *certi::FederationAlreadyPaused::_ex = "FederationAlreadyPaused" ;
const char *certi::FederationNotPaused::_ex = "FederationNotPaused" ;
const char *certi::AttributeNotSubscribed::_ex = "AttributeNotSubscribed" ;
const char *certi::FederateAlreadyPaused::_ex = "FederateAlreadyPaused" ;
const char *certi::FederateDoesNotExist::_ex = "FederateDoesNotExist" ;
const char *certi::FederateNameAlreadyInUse::_ex = "FederateNameAlreadyInUse" ;
const char *certi::FederateNotPaused::_ex = "FederateNotPaused" ;
const char *certi::IDsupplyExhausted::_ex = "IDsupplyExhausted" ;
const char *certi::InvalidDivestitureCondition::_ex = "InvalidDivestitureCondition" ;
const char *certi::InvalidFederationTimeDelta::_ex = "InvalidFederationTimeDelta" ;
const char *certi::InvalidRoutingSpace::_ex = "InvalidRoutingSpace" ;
const char *certi::NoPauseRequested::_ex = "NoPauseRequested" ;
const char *certi::NoResumeRequested::_ex = "NoResumeRequested" ;
const char *certi::TooManyIDsRequested::_ex = "TooManyIDsRequested" ;
const char *certi::UnimplementedService::_ex = "UnimplementedService" ;
const char *certi::UnknownLabel::_ex = "UnknownLabel" ;
const char *certi::NetworkError::_ex = "NetworkError" ;
const char *certi::NetworkSignal::_ex = "NetworkSignal" ;

//  TypeException managing (how to obtain TypeException from Exception name ?)
long certi::FederateNotPublishing::type = certi::e_FederateNotPublishing ;
long certi::FederateNotSubscribing::type = certi::e_FederateNotSubscribing ;
long certi::InvalidObjectHandle::type = certi::e_InvalidObjectHandle ;
long certi::SecurityError::type = certi::e_SecurityError ;
long certi::CouldNotOpenRID::type = certi::e_CouldNotOpenRID ;
long certi::ErrorReadingRID::type = certi::e_ErrorReadingRID ;
long certi::FederationAlreadyPaused::type = certi::e_FederationAlreadyPaused ;
long certi::FederationNotPaused::type = certi::e_FederationNotPaused ;
long certi::AttributeNotSubscribed::type = certi::e_AttributeNotSubscribed ;
long certi::FederateAlreadyPaused::type = certi::e_FederateAlreadyPaused ;
long certi::FederateDoesNotExist::type = certi::e_FederateDoesNotExist ;
long certi::FederateNameAlreadyInUse::type = certi::e_FederateNameAlreadyInUse ;
long certi::FederateNotPaused::type = certi::e_FederateNotPaused ;
long certi::IDsupplyExhausted::type = certi::e_IDsupplyExhausted ;
long certi::InvalidDivestitureCondition::type = certi::e_InvalidDivestitureCondition ;
long certi::InvalidFederationTimeDelta::type = certi::e_InvalidFederationTimeDelta ;
long certi::InvalidRoutingSpace::type = certi::e_InvalidRoutingSpace ;
long certi::NoPauseRequested::type = certi::e_NoPauseRequested ;
long certi::NoResumeRequested::type = certi::e_NoResumeRequested ;
long certi::TooManyIDsRequested::type = certi::e_TooManyIDsRequested ;
long certi::UnimplementedService::type = certi::e_UnimplementedService ;
long certi::UnknownLabel::type = certi::e_UnknownLabel ;
long certi::NetworkError::type = certi::e_NetworkError ;
long certi::NetworkSignal::type = certi::e_NetworkSignal ;

// $Id: Exception.cc,v 3.16 2008/11/25 16:32:14 gotthardp Exp $
