#include "common/omCommon.h"
#include "common/omException.h"
#include "common/omStd.h"
#include "datalayer/archive/omDataArchiveQT.h"
#include "datalayer/hdf5/omHdf5Manager.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataWrapper.h"
#include "datalayer/omDataWriter.h"
#include "project/omProject.h"
#include "project/omProjectSaveAction.h"
#include "segment/actions/omSegmentEditor.h"
#include "system/cache/omCacheManager.h"
#include "system/omEventManager.h"
#include "system/omGarbage.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omPreferences.h"
#include "system/omProjectData.h"
#include "system/omStateManager.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"
#include "mesh/omMeshSegmentList.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

//init instance pointer
OmProject *OmProject::mspInstance = 0;

/////////////////////////////////
///////          OmProject
OmProject::OmProject()
{
	mCanFlush = true;
	mThreadPool.start(3);
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
QString OmProject::New( QString fileNameAndPath )
{
	if (!fileNameAndPath.endsWith(".omni")) {
		fileNameAndPath.append(".omni");
	}

	QFileInfo fileInfo( fileNameAndPath );
	Instance()->mFileName = fileInfo.fileName();
	Instance()->mDirectoryPath = fileInfo.absolutePath();

	QDir dir(Instance()->mDirectoryPath);
	if( !dir.exists() ){
		if( !dir.mkpath(Instance()->mDirectoryPath) ){
			QString err = "could not make path " + Instance()->mDirectoryPath;
			throw OmIoException( qPrintable(err) );
		}
	}

	OmProjectData::instantiateProjectData( fileNameAndPath );
	OmProjectData::Create();
	OmProjectData::Open();

	//load default project preferences
	omSetDefaultAllPreferences();

	Save();

	return fileInfo.absoluteFilePath();
}

void OmProject::Save()
{
	SetCanFlush(true);

	if (!OmProjectData::IsOpen()) {
		return;
	}

	//TODO: move this into omProjectData

	foreach( const OmId & segID, OmProject::GetValidSegmentationIds() ){
		OmProject::GetSegmentation( segID ).Flush();
		OmProject::GetSegmentation( segID ).FlushDirtySegments();
		OmProject::GetSegmentation( segID ).FlushDendUserEdges();
	}

	OmDataArchiveQT::ArchiveWrite(OmDataPaths::getProjectArchiveNameQT(), Instance());

	OmProjectData::GetDataWriter()->flush();

	printf("omni project saved!\n");
}

void OmProject::Commit()
{
	Save();
	OmStateManager::ClearUndoStack();
}

void OmProject::Load( QString fileNameAndPath  )
{
	QFileInfo fileInfo( fileNameAndPath );
	Instance()->mFileName = fileInfo.fileName();
	Instance()->mDirectoryPath = fileInfo.absolutePath();

	QFile projectFile( fileNameAndPath );
	if( !projectFile.exists() ){
		QString err = "Project file not found at \"" + fileNameAndPath + "\"";
		throw OmIoException( qPrintable( err ));
	}

	OmProjectData::instantiateProjectData( fileNameAndPath );
	OmProjectData::Open();

	try {
		OmDataArchiveQT::ArchiveRead(OmDataPaths::getProjectArchiveNameQT(), Instance());
	} catch( ... ) {
		OmProjectData::Close();
		throw;
	}
}

void OmProject::Close(bool doSave)
{
	SetCanFlush(doSave);

	// OmProject must be deleted first: it depends on the remaining classes...
	OmCacheManager::SignalCachesToCloseDown();
	Delete();
	OmCacheManager::Delete();

	//delete all singletons
	OmMeshSegmentList::Delete();
	OmSegmentEditor::Delete();
	OmEventManager::Delete();
	OmGarbage::Delete();
	OmPreferences::Delete();
	OmStateManager::Delete();
	//OmLocalPreferences

	//close project data
	OmProjectData::Close();
	OmProjectData::Delete();

	OmHdf5Manager::Delete();
}

/////////////////////////////////
///////          Channel Manager Method

OmChannel & OmProject::GetChannel(const OmId id)
{
        return Instance()->mChannelManager.Get(id);
}

OmChannel & OmProject::AddChannel()
{
        OmChannel & r_channel = Instance()->mChannelManager.Add();
        (new OmProjectSaveAction())->Run();
        return r_channel;
}

void OmProject::RemoveChannel(const OmId id)
{
	GetChannel(id).CloseDownThreads();

	OmDataPath path(GetChannel(id).GetDirectoryPath());

        Instance()->mChannelManager.Remove(id);

	OmProjectData::DeleteInternalData(path);

	(new OmProjectSaveAction())->Run();
}

bool OmProject::IsChannelValid(const OmId id)
{
        return Instance()->mChannelManager.IsValid(id);
}

const OmIDsSet & OmProject::GetValidChannelIds()
{
        return Instance()->mChannelManager.GetValidIds();
}

bool OmProject::IsChannelEnabled(const OmId id)
{
        return Instance()->mChannelManager.IsEnabled(id);
}

void OmProject::SetChannelEnabled(const OmId id, bool enable)
{
        Instance()->mChannelManager.SetEnabled(id, enable);
}

/////////////////////////////////
///////          Segmentation Manager Method

OmSegmentation & OmProject::GetSegmentation(const OmId id)
{
        return Instance()->mSegmentationManager.Get(id);
}

OmSegmentation & OmProject::AddSegmentation()
{
        OmSegmentation & r_segmentation = Instance()->mSegmentationManager.Add();
	(new OmProjectSaveAction())->Run();
        return r_segmentation;
}

void OmProject::RemoveSegmentation(const OmId id)
{
        foreach( OmId channelID, OmProject::GetValidChannelIds()) {
                OmChannel & channel = OmProject::GetChannel(channelID);
                foreach( OmId filterID, channel.GetValidFilterIds()) {
                        OmFilter2d &filter = channel.GetFilter(filterID);
                        if (filter.GetSegmentation() == id){
                                filter.SetSegmentation(0);
                        }
                }
        }

	GetSegmentation(id).CloseDownThreads();

	OmDataPath path(GetSegmentation(id).GetDirectoryPath());

        Instance()->mSegmentationManager.Remove(id);

	OmProjectData::DeleteInternalData(path);

	(new OmProjectSaveAction())->Run();
}

bool OmProject::IsSegmentationValid(const OmId id)
{
        return Instance()->mSegmentationManager.IsValid(id);
}

const OmIDsSet & OmProject::GetValidSegmentationIds()
{
        return Instance()->mSegmentationManager.GetValidIds();
}

bool OmProject::IsSegmentationEnabled(const OmId id)
{
        return Instance()->mSegmentationManager.IsEnabled(id);
}

void OmProject::SetSegmentationEnabled(const OmId id, const bool enable)
{
        Instance()->mSegmentationManager.SetEnabled(id, enable);
}

void OmProject::SetCanFlush(bool canFlush)
{
	Instance()->mCanFlush = canFlush;
}

bool OmProject::GetCanFlush()
{
	return Instance()->mCanFlush;
}

