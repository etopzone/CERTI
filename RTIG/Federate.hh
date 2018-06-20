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

#ifndef CERTI_RTIG_FEDERATE_HH
#define CERTI_RTIG_FEDERATE_HH

#include <string>
#include <unordered_map>
#include <unordered_set>

#include <libCERTI/FedTimeD.hh>
// #include "certi.hh"
#include <libCERTI/Handle.hh>

namespace certi {
namespace rtig {

/** This class manages the federate status and other relevant information. */
class Federate {
public:
    /** Construct new federate
     * 
     * handle must be valid
     */
    Federate(const std::string& name,
             const std::string& type,
             const RtiVersion rti_version,
             const FederateHandle handle);

    FederateHandle getHandle() const noexcept;

    void setHandle(const FederateHandle h);

    std::string getName() const noexcept;

    std::string getType() const noexcept;
    
    RtiVersion getRtiVersion() const noexcept;

    bool isConstrained() const noexcept;

    void setConstrained(const bool c);

    bool isRegulator() const noexcept;

    void setRegulator(const bool r);

    bool isUsingNERx() const noexcept;

    void setIsUsingNERx(const bool unx) noexcept;

    const FederationTime getLastNERxValue() const noexcept;

    void setLastNERxValue(const FederationTime t) noexcept;

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
    void setClassRelevanceAdvisorySwitch(const bool val);

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
    void setInteractionRelevanceAdvisorySwitch(const bool val);

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
    void setAttributeRelevanceAdvisorySwitch(const bool val);

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
    void setAttributeScopeAdvisorySwitch(const bool val);

    bool isConveyRegionDesignatorSetsSwitch() const noexcept;
    void setConveyRegionDesignatorSetsSwitch(const bool val);

    bool isConveyProducingFederateSwitch() const noexcept;
    void setConveyProducingFederateSwitch(const bool val);

    bool isServiceReportingSwitch() const noexcept;
    void setServiceReportingSwitch(const bool val);

    bool isExceptionReportingSwitch() const noexcept;
    void setExceptionReportingSwitch(const bool val);

    bool isSaving() const noexcept;

    void setSaving(const bool s) noexcept;

    bool isRestoring() const noexcept;

    void setRestoring(const bool r) noexcept;

    /**
     *  Add a synchronization label to federate.
     *  @param label the synchronization label to be added
     */
    void addSynchronizationLabel(const std::string& label);

    /// Removes a synchronization label from federate.
    void removeSynchronizationLabel(const std::string& label);

    /// Returns whether the federate is already synchronized with this label.
    bool hasSynchronizationLabel(const std::string& label) const;

private:
    FederateHandle my_handle; /// Federate ID.
    std::string my_name; /// Federate name.
    std::string my_type;
    RtiVersion my_rti_version;

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
    bool my_conveyRegionDesignatorSetsSwitch{false};
    bool my_conveyProducingFederateSwitch{false};
    bool my_serviceReportingSwitch{false};
    bool my_exceptionReportingSwitch{false};

    bool my_isCurrentlySaving{false};
    bool my_isCurrentlyRestoring{false};

    /// List of labels to synchronize.
    std::unordered_set<std::string> my_syncLabels{};
};
}
} // namespace certi::rtig

#endif // CERTI_RTIG_FEDERATE_HH
