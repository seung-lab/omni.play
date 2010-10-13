#include "datalayer/omDataLayer.h"
#include "datalayer/hdf5/omHdf5.h"
#include "datalayer/omDummyWriter.h"

#include <QFileInfo>

OmIDataReader* OmDataLayer::getReader(const std::string& fileNameAndPath,
									  const bool readOnly)
{
	return OmHdf5::getHDF5(fileNameAndPath, readOnly);
}

OmIDataWriter* OmDataLayer::getWriter(const std::string& fileNameAndPath,
				      const bool readOnly)
{
	if( readOnly ){
		printf("%s: in read-only mode...\n", __FUNCTION__);
		return new OmDummyWriter(fileNameAndPath);
	}

	return OmHdf5::getHDF5(fileNameAndPath, false);
}
