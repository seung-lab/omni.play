#include "datalayer/omDataLayer.h"
#include "datalayer/hdf5/omHdf5.h"
#include "datalayer/hdf5/omHdf5Writer.h"
#include "datalayer/omDummyWriter.h"

#include <QFileInfo>

OmIDataReader* OmDataLayer::getReader(const QString& fileNameAndPath,
				      const bool readOnly)
{
	return OmHdf5::getHDF5(fileNameAndPath, readOnly);
}

OmDataWriter* OmDataLayer::getWriter(const QString& fileNameAndPath,
				      const bool readOnly)
{
	if( readOnly ){
		printf("%s: in read-only mode...\n", __FUNCTION__);
		return new OmDummyWriter( fileNameAndPath );
	}

	return new OmHdf5Writer( fileNameAndPath );
}
