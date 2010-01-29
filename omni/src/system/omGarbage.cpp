
#include "omGarbage.h"
#include "omException.h"
#include "omPreferenceDefinitions.h"
#include "omCacheManager.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include "system/omDebug.h"
namespace bfs = boost::filesystem;

#define DEBUG 0

//init instance pointer
OmGarbage *OmGarbage::mspInstance = 0;

#pragma mark -
#pragma mark OmGarbage
/////////////////////////////////
///////
///////          OmGarbage
///////

OmGarbage::OmGarbage()
{
}

OmGarbage::~OmGarbage()
{

}

OmGarbage *OmGarbage::Instance()
{
	if (NULL == mspInstance) {
		mspInstance = new OmGarbage;
	}
	pthread_mutex_init(&mspInstance->mTextureMutex, NULL);
	return mspInstance;
}

void OmGarbage::Delete()
{
	if (mspInstance) {
		pthread_mutex_destroy(&OmGarbage::Instance()->mTextureMutex);
		delete mspInstance;
	}
	mspInstance = NULL;
}

void OmGarbage::asOmTextureId(GLuint texture)
{
	OmGarbage::Instance()->Lock();
	OmGarbage::Instance()->mTextures.push_back(texture);
	OmGarbage::Instance()->Unlock();
}

void OmGarbage::Lock()
{
	pthread_mutex_lock(&OmGarbage::Instance()->mTextureMutex);
}

vector < GLuint > &OmGarbage::LockTextures()
{
	OmGarbage::Instance()->Lock();
	return OmGarbage::Instance()->mTextures;
}

void OmGarbage::Unlock()
{
	pthread_mutex_unlock(&OmGarbage::Instance()->mTextureMutex);
}

void OmGarbage::UnlockTextures()
{
	OmGarbage::Instance()->Unlock();
}

void OmGarbage::safeCleanTextureIds()
{
	OmGarbage::Instance()->Lock();

	//debug("FIXME",  << "freeing... " << OmGarbage::Instance()->mTextures.size()<< endl;

	glDeleteTextures(OmGarbage::Instance()->mTextures.size(), &OmGarbage::Instance()->mTextures[0]);
	OmGarbage::Instance()->mTextures.clear();

	OmGarbage::Instance()->Unlock();
}
