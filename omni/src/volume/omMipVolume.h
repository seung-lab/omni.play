#ifndef OM_MIP_VOLUME_H 
#define OM_MIP_VOLUME_H

/*
 *	OmMipVolume allows a source volume to be coverted to and accessed as compressed segments
 *	of regular size called MipChunks.  MipVolume extends GenericCache to allow for MipChunk caching.
 *
 *	Brett Warne - bwarne@mit.edu - 7/19/09
 */
class OmVolume;
#include "volume/omVolume.h"
#include "omSimpleChunkThreadedCache.h"
#include "omMipChunkCoord.h"
#include "system/omThreadedCache.h"
#include "datalayer/omDataPath.h"
#include "common/omStd.h"

#include <QFileInfo>

class OmMipChunk;
class vtkImageData;

//enum subsampling methods
enum SubsampleMode { SUBSAMPLE_MEAN = 0, SUBSAMPLE_MODE, SUBSAMPLE_RANDOM, SUBSAMPLE_NONE };

//mipvolume state
enum MipVolumeBuildState { MIPVOL_UNBUILT = 0, MIPVOL_BUILT, MIPVOL_BUILDING };

//typedef chunk cache
typedef OmThreadedCache< OmMipChunkCoord, OmMipChunk > MipChunkThreadedCache;

class OmMipVolume : public OmVolume, public MipChunkThreadedCache {
	
public:
	OmMipVolume();
	~OmMipVolume();
	
	void Flush();
	void PrepareForCompleteDelete();

	void SetFilename(const QString &);
	QString GetFilename();
	virtual void SetDirectoryPath(const QString &);
	QString GetDirectoryPath();
	
	QString MipLevelInternalDataPath(int level);
	QString MipChunkMetaDataPath(const OmMipChunkCoord &rMipCoord);
	
	//source data properties
	void SetSourceFilenamesAndPaths( QFileInfoList );
	QFileInfoList GetSourceFilenamesAndPaths();
	bool IsSourceValid();
	
	
	// data properties
	const DataBbox& GetExtent();
	int GetChunkDimension();
	Vector3<int> GetChunkDimensions();
	
	void SetSubsampleMode(int);
	int GetSubsampleMode();
	
	void SetChunksStoreMetaData(bool);
	bool GetChunksStoreMetaData();
	
	bool IsBuilt();
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
	
	//mip coord
	OmMipChunkCoord DataToMipCoord(const DataCoord &vox, int level);
	OmMipChunkCoord NormToMipCoord(const NormCoord &normCoord, int level);
	DataBbox MipCoordToDataBbox(const OmMipChunkCoord &, int level);
	NormBbox MipCoordToNormBbox(const OmMipChunkCoord &);
	
	//mip chunk methods
	OmMipChunkCoord RootMipChunkCoordinate();
	int MipChunkDimension(int level);
	bool ContainsMipChunkCoord(const OmMipChunkCoord &mipCoord);
	void ValidMipChunkCoordChildren(const OmMipChunkCoord &mipCoord, set<OmMipChunkCoord> &children);
	void GetChunk(QExplicitlySharedDataPointer<OmMipChunk> &p_value, const OmMipChunkCoord &rMipCoord, bool block=true);
	void GetSimpleChunk(QExplicitlySharedDataPointer<OmSimpleChunk> &p_value, const OmMipChunkCoord &rMipCoord, bool block=true);
	void StoreChunk(const OmMipChunkCoord &, OmMipChunk *);
	
	//mip data accessors
	quint32 GetVoxelValue(const DataCoord &vox);
	void SetVoxelValue(const DataCoord &vox, quint32 value);

	//build methods
	void Build(OmDataPath & dataset);
	bool BuildVolume();
	virtual void BuildChunk(const OmMipChunkCoord &);
	void BuildChunkAndParents(const OmMipChunkCoord &mipCoord);
	void BuildEditedLeafChunks();
	
	//io
	bool ImportSourceData(OmDataPath & dataset);
	void ImportSourceDataSlice();
	void ExportInternalData(QString fileNameAndPath);
	virtual void ExportDataFilter(vtkImageData *) { }
	void DeleteVolumeData();

	bool ContainsVoxel(const DataCoord &vox);
	int GetBytesPerSample();
	void SetBytesPerSample(int);

	//Simple Chunk Stuff
	OmSimpleChunkThreadedCache* GetSimpleChunkThreadedCache();

	
protected:		
	//state
	void SetBuildState(MipVolumeBuildState);
	
	//mipvolume disk data
	void AllocInternalData();
	void DeleteInternalData();
	void UpdateRootMipLevel();
	
	//mip properties
	QFileInfoList mSourceFilenamesAndPaths;
	
	//mip data
	int mBuildState;
	int mMipLeafDim;			//must be even
	int mMipRootLevel;			//inferred from leaf dim and source data extent
	int mSubsampleMode;		//method to use when subsampling	
	bool mStoreChunkMetaData;		//do chunks have metadata

	set< OmMipChunkCoord > mEditedLeafChunks;	//set of edited chunks that need rebuild

	
private:
	OmMipChunk* HandleCacheMiss(const OmMipChunkCoord &key);
	OmSimpleChunkThreadedCache* mSimpleChunkThreadedCache;

	int mBytesPerSample;		//VTK_UNSIGNED_CHAR (1 byte) or VTK_UNSIGNED_INT (4 bytes)
	
	//subsample data methods
	vtkImageData* GetSubsampledImageDataFromChildren(const OmMipChunkCoord &mipCoord);
	vtkImageData* GetSubsampledChunkImageData(const OmMipChunkCoord &mipCoord);
	
	template< typename T> vtkImageData* SubsampleImageData(vtkImageData* srcData, int subsampleMode);
	template< typename T > T CalculateMode( T* array, int size);
	template< typename T > T CalculateAverage( T* array, int size);
	
	QString mDirectoryPath;          // ex. "./" or "images/out/"
	QString mFilename;
	bool sourceFilesWereSet;
	bool mCompleteDelete;

	friend class OmDataArchiveProject;
};

#endif

