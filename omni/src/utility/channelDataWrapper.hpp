#ifndef CHANNEL_DATA_WRAPPER_HPP
#define CHANNEL_DATA_WRAPPER_HPP

#include "project/omProject.h"
#include "volume/omChannel.h"

#include <QHash>

class FilterDataWrapper;

class ChannelDataWrapper {
private:
	OmID mID;
	ObjectType mType;
public:
	ChannelDataWrapper(){}

	explicit ChannelDataWrapper(const OmID ID)
		: mID(ID), mType(CHANNEL)
	{}

	OmID getID(){
		return mID;
	}

	QString getName(){
		return QString::fromStdString(OmProject::GetChannel(mID).GetName());
	}

	bool isEnabled(){
		return OmProject::IsChannelEnabled(mID);
	}

	QString getNote(){
		return OmProject::GetChannel(mID).GetNote();
	}

	QHash<OmID, FilterDataWrapper> getAllFilterIDsAndNames();
};

#endif
