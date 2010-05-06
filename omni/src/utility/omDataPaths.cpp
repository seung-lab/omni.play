#include "utility/omDataPaths.h"

OmHdf5Path OmDataPaths::getSegmentPagePath( OmId segmentationID, const quint32 pageNum )
{
        QString p = QString("segmentations/segmentation%1/segment_page%2")
		.arg( segmentationID )
		.arg( pageNum );

	OmHdf5Path path;
	path.setPathQstr(p);
	return path;
}

OmHdf5Path OmDataPaths::getValuePagePath( OmId segmentationID, const quint32 pageNum )
{
	QString p = QString( "segmentations/segmentation%1/value_page%2" )
		.arg( segmentationID )
		.arg( pageNum );

        OmHdf5Path path;
        path.setPathQstr( p );
	return path;
}
