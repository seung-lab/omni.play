#ifndef OM_DATA_PATHS_H
#define OM_DATA_PATHS_H

class OmChannel;
class OmDataPath;
class OmMipChunkCoord;
class OmMipMeshCoord;
class OmMipMeshManager;
class OmSegmentation;

class OmDataPaths {
public:
	static string getDefaultHDF5channelDatasetName();

	static OmDataPath getDefaultDatasetName();
	static OmDataPath getProjectArchiveNameQT();

	static OmDataPath getSegmentPagePath( const OmId segmentationID,
					      const quint32 pageNum );

	static string getMeshDirectoryPath( const OmMipMeshCoord &,
					    OmMipMeshManager *const );

	static string getMeshFileName( const OmMipMeshCoord & meshCoordinate );
	static string getLocalPathForHd5fChunk(OmMipMeshCoord & meshCoordinate,
					       const OmId segmentationID);

	static std::string getDirectoryPath(OmChannel* chan);
	static std::string getDirectoryPath(OmSegmentation* seg);

	static std::string MipLevelInternalDataPath(const std::string & dirPath,
						    const int level);
	static std::string MipChunkMetaDataPath(const std::string & dirPath,
						const OmMipChunkCoord & coord);
};

#endif
