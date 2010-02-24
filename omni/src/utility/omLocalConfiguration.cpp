#include "omLocalConfiguration.h"
#include "stddef.h"

OmLocalConfiguration *OmLocalConfiguration::mspInstance = 0;

OmLocalConfiguration::OmLocalConfiguration()
{
}

OmLocalConfiguration::~OmLocalConfiguration()
{
}

OmLocalConfiguration *OmLocalConfiguration::Instance()
{
	if (NULL == mspInstance) {
		mspInstance = new OmLocalConfiguration;
	}

	return mspInstance;
}

void OmLocalConfiguration::Delete()
{
	if (mspInstance) {
		delete mspInstance;
	}
	mspInstance = NULL;
}
