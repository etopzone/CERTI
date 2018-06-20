// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
//
// This program is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// ----------------------------------------------------------------------------

#include "Federate.hh"

#include <libCERTI/Exception.hh>
#include <libCERTI/PrettyDebug.hh>

static PrettyDebug G("GENDOC", __FILE__);

namespace certi {
namespace rtig {

Federate::Federate(const std::string& name,
                   const std::string& type,
                   const RtiVersion rti_version,
                   const FederateHandle handle)
    : my_handle{handle}, my_name{name}, my_type{type}, my_rti_version{rti_version}
{
    if (my_handle == 0) {
        throw RTIinternalError("Bad initialization parameter for Federate.");
    }
}

FederateHandle Federate::getHandle() const noexcept
{
    return my_handle;
}

void Federate::setHandle(const FederateHandle h)
{
    if (h == 0) {
        throw RTIinternalError("Federate Handle cannot be null.");
    }
    if (my_handle == h) {
        throw RTIinternalError("Federate Handle already set.");
    }

    my_handle = h;
}

std::string Federate::getName() const noexcept
{
    return my_name;
}

std::string Federate::getType() const noexcept
{
    return my_type;
}
    
RtiVersion Federate::getRtiVersion() const noexcept
{
    return my_rti_version;
}

bool Federate::isConstrained() const noexcept
{
    return my_isConstrained;
}

void Federate::setConstrained(const bool c)
{
    if (my_isConstrained == c) {
        throw RTIinternalError("Constrained already set.");
    }

    my_isConstrained = c;
}

bool Federate::isRegulator() const noexcept
{
    return my_isRegulator;
}

void Federate::setRegulator(const bool r)
{
    if (my_isRegulator == r) {
        throw RTIinternalError("Regulator already set.");
    }

    my_isRegulator = r;
}

bool Federate::isUsingNERx() const noexcept
{
    return my_isUsingNERx;
}

void Federate::setIsUsingNERx(const bool unx) noexcept
{
    my_isUsingNERx = unx;
}

const FederationTime Federate::getLastNERxValue() const noexcept
{
    return my_lastNERxValueReceived;
}

void Federate::setLastNERxValue(const FederationTime t) noexcept
{
    my_lastNERxValueReceived = t;
    my_isUsingNERx = true;
}

bool Federate::isClassRelevanceAdvisorySwitch() const noexcept
{
    return my_classRelevanceAdvisorySwitch;
}

void Federate::setClassRelevanceAdvisorySwitch(const bool val)
{
    if (my_classRelevanceAdvisorySwitch == val) {
        throw RTIinternalError("CRAS already set.");
    }

    my_classRelevanceAdvisorySwitch = val;
}

bool Federate::isInteractionRelevanceAdvisorySwitch() const noexcept
{
    return my_interactionRelevanceAdvisorySwitch;
}

void Federate::setInteractionRelevanceAdvisorySwitch(const bool val)
{
    if (my_interactionRelevanceAdvisorySwitch == val) {
        throw RTIinternalError("IRAS already set.");
    }

    my_interactionRelevanceAdvisorySwitch = val;
}

bool Federate::isAttributeRelevanceAdvisorySwitch() const noexcept
{
    return my_attributeRelevanceAdvisorySwitch;
}

void Federate::setAttributeRelevanceAdvisorySwitch(const bool val)
{
    if (my_attributeRelevanceAdvisorySwitch == val) {
        throw RTIinternalError("ARAS already set.");
    }

    my_attributeRelevanceAdvisorySwitch = val;
}

bool Federate::isAttributeScopeAdvisorySwitch() const noexcept
{
    return my_attributeScopeAdvisorySwitch;
}

void Federate::setAttributeScopeAdvisorySwitch(const bool val)
{
    if (my_attributeScopeAdvisorySwitch == val) {
        throw RTIinternalError("ASAS already set.");
    }

    my_attributeScopeAdvisorySwitch = val;
}

bool Federate::isConveyRegionDesignatorSetsSwitch() const noexcept
{
    return my_conveyRegionDesignatorSetsSwitch;
}

void Federate::setConveyRegionDesignatorSetsSwitch(const bool val)
{
    my_conveyRegionDesignatorSetsSwitch = val;
}

bool Federate::isConveyProducingFederateSwitch() const noexcept
{
    return my_conveyProducingFederateSwitch;
}

void Federate::setConveyProducingFederateSwitch(const bool val)
{
    my_conveyProducingFederateSwitch = val;
}

bool Federate::isServiceReportingSwitch() const noexcept
{
    return my_serviceReportingSwitch;
}

void Federate::setServiceReportingSwitch(const bool val)
{
    my_serviceReportingSwitch = val;
}

bool Federate::isExceptionReportingSwitch() const noexcept
{
    return my_exceptionReportingSwitch;
}

void Federate::setExceptionReportingSwitch(const bool val)
{
    my_exceptionReportingSwitch = val;
}

bool Federate::isSaving() const noexcept
{
    return my_isCurrentlySaving;
}

void Federate::setSaving(const bool s) noexcept
{
    my_isCurrentlySaving = s;
}

bool Federate::isRestoring() const noexcept
{
    return my_isCurrentlyRestoring;
}

void Federate::setRestoring(const bool r) noexcept
{
    my_isCurrentlyRestoring = r;
}

void Federate::addSynchronizationLabel(const std::string& label)
{
    Debug(G, pdGendoc) << "enter Federate::addSynchronizationLabel" << std::endl;

    auto result = my_syncLabels.insert(label).second;
    if (!result) {
        throw RTIinternalError(getName() + " Synchronization label pending in federate.");
    }

    Debug(G, pdGendoc) << "exit  Federate::addSynchronizationLabel" << std::endl;
}

void Federate::removeSynchronizationLabel(const std::string& label)
{
    auto result = my_syncLabels.erase(label);
    if (!result) {
        throw RTIinternalError(getName() + " Sync. label not in federate.");
    }
}

bool Federate::hasSynchronizationLabel(const std::string& label) const
{
    return my_syncLabels.find(label) != end(my_syncLabels);
}
}
}
