#include "common/omDebug.h"
#include "datalayer/omDataPaths.h"
#include "project/omProject.h"
#include "project/omProjectSaveAction.h"
#include "system/events/omProgressEvent.h"
#include "system/omEventManager.h"
#include "volume/omChannel.h"
#include "volume/omFilter2d.h"
#include "volume/omVolume.h"

OmChannel::OmChannel()
{
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

	//do not use meta data
	mStoreChunkMetaData = false;

	//build blank data
	BuildVolumeData();

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
	OmDataPath path = OmDataPath(OmDataPaths::getDefaultHDF5channelDatasetName());
	OmMipVolume::Build(path);
}

OmFilter2d& OmChannel::AddFilter() {
	OmFilter2d& filter = mFilter2dManager.AddFilter();
	(new OmProjectSaveAction())->Run();
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
