#include "omVolume.h"
#include "omVolumeCuller.h"

#include "segment/omSegmentEditor.h"
#include "common/omGl.h"
#include "common/omDebug.h"
#include "project/omProject.h"

#define DEBUG 0

//init instance pointer
OmVolume *OmVolume::mspInstance = 0;

/////////////////////////////////
///////
///////         OmVolume
///////

OmVolume::OmVolume()
{
	//debug("genone","OmVolume::OmVolume()");
}

OmVolume::~OmVolume()
{
	//debug("genone","OmVolume::~OmVolume()");
}

void OmVolume::Initialize()
{
	mNormToSpaceMat = Matrix4 < float >::IDENTITY;
	mNormToSpaceInvMat = Matrix4 < float >::IDENTITY;

	mSpaceToUserMat = Matrix4 < float >::IDENTITY;
	mSpaceToUserInvMat = Matrix4 < float >::IDENTITY;

	//defaults
	SetChunkDimension(128);
	SetDataDimensions(Vector3i(128, 128, 128));

	SetUserScale(Vector3i(1, 1, 1));
	SetScale(Vector3i(10, 10, 10));

	mDataResolution = Vector3f::ONE;
	SetStretchValues();
}

OmVolume *OmVolume::Instance()
{
	if (NULL == mspInstance) {
		mspInstance = new OmVolume;
		mspInstance->Initialize();
	}
	return mspInstance;
}

void OmVolume::Delete()
{
	if (mspInstance)
		delete mspInstance;
	mspInstance = NULL;
}

/////////////////////////////////
///////          Transform Methods

Vector3 < float > OmVolume::GetScale()
{
	return Vector3 < float >(
	       Instance()->mNormToSpaceMat.m00,
	       Instance()->mNormToSpaceMat.m11,
	       Instance()->mNormToSpaceMat.m22);

}

bool OmVolume::SetScale(const Vector3 < float >&scale)
{
	//set scale
	Instance()->mNormToSpaceMat.m[0][0] = scale.x;
	Instance()->mNormToSpaceMat.m[1][1] = scale.y;
	Instance()->mNormToSpaceMat.m[2][2] = scale.z;
	//set inverse and return if invertable
	return Instance()->mNormToSpaceMat.getInverse(Instance()->mNormToSpaceInvMat);
}

Vector3 < float > OmVolume::GetUserScale()
{
	return Vector3 < float >(Instance()->mSpaceToUserMat.m00,
				 Instance()->mSpaceToUserMat.m11,
				 Instance()->mSpaceToUserMat.m22);
}

bool OmVolume::SetUserScale(const Vector3 < float >&scale)
{
	//set scale
	Instance()->mSpaceToUserMat.m[0][0] = scale.x;
	Instance()->mSpaceToUserMat.m[1][1] = scale.y;
	Instance()->mSpaceToUserMat.m[2][2] = scale.z;
	//set inverse and return if invertable
	return Instance()->mSpaceToUserMat.getInverse(Instance()->mSpaceToUserInvMat);
}

QString OmVolume::GetUnit()
{
	return Instance()->unitString;
}

void OmVolume::SetUnit(QString unit)
{
	Instance()->unitString = unit;
}

/////////////////////////////////
///////          Coordinate Frame Methods

SpaceCoord OmVolume::NormToSpaceCoord(const NormCoord & norm)
{
	return Instance()->mNormToSpaceMat * norm;
}

NormCoord OmVolume::SpaceToNormCoord(const SpaceCoord & spatial)
{
	return Instance()->mNormToSpaceInvMat * spatial;
}

NormBbox OmVolume::SpaceToNormBbox(const SpaceBbox &spacialBbox)
{
	return NormBbox(SpaceToNormCoord(spacialBbox.getMin()), SpaceToNormCoord(spacialBbox.getMax()));
}

SpaceBbox OmVolume::NormToSpaceBbox(const NormBbox & normBbox)
{
	return SpaceBbox(NormToSpaceCoord(normBbox.getMin()), NormToSpaceCoord(normBbox.getMax()));
}

/**
 *	Converts DataCoord to NormCoord based on source extent of data volume.
 *	NormCoords are used for relative location information and not absolute data access.
 *	centered : causes normalized coordinates to represent center of rectangular pixel.
 */
NormCoord OmVolume::DataToNormCoord(const DataCoord & data, bool centered)
{
	const DataBbox & extent = GetDataExtent();
	Vector3 < float >scale = extent.getMax() - extent.getMin() + Vector3 < int >::ONE;
	Vector3 < float >offset = centered ? Vector3 < float >(0.5f, 0.5f, 0.5f) : Vector3 < float >::ZERO;
	return NormCoord((offset + data) / scale);
}

/**
 *	Returns rectangular pixel that contains given normalized coordinate.
 */
