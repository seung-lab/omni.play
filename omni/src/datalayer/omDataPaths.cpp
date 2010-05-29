#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"

OmDataPath OmDataPaths::getDefaultDatasetName()
{
	OmDataPath path;
	path.setPath("main");
	return path;
}

OmDataPath OmDataPaths::getProjectArchiveNameQT()
{
	OmDataPath path;
	path.setPath("project.qt.dat");
	return path;
}

OmDataPath OmDataPaths::getSegmentPagePath( const OmId segmentationID, const quint32 pageNum )
{
        QString p = QString("segmentations/segmentation%1/segment_page%2")
		.arg( segmentationID )
		.arg( pageNum );

	OmDataPath path;
	path.setPathQstr(p);
	return path;
}
