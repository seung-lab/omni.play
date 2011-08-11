#pragma once

#include "datalayer/omDataPath.h"
#include "volume/build/omBuildVolumes.hpp"
#include "volume/build/omVolumeBuilder.hpp"
#include "volume/omChannel.h"
#include "zi/omThreads.h"

class OmBuildChannel : public OmBuildVolumes {
private:
    OmChannel *const chan_;

public:
    OmBuildChannel(OmChannel* chan)
        : OmBuildVolumes()
        , chan_(chan)
    {}

    void BuildNonBlocking()
    {
        zi::thread th(
            zi::run_fn(
                zi::bind(&OmBuildChannel::Build, this)));

        th.start();
    }

    void BuildEmptyChannel()
    {
        OmVolumeBuilder<OmChannel> builder(chan_);
        builder.BuildEmptyChannel();
    }

    void Build()
    {
        const QString type = "channel";

        if(!checkSettings()){
            return;
        }

        OmTimer build_timer;
        startTiming(type, build_timer);

        OmVolumeBuilder<OmChannel> builder(chan_,
                                           mFileNamesAndPaths,
                                           chan_->GetDefaultHDF5DatasetName());
        builder.Build();

        stopTimingAndSave(type, build_timer);
    }
};