DataCoord OmVolume::NormToDataCoord(const NormCoord & norm)
{
	const DataBbox & extent = GetDataExtent();
	Vector3 < int >scale = extent.getMax() - extent.getMin() + Vector3 < int >::ONE;
	return DataCoord(floor(norm.x * scale.x), floor(norm.y * scale.y), floor(norm.z * scale.z));
}

/**
 *	Returns normalized bbox that encloses rectangular pixels in given data bbox.
 */
NormBbox OmVolume::DataToNormBbox(const DataBbox & dataBbox)
{
	return NormBbox(DataToNormCoord(dataBbox.getMin(), false),
			DataToNormCoord(dataBbox.getMax() + Vector3 < int >::ONE, false));
}

/**
 *	Returns data bbox contained by given normalized bounding box.
 */
DataBbox OmVolume::NormToDataBbox(const NormBbox & normBbox)
{
	const DataBbox & extent = GetDataExtent();
	Vector3 < float >normalized_pixel_dim =
	    Vector3 < float >::ONE / (extent.getMax() - extent.getMin() + Vector3 < float >::ONE);

	NormCoord extent_min = normBbox.getMin() + normalized_pixel_dim * 0.5f;
	NormCoord extent_max = normBbox.getMax() - normalized_pixel_dim * 0.5f;

	return DataBbox(NormToDataCoord(extent_min), NormToDataCoord(extent_max));
}

/////////////////////////////////
///////          Data Properties

const DataBbox & OmVolume::GetDataExtent()
{
	return Instance()->mDataExtent;
}

void OmVolume::SetDataExtent(const DataBbox & extent)
{
	assert(false);
	Instance()->mDataExtent = extent;
}

Vector3i OmVolume::GetDataDimensions()
{
	return GetDataExtent().getMax() - GetDataExtent().getMin() + Vector3i::ONE;
}

void OmVolume::SetDataDimensions(const Vector3i & dim)
{
	Instance()->mDataExtent = DataBbox(Vector3i::ZERO, dim - Vector3i::ONE);
}

Vector3f OmVolume::GetDataResolution()
{
	return Instance()->mDataResolution;
}

bool OmVolume::SetDataResolution(const Vector3f & res)
{
	Instance()->mDataResolution = res;

	//update scale
	Vector3i data_dims = GetDataExtent().getMax() - GetDataExtent().getMin() + Vector3 < int >::ONE;
	SetStretchValues();
	return SetScale(Instance()->mDataResolution * data_dims);
}

void OmVolume::CheckDataResolution()
{
	Vector3f res=Instance()->mDataResolution;
	if (res == Vector3i::ZERO){
		res = Vector3i::ONE;

	}
	Instance()->SetDataResolution( res);	
}	
		
int OmVolume::GetChunkDimension()
{
	return Instance()->mChunkDim;
}

void OmVolume::SetChunkDimension(int dim)
{
	Instance()->mChunkDim = dim;
}

/////////////////////////////////
///////          Channel Manager Method

OmChannel & OmVolume::GetChannel(OmId id)
{
	return Instance()->mChannelManager.Get(id);
}

OmChannel & OmVolume::AddChannel()
{
	OmChannel & r_channel = Instance()->mChannelManager.Add();
	OmProject::Save();
	return r_channel;
}

void OmVolume::RemoveChannel(OmId id)
{
	Instance()->mChannelManager.Get(id).DeleteVolumeData();
	Instance()->mChannelManager.Remove(id);
	OmProject::Save();
}

bool OmVolume::IsChannelValid(OmId id)
{
	return Instance()->mChannelManager.IsValid(id);
}

const set < OmId > & OmVolume::GetValidChannelIds()
{
	return Instance()->mChannelManager.GetValidIds();
}

bool OmVolume::IsChannelEnabled(OmId id)
{
	return Instance()->mChannelManager.IsEnabled(id);
}

void OmVolume::SetChannelEnabled(OmId id, bool enable)
{
	Instance()->mChannelManager.SetEnabled(id, enable);
}

/////////////////////////////////
///////          Segmentation Manager Method

OmSegmentation & OmVolume::GetSegmentation(OmId id)
{
	Instance()->mSegmentationManager.Get(id).SetBytesPerSample(4);
	return Instance()->mSegmentationManager.Get(id);
}

OmSegmentation & OmVolume::AddSegmentation()
{
	OmSegmentation & r_segmentation = Instance()->mSegmentationManager.Add();
	OmProject::Save();
	return r_segmentation;
}

