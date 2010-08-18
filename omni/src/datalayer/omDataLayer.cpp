#include "datalayer/omDataLayer.h"
#include "datalayer/hdf5/omHdf5Reader.h"
#include "datalayer/hdf5/omHdf5Writer.h"
#include "datalayer/omDummyWriter.h"

#include <QFileInfo>

OmDataLayer::OmDataLayer()
{
}

OmDataLayer::~OmDataLayer()
{
	//TODO: fixme!
}

OmDataReader * OmDataLayer::getReader(QString fileNameAndPath, const bool readOnly)
{
	return new OmHdf5Reader( fileNameAndPath, readOnly );
}

OmDataWriter * OmDataLayer::getWriter(QString fileNameAndPath, const bool readOnly)
{
	if( readOnly ){
		printf("%s: in read-only mode...\n", __FUNCTION__);
		return new OmDummyWriter( fileNameAndPath );
	}

	return new OmHdf5Writer( fileNameAndPath );
}
