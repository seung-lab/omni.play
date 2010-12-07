#ifndef OM_PROJECT_DATA_H
#define OM_PROJECT_DATA_H

/*
 *	Wrapper for HDF5 data storage.
 *
 *	Brett Warne - bwarne@mit.edu - 3/30/09
 */

#include "common/omCommon.h"

#include <QDir>

class OmDataPath;
class OmDataLayer;
class OmIDataReader;
class OmIDataWriter;
class OmSegment;
class OmTileCache;

class OmProjectData : boost::noncopyable {
public:
	static void instantiateProjectData(const std::string& fileNameAndPath);

	static OmProjectData* Instance();
	static void Delete();

	static QString getFileNameAndPath();
	static QString getAbsoluteFileNameAndPath();
	static QString getAbsolutePath();
	static QDir GetFilesFolderPath();

	static void Create();
	static void Open();
	static void Close();
	static void DeleteInternalData(const OmDataPath & path);

	static bool IsOpen() {return Instance()->mIsOpen;}
	static bool IsReadOnly() {return Instance()->mIsReadOnly;}

	static OmIDataReader* GetProjectIDataReader();
	static OmIDataWriter* GetIDataWriter();

	static int getFileVersion(){ return Instance()->fileVersion_; }

private:
	OmProjectData();
	~OmProjectData(){}

	//singleton
	static OmProjectData* mspInstance;

	static void setFileVersion(const int v){ Instance()->fileVersion_ = v; }
	int fileVersion_;

	bool mIsOpen;
	bool mIsReadOnly;

	void setupDataLayer(const std::string&);
	OmIDataReader* dataReader;
	OmIDataWriter* dataWriter;

	friend class OmDataArchiveProject;
};

#endif
