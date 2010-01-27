#ifndef OM_VOLUME_H
#define OM_VOLUME_H

/*
 *	OmVolume is the abstract represntation of multiple forms of data enclosed by
 *	a region of space.
 *
 *	Brett Warne - bwarne@mit.edu - 2/6/09
 */



#include "omVolumeTypes.h"
#include "omChannel.h"
#include "omSegmentation.h"

#include "system/omSystemTypes.h"
#include "system/omGenericManager.h"
#include "system/omManageableObject.h"
#include "common/omGl.h"
#include "common/omStd.h"


#include <vmmlib/vmmlib.h>
#include <vmmlib/serialization.h>
using namespace vmml;



class OmChannel;
class OmVolumeCuller;

struct SegmentIDhelper {
	OmId    segmentationID;
	QString segmentationName;
	OmId    segmentID;
	QString segmentName;
};

class OmVolume : public OmManageableObject {

public:
	static OmVolume* Instance();
	static void Delete();
	
	//transform methods
	static Vector3<float> GetScale();
	static bool SetScale(const Vector3<float> &vec);
	
	//data properties
	static const DataBbox& GetDataExtent();
	static void SetDataExtent(const DataBbox& extent);
	static Vector3i GetDataDimensions();
	static void SetDataDimensions(const Vector3i& dim);
	static Vector3f GetDataResolution();
	static bool SetDataResolution(const Vector3f& res);
	static int GetChunkDimension();
	static void SetChunkDimension(int);
	
	//coordinate frame methods
	static NormCoord DataToNormCoord(const DataCoord &data, bool centered = true);
	static DataCoord NormToDataCoord(const NormCoord &norm);
	static SpaceCoord NormToSpaceCoord(const NormCoord &norm);
	static NormCoord SpaceToNormCoord(const SpaceCoord &spatial);
	
	static NormBbox DataToNormBbox(const DataBbox &dataBbox);
	static DataBbox NormToDataBbox(const NormBbox &normBbox);
	static SpaceBbox NormToSpaceBbox(const NormBbox &normBbox);
	static NormBbox SpaceToNormBbox(const SpaceBbox &spacialBbox);
	
	//mip class management
	static OmChannel& GetChannel(OmId id);
	static OmChannel& AddChannel();
	static void RemoveChannel(OmId id);
	static bool IsChannelValid(OmId id);
	static const set<OmId>& GetValidChannelIds();
	static bool IsChannelEnabled(OmId id);
	static void SetChannelEnabled(OmId id, bool enable);
	
	static OmSegmentation& GetSegmentation(OmId id);
	static OmSegmentation& AddSegmentation();
	static void RemoveSegmentation(OmId id);
	static bool IsSegmentationValid(OmId id);
	static const set<OmId>& GetValidSegmentationIds();
	static bool IsSegmentationEnabled(OmId id);
	static void SetSegmentationEnabled(OmId id, bool enable);
	static QList< SegmentIDhelper > GetSelectedSegmentIDs();
	
	//drawing
	static void Draw(const OmVolumeCuller &);
	static void DrawEditSelectionVoxels();
	
	
	void Print();
	
protected:
	// singleton constructor, copy constructor, assignment operator protected
	OmVolume();
	~OmVolume();
	OmVolume(const OmVolume&);
	OmVolume& operator= (const OmVolume&);
	
	
private:
	//internal initialization
	void Initialize();
	
	//singleton
	static OmVolume* mspInstance;
	
	//transforms from normailzed unit cube external spatial coverage
	Matrix4<float> mNormToSpaceMat;
	Matrix4<float> mNormToSpaceInvMat;

	//data properties
	DataBbox mDataExtent;
	Vector3f mDataResolution;	//units per voxel
	int mChunkDim;
	
	//data managers
	OmGenericManager<OmChannel> mChannelManager;
	OmGenericManager<OmSegmentation> mSegmentationManager;
	
	//serialization
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int file_version);
};






#pragma mark 
#pragma mark Serialization
/////////////////////////////////
///////		 Serialization

BOOST_CLASS_VERSION(OmVolume, 0)

template<class Archive>
void 
OmVolume::serialize(Archive & ar, const unsigned int file_version) {
	
	//volume props
	ar & mNormToSpaceMat & mNormToSpaceInvMat;
	ar & mDataExtent & mDataResolution;
	ar & mChunkDim;
	
	//managers
	ar & mChannelManager;
	ar & mSegmentationManager;
}




#endif

