#ifndef OM_PROJECT_DATA_H
#define OM_PROJECT_DATA_H

/*
 *	Wrapper for HDF5 data storage.
 *
 *	Brett Warne - bwarne@mit.edu - 3/30/09
 */

#include "utility/omDataLayer.h"
#include "common/omStd.h"
#include "segment/omSegment.h"


class vtkImageData;

class OmProjectData {
public:
	static void instantiateProjectData( QString fileNameAndPath, 
					    const bool autoOpenAndClose);
	static OmProjectData* Instance();
	static void Delete();
	
	static QString getFileNameAndPath();
	static QString getAbsoluteFileNameAndPath();
	static QString getAbsolutePath();

	static void Create();
	static void Open();
	static void Close();
	
	static bool IsOpen() {return Instance()->mIsOpen;}
	static bool IsReadOnly() {return Instance()->mIsReadOnly;}

	static OmDataLayer * GetDataLayer();
	static OmDataReader * GetProjectDataReader();
	static OmDataWriter * GetDataWriter();
		
protected:
	// singleton constructor, copy constructor, assignment operator protected
	OmProjectData();
	~OmProjectData();
	OmProjectData(const OmProjectData&);
	OmProjectData& operator= (const OmProjectData&);
	
private:
	//singleton
	static OmProjectData* mspInstance;

	bool mIsOpen;

	bool mIsReadOnly;

	void setupDataLayer( QString fileNameAndPath, const bool autoOpenAndClose);
	OmDataLayer  * dataLayer;
	OmDataReader * dataReader;
	OmDataWriter * dataWriter;
};

#endif
