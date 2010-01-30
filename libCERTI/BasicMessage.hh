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
// $Id: BasicMessage.hh,v 3.13 2010/01/30 18:41:37 erk Exp $
// ----------------------------------------------------------------------------

#ifndef LIBCERTI_BASIC_MESSAGE
#define LIBCERTI_BASIC_MESSAGE


#include "MessageBuffer.hh"
#include "FedTimeD.hh"
#include "BaseRegion.hh"
#include "Extent.hh"
#include <vector>

namespace certi {

/** 
 * Base class for messages. 
 * It contains common attributes and methods
 * for Message and NetworkMessage classes.
 * @sa Message, NetworkMessage
 */
class CERTI_EXPORT BasicMessage
{
public:
	/**
	 * Indicate if the message is dated/timestamped or not
	 */
	bool isDated() {return _isDated;};
	/**
	 * If ones set Date then this is a Dated message
	 * Message builder which setDate will generate a Dated message
	 */
	void setDate(FederationTime new_date) {_isDated=true; date = new_date;};
	const FederationTime getDate() const {return this->date;};

	/**
	 * Indicate if the message is Tagged or not
	 */
	bool isTagged() {return _isTagged;};
	void setTag(const std::string& new_tag) {_isTagged = true; tag = new_tag;};
	const std::string& getTag() const {return this->tag;};

	/**
	 * Indicate if the message is Labelled or not
	 */
	bool isLabelled() {return _isLabelled;};
	void setLabel(const std::string& new_label) {_isLabelled = true; label = new_label;}
	const std::string& getLabel() const {return this->label;};

	void setExtents(const std::vector<Extent> &);
	const std::vector<Extent> &getExtents() const ;

	void setRegions(const BaseRegion **, int);
	void setRegions(const std::vector<RegionHandle> &);
	const std::vector<RegionHandle> &getRegions() const ;

protected:
	BasicMessage();
	virtual ~BasicMessage();

	/**
	 * Serialize the message into a buffer
	 * @param[in] msgBuffer the serialization buffer
	 */
	virtual void serialize(MessageBuffer& msgBuffer);

	/**
	 * DeSerialize the message from a buffer
	 * @param[in] msgBuffer the deserialization buffer
	 */
	virtual void deserialize(MessageBuffer& msgBuffer);



	/**
	 * The date of message if it is dated.
	 * date field cannot be accessed directly but only using
	 * getter/setter.
	 */
	FederationTime date;
	/**
	 * True is the message is dated
	 * When a message is dated the date is transmitted
	 * over the network, when not dated the date is not sent.
	 */
	bool _isDated;

	/**
	 * The tag of message if it is tagged.
	 * date field cannot be accessed directly but only using
	 * getter/setter.
	 */
	std::string tag;
	/**
	 * True is the message contains a tag
	 * When a message is tagged the tag is transmitted
	 * over the network, when not tagged the tag is not sent.
	 */
	bool _isTagged;

	/**
	 * The label of message if it is labelled.
	 * date field cannot be accessed directly but only using
	 * getter/setter.
	 */
	std::string label;

	/**
	 * True is the message contains a label
	 * When a message is labelled the label is transmitted
	 * over the network, when not labelled the label is not sent.
	 */
	bool _isLabelled;

	void readExtents(MessageBuffer& msgBuffer);
	void writeExtents(MessageBuffer& msgBuffer) const ;

	void readRegions(MessageBuffer& msgBuffer);
	void writeRegions(MessageBuffer& msgBuffer);

	std::vector<Extent> extents ;
	std::vector<RegionHandle> regions ;
};

} // namespace certi

#endif // LIBCERTI_BASIC_MESSAGE

// $Id: BasicMessage.hh,v 3.13 2010/01/30 18:41:37 erk Exp $
