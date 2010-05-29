#include "omHdf5Helpers.h"

OmDataPath OmHdf5Helpers::getDefaultDatasetName()
{
	OmDataPath path;
	path.setPath("main");
	return path;
}

OmDataPath OmHdf5Helpers::getProjectArchiveNameQT()
{
	OmDataPath path;
	path.setPath("project.qt.dat");
	return path;
}
