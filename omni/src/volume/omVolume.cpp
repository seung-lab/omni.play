#include "volume/omVolume.h"

/////////////////////////////////
///////         OmVolume

OmVolume::OmVolume()
{
	//debug("genone","OmVolume::OmVolume()");
        mNormToSpaceMat = Matrix4 < float >::IDENTITY;
        mNormToSpaceInvMat = Matrix4 < float >::IDENTITY;

        mSpaceToUserMat = Matrix4 < float >::IDENTITY;
        mSpaceToUserInvMat = Matrix4 < float >::IDENTITY;

        //defaults
        mDataResolution = Vector3f::ONE;
        SetChunkDimension(128);
        SetDataDimensions(Vector3i(128, 128, 128));
        SetUserScale(Vector3i(1, 1, 1));

        unitString = "";
}

OmVolume::~OmVolume()
{
	//debug("genone","OmVolume::~OmVolume()");
}

/////////////////////////////////
///////          Transform Methods

Vector3 < float > OmVolume::GetScale()
{
	return Vector3 < float >(
	       mNormToSpaceMat.m00,
	       mNormToSpaceMat.m11,
	       mNormToSpaceMat.m22);
}

bool OmVolume::SetScale(const Vector3 < float >&scale)
{
	//set scale
	mNormToSpaceMat.m[0][0] = scale.x;
	mNormToSpaceMat.m[1][1] = scale.y;
	mNormToSpaceMat.m[2][2] = scale.z;

	//set inverse and return if invertable
	return mNormToSpaceMat.getInverse(mNormToSpaceInvMat);
}

Vector3 < float > OmVolume::GetUserScale()
{
	return Vector3 < float >(mSpaceToUserMat.m00,
				 mSpaceToUserMat.m11,
				 mSpaceToUserMat.m22);
}

bool OmVolume::SetUserScale(const Vector3 < float >&scale)
{
	//set scale
	mSpaceToUserMat.m[0][0] = scale.x;
	mSpaceToUserMat.m[1][1] = scale.y;
	mSpaceToUserMat.m[2][2] = scale.z;

	Update();

	//set inverse and return if invertable
	return mSpaceToUserMat.getInverse(mSpaceToUserInvMat);
}

QString OmVolume::GetUnit()
{
	return unitString;
}

void OmVolume::SetUnit(QString unit)
{
	unitString = unit;
}

/////////////////////////////////
///////          Coordinate Frame Methods

SpaceCoord OmVolume::NormToSpaceCoord(const NormCoord & norm)
{
	return mNormToSpaceMat * norm;
}

NormCoord OmVolume::SpaceToNormCoord(const SpaceCoord & spatial)
{
	return mNormToSpaceInvMat * spatial;
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
	return mDataExtent;
}

void OmVolume::SetDataExtent(const DataBbox & extent)
{
	assert(false);
	mDataExtent = extent;
	Update();
}

Vector3i OmVolume::GetDataDimensions()
{
	return GetDataExtent().getMax() - GetDataExtent().getMin() + Vector3i::ONE;
}

void OmVolume::SetDataDimensions(const Vector3i & dim)
{
	mDataExtent = DataBbox(Vector3i::ZERO, dim - Vector3i::ONE);
	Update();
}

Vector3f OmVolume::GetDataResolution()
{
	return mDataResolution;
}

bool OmVolume::SetDataResolution(const Vector3f & res)
{
	mDataResolution = res;
	Update();
}

bool OmVolume::Update()
{
	//update scale
	Vector3i data_dims = GetDataExtent().getMax() - GetDataExtent().getMin() + Vector3 < int >::ONE;
	SetStretchValues();
	return SetScale(mDataResolution * data_dims);
}

int OmVolume::GetChunkDimension()
{
	return mChunkDim;
}

void OmVolume::SetChunkDimension(int dim)
{
	mChunkDim = dim;
}

Vector2f OmVolume::GetStretchValues(ViewType plane)
{
	Vector3f stretch =mDataStretchValues;
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
	Vector3f res = mDataResolution;
	if ((res.x<=res.y)&&(res.x<=res.z)){
		mDataStretchValues.x = 1.0;
		mDataStretchValues.y = res.y/res.x;
		mDataStretchValues.z = res.z/res.x;
	} else {
		if (res.y<=res.z){
			mDataStretchValues.x = res.x/res.y;
			mDataStretchValues.y = 1.0;
			mDataStretchValues.z = res.z/res.y;
		} else {
			mDataStretchValues.x = res.x/res.z;
			mDataStretchValues.y = res.y/res.z;	
			mDataStretchValues.z = 1.0;
		}
	}
}

DataCoord OmVolume::SpaceToDataCoord(const SpaceCoord & spacec)
{
	return NormToDataCoord(SpaceToNormCoord(spacec));
}

SpaceCoord OmVolume::DataToSpaceCoord(const DataCoord & datac)
{
	return NormToSpaceCoord(DataToNormCoord(datac));
}

DataBbox OmVolume::SpaceToDataBbox(const SpaceBbox & spacebox)
{
	return NormToDataBbox(SpaceToNormBbox(spacebox));
}

SpaceBbox OmVolume::DataToSpaceBbox(const DataBbox & databox)
{
	return NormToSpaceBbox(DataToNormBbox(databox));
}
