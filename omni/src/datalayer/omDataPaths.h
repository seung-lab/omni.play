#ifndef OM_DATA_PATHS_H
#define OM_DATA_PATHS_H

class OmDataPath;

class OmDataPaths
{
 public:
	static OmDataPath getDefaultDatasetName();
	static OmDataPath getProjectArchiveNameQT();

	static OmDataPath getSegmentPagePath( const OmId segmentationID, const quint32 pageNum );

};

#endif
