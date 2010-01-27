
#include "omChannel.h"
#include "omVolume.h"

#include "system/omEventManager.h"
#include "system/events/omProgressEvent.h"

#include "system/omFilter.h"
#include "system/omDebug.h"

#define DEBUG 0

OmChannel::OmChannel(OmId id)
 : OmManageableObject(id)
{

	//set manageable object name
	char idchar[25];
	snprintf(idchar, sizeof(idchar), "%i", (int)id);
	SetName("channel" + string(idchar));

	//set permenant directory name
	SetDirectoryPath(string("channels/") + GetName() + string("/"));

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
}

#pragma mark
#pragma mark Properties
/////////////////////////////////
///////          Properties

/*
 *	Hue color property
 */
void
 OmChannel::SetHue(const Vector3f & hue)
{
	mHueColor = hue;
}

const Vector3f & OmChannel::GetHue()
{
	return mHueColor;
}

#pragma mark
#pragma mark Accessors
/////////////////////////////////
///////          Accessors

bool OmChannel::IsVolumeDataBuilt()
{
	return true;
}

void OmChannel::BuildVolumeData()
{
	//build volume
	OmMipVolume::Build();
}

OmFilter & OmChannel::AddFilter()
{
	return mFilterManager.AddFilter();
}

OmFilter & OmChannel::GetFilter(OmId id)
{
	return mFilterManager.GetFilter(id);
}

const set < OmId > & OmChannel::GetValidFilterIds()
{
	return mFilterManager.GetValidFilterIds();
}

bool OmChannel::IsFilterEnabled(OmId id)
{
	return mFilterManager.IsFilterEnabled(id);
}

#pragma mark
#pragma mark Print Methods
/////////////////////////////////
///////          Print Methods

void OmChannel::Print()
{
	cout << "\t" << mName << " (" << mId << ")" << endl;
}
