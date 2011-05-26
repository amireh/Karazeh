/*
 *  This file is part of Vertigo.
 *
 *  Vertigo - a cross-platform arcade game powered by Ogre3D.
 *  Copyright (C) 2011  Ahmad Amireh <ahmad@amireh.net>
 * 
 *  Vertigo is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Vertigo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Vertigo.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
 
#include "Event.h"
#include "EventManager.h"
#include "Utility.h"
namespace Pixy
{

	/*
    Event::Event(const int inId, const std::string inName) : 
	mId(inId),
	mName(inName)
    {
		reset();
		std::cout << "Event " << mName << "@" << mId << " created\n";
	}
	*/
	
    Event::Event(const std::string inName)
    {
		reset();
		mName = inName;
		//std::cout << this << " created\n";
	}
	
	Event::~Event()
  {
		//std::cout << this << " destroyed\n";
		//reset();
  }
	
	void Event::reset() {
		mName.clear();
		mCategory.clear();
		mWeight = MINOR_EVT;
		mName = "Dummy";
		mCategory = "Event";
		mType = EVT_REQ;
		mFeedback = EVT_ERROR;
		fConfidential = false;
		nrHandlers = 0;
		nrProperties = 0;
		mUserData = NULL;
	}
	
	/*
	Event::Event(const Event& src) {
		clone(src);
	}

	Event& Event::operator=(const Event& rhs) {
		if (this != &rhs)
			clone(rhs);
	
		return *this;
	}

	void Event::clone(const Event& src) {
		std::cout << "Event " << src.mName << " is getting cloned";
		reset();
		
		//this->mId = src.mId;
		//this->mUID = src.mUID;
		this->mMsgId = src.mMsgId;
		this->mWeight = src.mWeight;
		this->mType = src.mType;
		this->mFeedback = src.mFeedback;
		this->mName = src.mName;
		this->mCategory = src.mCategory;
		this->fConfidential = src.fConfidential;
		//this->nrHandlers = src.nrHandlers;
		this->nrProperties = src.nrProperties;
		
		property_t::const_iterator lProperty = src.mProperties.begin();
		for (lProperty; lProperty != src.mProperties.end(); ++lProperty) {
			setProperty(lProperty->first, lProperty->second);
		}
	}
	*/
	/*
	void Event::serialize(BitStream& inStream) {

		inStream.Write<MessageID>(mMsgId);
		//inStream.Write<int>(getId());
		//inStream.Write<EVENT_UID>(getUID());
		inStream.Write<EVENT_WEIGHT>(getWeight());
		inStream.Write<EVENT_TYPE>(getType());
		inStream.Write<EVENT_FEEDBACK>(getFeedback());
		inStream.Write<RakString>(RakString(getName().c_str()));
		inStream.Write<RakString>(RakString(getCategory().c_str()));
		inStream.Write<bool>(isConfidential());
		inStream.Write<int>(nrProperties);
		//std::cout << "serializing " << nrProperties << "properties\n";
		property_t::const_iterator lProperty = mProperties.begin();
		for (lProperty; lProperty != mProperties.end(); ++lProperty) {
			inStream.Write<RakString>(RakString(lProperty->first.c_str()));
			inStream.Write<RakString>(RakString(lProperty->second.c_str()));
		}
	}
	
	void Event::deserialize(BitStream& inStream) {
		//std::cout << "deserializing event\n";
		
		//inStream.IgnoreBytes(1);
		RakString lEvtName, lEvtCat;
		
		//std::cout << "deserializing attributes\n";
		if (//!inStream.Read<int>(mId) ||
			//!inStream.Read<EVENT_UID>(mUID) ||
			!inStream.Read<MessageID>(mMsgId) ||
			!inStream.Read<EVENT_WEIGHT>(mWeight) ||
			!inStream.Read<EVENT_TYPE>(mType) ||
			!inStream.Read<EVENT_FEEDBACK>(mFeedback) ||
			!inStream.Read<RakString>(lEvtName) ||
			!inStream.Read<RakString>(lEvtCat) ||
			!inStream.Read<bool>(fConfidential) ||
			!inStream.Read<int>(nrProperties)
			)
			throw BadStream("Invalid BitStream provided for evt parsing");
		
		// parse properties
		int i=0, count = nrProperties;
		RakString lName, lValue;
		
		//std::cout << "deserializing " << nrProperties << " properties\n";
		
		// reset properties
		nrProperties = 0;
		mProperties.clear();
		// populate them
		for (i=0; i < count; ++i) {
			inStream.Read<RakString>(lName);
			inStream.Read<RakString>(lValue);
			//std::cout << "setting " << lName << " to " << lValue << "\n";
			setProperty(lName.C_String(), lValue.C_String());
		}
		
		mName = lEvtName.C_String();
		mCategory = lEvtCat.C_String();
	}
	*/
	ostream& operator<<(ostream& outStream, Event* inEvt) {
		//std::string _type = (inEvt->getType() == EVT_REQ) ? "REQUEST" : "RESPONS";
		
		//outStream << _type << "> " << inEvt->getName();
		outStream << "Event: " << inEvt->getName();
		
		//_type.clear();
		return outStream;
	}
	
	
	void Event::setCategory(const std::string inCategory) {
		mCategory = inCategory;
	}
	

	std::string Event::getCategory() const {
		return mCategory;
	}
	
    void Event::setName(const std::string inName) {
        mName = inName;
    }
	
	
    std::string Event::getName() const {
        return mName;
    }
	
    void Event::setWeight(const EVENT_WEIGHT inRank) {
        mWeight = inRank;
    }
	
    EVENT_WEIGHT Event::getWeight() const {
        return mWeight;
    }
	/*
    bool Event::operator==(const Event& rhs) {
        if (mId == rhs.mId)
            return true;
        else
            return false;
    }
	 */
	
	void Event::setFeedback(EVENT_FEEDBACK inMsg) {
		mFeedback = inMsg;
	}
	
	EVENT_FEEDBACK Event::getFeedback() const {
		return mFeedback;
	}
	
	void Event::setType(EVENT_TYPE inType) {
		mType = inType;
	}
	
	EVENT_TYPE Event::getType() const {
		return mType;
	}
	
	/*
	int Event::getId() const {
		return mId;
	}
	*/
	bool Event::isConfidential() const {
		return fConfidential;
	}
	
	void Event::addHandler() {
		nrHandlers++;
	}
	
	void Event::removeHandler() {
		nrHandlers--;
		if (nrHandlers == 0) 
			EventManager::getSingletonPtr()->releaseEvt(this);
		
	}
	
	void Event::setProperty(const string inName, const string inValue) {
		if ( propertyExists(inName) ) {
			mProperties.find(inName)->second = inValue;
			return;
		}
		
		mProperties.insert( make_pair<string, string>(inName, inValue) );
		nrProperties++;
	}
	
	string Event::getProperty(const string inName) const {
		return mProperties.find(inName)->second;
	}
	
	bool Event::propertyExists(const string& inName) {
		return (mProperties.find(inName) != mProperties.end());
	}
	
	void Event::dump(std::ostream& inStream) {
		inStream 
		//<< "id: " << mId << "\n"
		//<< "uid: " << mUID << "\n"
		<< "name: " << mName << "\n"
		<< "category: " << mCategory << "\n"
		<< "weight: " << (mWeight == MINOR_EVT) ? "minor" : "major";
		inStream << "\n";
		inStream << "type: " << (mType == EVT_REQ) ? "request" : "response";
		inStream << "\n";
		inStream << "feedback: " << (mFeedback == EVT_ERROR) ? "error" : "ok";
		inStream << "\n";
		inStream << "confidential? " << fConfidential << "\n"
		<< "current handlers count: " << nrHandlers << "\n"
		<< "properties count: " << nrProperties << "\n";
		
		inStream << "properties: \n";
		property_t::const_iterator lProperty = mProperties.begin();
		for (lProperty; lProperty != mProperties.end(); ++lProperty) {
			inStream << "\t" << lProperty->first << " : " << lProperty->second << "\n";
		}
	}
	/*
	MessageID Event::getMsgId() {
		return mMsgId;
	}
	*/
	
	
	uint8_t Event::getNrHandlers() { return nrHandlers; };
	
	void Event::setAny(void* inAny) {
	  mUserData = inAny;
	}
	
	void* Event::getAny() { return mUserData; mUserData = NULL; }
}
