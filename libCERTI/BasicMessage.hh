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
// ----------------------------------------------------------------------------

#ifndef LIBCERTI_BASIC_MESSAGE
#define LIBCERTI_BASIC_MESSAGE


#include "MessageBuffer.hh"
using libhla::MessageBuffer;
#include "FedTimeD.hh"
#include "BaseRegion.hh"
#include "Extent.hh"
#include <vector>

namespace certi {

/** 
 * Base class for messages. 
 * It contains common attributes and methods for Message and NetworkMessage classes.
 * Currently there is too much information in this class.
 * FIXME Extents and Region should be in derived specific classes.
 * FIXME BasicMessage is mostly used for Message, NetworkMessage goes
 *       on its own, this is due to historical and MUST be changed.
 * @sa Message, NetworkMessage
 */
class CERTI_EXPORT BasicMessage
{
public:
	/**
	 * Indicate if the message is dated/time-stamped or not
	 */
	bool isDated() {return _isDated;};
	/**
	 * If ones set Date then this is a Dated message
	 * Message builder which setDate will generate a Dated TimeStamped message
	 */
	void setDate(FederationTime new_date) {_isDated=true; date = new_date;};
	const FederationTime getDate() const {return this->date;};

	/**
	 * Indicate if the message is Tagged or not
	 */
	bool isTagged() {return _isTagged;};
	void setTag(const std::string& new_tag) 
	{
		_isTagged = true; 
		tag = new_tag;
		wTag = std::wstring(new_tag.begin(), new_tag.end());
	};
	const std::string& getTag() const {return this->tag;};
	const std::wstring& getTagW() const { return this->wTag; }

	/**
	 * Indicate if the message is Labelled or not
	 */
	bool isLabelled() {return _isLabelled;};
	void setLabel(const std::string& new_label) 
	{
		_isLabelled = true; 
		label = new_label;
		wLabel = std::wstring(new_label.begin(), new_label.end());
	}
	const std::string& getLabel() const {return this->label;};
	const std::wstring& getLabelW() const { return this->wLabel; }

	void setExtents(const std::vector<Extent> &);
	const std::vector<Extent> &getExtents() const ;

	void setRegions(const BaseRegion **, int);
	void setRegions(const std::vector<RegionHandle> &);
	const std::vector<RegionHandle> &getRegions() const ;

	virtual std::ostream& show(std::ostream& out);

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
	 * Serialize the message into a buffer
	 * @param[in] msgBuffer the serialization buffer
	 */
	virtual void serializeExtent(MessageBuffer& msgBuffer);

	/**
	 * DeSerialize the message from a buffer
	 * @param[in] msgBuffer the deserialization buffer
	 */
	virtual void deserializeExtent(MessageBuffer& msgBuffer);

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
	std::wstring wTag;
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
	std::wstring wLabel;

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
