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
// $Id: Federate.hh,v 3.18 2010/08/10 16:34:09 erk Exp $
// ----------------------------------------------------------------------------

#ifndef CERTI_RTIG_FEDERATE_HH
#define CERTI_RTIG_FEDERATE_HH

#include "certi.hh"
#include "Exception.hh"
#include "FedTimeD.hh"

#include <vector>
#include <string>

namespace certi {
namespace rtig {

/*! This class manages the federate status and other relevant information.
 */
class Federate
{
public:
    Federate(const std::string& the_name, FederateHandle) throw (RTIinternalError);

    FederateHandle getHandle() const { return handle ; };
    void setHandle(FederateHandle h) { handle = h ; };

    const std::string& getName() const { return name; };
    bool isConstrained() const { return constrained ; };
    bool isRegulator() const { return regulator ; };
    void setConstrained(bool c) { constrained = c ; };
    void setRegulator(bool r) { regulator = r ; };
    void setIsUsingNERx(bool unx) { usingNERx = unx;};
    bool isUsingNERx() const {return usingNERx;};
    const FederationTime getLastNERxValue() const {return lastNERxValue;};
    void setLastNERxValue(const FederationTime t) {lastNERxValue=t; usingNERx=true;};

    /**
     * Sets the ClassRelevanceAdvisorySwitch of the federate to the value of 
     * the input parameter val. The CRA switch is enabled by default.
     * @param[in] val new value for the switch true means enable
     * false means disable. 
     */
    void setClassRelevanceAdvisorySwitch(bool val) { cras = val ; };

    /**
     * Sets the InteractionRelevanceAdvisorySwitch of the federate to the 
     * value of the input parameter val. The IRA switch is enabled by default.
     * @param[in] val new value for the switch true means enable
     * false means disable. 
     */
    void setInteractionRelevanceAdvisorySwitch(bool val) { iras = val ; };

    /**
     * Sets the AttributeRelevanceAdvisorySwitch of the federate to the value
     * of the input parameter val. The ARA switch is disabled by default.
     * @param[in] val new value for the switch true means enable
     * false means disable. 
     */
    void setAttributeRelevanceAdvisorySwitch(bool val) { aras = val ; };

    /**
     * Sets the AttributeScopeAdvisorySwitch of the federate to the value of 
     * the input parameter val. The ASA switch is disabled by default.
     * @param[in] val new value for the switch true means enable
     * false means disable. 
     */
    void setAttributeScopeAdvisorySwitch(bool val) { asas = val ; };

    /**
     * Returns the current state of the ClassRelevanceAdvisory switch.
     * @return a boolean indicating the current state of the switch,
     * true means enabled, false means disabled
     */
    bool isClassRelevanceAdvisorySwitch() const { return cras ; };

    /**
     * Returns the current state of the InteractionRelevanceAdvisory switch.
     * @return a boolean indicating the current state of the switch,
     * true means enabled, false means disabled
     */
    bool isInteractionRelevanceAdvisorySwitch() const { return iras ; };

    /**
     * Returns the current state of the AttributeRelevanceAdvisory switch.
     * @return a boolean indicating the current state of the switch,
     * true means enabled, false means disabled
     */
    bool isAttributeRelevanceAdvisorySwitch() const { return aras ; };

    /**
     * Returns the current state of the AttributeScopeAdvisory switch.
     * @return a boolean indicating the current state of the switch,
     * true means enabled, false means disabled
     */
    bool isAttributeScopeAdvisorySwitch() const { return asas ; };

    bool isSaving() const { return saving ; };
    bool isRestoring() const { return restoring ; };
    void setSaving(bool s) { saving = s ; };
    void setRestoring(bool r) { restoring = r ; };

    /**
     *  Add a synchronization label to federate.
     *  @param[in] label the synchronization label to be added
     */
    void addSynchronizationLabel(const std::string& label) throw (RTIinternalError);
    void removeSynchronizationLabel(const std::string&) throw (RTIinternalError);
    bool isSynchronizationLabel(const std::string&) const ;

private:
    FederateHandle handle ; //!< Federate ID.
    std::string name ; //!< Federate name.
    /*! = false by default -- Used only on the RTIA, because on RTIG there is a
      upper level list of regulator Federates (in Federation).
    */
    bool regulator ;
    bool constrained ; //!< = false by default.
    /**
     * If the federate is currently using NER or NERA in order to advance time.
     * Initialized to be false.
     * Will be set to true whenever NULL PRIME Message is received from
     * the concerned federate (i.e. from its RTIA). Note that NULL PRIME
     * message are sent iff federate has zero lookahead.
     * Will be set to false whenever a NULL Message is received from
     * the concerned federate.
     */
    bool usingNERx;
    /**
     * The last NERx timestamp value received for this federate.
     */
    FederationTime lastNERxValue;

    bool cras ; //!< = class relevance advisory switch -> true  by default.
    bool iras ; //!< = interaction relevance advisory siwtch -> true  by default.
    bool aras ; //!< = attribute relevance advisory switch -> false by default.
    bool asas ; //!< = attribute scope advisory switch -> false by default.

    typedef std::vector<std::string> SyncList ;
    SyncList syncLabels ; // List of labels to synchronize.

    bool saving ; //!< True when saving has been initiated on federate.
    bool restoring ; //!< True when restoring has been initiated on federate.
};

}} // namespace certi::rtig

#endif // CERTI_RTIG_FEDERATE_HH

// $Id: Federate.hh,v 3.18 2010/08/10 16:34:09 erk Exp $
