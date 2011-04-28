#ifndef CHANNEL_DATA_WRAPPER_HPP
#define CHANNEL_DATA_WRAPPER_HPP

#include "project/omProject.h"
#include "project/details/omProjectVolumes.h"
#include "project/details/omChannelManager.h"
#include "volume/omChannel.h"

#include <QHash>

class FilterDataWrapper;

class ChannelDataWrapper {
public:
    static const OmIDsSet& ValidIDs(){
        return OmProject::Volumes().Channels().GetValidChannelIds();
    }

    static inline const std::vector<OmChannel*> GetPtrVec(){
        return OmProject::Volumes().Channels().GetPtrVec();
    }

    static void Remove(const OmID id){
        OmProject::Volumes().Channels().RemoveChannel(id);
    }

private:
    OmID id_;

public:
    ChannelDataWrapper()
        : id_(0)
    {}

    explicit ChannelDataWrapper(const OmID ID)
        : id_(ID)
    {}

    inline OmID getID() const {
        return id_;
    }

    inline OmID GetChannelID() const {
        return id_;
    }

    OmChannel& Create()
    {
        OmChannel& c = OmProject::Volumes().Channels().AddChannel();
        id_ = c.GetID();
        printf("create channel %d\n", id_);
        return c;
    }

    void Remove()
    {
        Remove(id_);
        id_ = 0;
    }

    inline QString getName() const {
        return QString::fromStdString(GetChannel().GetName());
    }

    inline bool isEnabled() const {
        return OmProject::Volumes().Channels().IsChannelEnabled(id_);
    }

    inline QString getNote() const {
        return GetChannel().GetNote();
    }

    inline OmChannel& GetChannel() const {
        return OmProject::Volumes().Channels().GetChannel(id_);
    }

    inline OmChannel* GetChannelPtr() const
    {
        OmChannel& chan = GetChannel();
        return &chan;
    }

    inline bool IsValidWrapper() const {
        return IsChannelValid();
    }

    inline bool IsChannelValid() const
    {
        if(!id_){
            return false;
        }
        return OmProject::Volumes().Channels().IsChannelValid(id_);
    }

    inline bool IsBuilt() const {
        return GetChannel().IsBuilt();
    }

    std::vector<OmFilter2d*> GetFilters() const;
};

#endif
