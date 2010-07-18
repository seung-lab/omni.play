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

#include <QFileInfo>
#include <QExplicitlySharedDataPointer>
#include <QImage>
#include <QMutex>

class OmMipChunkCoord;
class OmDataPath;
class OmMipChunk;
class OmMipVolumeCache;
class OmThreadChunkLevel;
class OmThreadChunkThreadedCache;
class OmVolume;
class vtkImageData;

//mipvolume state
enum MipVolumeBuildState { MIPVOL_UNBUILT = 0, MIPVOL_BUILT, MIPVOL_BUILDING };

class OmMipVolume : public OmVolume {
	
public:
        OmMipVolume();
	~OmMipVolume();

	void Flush();

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

	//thread chunk methods
	int GetThreadChunkDimension();
	Vector3<int> GetThreadChunkDimensions();
	Vector3<int> MipVolumeDimensionsInThreadChunks();
	int ThreadChunksInVolume();
	
	//mip coord
	OmMipChunkCoord DataToMipCoord(const DataCoord &vox, int level);
	OmMipChunkCoord NormToMipCoord(const NormCoord &normCoord, int level);
	DataBbox MipCoordToDataBbox(const OmMipChunkCoord &, int level);
	NormBbox MipCoordToNormBbox(const OmMipChunkCoord &);
       	DataBbox MipCoordToThreadDataBbox(const OmMipChunkCoord &);
	
	//mip chunk methods
	OmMipChunkCoord RootMipChunkCoordinate();
	int MipChunkDimension(int level);
	bool ContainsMipChunkCoord(const OmMipChunkCoord &mipCoord);
	bool ContainsThreadChunkCoord(const OmMipChunkCoord &mipCoord);
	void ValidMipChunkCoordChildren(const OmMipChunkCoord &mipCoord, set<OmMipChunkCoord> &children);
	void GetChunk(QExplicitlySharedDataPointer<OmMipChunk> &p_value, const OmMipChunkCoord &rMipCoord, bool block=true);
	void GetThreadChunkLevel(QExplicitlySharedDataPointer<OmThreadChunkLevel> &p_value, const OmMipChunkCoord &rMipCoord, bool block=true);
	void StoreChunk(const OmMipChunkCoord &, OmMipChunk *);
	
	//mip data accessors
	quint32 GetVoxelValue(const DataCoord &vox);
	void SetVoxelValue(const DataCoord &vox, quint32 value);
	
	//build methods
	void Build(OmDataPath & dataset);
	bool BuildVolume();
	bool BuildSerialVolume();
	bool BuildThreadedVolume();
	virtual void BuildChunk(const OmMipChunkCoord &);
	void BuildChunkAndParents(const OmMipChunkCoord &mipCoord);
	void BuildEditedLeafChunks();	
	virtual vtkImageData* BuildThreadChunkLevel(const OmMipChunkCoord &, vtkImageData *p_source_data);
	void BuildThreadChunk(const OmMipChunkCoord &, vtkImageData *p_source_data);

	//comparison methods
	static bool CompareVolumes(OmMipVolume *, OmMipVolume *, bool verbose);
	static bool CompareChunks(OmMipChunk *, OmMipChunk *, bool verbose);
	
	//io
	bool ImportSourceData(OmDataPath & dataset);
	bool ImportSourceDataQT();
	bool ImportSourceDataVTK(OmDataPath & dataset);
	void copyDataIn( std::set<OmMipChunkCoord> & chunksToCopy);
	void addToChunkCoords(const OmMipChunkCoord chunk_coord);
	std::set<OmMipChunkCoord> chunksToCopy;;
	std::pair<int,QString> getNextImgToProcess();
	int mSliceNum;
	std::vector<QFile*> mFileVec;
	void AllocMemMapFiles();
	unsigned char * getChunkPtr( OmMipChunkCoord & coord);
	void figureOutNumberOfBytesImg();
	bool areImportFilesImages();
	Vector3i get_dims(const OmDataPath dataset );
	
	void ImportSourceDataSlice();
	void ExportInternalData(QString fileNameAndPath);
	virtual void ExportDataFilter(vtkImageData *) { }
	void DeleteVolumeData();

	bool ContainsVoxel(const DataCoord &vox);
	int GetBytesPerSample();
	void SetBytesPerSample(int);

	//Thread Chunk Cache
	OmThreadChunkThreadedCache* GetThreadChunkThreadedCache();

	mutable QMutex mChunkCoords;

protected:		
	OmMipVolumeCache *const mDataCache;

	//state
	void SetBuildState(MipVolumeBuildState);
	
	//mipvolume disk data
	void AllocInternalData();
	void UpdateRootMipLevel();
	
	//mip properties
	QFileInfoList mSourceFilenamesAndPaths;
	
	//mip data
	int mBuildState;
	int mMipLeafDim;			//must be even
	int mMipRootLevel;			//inferred from leaf dim and source data extent
	bool mStoreChunkMetaData;		//do chunks have metadata
	
	//subsample data methods
	vtkImageData* GetSubsampledImageDataFromChildren(const OmMipChunkCoord &mipCoord);
	vtkImageData* GetSubsampledChunkImageData(const OmMipChunkCoord &mipCoord);
	
	template< typename T> vtkImageData* SubsampleImageData(vtkImageData* srcData);

	set< OmMipChunkCoord > mEditedLeafChunks;	//set of edited chunks that need rebuild
	
private:
	OmThreadChunkThreadedCache* mThreadChunkThreadedCache;

	int mBytesPerSample;		//VTK_UNSIGNED_CHAR (1 byte) or VTK_UNSIGNED_INT (4 bytes)
	
	QString mDirectoryPath;          // ex. "./" or "images/out/"
	QString mFilename;
	bool sourceFilesWereSet;

	friend class OmDataArchiveProject;
};

#endif

