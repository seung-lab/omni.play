
#include "omTagManager.h"
#include "system/omDebug.h"

#define DEBUG 0

//init instance pointer
OmTagManager *OmTagManager::mspInstance = 0;

#pragma mark -
#pragma mark OmStateManager
/////////////////////////////////
///////
///////          OmStateManager
///////

OmTagManager::OmTagManager()
{

}

OmTagManager::~OmTagManager()
{

}

OmTagManager *OmTagManager::Instance()
{
	if (NULL == mspInstance) {
		mspInstance = new OmTagManager;
	}
	return mspInstance;
}

void OmTagManager::Delete()
{
	if (mspInstance)
		delete mspInstance;
	mspInstance = NULL;
}

#pragma mark
#pragma mark Tag Management
/////////////////////////////////
///////          Tag Management

OmTag & OmTagManager::GetTag(OmId id)
{
	return mManager.Get(id);
}

OmTag & OmTagManager::AddTag(OmId id)
{
	return mManager.Add(id);
}

void OmTagManager::RemoveTag(OmId id)
{
	mManager.Remove(id);
}

const OmIds & OmTagManager::GetValidTagIds()
{
	return mManager.GetValidIds();
}
