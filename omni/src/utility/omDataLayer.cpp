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

OmDataReader * OmDataLayer::getReader(  QString fileNameAndPath, const bool autoOpenAndClose )
{
	return new OmHdf5Reader( fileNameAndPath, autoOpenAndClose );
}

OmDataWriter * OmDataLayer::getWriter(  QString fileNameAndPath, const bool autoOpenAndClose )
{
	QFileInfo file(fileNameAndPath);
	if (file.permission(QFile::WriteUser)){
		return new OmHdf5Writer( fileNameAndPath, autoOpenAndClose );
	} else {
		return new OmDummyWriter( fileNameAndPath, autoOpenAndClose );
	}
}
