#include "omHdf5Helpers.h"

OmHdf5Path OmHdf5Helpers::getDefaultDatasetName()
{
	OmHdf5Path path;
	path.setPath("main");
	return path;
}

OmHdf5Path OmHdf5Helpers::getProjectArchiveName()
{
	OmHdf5Path path;
	path.setPath("project.dat");
	return path;
}
