#ifndef OM_BUILD_CHANNEL_H
#define OM_BUILD_CHANNEL_H

#include "system/omBuildVolumes.h"
#include "volume/omChannel.h"
#include "volume/build/omVolumeBuilder.hpp"
#include "datalayer/omDataPath.h"
#include "zi/omThreads.h"

class OmBuildChannel : public OmBuildVolumes {
private:
	OmChannel * mChann;

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

		OmVolumeBuilder<OmChannel> builder(mChann);
		builder.SetSourceFilenamesAndPaths( mFileNamesAndPaths );
		OmDataPath path(OmDataPaths::getDefaultHDF5channelDatasetName());
		builder.Build(path);

		stopTimingAndSave(type, build_timer);
	}

};

#endif
