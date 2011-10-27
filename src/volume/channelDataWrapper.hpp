#pragma once

#include "project/project.h"
#include "project/details/projectVolumes.h"
#include "project/details/channelManager.h"
#include "volume/channel.h"

namespace om {
namespace volume {

class ChannelDataWrapper {
public:
    static const common::idSet& ValidIDs(){
        return project::Volumes().Channels().GetValidChannelIds();
    }

    static inline const std::vector<channel*> GetPtrVec(){
        return project::Volumes().Channels().GetPtrVec();
    }

    static void Remove(const common::id id){
        project::Volumes().Channels().RemoveChannel(id);
    }

private:
    common::id id_;

public:
    ChannelDataWrapper()
        : id_(0)
    {}

    explicit ChannelDataWrapper(const common::id ID)
        : id_(ID)
    {}

    inline common::id GetID() const {
        return id_;
    }

    inline common::id GetChannelID() const {
        return id_;
    }

    channel& Create()
    {
        channel& c = project::Volumes().Channels().AddChannel();
        id_ = c.GetID();
        printf("create channel %d\n", id_);
        return c;
    }

    void Remove()
    {
        Remove(id_);
        id_ = 0;
    }

    inline std::string GetName() const {
        return GetChannel().GetName();
    }

    inline bool isEnabled() const {
        return project::Volumes().Channels().IsChannelEnabled(id_);
    }

    inline std::string getNote() const {
        return GetChannel().GetNote();
    }

    inline channel& GetChannel() const {
        return project::Volumes().Channels().GetChannel(id_);
    }

    inline channel* GetChannelPtr() const
    {
        channel& chan = GetChannel();
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
        return project::Volumes().Channels().IsChannelValid(id_);
    }

    inline bool IsBuilt() const {
        return GetChannel().IsBuilt();
    }
};

} // namespace volume
} // namespace om
