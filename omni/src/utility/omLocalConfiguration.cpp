#include "omLocalConfiguration.h"
#include "stddef.h"
#include "utility/omNumCores.h"

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

int OmLocalConfiguration::get_num_cores()
{
	return (int)OmNumCores::get_num_cores();
}

int OmLocalConfiguration::numAllowedWorkerThreads()
{
	const int numCoresRaw = get_num_cores();
	int numCores = numCoresRaw - 1;
	if( 1 == numCoresRaw ){
		numCores = 1;
	}
	return numCores;
}
