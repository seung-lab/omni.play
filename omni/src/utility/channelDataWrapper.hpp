#ifndef CHANNEL_DATA_WRAPPER_HPP
#define CHANNEL_DATA_WRAPPER_HPP

#include "project/omProject.h"
#include "volume/omChannel.h"

#include <QHash>

class FilterDataWrapper;

class ChannelDataWrapper {
private:
	OmID mID;

public:
	ChannelDataWrapper()
		: mID(0)
	{}

	explicit ChannelDataWrapper(const OmID ID)
		: mID(ID)
	{}

	inline OmID getID() const {
		return mID;
	}

	inline OmID GetChannelID() const {
		return mID;
	}

	inline QString getName() {
		return QString::fromStdString(GetChannel().GetName());
	}

	inline bool isEnabled() const {
		return OmProject::IsChannelEnabled(mID);
	}

	inline QString getNote() {
		return GetChannel().GetNote();
	}

	inline OmChannel& GetChannel(){
		return OmProject::GetChannel(mID);
	}

	inline OmChannel* GetChannelPtr()
	{
		OmChannel& chan = GetChannel();
		return &chan;
	}

	inline bool IsChannelValid() const
	{
		if(!mID){
			return false;
		}
		return OmProject::IsChannelValid(mID);
	}

	QHash<OmID, FilterDataWrapper> getAllFilterIDsAndNames();
};

#endif
