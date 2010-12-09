#include "actions/io/omActionLogger.hpp"
#include "actions/io/omActionReplayer.hpp"
#include "actions/omActions.hpp"
#include "common/omCommon.h"
#include "common/omException.h"
#include "common/omStd.h"
#include "datalayer/archive/omDataArchiveProject.h"
#include "datalayer/hdf5/omHdf5Manager.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataWrapper.h"
#include "datalayer/omIDataWriter.h"
#include "mesh/omMeshSegmentList.h"
#include "project/omProject.h"
#include "segment/omSegmentSelected.hpp"
#include "system/cache/omCacheManager.h"
#include "system/events/omEventManager.h"
#include "system/omGarbage.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omPreferences.h"
#include "system/omProjectData.h"
#include "system/omStateManager.h"
#include "tiles/cache/omTileCache.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"

#include <QDir>
#include <QFile>

//init instance pointer
OmProject *OmProject::mspInstance = 0;

/////////////////////////////////
///////          OmProject
OmProject::OmProject()
{
}

OmProject::~OmProject()
{
}

OmProject *OmProject::Instance()
{
	if (NULL == mspInstance) {
		mspInstance = new OmProject();
	}
	return mspInstance;
}

void OmProject::Delete()
{
	if (mspInstance) {
		delete mspInstance;
	}
	mspInstance = NULL;
}

/////////////////////////////////
///////          Project IO
QString OmProject::New(const QString& fileNameAndPathIn)
{
	QString fileNameAndPath = fileNameAndPathIn;

	if (!fileNameAndPath.endsWith(".omni")) {
		fileNameAndPath.append(".omni");
	}

	Instance()->projectFileNameAndPath_ = QFileInfo(fileNameAndPath);

	const QString dirStr = Instance()->projectFileNameAndPath_.absolutePath();
	QDir dir(dirStr);
	if( !dir.exists() ){
		if( !dir.mkpath(dirStr) ){
			const QString err = "could not make path " + dirStr;
			throw OmIoException(err.toStdString());
		}
	}

	OmProjectData::instantiateProjectData(fileNameAndPath.toStdString());
	OmProjectData::Create();
	OmProjectData::Open();
	OmCacheManager::Reset();
	OmTileCache::Reset();

	OmPreferenceDefaults::SetDefaultAllPreferences();

	Save();

	return Instance()->projectFileNameAndPath_.absoluteFilePath();
}

void OmProject::Save()
{
	if (!OmProjectData::IsOpen()) {
		return;
	}

	//TODO: move this into omProjectData?

	foreach( const OmID & segID, OmProject::GetValidSegmentationIds() ){
		OmProject::GetSegmentation( segID ).Flush();
	}

	OmDataArchiveProject::ArchiveWrite(OmDataPaths::getProjectArchiveNameQT(),
									   Instance());

	OmProjectData::GetIDataWriter()->flush();

	printf("omni project saved!\n");
}

void OmProject::Commit()
{
	Save();
	OmStateManager::ClearUndoStack();
}

void OmProject::Load(const QString& fileNameAndPath)
{
	Instance()->projectFileNameAndPath_ = QFileInfo(fileNameAndPath);

	QFile projectFile(Instance()->projectFileNameAndPath_.absoluteFilePath());
	if( !projectFile.exists() ){
		throw OmIoException("Project file not found at", fileNameAndPath);
	}

	OmProjectData::instantiateProjectData(fileNameAndPath.toStdString());
	OmProjectData::Open();
	OmCacheManager::Reset();
	OmTileCache::Reset();

	try {
		OmDataArchiveProject::ArchiveRead(OmDataPaths::getProjectArchiveNameQT(),
										  Instance());
	} catch( ... ) {
		OmProjectData::Close();
		throw;
	}

	OmActionReplayer::Replay();
}

void OmProject::Close()
{
	OmActions::Close();

	// OmProject must be deleted first: it depends on the remaining classes...
	OmCacheManager::SignalCachesToCloseDown();
	OmMeshSegmentList::Delete();
//	zi::all_threads::join_all();
	Delete();

	//delete all singletons
	OmMeshSegmentList::Delete();
	OmSegmentSelected::Delete();
	OmEventManager::Delete();
	OmGarbage::Delete();
	OmPreferences::Delete();
	OmStateManager::Delete();
	//OmLocalPreferences

	//close project data
	OmTileCache::Delete();
	OmCacheManager::Delete();
	OmProjectData::Close();
	OmProjectData::Delete();

	OmHdf5Manager::Delete();
}

/////////////////////////////////
///////          Channel Manager Method

OmChannel & OmProject::GetChannel(const OmID id)
{
	return Instance()->mChannelManager.Get(id);
}

OmChannel & OmProject::AddChannel()
{
	OmChannel & r_channel = Instance()->mChannelManager.Add();
	OmActions::Save();
	return r_channel;
}

void OmProject::RemoveChannel(const OmID id)
{
	GetChannel(id).CloseDownThreads();

	OmDataPath path(GetChannel(id).GetDirectoryPath());

	Instance()->mChannelManager.Remove(id);

	OmProjectData::DeleteInternalData(path);

	OmActions::Save();
}

bool OmProject::IsChannelValid(const OmID id)
{
	return Instance()->mChannelManager.IsValid(id);
}

const OmIDsSet & OmProject::GetValidChannelIds()
{
	return Instance()->mChannelManager.GetValidIds();
}

bool OmProject::IsChannelEnabled(const OmID id)
{
	return Instance()->mChannelManager.IsEnabled(id);
}

void OmProject::SetChannelEnabled(const OmID id, bool enable)
{
	Instance()->mChannelManager.SetEnabled(id, enable);
}

/////////////////////////////////
///////          Segmentation Manager Method

OmSegmentation & OmProject::GetSegmentation(const OmID id)
{
	return Instance()->mSegmentationManager.Get(id);
}

OmSegmentation & OmProject::AddSegmentation()
{
	OmSegmentation & r_segmentation = Instance()->mSegmentationManager.Add();
	OmActions::Save();
	return r_segmentation;
}

void OmProject::RemoveSegmentation(const OmID id)
{
	foreach( OmID channelID, OmProject::GetValidChannelIds()) {
		OmChannel & channel = OmProject::GetChannel(channelID);
		foreach( OmID filterID, channel.GetValidFilterIds()) {
			OmFilter2d &filter = channel.GetFilter(filterID);
			if (filter.GetSegmentationWrapper().GetSegmentationID() == id){
				filter.SetSegmentation(0);
			}
		}
	}

	GetSegmentation(id).CloseDownThreads();

	OmDataPath path(GetSegmentation(id).GetDirectoryPath());

	Instance()->mSegmentationManager.Remove(id);

	OmProjectData::DeleteInternalData(path);

	OmActions::Save();
}

bool OmProject::IsSegmentationValid(const OmID id)
{
	return Instance()->mSegmentationManager.IsValid(id);
}

const OmIDsSet & OmProject::GetValidSegmentationIds()
{
	return Instance()->mSegmentationManager.GetValidIds();
}

bool OmProject::IsSegmentationEnabled(const OmID id)
{
	return Instance()->mSegmentationManager.IsEnabled(id);
}

void OmProject::SetSegmentationEnabled(const OmID id, const bool enable)
{
	Instance()->mSegmentationManager.SetEnabled(id, enable);
}
