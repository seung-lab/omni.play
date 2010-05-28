#include "utility/omHdf5Manager.h"
#include "common/omDebug.h"
#include "utility/omHdf5.h"

#include <QFileInfo>

//init instance pointer
OmHdf5Manager *OmHdf5Manager::mspInstance = 0;


OmHdf5Manager::OmHdf5Manager()
{
}

OmHdf5Manager::~OmHdf5Manager()
{
	foreach( OmHdf5 * hdf, hdf5Files ){
		delete hdf;
	}
}

OmHdf5* OmHdf5Manager::getOmHdf5File( QString fileNameAndPath, const bool autoOpenAndClose, 
				      const bool readOnly )
{
	return Instance()->doGetOmHdf5File( fileNameAndPath, autoOpenAndClose, readOnly );
}

OmHdf5Manager * OmHdf5Manager::Instance()
{
	if (NULL == mspInstance) {
		mspInstance = new OmHdf5Manager();
	}
	return mspInstance;
}

void OmHdf5Manager::Delete()
{
	if (mspInstance) {
		delete mspInstance;
	}
	mspInstance = NULL;
}

OmHdf5* OmHdf5Manager::doGetOmHdf5File( QString fileNameAndPath, const bool autoOpenAndClose, 
					const bool readOnly )
{
	QMutexLocker locker(&lock);

	QFileInfo fInfo(fileNameAndPath);
	QString abs_fnpn = fInfo.absoluteFilePath();

	if( hdf5Files.contains( abs_fnpn ) ){
		return hdf5Files.value( abs_fnpn );
	}

	OmHdf5 * hdf5File = new OmHdf5(fileNameAndPath, autoOpenAndClose, readOnly );
	hdf5Files.insert( abs_fnpn, hdf5File );
	return hdf5File;
}
