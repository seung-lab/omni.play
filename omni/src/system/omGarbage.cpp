
#include "omGarbage.h"
#include "omPreferenceDefinitions.h"
#include "omCacheManager.h"

#include "common/omException.h"
#include "common/omDebug.h"
#include "system/omProjectData.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <QFileInfo>
#include <QtNetwork/QTcpSocket>


#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/utsname.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 


namespace bfs = boost::filesystem;

#define DEBUG 0

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

