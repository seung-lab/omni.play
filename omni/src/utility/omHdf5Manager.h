#ifndef OM_HDF5_MANAGER_H
#define OM_HDF5_MANAGER_H

/*
 *
 */

#include "omHdf5.h"
#include <QHash>

class OmHdf5Manager {

public:
	static OmHdf5Manager* Instance();
	static void Delete();
	static OmHdf5* getOmHdf5File( QString fileNameAndPath, const bool autoOpenAndClose, const bool readOnly);
	
protected:
	// singleton constructor, copy constructor, assignment operator protected
	OmHdf5Manager();
	~OmHdf5Manager();
	OmHdf5Manager(const OmHdf5Manager&);
	OmHdf5Manager& operator= (const OmHdf5Manager&);
	
private:
	static OmHdf5Manager* mspInstance;

	OmHdf5* doGetOmHdf5File( QString fileNameAndPath, const bool autoOpenAndClose, const bool readOnly );
	QHash<QString, OmHdf5*> hdf5Files;
	QMutex lock;
};

#endif
