
#include "omGarbage.h"
#include "omPreferenceDefinitions.h"
#include "omCacheManager.h"

#include "common/omException.h"
#include "common/omDebug.h"
#include "system/omProjectData.h"

//init instance pointer
OmGarbage *OmGarbage::mspInstance = 0;

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
		mspInstance = new OmGarbage();
	}

	return mspInstance;
}

void OmGarbage::Delete()
{
	if (mspInstance) {
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

void OmGarbage::asOmGenlistId(GLuint genlist)
{
        OmGarbage::Instance()->LockGenlists();
        OmGarbage::Instance()->mGenlists.push_back(genlist);
        OmGarbage::Instance()->UnlockGenlists();
}


vector < GLuint > & OmGarbage::LockGenlists()
{
	OmGarbage::Instance()->mGenlistMutex.lock();
	return OmGarbage::Instance()->mGenlists;
}

void OmGarbage::UnlockGenlists()
{
	OmGarbage::Instance()->mGenlistMutex.unlock();
}

void OmGarbage::Lock()
{
	OmGarbage::Instance()->mTextureMutex.lock();
}

vector < GLuint > &OmGarbage::LockTextures()
{
	OmGarbage::Instance()->Lock();
	return OmGarbage::Instance()->mTextures;
}

void OmGarbage::Unlock()
{
	OmGarbage::Instance()->mTextureMutex.unlock();
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


void OmGarbage::safeCleanGenlistIds()
{
	OmGarbage::Instance()->LockGenlists();

	vector <GLuint>::iterator iter;
	for(iter = OmGarbage::Instance()->mGenlists.begin(); iter != OmGarbage::Instance()->mGenlists.end(); iter++) {
		glDeleteLists((*iter), 1);
	}
	OmGarbage::Instance()->mGenlists.clear();

	OmGarbage::Instance()->UnlockGenlists();
}

