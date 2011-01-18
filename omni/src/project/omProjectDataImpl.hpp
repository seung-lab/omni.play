#ifndef OM_PROJECT_DATA_IMPL_HPP
#define OM_PROJECT_DATA_IMPL_HPP

#include "project/omProject.h"
#include "common/omDebug.h"
#include "common/omException.h"
#include "datalayer/hdf5/omHdf5Manager.h"
#include "datalayer/omDataLayer.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omIDataReader.h"
#include "datalayer/omIDataWriter.h"
#include "segment/omSegment.h"
#include "tiles/cache/omTileCache.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omProjectData.h"
#include "utility/fileHelpers.h"

#include <QFile>
#include <QFileInfo>

class OmProjectDataImpl{
private:
	int fileVersion_;

	bool mIsOpen;
	bool mIsReadOnly;

	OmIDataReader* dataReader;
	OmIDataWriter* dataWriter;

	QDir filesFolderPath_;

	void getFilesFolderPath()
	{
		if( "" == getAbsoluteFileNameAndPath()){
			throw OmIoException("project file name not set");
		}

		filesFolderPath_ =
			QDir(getAbsoluteFileNameAndPath() + ".files");
	}

	QString getFileNameAndPath(){
		return OmProject::GetFileNameAndPath();
	}

	QString getAbsoluteFileNameAndPath(){
		return OmProject::GetFileNameAndPath();
	}

	QString getAbsolutePath(){
		return OmProject::GetPath();
	}

public:
	OmProjectDataImpl()
		: fileVersion_(0)
		, mIsOpen(false)
		, mIsReadOnly(false)
		, dataReader(NULL)
		, dataWriter(NULL)
	{
		getFilesFolderPath();
	}

	void instantiateProjectData(const std::string& fileNameAndPath )
	{
		mIsReadOnly = FileHelpers::isFileReadOnly(fileNameAndPath);
		dataReader = OmDataLayer::getReader(fileNameAndPath, mIsReadOnly);
		dataWriter = OmDataLayer::getWriter(fileNameAndPath, mIsReadOnly);
	}

	void setFileVersion(const int v){
		fileVersion_ = v;
	}

	bool IsOpen() {
		return mIsOpen;
	}

	bool IsReadOnly() {
		return mIsReadOnly;
	}

	int getFileVersion(){
		return fileVersion_;
	}

	const QDir& GetFilesFolderPath(){
		return filesFolderPath_;
	}

	void CreateProject()
	{
		QFile projectFile(getFileNameAndPath());
		if( projectFile.exists() ){
			projectFile.remove();
		}

		QDir filesDir = GetFilesFolderPath();
		const QString path = filesDir.absolutePath();
		if(filesDir.exists()){
			printf("removing folder %s...", qPrintable(path));
			fflush(stdout);
			if(FileHelpers::removeDir(path)){
				printf("done!\n");
			} else {
				throw OmIoException("could not remove folder", path);
			}
		}

		if(filesDir.mkpath(path)){
			printf("made folder \"%s\"\n", qPrintable(path));
		} else{
			throw OmIoException("could not make folder", path);
		}

		dataWriter->create();
	}

	void Open()
	{
		dataReader->open();
		mIsOpen = true;
	}

	void Close()
	{
		if(!mIsOpen){
			return;
		}
		dataReader->close();
		mIsOpen = false;
	}

/**
 *	Deletes all data on disk associated with path if it exists.
 */
	void DeleteInternalData(const OmDataPath & path)
	{
		//TODO: mutex lock this?
		// TODO: obsolete; need to delete from filesystem now (purcaro)
		if (GetProjectIDataReader()->group_exists(path)) {
			GetIDataWriter()->group_delete(path);
		}
	}

	OmIDataReader* GetProjectIDataReader(){
		return dataReader;
	}

	OmIDataWriter* GetIDataWriter(){
		return dataWriter;
	}
};
#endif
