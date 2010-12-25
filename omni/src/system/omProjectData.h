#ifndef OM_PROJECT_DATA_H
#define OM_PROJECT_DATA_H

/*
 *	Wrapper for HDF5 data storage.
 *
 *	Brett Warne - bwarne@mit.edu - 3/30/09
 */

#include "common/omCommon.h"
#include "zi/omUtility.h"

#include <QDir>

class OmDataPath;
class OmDataLayer;
class OmIDataReader;
class OmIDataWriter;
class OmSegment;
class OmProjectDataImpl;

class OmProjectData : om::singletonBase<OmProjectData> {
private:
	boost::shared_ptr<OmProjectDataImpl> impl_;

public:
	static void instantiateProjectData(const std::string& fileNameAndPath);

	static void Delete();

	static QString getFileNameAndPath();
	static QString getAbsoluteFileNameAndPath();
	static QString getAbsolutePath();
	static const QDir& GetFilesFolderPath();

	static void Create();
	static void Open();
	static void Close();
	static void DeleteInternalData(const OmDataPath & path);

	static bool IsOpen();
	static bool IsReadOnly();

	static OmIDataReader* GetProjectIDataReader();
	static OmIDataWriter* GetIDataWriter();

	static int getFileVersion();

private:
	OmProjectData();
	~OmProjectData(){}

	static void setFileVersion(const int fileVersion);

	friend class OmDataArchiveProject;
	friend class zi::singleton<OmProjectData>;
};

#endif
