
#include "omProject.h"
#include "system/omPreferenceDefinitions.h"

#include "volume/omVolume.h"
#include "mesh/omMipChunkMesher.h"
#include "segment/omSegmentEditor.h"
#include "system/omCacheManager.h"
#include "system/omEventManager.h"
#include "system/omGarbage.h"
#include "system/omKeyManager.h"
#include "system/omPreferences.h"
#include "system/omStateManager.h"
#include "system/omTagManager.h"
#include "system/omProjectData.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
namespace bfs = boost::filesystem;

#include "common/omDebug.h"
#include "common/omException.h"


#define DEBUG 0

static const string PROJECT_ARCHIVE_NAME = "project.dat";

//init instance pointer
OmProject *OmProject::mspInstance = 0;

#pragma mark -
#pragma mark OmProject
/////////////////////////////////
///////
///////          OmProject
///////

OmProject::OmProject()
{
}

OmProject::~OmProject()
{

}

OmProject *OmProject::Instance()
{
	if (NULL == mspInstance) {
		mspInstance = new OmProject;
	}
	return mspInstance;
}

void OmProject::Delete()
{
	if (mspInstance)
		delete mspInstance;
	mspInstance = NULL;
}




#pragma mark
#pragma mark Project IO
/////////////////////////////////
///////          Project IO

void OmProject::New(string dpath, string fname)
{

	//delete previous project
	if (OmProjectData::IsOpen())
		Close();

	//store paths
	OmProjectData::SetFileName(fname);
	OmProjectData::SetDirectoryPath(dpath);

	//create all intermediate directories
	//if(!bfs::create_directories(bfs::path(GetDirectoryPath())))
	//      throw OmIoException("Could not create project directory.");

	//string fpath = OmProjectData::GetDirectoryPath() + OmProjectData::GetFileName();
	//debug("genone","OmProject::New: %s \n", fpath.data());

	//create and open data
	//debug("genone","OmProject::New: creating");
	OmProjectData::Create();
	OmProjectData::Open();

	//load default project preferences
	omSetDefaultAllPreferences();
	OmKeyManager::SetDefaults();

	//save project
	//debug("genone","OmProject::New: save\n");
	Save();

	//debug("genone","OmProject::New: flushing\n");
	OmProjectData::Flush();
}

void OmProject::Save()
{

	if (!OmProjectData::IsOpen()) {
		return;
	}

	if (!OmProjectData::IsOpen()) {
		return;
	}

	string fpath = OmProjectData::GetDirectoryPath() + OmProjectData::GetFileName();

	//store archive
	OmProjectData::ArchiveWrite < OmProject > (PROJECT_ARCHIVE_NAME, Instance());
	OmProjectData::Flush();
	printf("saved project %s\n", fpath.c_str());
}

void OmProject::SaveAs(string dpath, string fname)
{

	//copy h5 file and write project file to it
	string current_fpath = OmProjectData::GetDirectoryPath() + OmProjectData::GetFileName();
	string new_fpath = dpath + fname;
	bfs::copy_file(current_fpath, new_fpath);

	//make new file the current project
	OmProjectData::SetFileName(fname);
	OmProjectData::SetDirectoryPath(dpath);

	string fpath = OmProjectData::GetDirectoryPath() + OmProjectData::GetFileName();

	//debug("genone","OmProject::SaveAs: %s\n", fpath.data());

	//store archive to new file
	OmProjectData::ArchiveWrite < OmProject > (PROJECT_ARCHIVE_NAME, Instance());
	OmProjectData::Flush();
}

void OmProject::Commit()
{
	Save();
	OmStateManager::ClearUndoStack();
}

/*
 *	Load project file from disk.
 *	dpath: 'usr/bwarne/project/'
 *	fname: 'project.omni'
 */
void OmProject::Load(string dpath, string fname)
{

	//close prev project if one is open
	if (OmProjectData::IsOpen())
		OmProjectData::Close();

	//form full pathname
	string fpath = dpath + fname;
	//debug("genone","OmProject::Load: %s\n", fpath.data());

	//check path validity
	if (!bfs::exists(bfs::path(fpath)))
		throw OmIoException("Project file not found:" + fpath);

	//valid path so store
	OmProjectData::SetFileName(fname);
	OmProjectData::SetDirectoryPath(dpath);

	//open project data
	OmProjectData::Open();

	//read archive
	OmProjectData::ArchiveRead < OmProject > (PROJECT_ARCHIVE_NAME, Instance());
}

void OmProject::Close()
{
	//delete all singletons

	//volume depends on the following so it must be deleted first
	OmVolume::Delete();

	OmMipChunkMesher::Delete();
	OmSegmentEditor::Delete();
	OmCacheManager::Delete();
	OmEventManager::Delete();
	OmGarbage::Delete();
	OmKeyManager::Delete();
	OmPreferences::Delete();
	OmStateManager::Delete();
	OmTagManager::Delete();

	//close project data
	OmProjectData::Close();
	OmProjectData::Delete();
}
