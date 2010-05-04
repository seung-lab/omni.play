#include "system/omBuildChannel.h"
#include "utility/omImageDataIo.h"
#include "project/omProject.h"
#include "utility/omDataLayer.h"
#include "utility/omDataReader.h"
#include "utility/stringHelpers.h"

#include <QTextStream>

OmBuildChannel::OmBuildChannel(OmChannel * chan)
	: OmBuildVolumes()
{
	mChann = chan;
}

void OmBuildChannel::build_channel()
{
	start();
}

void OmBuildChannel::run()
{
	QString type = "channel";
	if(!checkSettingsAndTime(type) ){
		return;
	}

	mChann->SetSourceFilenamesAndPaths( mFileNamesAndPaths );
	mChann->BuildVolumeData();

	stopTiming(type);
}
