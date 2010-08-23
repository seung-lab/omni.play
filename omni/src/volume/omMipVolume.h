#ifndef OM_MIP_VOLUME_H
#define OM_MIP_VOLUME_H

/*
 *	OmMipVolume allows a source volume to be coverted to and accessed as compressed segments
 *	of regular size called MipChunks.  MipVolume extends GenericCache to allow for MipChunk caching.
 *
 *	Brett Warne - bwarne@mit.edu - 7/19/09
 */

#include "volume/omVolume.h"
#include "common/omCommon.h"
#include "system/cache/omMipVolumeCache.h"
#include "system/cache/omThreadChunkThreadedCache.h"

#include <QFileInfo>

class OmMipChunkCoord;
class OmDataPath;
class OmMipChunk;
class OmThreadChunkLevel;
class OmVolume;
class vtkImageData;
class OmHdf5;
class OmVolumeData;

enum MipVolumeBuildState { MIPVOL_UNBUILT = 0,
			   MIPVOL_BUILT,
			   MIPVOL_BUILDING };

class OmMipVolume : public OmVolume {
public:
        OmMipVolume();
	~OmMipVolume();

	void Flush();

	virtual std::string GetDirectoryPath() = 0;
	virtual std::string GetName() = 0;
	virtual void loadVolData() = 0;
	virtual boost::shared_ptr<OmVolumeData> getVolData() = 0;

	std::string MipLevelInternalDataPath(const int level);
	std::string MipChunkMetaDataPath(const OmMipChunkCoord &rMipCoord);

	//source data properties
	void SetSourceFilenamesAndPaths( QFileInfoList );
	QFileInfoList GetSourceFilenamesAndPaths();
	bool IsSourceValid();

	// data properties
	int GetChunkDimension();
	Vector3<int> GetChunkDimensions();

	void SetChunksStoreMetaData(bool);
	bool GetChunksStoreMetaData();

	bool IsVolumeDataBuilt();
	bool IsBuilding();

	void UpdateMipProperties(OmDataPath&);

	//TODO: move to volume
	//mip level method
	void UpdateRootLevel();
	int GetRootMipLevel();
	Vector3<int> MipLevelDataDimensions(int);
	Vector3<int> MipLevelDimensionsInMipChunks(int level);
	int MipChunksInMipLevel(int level);
	int MipChunksInVolume();

	//thread chunk methods
	int GetThreadChunkDimension();
	Vector3<int> GetThreadChunkDimensions();
	int GetMaxConsecutiveSubsamples();
	int GetThreadChunkLevelDimension(int level);
	Vector3<int> GetThreadChunkLevelDimensions(int level);
	Vector3<int> MipLevelDimensionsInThreadChunks(int level);
	Vector3<int> MipLevelDimensionsInThreadChunkLevels(int level);
	int ThreadChunksInMipLevel(int level);
	int ThreadChunksInVolume();

	//mip coord
	OmMipChunkCoord DataToMipCoord(const DataCoord &vox, int level);
	OmMipChunkCoord NormToMipCoord(const NormCoord &normCoord, int level);
	DataBbox MipCoordToDataBbox(const OmMipChunkCoord &, int level);
	NormBbox MipCoordToNormBbox(const OmMipChunkCoord &);
       	DataBbox MipCoordToThreadDataBbox(const OmMipChunkCoord &);
       	DataBbox MipCoordToThreadLevelDataBbox(const OmMipChunkCoord &);

	//mip chunk methods
	OmMipChunkCoord RootMipChunkCoordinate();
	int MipChunkDimension(int level);
	bool ContainsMipChunkCoord(const OmMipChunkCoord &rMipCoord);
	bool ContainsThreadChunkCoord(const OmMipChunkCoord &rMipCoord);
	bool ContainsThreadChunkLevelCoord(const OmMipChunkCoord &rMipCoord);
	void ValidMipChunkCoordChildren(const OmMipChunkCoord &rMipCoord, set<OmMipChunkCoord> &children);
	void GetChunk(OmMipChunkPtr& p_value, const OmMipChunkCoord &rMipCoord, bool block=true);
	void GetThreadChunkLevel(OmThreadChunkLevelPtr& p_value, const OmMipChunkCoord &rMipCoord, bool block=true);

	//mip data accessors
	quint32 GetVoxelValue(const DataCoord &vox);
	void SetVoxelValue(const DataCoord &vox, quint32 value);

	//build methods
	void Build(OmDataPath & dataset);
	bool BuildVolume();
	virtual bool BuildThreadedVolume();
	virtual void BuildChunk(const OmMipChunkCoord &, bool remesh=false);
	void BuildChunkAndParents(const OmMipChunkCoord &);
	void BuildEditedLeafChunks();

	virtual OmDataWrapperPtr BuildThreadChunkLevel(const OmMipChunkCoord &, OmDataWrapperPtr p_source_data, bool initCall);
	void BuildThreadChunk(const OmMipChunkCoord &, OmDataWrapperPtr data, bool initCall);

	//comparison methods
	static bool CompareVolumes(OmMipVolume *, OmMipVolume *);
	static bool CompareChunks(const OmMipChunkCoord&,
				  OmMipVolume*, OmMipVolume*);

	void copyDataIn();
        void copyAllMipDataIntoMemMap();
	bool areImportFilesImages();

	Vector3i get_dims(const OmDataPath dataset );
	virtual bool GetBounds(float & , float &) { assert(0 && "the data for this mip volume has no bounds."); }

	void ImportSourceDataSlice();
	void ExportInternalData(QString fileNameAndPath);
	virtual void ExportDataFilter(OmDataWrapperPtr) { }
	void DeleteVolumeData();

	bool ContainsVoxel(const DataCoord &vox);

	//Thread Chunk Cache
	OmThreadChunkThreadedCache* GetThreadChunkThreadedCache();

	QFileInfoList mSourceFilenamesAndPaths;

	Vector3i getDimsRoundedToNearestChunk(const int level);
	OmVolDataType getVolDataType(){ return mVolDataType; }

protected:
        OmMipVolumeCache *const mDataCache;
        OmVolDataType mVolDataType;

	void BuildBlankVolume(const Vector3i & dims);

	//state
	void SetBuildState(MipVolumeBuildState);

	virtual bool ImportSourceData(OmDataPath & dataset) = 0;

	//mipvolume disk data
	void UpdateRootMipLevel();

	//mip data
	int mBuildState;
	int mMipLeafDim;			//must be even
	int mMipRootLevel;			//inferred from leaf dim and source data extent
	bool mStoreChunkMetaData;		//do chunks have metadata

	set< OmMipChunkCoord > mEditedLeafChunks;	//set of edited chunks that need rebuild

private:
	OmThreadChunkThreadedCache* mThreadChunkThreadedCache;

	bool sourceFilesWereSet;

	void doExportChunk(const OmMipChunkCoord &, OmHdf5 &);
	void copyChunkFromMemMapToHDF5(const OmMipChunkCoord& coord);

	friend class OmMipChunk;
	friend class OmDataArchiveProject;
	friend class OmVolumeData;
};

#endif

