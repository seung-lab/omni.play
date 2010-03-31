#ifndef OM_MIP_VOLUME_H 
#define OM_MIP_VOLUME_H

/*
 *	OmMipVolume allows a source volume to be coverted to and accessed as compressed segments
 *	of regular size called MipChunks.  MipVolume extends GenericCache to allow for MipChunk caching.
 *
 *	Brett Warne - bwarne@mit.edu - 7/19/09
 */

#include "omDataVolume.h"
#include "omMipChunkCoord.h"
#include "system/omThreadedCache.h"
#include "system/omSystemTypes.h"
#include "common/omStd.h"
#include "common/omSerialization.h"
#include <vmmlib/vmmlib.h>
#include <vmmlib/serialization.h>
using namespace vmml;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include <list>
using std::list;

#include <QFileInfo>

class OmMipChunk;
class vtkImageData;

//enum subsampling methods
enum SubsampleMode { SUBSAMPLE_MEAN, SUBSAMPLE_MODE, SUBSAMPLE_RANDOM, SUBSAMPLE_NONE };

//mipvolume state
enum MipVolumeBuildState { MIPVOL_UNBUILT, MIPVOL_BUILT, MIPVOL_BUILDING };

//typedef chunk cache
typedef OmThreadedCache< OmMipChunkCoord, OmMipChunk > MipChunkThreadedCache;

class OmMipVolume : public OmDataVolume, public MipChunkThreadedCache {
	
public:
	OmMipVolume();
	~OmMipVolume();
	
	void Flush();

	void SetFilename(const string &);
	string GetFilename();
	virtual void SetDirectoryPath(const string &);
	string GetDirectoryPath();
	
	string MipLevelInternalDataPath(int level);
	string MipChunkMetaDataPath(const OmMipChunkCoord &rMipCoord);
	
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
	
	void UpdateMipProperties();
		
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
	void GetChunk(shared_ptr<OmMipChunk> &p_value, const OmMipChunkCoord &rMipCoord, bool block=true);
	void StoreChunk(const OmMipChunkCoord &, OmMipChunk *);
	
	//mip data accessors
	uint32_t GetVoxelValue(const DataCoord &vox);
	void SetVoxelValue(const DataCoord &vox, uint32_t value);

	//build methods
	void Build();
	bool BuildVolume();
	virtual void BuildChunk(const OmMipChunkCoord &);
	void BuildChunkAndParents(const OmMipChunkCoord &mipCoord);
	void BuildEditedLeafChunks();
	
	//io
	bool ImportSourceData();
	void ImportSourceDataSlice();
	void ExportInternalData(QString fileNameAndPath);
	virtual void ExportDataFilter(vtkImageData *) { }

	
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
	MipVolumeBuildState mBuildState;
	int mMipLeafDim;			//must be even
	int mMipRootLevel;			//inferred from leaf dim and source data extent
	SubsampleMode mSubsampleMode;		//method to use when subsampling	
	bool mStoreChunkMetaData;		//do chunks have metadata

	set< OmMipChunkCoord > mEditedLeafChunks;	//set of edited chunks that need rebuild

	
private:
	OmMipChunk* HandleCacheMiss(const OmMipChunkCoord &key);
	
	//subsample data methods
	vtkImageData* GetSubsampledImageDataFromChildren(const OmMipChunkCoord &mipCoord);
	vtkImageData* GetSubsampledChunkImageData(const OmMipChunkCoord &mipCoord);
	
	template< typename T> vtkImageData* SubsampleImageData(vtkImageData* srcData, int subsampleMode);
	template< typename T > T CalculateMode( T* array, int size);
	template< typename T > T CalculateAverage( T* array, int size);
	
	string mDirectoryPath;          // ex. "./" or "images/out/"
	string mFilename;
	bool sourceFilesWereSet;

	// TODO: delete these: no longer used
	string mSourceDirectoryPath;
	string mSourceFilenameRegex;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int file_version);
};

/////////////////////////////////
///////		 Serialization

BOOST_CLASS_VERSION(OmMipVolume, 0)

template<class Archive>
void 
OmMipVolume::serialize(Archive & ar, const unsigned int file_version) 
{
	//mip volume
	ar & mDirectoryPath;
	ar & mMipLeafDim;
	ar & mMipRootLevel;
	
	ar & mSourceDirectoryPath;
	ar & mSourceFilenameRegex;
	
	ar & mSubsampleMode;
	ar & mBuildState;
	ar & mStoreChunkMetaData;
	
}


#endif

