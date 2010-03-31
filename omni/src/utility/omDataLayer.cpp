#include "omDataLayer.h"
#include "omHdf5Manager.h"
#include "utility/omHdf5Reader.h"
#include "utility/omHdf5Writer.h"
#include "utility/omDummyWriter.h"
#include <QFileInfo>

OmDataLayer::OmDataLayer()
{
}

OmDataLayer::~OmDataLayer()
{
	//TODO: fixme!
}

OmDataReader * OmDataLayer::getReader(  QString fileNameAndPath, const bool autoOpenAndClose, const bool readOnly )
{
	return new OmHdf5Reader( fileNameAndPath, autoOpenAndClose, readOnly );
}

OmDataWriter * OmDataLayer::getWriter(  QString fileNameAndPath, const bool autoOpenAndClose, const bool readOnly )
{
	if( readOnly ){
		printf("%s: in read-only mode...\n", __FUNCTION__);
		return new OmDummyWriter( fileNameAndPath, autoOpenAndClose );
	}
	
	return new OmHdf5Writer( fileNameAndPath, autoOpenAndClose );
}