void OmVolume::RemoveSegmentation(OmId id)
{


	//const set < OmId > channelIDs = Instance()->mChannelManager.Get(id).GetValidFilterIds();
	//set < OmId >::iterator obj_it;
	foreach(OmId channelID, OmVolume::GetValidChannelIds()) {
		OmChannel & channel = OmVolume::GetChannel(channelID);
		foreach(OmId filterID, channel.GetValidFilterIds()) {
			OmFilter2d &filter = channel.GetFilter(filterID);
			if (filter.GetSegmentation() == id){
				filter.SetSegmentation(0);				
			}
		}
	}	
	Instance()->mSegmentationManager.Get(id).DeleteCaches();
	Instance()->mSegmentationManager.Get(id).PrepareForCompleteDelete();
	Instance()->mSegmentationManager.Get(id).DeleteVolumeData();
	Instance()->mSegmentationManager.Remove(id);
	OmProject::Save();
}

bool OmVolume::IsSegmentationValid(OmId id)
{
	return Instance()->mSegmentationManager.IsValid(id);
}

const set < OmId > & OmVolume::GetValidSegmentationIds()
{
	return Instance()->mSegmentationManager.GetValidIds();
}

bool OmVolume::IsSegmentationEnabled(OmId id)
{
	return Instance()->mSegmentationManager.IsEnabled(id);
}

void OmVolume::SetSegmentationEnabled(OmId id, bool enable)
{
	Instance()->mSegmentationManager.SetEnabled(id, enable);
}

QList < SegmentDataWrapper > OmVolume::GetSelectedSegmentIDs()
{
	QList < SegmentDataWrapper > segmentationsAndSegments;

	foreach(OmId segmentationID, OmVolume::GetValidSegmentationIds()) {
		OmSegmentation & segmentation = OmVolume::GetSegmentation(segmentationID);

		foreach(OmId segmentID, segmentation.GetSelectedSegmentIds()) {
			SegmentDataWrapper seg( segmentationID, segmentID);
			segmentationsAndSegments.append(seg);
		}
	}

	return segmentationsAndSegments;
}

/////////////////////////////////
///////          Draw Method

/*
 *	Draw 3d components of the volume.  Call segmentation manager to draw each 
 *	segmentation in the volume using a transformed volume culler.
 */
void OmVolume::Draw(const OmVolumeCuller & rCuller)
{
	//transform to normal frame
	glPushMatrix();
	glMultMatrixf(Instance()->mNormToSpaceMat.ml);

	//draw volume axis
	if (rCuller.CheckDrawOption(DRAWOP_DRAW_VOLUME_AXIS)) {
		glDrawPositiveAxis();
	}
	//return if no chunk level drawing
	if (!rCuller.CheckDrawOption(DRAWOP_LEVEL_CHUNKS)) {
		glPopMatrix();
		return;
	}
	//form culler for this volume and call draw on all volumes
	OmVolumeCuller volume_culler =
	    rCuller.GetTransformedCuller(Instance()->mNormToSpaceMat, Instance()->mNormToSpaceInvMat);
	Instance()->mSegmentationManager.CallEnabled < const OmVolumeCuller & >(&OmSegmentation::Draw, volume_culler);

	//pop matrix
	glPopMatrix();
}

/*
 *	Draw voxels that have been selected for editing.
 */
void OmVolume::DrawEditSelectionVoxels()
{
	//transform to normal frame
	glPushMatrix();
	glMultMatrixf(Instance()->mNormToSpaceMat.ml);

	OmSegmentEditor::DrawEditSelectionVoxels();

	//pop matrix
	glPopMatrix();
}

Vector2f OmVolume::GetStretchValues(ViewType plane)
{
	Vector3f stretch =Instance()->mDataStretchValues;
	Vector2f ret;

	switch(plane){
	case XY_VIEW:
		ret = Vector2f(stretch.x,stretch.y);
		break;
	case YZ_VIEW:
		ret = Vector2f(stretch.z,stretch.y);
		break;
	case XZ_VIEW:
		ret = Vector2f(stretch.x,stretch.z);
		break;
	}

	return ret;
}

void OmVolume::SetStretchValues()
{
	Vector3f res =Instance()->mDataResolution;
	if ((res.x<=res.y)&&(res.x<=res.z)){
		Instance()->mDataStretchValues.x = 1.0;
		Instance()->mDataStretchValues.y = res.y/res.x;
		Instance()->mDataStretchValues.z = res.z/res.x;
	} else {
		if (res.y<=res.z){
			Instance()->mDataStretchValues.x = res.x/res.y;
			Instance()->mDataStretchValues.y = 1.0;
			Instance()->mDataStretchValues.z = res.z/res.y;
		} else {
			Instance()->mDataStretchValues.x = res.x/res.z;
			Instance()->mDataStretchValues.y = res.y/res.z;	
			Instance()->mDataStretchValues.z = 1.0;
		}
	}
}


/////////////////////////////////
///////         Print Method

void OmVolume::Print()
{

	//debug("FIXME", << "   Channels:" << endl;
	mChannelManager.CallValid(&OmChannel::Print);

	//debug("FIXME", << "   Segmentations:" << endl;
	mSegmentationManager.CallValid(&OmSegmentation::Print);
}
