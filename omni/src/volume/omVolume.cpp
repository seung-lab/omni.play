
#include "omVolume.h"
#include "omVolumeCuller.h"

#include "segment/omSegmentEditor.h"
#include "common/omGl.h"
#include "system/omDebug.h"
#include "system/omProject.h"

#define DEBUG 0

//init instance pointer
OmVolume *OmVolume::mspInstance = 0;

#pragma mark -
#pragma mark OmVolume
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

void
 OmVolume::Initialize()
{
	mNormToSpaceMat = Matrix4 < float >::IDENTITY;
	mNormToSpaceInvMat = Matrix4 < float >::IDENTITY;

	//defaults
	SetChunkDimension(128);
	SetDataDimensions(Vector3i(128, 128, 128));
	SetScale(Vector3i(10, 10, 10));

	//temp
	mDataResolution = Vector3i::ZERO;
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

#pragma mark
#pragma mark Accessor Methods
/////////////////////////////////
///////          Accessor Methods

#pragma mark
#pragma mark Transform Methods
/////////////////////////////////
///////          Transform Methods

Vector3 < float > OmVolume::GetScale()
{
	return Vector3 < float >(Instance()->mNormToSpaceMat.m00,
				 Instance()->mNormToSpaceMat.m11, Instance()->mNormToSpaceMat.m22);
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

/*
Vector3<float>
OmVolume::GetTranslation() {
	return Instance()->mNormToSpaceMat.getTranslation();
}

bool 
OmVolume::SetTranslation(const Vector3<float> &trans) {
	//set translation
	Instance()->mNormToSpaceMat.setTranslation(trans);
	//set inverse and return if invertable
	return Instance()->mNormToSpaceMat.getInverse(Instance()->mNormToSpaceInvMat);
}
*/

#pragma mark
#pragma mark Coordinate Frame Methods
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

NormBbox OmVolume::SpaceToNormBbox(const AxisAlignedBoundingBox < float >&spacialBbox)
{
	return NormBbox(SpaceToNormCoord(spacialBbox.getMin()), SpaceToNormCoord(spacialBbox.getMax()));
}

SpaceBbox OmVolume::NormToSpaceBbox(const NormBbox & normBbox)
{
	return SpaceBbox(NormToSpaceCoord(normBbox.getMin()), NormToSpaceCoord(normBbox.getMax()));
}

/*
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

/*
 *	Returns rectangular pixel that contains given normalized coordinate.
 */
DataCoord OmVolume::NormToDataCoord(const NormCoord & norm)
{

	const DataBbox & extent = GetDataExtent();
	Vector3 < int >scale = extent.getMax() - extent.getMin() + Vector3 < int >::ONE;
	return DataCoord(floor(norm.x * scale.x), floor(norm.y * scale.y), floor(norm.z * scale.z));
}

/*
 *	Returns normalized bbox that encloses rectangular pixels in given data bbox.
 */
NormBbox OmVolume::DataToNormBbox(const DataBbox & dataBbox)
{

	return NormBbox(DataToNormCoord(dataBbox.getMin(), false),
			DataToNormCoord(dataBbox.getMax() + Vector3 < int >::ONE, false));
}

/*
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

#pragma mark
#pragma mark Data Properties
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
	return SetScale(Instance()->mDataResolution * data_dims);
}

int OmVolume::GetChunkDimension()
{
	return Instance()->mChunkDim;
}

void OmVolume::SetChunkDimension(int dim)
{
	Instance()->mChunkDim = dim;
}

#pragma mark
#pragma mark Channel Manager Methods
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

#pragma mark
#pragma mark Segmentation Manager Methods
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

#pragma mark
#pragma mark Draw Methods
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

#pragma mark
#pragma mark Print Methods
/////////////////////////////////
///////         Print Method

void OmVolume::Print()
{

	//debug("FIXME", << "   Channels:" << endl;
	mChannelManager.CallValid(&OmChannel::Print);

	//debug("FIXME", << "   Segmentations:" << endl;
	mSegmentationManager.CallValid(&OmSegmentation::Print);
}
