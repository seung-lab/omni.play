#ifndef OM_DATA_PATHS_H
#define OM_DATA_PATHS_H

class OmDataPath;
class OmMipMeshManager;
class OmMipMeshCoord;

class OmDataPaths
{
 public:
	static OmDataPath getDefaultDatasetName();
	static OmDataPath getProjectArchiveNameQT();

	static OmDataPath getSegmentPagePath( const OmId segmentationID, const quint32 pageNum );

	static string getMeshDirectoryPath( const OmMipMeshCoord & meshCoordinate,
					    OmMipMeshManager * const mipMeshManager );

	static string getMeshFileName( const OmMipMeshCoord & meshCoordinate );
	static string getLocalPathForHd5fChunk(OmMipMeshCoord & meshCoordinate, const OmId segmentationID);



};

#endif
