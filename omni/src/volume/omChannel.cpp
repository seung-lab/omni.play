#include "common/omCommon.h"
#include "datalayer/omDataPath.h"
#include "system/cache/omMipVolumeCache.h"
#include "common/omDebug.h"
#include "datalayer/omDataPaths.h"
#include "project/omProject.h"
#include "project/omProjectSaveAction.h"
#include "system/events/omProgressEvent.h"
#include "system/omEventManager.h"
#include "volume/omChannel.h"
#include "volume/omFilter2d.h"
#include "volume/omVolume.h"
#include "utility/omTimer.h"
#include "volume/omMipChunk.h"
#include "volume/omMipThreadManager.h"
#include "volume/omThreadChunkLevel.h"
#include <float.h>


OmChannel::OmChannel()
{
        mMaxVal = FLT_MIN;
        mMinVal = FLT_MAX;
	mWasBounded = false;
}

OmChannel::OmChannel(OmId id)
	: OmManageableObject(id)
{
        mMaxVal = FLT_MIN;
        mMinVal = FLT_MAX;
	mWasBounded = false;

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

bool OmChannel::BuildThreadedVolume()
{
        OmTimer vol_timer;

        if (isDebugCategoryEnabled("perftest")){
                //timer start
                vol_timer.start();
        }

        if (!OmMipVolume::BuildThreadedVolume()){
                return false;
        }

        if (!BuildThreadedChannel()){
                return false;
        }

        if (isDebugCategoryEnabled("perftest")){
                //timer stop
                vol_timer.stop();
                printf("OmChannel::BuildThreadedVolume() done : %.6f secs\n",vol_timer.s_elapsed());
        }

        return true;
}

bool OmChannel::BuildThreadedChannel()
{

        //init progress bar
        OmEventManager::
            PostEvent(new
                      OmProgressEvent(OmProgressEvent::PROGRESS_SHOW, string("Building volume...               "), 0,
                                      MipChunksInVolume()));

        OmTimer vol_timer;

        if (isDebugCategoryEnabled("perftest")){
                //timer start
                vol_timer.start();
        }

        OmMipThreadManager *mipThreadManager = new OmMipThreadManager(this,OmMipThread::MIP_CHUNK,0,false);
        mipThreadManager->SpawnThreads(MipChunksInVolume());
        mipThreadManager->run();
        mipThreadManager->wait();
        mipThreadManager->StopThreads();
        delete mipThreadManager;

        //flush cache so that all thread chunks are flushed to disk
        Flush();
        printf("done\n");

        if (isDebugCategoryEnabled("perftest")){

                //timer end
                vol_timer.stop();
                printf("OmSegmentation::BuildThreadedSegmentation() done : %.6f secs\n",vol_timer.s_elapsed());

        }

        //hide progress bar
        OmEventManager::PostEvent(new OmProgressEvent(OmProgressEvent::PROGRESS_HIDE));

	mWasBounded = true;

	debug("chanbuild", "max=%f min=%f\n", mMaxVal, mMinVal);

        return true;

}

void OmChannel::BuildChunk(const OmMipChunkCoord & mipCoord)
{
	debug("chanbuild", "in OmChannel::BuildChunk\n");
        OmMipChunkPtr p_chunk;
        GetChunk(p_chunk, mipCoord);

        const bool isMIPzero = p_chunk->IsLeaf();

	if(isMIPzero) {
		//mMaxVal = std::max(p_chunk->getMax(), mMaxVal);
		//mMinVal = std::min(p_chunk->getMin(), mMinVal);
	}
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

void OmChannel::CloseDownThreads()
{
	mDataCache->closeDownThreads();
}
