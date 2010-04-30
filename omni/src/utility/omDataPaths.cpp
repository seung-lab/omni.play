#include "utility/omDataPaths.h"

OmHdf5Path OmDataPaths::getSegmentPath( OmId segmentationID, OmId segmentID)
{
        QString p = QString("segmentations/segmentation%1/segments/%2")
		.arg( segmentationID )
		.arg(segmentID);

	OmHdf5Path path;
	path.setPathQstr(p);
	return path;
}

OmHdf5Path OmDataPaths::getSegmentValuePath( OmId segmentationID, SEGMENT_DATA_TYPE value )
{
	QString p = QString( "segmentations/segmentation%1/values/%2" )
		.arg( segmentationID )
		.arg( value);

        OmHdf5Path path;
        path.setPathQstr( p );
	return path;
}
