#pragma once
#include "precomp.h"

#include "datalayer/omDataPath.h"
#include "volume/build/omBuildVolumes.hpp"
#include "volume/build/omVolumeBuilder.hpp"
#include "volume/omAffinityChannel.h"
#include "volume/omChannel.h"

class OmBuildAffinityChannel : public OmBuildVolumes {
 private:
  OmAffinityChannel& affChan_;
  const om::common::AffinityGraph aff_;

 public:
  OmBuildAffinityChannel(OmAffinityChannel& chan,
                         const om::common::AffinityGraph aff)
      : OmBuildVolumes(), affChan_(chan), aff_(aff) {}

  void BuildBlocking() { do_build_channel(); }

  void BuildNonBlocking() {
    zi::thread th(
        zi::run_fn(zi::bind(&OmBuildAffinityChannel::do_build_channel, this)));
    th.start();
  }

  void do_build_channel() {
    const QString type = "affinity channel";

    if (!checkSettings()) {
      return;
    }

    OmTimer build_timer;
    startTiming(type, build_timer);

    OmVolumeBuilder<OmChannel> builder(affChan_, mFileNamesAndPaths,
                                       affChan_.GetDefaultHDF5DatasetName());
    builder.Build(aff_);

    stopTimingAndSave(type, build_timer);
  }
};
