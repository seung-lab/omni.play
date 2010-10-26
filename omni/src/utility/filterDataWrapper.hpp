#ifndef FILTER_DATA_WRAPPER_HPP
#define FILTER_DATA_WRAPPER_HPP

class FilterDataWrapper {
private:
	OmID mID;
	OmID mChannelID;
public:
	FilterDataWrapper(){}

	FilterDataWrapper(const OmID channelID, const OmID ID)
		: mID(ID)
		, mChannelID(channelID)
	{}

	OmID getChannelID(){
		return mChannelID;
	}
	OmID getID(){
		return mID;
	}

	bool isValid()
	{
		if( OmProject::IsChannelValid(mChannelID) ){
			if( OmProject::GetChannel(mChannelID).IsFilterValid(mID) ){
				return true;
			}
		}

		return false;
	}

	OmFilter2d* getFilter()
	{
		if(!isValid()){
			return NULL;
		}

		return &OmProject::GetChannel(mChannelID).GetFilter(mID);
	}

	QString getName(){
		OmFilter2d* f = getFilter();
		if(!f){
			return "";
		}
		return QString::fromStdString(f->GetName());
	}

	bool isEnabled(){
		return OmProject::GetChannel(mChannelID).IsFilterEnabled(mID);
	}

};

#endif
