
#include "omProject.h"
#include "omException.h"
#include "omPreferenceDefinitions.h"

#include "volume/omVolume.h"
#include "mesh/omMipChunkMesher.h"
#include "segment/omSegmentEditor.h"
#include "omCacheManager.h"
#include "omEventManager.h"
#include "omGarbage.h"
#include "omKeyManager.h"
#include "omPreferences.h"
#include "omStateManager.h"
#include "omTagManager.h"
#include "omProjectData.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include "system/omDebug.h"
namespace bfs = boost::filesystem;

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
	OmStateManager::CreatePrimaryView3dWidget();
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
#pragma mark Project
/////////////////////////////////
///////          Project

//project
const string & OmProject::GetFileName()
{
	return Instance()->mFileName;
}

void OmProject::SetFileName(const string & fname)
{
	Instance()->mFileName = fname;
}

const string & OmProject::GetDirectoryPath()
{
	return Instance()->mDirectoryPath;
}

void OmProject::SetDirectoryPath(const string & dpath)
{
	Instance()->mDirectoryPath = dpath;
}

string OmProject::GetTempDirectoryPath()
{
	string dpath = GetDirectoryPath();
	return dpath + "temp/";
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
	Instance()->mFileName = fname;
	Instance()->mDirectoryPath = dpath;

	//create all intermediate directories
	//if(!bfs::create_directories(bfs::path(GetDirectoryPath())))
	//      throw OmIoException("Could not create project directory.");

	string fpath = GetDirectoryPath() + GetFileName();
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

	string fpath = GetDirectoryPath() + GetFileName();
	//debug("genone","OmProject::Save: %s\n", fpath.data());

	//store archive
	OmProjectData::ArchiveWrite < OmProject > (PROJECT_ARCHIVE_NAME, Instance());
	OmProjectData::Flush();
}

void OmProject::SaveAs(string dpath, string fname)
{

	//copy h5 file and write project file to it
	string current_fpath = GetDirectoryPath() + GetFileName();
	string new_fpath = dpath + fname;
	bfs::copy_file(current_fpath, new_fpath);

	//make new file the current project
	Instance()->mFileName = fname;
	Instance()->mDirectoryPath = dpath;

	string fpath = GetDirectoryPath() + GetFileName();

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
	Instance()->mFileName = fname;
	Instance()->mDirectoryPath = dpath;

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
