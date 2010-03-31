#include "omDataVolume.h"

#include "common/omVtk.h"
#include "common/omException.h"
#include "project/omProject.h"
#include "utility/omImageDataIo.h"

/////////////////////////////////
///////         OmDataVolume Class
OmDataVolume::OmDataVolume()
{
	mBytesPerSample = -1;
	mIsOpen = false;
}

OmDataVolume::~OmDataVolume()
{
	Close();
}

/////////////////////////////////
///////          Property Accessor Methods

const Vector3 < int > OmDataVolume::GetDimensions()
{
	return GetExtent().getUnitDimensions();
}

void OmDataVolume::SetBytesPerSample(int bytesPerSample)
{
	mBytesPerSample = bytesPerSample;
}

const int OmDataVolume::GetBytesPerSample()
{
	return mBytesPerSample;
}

void OmDataVolume::SetOpen(bool state)
{
	mIsOpen = state;
}

/////////////////////////////////
///////          State Accessor Methods

bool OmDataVolume::IsOpen() const
{
	return mIsOpen;
}

bool OmDataVolume::IsDirty() const
{
	assert(false);
	return false;
}

/////////////////////////////////
///////          Data I/O Methods

/*
 *	Opens this DataVolume with entire source data
 */
void OmDataVolume::Open()
{
	//debug("genone","OmDataVolume::Open()");
	SetOpen(true);
}

/*
 *	Flushes data volume to disk causing DataVolume
 *	to no longer be dirty.  Overridden method.
 */
void OmDataVolume::Flush()
{
	//debug("genone","OmDataVolume::Flush()");
	assert(false);
}

/*
 *	Closes this volume.  Ignores if already closed.
 */
void OmDataVolume::Close()
{
	//debug("genone","OmDataVolume::Close()");

	//ignore if already closed
	if (!IsOpen())
		return;

	//flush if dirty
	if (IsDirty())
		Flush();

	//close
	SetOpen(false);
}

/////////////////////////////////
///////          Voxel Accessors

bool OmDataVolume::ContainsVoxel(const DataCoord & vox)
{
	return GetExtent().contains(vox);
}

/////////////////////////////////
///////          ostream

ostream & operator<<(ostream & out, const OmDataVolume & v)
{
	//DataBbox extent =  v.GetExtent();
	//out << "Extent: " << extent << "\n";
	out << "State: " << (v.IsOpen()? "Open" : "Closed") << endl;
	return out;
}
