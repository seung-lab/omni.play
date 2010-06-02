#include "common/omDebug.h"
#include "project/omProject.h"
#include "system/events/omProgressEvent.h"
#include "system/omEventManager.h"
#include "volume/omChannel.h"
#include "volume/omFilter2d.h"
#include "volume/omVolume.h"

OmChannel::OmChannel()
{
        /** Set The Name of the Cache */
        SetCacheName("OmChannel --> OmMipVolume");
        int chunkDim = GetChunkDimension();
        SetObjectSize(chunkDim*chunkDim*chunkDim);
}

OmChannel::OmChannel(OmId id)
	: OmManageableObject(id)
{

	//set manageable object name
	char idchar[25];
	snprintf(idchar, sizeof(idchar), "%i", (int)id);
	SetName( QString("channel%1").arg(idchar) );

	//set permenant directory name
	SetDirectoryPath( QString("channels/%1/").arg(GetName()) );

	//init properties
	SetHue(Vector3f::ONE);

	//channels have 1 byte per sample
	SetBytesPerSample(1);

	//interpolate channel data
	SetSubsampleMode(SUBSAMPLE_NONE);

	//do not use meta data
	mStoreChunkMetaData = false;

	//build blank data
	BuildVolumeData();

	/** Set The Name of the Cache */
        SetCacheName("OmChannel --> OmMipVolume");
        int chunkDim = GetChunkDimension();
        SetObjectSize(chunkDim*chunkDim*chunkDim);

	AddFilter();	
}

/////////////////////////////////
///////          Properties

/*
 *	Hue color property
 */
void OmChannel::SetHue(const vmml::Vector3< float > & hue)
{
	mHueColor = hue;
}

const Vector3f & OmChannel::GetHue()
{
	return mHueColor;
}

/////////////////////////////////
///////          Accessors

bool OmChannel::IsVolumeDataBuilt()
{
	return true;
}

void OmChannel::BuildVolumeData()
{
	OmMipVolume::Build();
}

OmFilter2d& OmChannel::AddFilter() {
	OmFilter2d& filter = mFilter2dManager.AddFilter();
	OmProject::Save();
        return filter;
}

OmFilter2d& OmChannel::GetFilter(OmId id) {
        return mFilter2dManager.GetFilter(id);
}

const OmIDsSet & OmChannel::GetValidFilterIds()
{
	return mFilter2dManager.GetValidFilterIds();
}

bool OmChannel::IsFilterValid(const OmId id)
{
	return mFilter2dManager.IsFilterValid(id);
}

bool OmChannel::IsFilterEnabled(OmId id)
{
	return mFilter2dManager.IsFilterEnabled(id);
}

/////////////////////////////////
///////          Print Methods

void OmChannel::Print()
{
	//debug("FIXME", << "\t" << mName << " (" << mId << ")" << endl;
}
