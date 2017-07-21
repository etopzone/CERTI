// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
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

#include <algorithm>
#include <string>

#include "PrettyDebug.hh"

static PrettyDebug G("GENDOC", __FILE__);

namespace certi {
namespace rtig {

Federate::Federate(const std::string& name, const FederateHandle handle) : my_handle{handle}, my_name{name}
{
    if (my_handle == 0) {
        throw RTIinternalError("Bad initialization parameter for Federate.");
    }
}

FederateHandle Federate::getHandle() const noexcept
{
    return my_handle;
};

void Federate::setHandle(const FederateHandle h)
{
    if(h == 0) {
        throw RTIinternalError("Federate Handle cannot be null.");
    }
    if(my_handle == h) {
        throw RTIinternalError("Federate Handle already set.");
    }
    
    my_handle = h;
};

std::string Federate::getName() const noexcept
{
    return my_name;
};

bool Federate::isConstrained() const noexcept
{
    return my_isConstrained;
};

void Federate::setConstrained(const bool c)
{
    if(my_isConstrained == c) {
        throw RTIinternalError("Constrained already set.");
    }
    
    my_isConstrained = c;
};

bool Federate::isRegulator() const noexcept
{
    return my_isRegulator;
};

void Federate::setRegulator(const bool r)
{
    if(my_isRegulator == r) {
        throw RTIinternalError("Regulator already set.");
    }
    
    my_isRegulator = r;
};

bool Federate::isUsingNERx() const noexcept
{
    return my_isUsingNERx;
};

void Federate::setIsUsingNERx(const bool unx) noexcept
{
    my_isUsingNERx = unx;
};

const FederationTime Federate::getLastNERxValue() const noexcept
{
    return my_lastNERxValueReceived;
};

void Federate::setLastNERxValue(const FederationTime t) noexcept
{
    my_lastNERxValueReceived = t;
    my_isUsingNERx = true;
};

bool Federate::isClassRelevanceAdvisorySwitch() const noexcept
{
    return my_classRelevanceAdvisorySwitch;
};

void Federate::setClassRelevanceAdvisorySwitch(const bool val)
{
    if(my_classRelevanceAdvisorySwitch == val) {
        throw RTIinternalError("CRAS already set.");
    }
    
    my_classRelevanceAdvisorySwitch = val;
};

bool Federate::isInteractionRelevanceAdvisorySwitch() const noexcept
{
    return my_interactionRelevanceAdvisorySwitch;
};

void Federate::setInteractionRelevanceAdvisorySwitch(const bool val)
{
    if(my_interactionRelevanceAdvisorySwitch == val) {
        throw RTIinternalError("IRAS already set.");
    }
    
    my_interactionRelevanceAdvisorySwitch = val;
};

bool Federate::isAttributeRelevanceAdvisorySwitch() const noexcept
{
    return my_attributeRelevanceAdvisorySwitch;
};

void Federate::setAttributeRelevanceAdvisorySwitch(const bool val)
{
    if(my_attributeRelevanceAdvisorySwitch == val) {
        throw RTIinternalError("ARAS already set.");
    }
    
    my_attributeRelevanceAdvisorySwitch = val;
};

bool Federate::isAttributeScopeAdvisorySwitch() const noexcept
{
    return my_attributeScopeAdvisorySwitch;
};

void Federate::setAttributeScopeAdvisorySwitch(const bool val)
{
    if(my_attributeScopeAdvisorySwitch == val) {
        throw RTIinternalError("ASAS already set.");
    }
    
    my_attributeScopeAdvisorySwitch = val;
};

bool Federate::isSaving() const noexcept
{
    return my_isCurrentlySaving;
};

void Federate::setSaving(const bool s) noexcept
{
    my_isCurrentlySaving = s;
};

bool Federate::isRestoring() const noexcept
{
    return my_isCurrentlyRestoring;
};

void Federate::setRestoring(const bool r) noexcept
{
    my_isCurrentlyRestoring = r;
};

void Federate::addSynchronizationLabel(const std::string& label)
{
    Debug(G, pdGendoc) << "enter Federate::addSynchronizationLabel" << std::endl;

    if (isSynchronizationLabel(label)) {
        throw RTIinternalError(getName() + " Synchronization label pending in federate.");
    }

    my_syncLabels.push_back(label);

    Debug(G, pdGendoc) << "exit  Federate::addSynchronizationLabel" << std::endl;
}

void Federate::removeSynchronizationLabel(const std::string& label)
{
    if (!isSynchronizationLabel(label)) {
        throw RTIinternalError(getName() + " Sync. label not in federate.");
    }

    my_syncLabels.erase(std::remove(begin(my_syncLabels), end(my_syncLabels), label), end(my_syncLabels));
}

bool Federate::isSynchronizationLabel(const std::string& label) const
{
    return std::find(begin(my_syncLabels), end(my_syncLabels), label) != end(my_syncLabels);
}
}
}
