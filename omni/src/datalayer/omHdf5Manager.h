#ifndef OM_HDF5_MANAGER_H
#define OM_HDF5_MANAGER_H

#include "common/omCommon.h"

#include <QHash>
#include <QMutex>

class OmHdf5;

class OmHdf5Manager : boost::noncopyable {

 public:
	static OmHdf5Manager* Instance();
	static void Delete();
	static OmHdf5* getOmHdf5File( QString fileNameAndPath, const bool autoOpenAndClose, const bool readOnly);
	
 private:
	OmHdf5Manager();
	~OmHdf5Manager();

	static OmHdf5Manager* mspInstance;

	OmHdf5* doGetOmHdf5File( QString fileNameAndPath, const bool autoOpenAndClose, const bool readOnly );
	QHash<QString, OmHdf5*> hdf5Files;
	QMutex lock;
};

#endif
