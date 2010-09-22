#include "system/omGarbage.h"

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
	Instance().mTextures.push_back(textureID);
}

void OmGarbage::safeCleanTextureIds()
{
	zi::Guard g(Instance().mTextureMutex);

	glDeleteTextures(Instance().mTextures.size(),
			 &Instance().mTextures[0]);

	Instance().mTextures.clear();
}

void OmGarbage::assignOmGenlistId(const GLuint genlistID)
{
	zi::Guard g(Instance().mGenlistMutex);
        Instance().mGenlists.push_back(genlistID);
}

void OmGarbage::safeCleanGenlistIds()
{
	zi::Guard g(Instance().mGenlistMutex);

	FOR_EACH(iter, Instance().mGenlists){
		glDeleteLists((*iter), 1);
	}

	Instance().mGenlists.clear();
}

