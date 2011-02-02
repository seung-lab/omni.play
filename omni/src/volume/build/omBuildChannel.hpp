#ifndef OM_BUILD_CHANNEL_H
#define OM_BUILD_CHANNEL_H

#include "datalayer/omDataPath.h"
#include "volume/build/omBuildVolumes.hpp"
#include "volume/build/omVolumeBuilder.hpp"
#include "volume/omChannel.h"
#include "zi/omThreads.h"

class OmBuildChannel : public OmBuildVolumes {
private:
	OmChannel *const mChann;

public:
	OmBuildChannel(OmChannel* chan)
		: OmBuildVolumes()
		, mChann(chan)
	{}

	void BuildBlocking()
	{
		do_build_channel();
	}

	void BuildNonBlocking()
	{
		zi::thread th( zi::run_fn( zi::bind( &OmBuildChannel::do_build_channel, this)));
		th.start();
	}

	void do_build_channel()
	{
		const QString type = "channel";

		if( !checkSettings() ){
			return;
		}

		OmTimer build_timer;
		startTiming(type, build_timer);

		OmVolumeBuilder<OmChannel> builder(mChann,
										   mFileNamesAndPaths,
										   mChann->GetDefaultHDF5DatasetName());
		builder.Build();

		stopTimingAndSave(type, build_timer);
	}

};

#endif
