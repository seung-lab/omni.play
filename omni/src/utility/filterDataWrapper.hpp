#ifndef FILTER_DATA_WRAPPER_HPP
#define FILTER_DATA_WRAPPER_HPP

#include "volume/omFilter2d.h"
#include "project/details/omChannelManager.h"

class FilterDataWrapper {
private:
    OmID mID;
    OmID mChannelID;

public:
    FilterDataWrapper()
    {}

    FilterDataWrapper(const OmID channelID, const OmID id)
        : mID(id)
        , mChannelID(channelID)
    {}

    OmID getChannelID() const {
        return mChannelID;
    }

    OmID GetID() const {
        return mID;
    }

    OmChannel& GetChannel() const {
        return OmProject::Volumes().Channels().GetChannel(mChannelID);
    }

    bool isValid() const
    {
        if( OmProject::Volumes().Channels().IsChannelValid(mChannelID) ){
            if( GetChannel().FilterManager().IsFilterValid(mID) ){
                return true;
            }
        }

        return false;
    }

    OmFilter2d* getFilter() const
    {
        if(!isValid()){
            return NULL;
        }

        return &GetChannel().FilterManager().GetFilter(mID);
    }

    QString getName() const
    {
        OmFilter2d* f = getFilter();
        if(!f){
            return "";
        }
        return QString::fromStdString(f->GetName());
    }

    bool isEnabled() const {
        return GetChannel().FilterManager().IsFilterEnabled(mID);
    }
};

#endif
