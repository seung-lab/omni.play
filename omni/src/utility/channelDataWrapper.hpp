#ifndef CHANNEL_DATA_WRAPPER_HPP
#define CHANNEL_DATA_WRAPPER_HPP

#include "project/omProject.h"
#include "project/omProjectVolumes.h"
#include "project/omChannelManager.h"
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

    inline QString getName() const {
        return QString::fromStdString(GetChannel().GetName());
    }

    inline bool isEnabled() const {
        return OmProject::Volumes().Channels().IsChannelEnabled(mID);
    }

    inline QString getNote() const {
        return GetChannel().GetNote();
    }

    inline OmChannel& GetChannel() const {
        return OmProject::Volumes().Channels().GetChannel(mID);
    }

    inline OmChannel* GetChannelPtr() const
    {
        OmChannel& chan = GetChannel();
        return &chan;
    }

    inline bool IsChannelValid() const
    {
        if(!mID){
            return false;
        }
        return OmProject::Volumes().Channels().IsChannelValid(mID);
    }

    inline bool IsBuilt() const {
        return GetChannel().IsBuilt();
    }

    std::vector<OmFilter2d*> GetFilters() const;
};

#endif
