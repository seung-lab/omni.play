#pragma once
#include "precomp.h"

#include "datalayer/omDataPath.h"
#include "volume/build/omBuildVolumes.hpp"
#include "volume/build/omVolumeBuilder.hpp"
#include "volume/omChannel.h"

class OmBuildChannel : public OmBuildVolumes {
 private:
  ChannelDataWrapper cdw_;
  OmChannel* chan_;

 public:
  OmBuildChannel() : OmBuildVolumes(), chan_(&cdw_.Create()) {}

  OmBuildChannel(const ChannelDataWrapper cdw)
      : OmBuildVolumes(), cdw_(cdw), chan_(cdw_.GetChannel()) {
    if (!cdw_.IsValidWrapper()) {
      throw om::ArgException("Invalid ChannelDataWrapper");
    }
  }

  void BuildNonBlocking() {
    zi::thread th(zi::run_fn(zi::bind(&OmBuildChannel::Build, this)));

    th.start();
  }

  void BuildEmptyChannel() {
    OmVolumeBuilder<OmChannel> builder(*chan_);
    builder.BuildEmptyChannel();
  }

  void Build() {
    if (!checkSettings()) {
      return;
    }    
    const QString type = "channel";
    OmTimer build_timer;
    startTiming(type, build_timer);

    OmVolumeBuilder<OmChannel> builder(*chan_, mFileNamesAndPaths,
                                       chan_->GetDefaultHDF5DatasetName());
    builder.Build();

    stopTiming(type, build_timer);
  }
};
