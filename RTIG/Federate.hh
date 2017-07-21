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

#ifndef CERTI_RTIG_FEDERATE_HH
#define CERTI_RTIG_FEDERATE_HH

#include "Exception.hh"
#include "FedTimeD.hh"
#include "certi.hh"

#include <string>
#include <vector>

namespace certi {
namespace rtig {

/** This class manages the federate status and other relevant information. */
class Federate {
public:
    /** A new FederateName is allocated. theLink must have been opened before. */
    Federate(const std::string& name, const FederateHandle handle);

    FederateHandle getHandle() const noexcept;
    
    [[deprecated("use new version")]]
    void setHandle(const FederateHandle h);
    void new_setHandle(const FederateHandle h); //

    std::string getName() const noexcept;

    bool isConstrained() const noexcept;
    
    [[deprecated("use new version")]]
    void setConstrained(const bool c);
    void new_setConstrained(const bool c); //

    bool isRegulator() const noexcept;
    
    [[deprecated("use new version")]]
    void setRegulator(const bool r);
    void new_setRegulator(const bool r); //

    bool isUsingNERx() const noexcept;
    
    void setIsUsingNERx(const bool unx);

    const FederationTime getLastNERxValue() const noexcept;
    
    void setLastNERxValue(const FederationTime t);

    /**
     * Returns the current state of the ClassRelevanceAdvisory switch.
     * @return a boolean indicating the current state of the switch,
     * true means enabled, false means disabled
     */
    bool isClassRelevanceAdvisorySwitch() const noexcept;
    /**
     * Sets the ClassRelevanceAdvisorySwitch of the federate to the value of 
     * the input parameter val. The CRA switch is enabled by default.
     * @param[in] val new value for the switch true means enable
     * false means disable. 
     */
    [[deprecated("use new version")]]
    void setClassRelevanceAdvisorySwitch(const bool val);
    void new_setClassRelevanceAdvisorySwitch(const bool val); //

    /**
     * Returns the current state of the InteractionRelevanceAdvisory switch.
     * @return a boolean indicating the current state of the switch,
     * true means enabled, false means disabled
     */
    bool isInteractionRelevanceAdvisorySwitch() const noexcept;
    /**
     * Sets the InteractionRelevanceAdvisorySwitch of the federate to the 
     * value of the input parameter val. The IRA switch is enabled by default.
     * @param[in] val new value for the switch true means enable
     * false means disable. 
     */
    [[deprecated("use new version")]]
    void setInteractionRelevanceAdvisorySwitch(const bool val);
    void new_setInteractionRelevanceAdvisorySwitch(const bool val); //

    /**
     * Returns the current state of the AttributeRelevanceAdvisory switch.
     * @return a boolean indicating the current state of the switch,
     * true means enabled, false means disabled
     */
    bool isAttributeRelevanceAdvisorySwitch() const noexcept;
    /**
     * Sets the AttributeRelevanceAdvisorySwitch of the federate to the value
     * of the input parameter val. The ARA switch is disabled by default.
     * @param[in] val new value for the switch true means enable
     * false means disable. 
     */
    [[deprecated("use new version")]]
    void setAttributeRelevanceAdvisorySwitch(const bool val);
    void new_setAttributeRelevanceAdvisorySwitch(const bool val); //

    /**
     * Returns the current state of the AttributeScopeAdvisory switch.
     * @return a boolean indicating the current state of the switch,
     * true means enabled, false means disabled
     */
    bool isAttributeScopeAdvisorySwitch() const noexcept;
    /**
     * Sets the AttributeScopeAdvisorySwitch of the federate to the value of 
     * the input parameter val. The ASA switch is disabled by default.
     * @param[in] val new value for the switch true means enable
     * false means disable. 
     */
    [[deprecated("use new version")]]
    void setAttributeScopeAdvisorySwitch(const bool val);
    void new_setAttributeScopeAdvisorySwitch(const bool val); //

    bool isSaving() const noexcept;
    
    void setSaving(const bool s);

    bool isRestoring() const noexcept;
    
    void setRestoring(const bool r);

    /**
     *  Add a synchronization label to federate.
     *  @param[in] label the synchronization label to be added
     */
    void addSynchronizationLabel(const std::string& label);

    /// Removes a synchronization label from federate.
    void removeSynchronizationLabel(const std::string&);

    /// Returns whether the federate is already synchronized with this label.
    bool isSynchronizationLabel(const std::string&) const;

private:
    FederateHandle my_handle; /// Federate ID.
    std::string my_name; /// Federate name.

    /** Used only on the RTIA, because on RTIG there is a
     *  upper level list of regulator Federates (in Federation).
    */
    bool my_isRegulator{false};

    bool my_isConstrained{false};

    /**
     * If the federate is currently using NER or NERA in order to advance time.
     * Initialized to be false.
     * Will be set to true whenever NULL PRIME Message is received from
     * the concerned federate (i.e. from its RTIA). Note that NULL PRIME
     * message are sent iff federate has zero lookahead.
     * Will be set to false whenever a NULL Message is received from
     * the concerned federate.
     */
    bool my_isUsingNERx{false};

    /// The last NERx timestamp value received for this federate.
    FederationTime my_lastNERxValueReceived{};

    bool my_classRelevanceAdvisorySwitch{true};
    bool my_interactionRelevanceAdvisorySwitch{true};
    bool my_attributeRelevanceAdvisorySwitch{false};
    bool my_attributeScopeAdvisorySwitch{false};

    bool my_isCurrentlySaving{false};
    bool my_isCurrentlyRestoring{false};

    /// List of labels to synchronize.
    /// TODO use set
    std::vector<std::string> my_syncLabels{};
};
}
} // namespace certi::rtig

#endif // CERTI_RTIG_FEDERATE_HH
