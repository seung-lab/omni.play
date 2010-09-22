#include "datalayer/omDataLayer.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataReader.h"
#include "project/omProject.h"
#include "system/omBuildChannel.h"
#include "system/omProjectData.h"
#include "utility/omImageDataIo.h"
#include "utility/stringHelpers.h"
#include "volume/omMipChunk.h"

#include <vtkImageData.h>

#include <QTextStream>
#include <QImage>

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
	const QString type = "channel";

	if( !checkSettings() ){
		return;
	}

	OmTimer build_timer;
	startTiming(type, build_timer);

	mChann->SetSourceFilenamesAndPaths( mFileNamesAndPaths );
	mChann->BuildVolumeData();

	stopTimingAndSave(type, build_timer);
}
