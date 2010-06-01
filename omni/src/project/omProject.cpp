#include "common/omCommon.h"
#include "common/omException.h"
#include "common/omStd.h"
#include "datalayer/archive/omDataArchiveQT.h"
#include "datalayer/hdf5/omHdf5Manager.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataWrapper.h"
#include "datalayer/omDataWriter.h"
#include "mesh/omMeshDrawer.h"
#include "project/omProject.h"
#include "segment/omSegmentEditor.h"
#include "system/omCacheManager.h"
#include "system/omEventManager.h"
#include "system/omGarbage.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omPreferences.h"
#include "system/omProjectData.h"
#include "system/omStateManager.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"

#include <QFile>
#include <QFileInfo>

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
QString OmProject::New( QString fileNameAndPath )
{
	if (!fileNameAndPath.endsWith(".omni")) {
		fileNameAndPath.append(".omni");
	}

	QFileInfo fileInfo( fileNameAndPath );
	Instance()->mFileName = fileInfo.fileName();
	Instance()->mDirectoryPath = fileInfo.filePath();

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
	if (!OmProjectData::IsOpen()) {
		return;
	}

	//TODO: move this into omProjectData

	foreach( const OmId & segID, OmProject::GetValidSegmentationIds() ){
		OmProject::GetSegmentation( segID ).FlushDirtySegments();
	}

	OmDataArchiveQT::ArchiveWrite(OmDataPaths::getProjectArchiveNameQT(), Instance());

	OmProjectData::GetDataWriter()->flush();
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
	Instance()->mDirectoryPath = fileInfo.filePath();

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

void OmProject::Close()
{
	// OmProject must be deleted first: it depends on the remaining classes...
	Delete();

	//delete all singletons
	OmSegmentEditor::Delete();
	OmCacheManager::Delete();
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

OmChannel & OmProject::GetChannel(OmId id)
{
        return Instance()->mChannelManager.Get(id);
}

OmChannel & OmProject::AddChannel()
{
        OmChannel & r_channel = Instance()->mChannelManager.Add();
        OmProject::Save();
        return r_channel;
}

void OmProject::RemoveChannel(OmId id)
{
        Instance()->mChannelManager.Get(id).DeleteVolumeData();
        Instance()->mChannelManager.Remove(id);
        OmProject::Save();
}

bool OmProject::IsChannelValid(OmId id)
{
        return Instance()->mChannelManager.IsValid(id);
}

const OmIDsSet & OmProject::GetValidChannelIds()
{
        return Instance()->mChannelManager.GetValidIds();
}

bool OmProject::IsChannelEnabled(OmId id)
{
        return Instance()->mChannelManager.IsEnabled(id);
}

void OmProject::SetChannelEnabled(OmId id, bool enable)
{
        Instance()->mChannelManager.SetEnabled(id, enable);
}

/////////////////////////////////
///////          Segmentation Manager Method

OmSegmentation & OmProject::GetSegmentation(OmId id)
{
        Instance()->mSegmentationManager.Get(id).SetBytesPerSample(4);
        return Instance()->mSegmentationManager.Get(id);
}

OmSegmentation & OmProject::AddSegmentation()
{
        OmSegmentation & r_segmentation = Instance()->mSegmentationManager.Add();
        OmProject::Save();
        return r_segmentation;
}

void OmProject::RemoveSegmentation(OmId id)
{
        foreach( const OmId & channelID, OmProject::GetValidChannelIds()) {
                OmChannel & channel = OmProject::GetChannel(channelID);
                foreach( const OmId & filterID, channel.GetValidFilterIds()) {
                        OmFilter2d &filter = channel.GetFilter(filterID);
                        if (filter.GetSegmentation() == id){
                                filter.SetSegmentation(0);
                        }
                }
        }

        Instance()->mSegmentationManager.Get(id).PrepareForCompleteDelete();
        Instance()->mSegmentationManager.Get(id).DeleteVolumeData();
        Instance()->mSegmentationManager.Remove(id);
        OmProject::Save();
}

bool OmProject::IsSegmentationValid(OmId id)
{
        return Instance()->mSegmentationManager.IsValid(id);
}

const OmIDsSet & OmProject::GetValidSegmentationIds()
{
        return Instance()->mSegmentationManager.GetValidIds();
}

bool OmProject::IsSegmentationEnabled(OmId id)
{
        return Instance()->mSegmentationManager.IsEnabled(id);
}

void OmProject::SetSegmentationEnabled(OmId id, bool enable)
{
        Instance()->mSegmentationManager.SetEnabled(id, enable);
}

void OmProject::Draw(OmVolumeCuller & rCuller, OmViewGroupState * vgs)
{
	OmIDsSet::const_iterator iter;
	const OmIDsSet & set = Instance()->mSegmentationManager.GetEnabledIds();
	for( iter = set.begin(); iter != set.end(); ++iter ){
		OmMeshDrawer drawer( *iter, vgs);
		drawer.Draw( rCuller );
        }
}
