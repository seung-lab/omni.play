#pragma once

#include "datalayer/omDataPath.h"
#include "volume/build/omBuildVolumes.hpp"
#include "volume/build/omVolumeBuilder.hpp"
#include "volume/omChannel.h"
#include "zi/omThreads.h"

class OmBuildChannel : public OmBuildVolumes {
 private:
  ChannelDataWrapper cdw_;
  OmChannel& chan_;

 public:
  OmBuildChannel() : OmBuildVolumes(), chan_(cdw_.Create()) {}

  OmBuildChannel(const ChannelDataWrapper cdw)
      : OmBuildVolumes(), cdw_(cdw), chan_(cdw_.GetChannel()) {}

  void BuildNonBlocking() {
    zi::thread th(zi::run_fn(zi::bind(&OmBuildChannel::Build, this)));

    th.start();
  }

  void BuildEmptyChannel() {
    OmVolumeBuilder<OmChannel> builder(&chan_);
    builder.BuildEmptyChannel();
  }

  void Build() {
    const QString type = "channel";

    if (!checkSettings()) {
      return;
    }

    OmTimer build_timer;
    startTiming(type, build_timer);

    OmVolumeBuilder<OmChannel> builder(&chan_, mFileNamesAndPaths,
                                       chan_.GetDefaultHDF5DatasetName());
    builder.Build();

    stopTimingAndSave(type, build_timer);
  }
};
