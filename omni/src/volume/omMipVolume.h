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
#include "volume/omMipChunkCoord.h"
#include "volume/omVolumeTypes.hpp"
#include "datalayer/omDataWrapper.h"

#include <QFileInfo>

class OmDataPath;
class OmHdf5;
class OmHdf5;
class OmMipChunk;
class OmMipVolumeCache;
class OmVolume;
class OmVolumeData;

enum MipVolumeBuildState { MIPVOL_UNBUILT = 0,
						   MIPVOL_BUILT,
						   MIPVOL_BUILDING };

class OmMipVolume : public OmVolume {
public:
	OmMipVolume();
	virtual ~OmMipVolume(){}

	virtual std::string GetDirectoryPath() = 0;
	virtual std::string GetName() = 0;
	virtual void loadVolData() = 0;
	virtual boost::shared_ptr<OmVolumeData> getVolData() = 0;
	virtual ObjectType getVolumeType() = 0;
	virtual OmId getID() = 0;
	virtual OmMipVolumeCache* getDataCache() = 0;

	inline bool IsVolumeReadyForDisplay(){
		return MIPVOL_BUILT == mBuildState;
	}

	void ExportInternalData(const QString& fileNameAndPath);

	std::string MipLevelInternalDataPath(const int level);
	std::string MipChunkMetaDataPath(const OmMipChunkCoord &rMipCoord);

	//source data properties
	void SetSourceFilenamesAndPaths(const QFileInfoList&);
	QFileInfoList GetSourceFilenamesAndPaths();
	bool IsSourceValid();

	// data properties
	Vector3i GetChunkDimensions() const;

	void SetChunksStoreMetaData(bool);
	bool GetChunksStoreMetaData();

	bool IsVolumeDataBuilt();
	bool IsBuilding();

	void UpdateMipProperties(OmDataPath&);

	//TODO: move to volume
	//mip level method
	void UpdateRootLevel();
	int GetRootMipLevel();
	Vector3i MipLevelDataDimensions(int) const;
	Vector3i MipLevelDimensionsInMipChunks(int level);

	/*
	 *	Returns MipChunkCoord containing given data coordinate for given MipLevel
	 */
	OmMipChunkCoord DataToMipCoord(const DataCoord& dataCoord, const int level) {
		return DataToMipCoord(dataCoord, level, GetChunkDimensions());
	}
	static OmMipChunkCoord DataToMipCoord(const DataCoord& dataCoord,
										  const int level,
										  const Vector3i& chunkDimensions){
		if( dataCoord.x < 0 ||
			dataCoord.y < 0 ||
			dataCoord.z < 0 ){
			return OmMipChunkCoord::NULL_COORD;
		}

		const int factor = om::pow2int(level);
		return OmMipChunkCoord(level,
							   dataCoord.x / factor / chunkDimensions.x,
							   dataCoord.y / factor / chunkDimensions.y,
							   dataCoord.z / factor / chunkDimensions.z);
	}

	OmMipChunkCoord NormToMipCoord(const NormCoord &normCoord, int level);
	DataBbox MipCoordToDataBbox(const OmMipChunkCoord &, int level);
	NormBbox MipCoordToNormBbox(const OmMipChunkCoord &);
	DataBbox DataToDataBBox(const DataCoord &vox, const int level){
		return MipCoordToDataBbox(DataToMipCoord(vox, level), level);
	}

	//mip chunk methods
	OmMipChunkCoord RootMipChunkCoordinate();
	int MipChunkDimension(int level);
	bool ContainsMipChunkCoord(const OmMipChunkCoord &rMipCoord);
	void ValidMipChunkCoordChildren(const OmMipChunkCoord &rMipCoord,
									std::set<OmMipChunkCoord> &children);
	void GetChunk(OmMipChunkPtr& p_value, const OmMipChunkCoord &rMipCoord);

	//mip data accessors
	quint32 GetVoxelValue(const DataCoord &vox);
	void SetVoxelValue(const DataCoord &vox, quint32 value);

	//build methods
	void Build(OmDataPath & dataset);
	bool BuildThreadedVolume();
	virtual void doBuildThreadedVolume() = 0;
	void BuildEditedLeafChunks();

	//comparison methods
	static bool CompareVolumes(OmMipVolume *, OmMipVolume *);
	static bool CompareChunks(const OmMipChunkCoord&,
							  OmMipVolume*, OmMipVolume*);

	void copyAllMipDataIntoMemMap();
	bool areImportFilesImages();

	Vector3i get_dims(const OmDataPath dataset );

	void ImportSourceDataSlice();

	void DeleteVolumeData();

	bool ContainsVoxel(const DataCoord &vox);

	QFileInfoList mSourceFilenamesAndPaths;

	Vector3i getDimsRoundedToNearestChunk(const int level) const;
	OmVolDataType getVolDataType(){ return mVolDataType; }
	std::string getVolDataTypeAsStr(){
		return OmVolumeTypeHelpers::GetTypeAsString(mVolDataType);
	}

	virtual int GetBytesPerSample() const = 0;

	virtual void SetVolDataType(const OmVolDataType) = 0;

	uint64_t ComputeChunkPtrOffset(const OmMipChunkCoord& coord) const;

protected:
	OmVolDataType mVolDataType;

	virtual OmDataWrapperPtr doExportChunk(const OmMipChunkCoord&)=0;

	void BuildBlankVolume(const Vector3i & dims);

	//state
	void SetBuildState(MipVolumeBuildState);

	virtual bool ImportSourceData(const OmDataPath&) = 0;

	//mipvolume disk data
	void UpdateRootMipLevel();

	//mip data
	int mBuildState;
	int mMipLeafDim;			//must be even
	int mMipRootLevel;			//inferred from leaf dim and source data extent
	bool mStoreChunkMetaData;		//do chunks have metadata

	std::set<OmMipChunkCoord> mEditedLeafChunks;	//set of edited chunks that need rebuild

	void updateMinMax(const double, const double);
	double mMaxVal;
	double mMinVal;
	bool mWasBounded;

private:
	bool sourceFilesWereSet;

	void copyChunkFromMemMapToHDF5(const OmMipChunkCoord& coord);
	uint32_t computeTotalNumChunks();

	friend class OmMipChunk;
	friend class OmDataArchiveProject;
	friend class OmVolumeData;
	friend class OmUpgradeTo14;
};

#endif

