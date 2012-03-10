#pragma once

#include "datalayer/omDataPath.h"
#include "volume/build/omBuildVolumes.hpp"
#include "volume/build/omVolumeBuilder.hpp"
#include "volume/omAffinityChannel.h"
#include "volume/omChannel.h"
#include "zi/omThreads.h"

class OmBuildAffinityChannel : public OmBuildVolumes {
private:
    OmAffinityChannel *const affChan_;
    const om::AffinityGraph aff_;

public:
    OmBuildAffinityChannel(OmAffinityChannel* chan,
                           const om::AffinityGraph aff)
        : OmBuildVolumes()
        , affChan_(chan)
        , aff_(aff)
    {}

    void BuildBlocking()
    {
        do_build_channel();
    }

    void BuildNonBlocking()
    {
        zi::thread th( zi::run_fn( zi::bind( &OmBuildAffinityChannel::do_build_channel, this)));
        th.start();
    }

    void do_build_channel()
    {
        const QString type = "affinity channel";

        if( !checkSettings() ){
            return;
        }

        OmTimer build_timer;
        startTiming(type, build_timer);

        OmVolumeBuilder<OmAffinityChannel> builder(affChan_,
                                                   mFileNamesAndPaths,
                                                   affChan_->GetDefaultHDF5DatasetName());
        builder.Build(aff_);

        stopTiming(type, build_timer);
    }
};

