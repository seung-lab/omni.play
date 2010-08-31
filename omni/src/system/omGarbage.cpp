#include "common/omDebug.h"
#include "common/omException.h"
#include "omGarbage.h"
#include "omPreferenceDefinitions.h"
#include "system/cache/omCacheManager.h"
#include "system/omProjectData.h"

OmGarbage::~OmGarbage()
{
	Delete();
}

void OmGarbage::Delete()
{
	OmGarbage::safeCleanTextureIds();
	OmGarbage::safeCleanGenlistIds();
}

void OmGarbage::assignOmTextureId(const GLuint textureID)
{
	zi::Guard g(Instance().mTextureMutex);
	OmGarbage::Instance().mTextures.push_back(textureID);
}

void OmGarbage::assignOmGenlistId(const GLuint genlistID)
{
	zi::Guard g(Instance().mGenlistMutex);
        OmGarbage::Instance().mGenlists.push_back(genlistID);
}

void OmGarbage::safeCleanTextureIds()
{
	zi::Guard g(Instance().mTextureMutex);

	glDeleteTextures(Instance().mTextures.size(),
			 &Instance().mTextures[0]);

	OmGarbage::Instance().mTextures.clear();
}

void OmGarbage::safeCleanGenlistIds()
{
	zi::Guard g(Instance().mGenlistMutex);

	FOR_EACH(iter, Instance().mGenlists){
		glDeleteLists((*iter), 1);
	}

	OmGarbage::Instance().mGenlists.clear();
}

