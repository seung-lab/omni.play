#ifndef OM_GARBAGE_H
#define OM_GARBAGE_H

/*
 * 	Sweaps all the bad stuff under the rug until those who can
 * 	do the real work execute next. This is an alterative
 * 	garbage collection system. The texture throughput should
 * 	measured in time spent in the paint event in the call
 * 	to safeCleanTextureIds.
 *
 *      Must be called from main GUI thread.
 *
 * 	Matt Wimer mwimer@mit.edu 10/21/09
 */

#include "common/omCommon.h"
#include "common/omGl.h"
#include "utility/omTimer.h"

#include "zi/omMutex.h"
#include "zi/omUtility.h"

class OmGarbage : private om::singletonBase<OmGarbage> {
public:

	static void Delete()
	{
		safeCleanTextureIds();
		CleanGenlists();
	}

	static void assignOmTextureId(const GLuint textureID)
	{
		zi::guard g(instance().textureMutex_);
		instance().mTextures.push_back(textureID);
	}

	static void safeCleanTextureIds()
	{
		zi::guard g(instance().textureMutex_);

		if(!instance().mTextures.size()){
			return;
		}

		OmTimer timer;
		const int size = instance().mTextures.size();

		glDeleteTextures(size,
						 &instance().mTextures[0]);

		instance().mTextures.clear();

		timer.Print("OmGarbage: cleared " +
					om::NumToStr(size) + " old GL textures");
	}

	static void assignOmGenlistId(const GLuint genlistID)
	{
		zi::guard g(instance().meshMutex_);
		instance().mGenlists.push_back(genlistID);
	}

	static void CleanGenlists()
	{
		zi::guard g(instance().meshMutex_);

		FOR_EACH(iter, instance().mGenlists){
			glDeleteLists((*iter), 1);
		}

		instance().mGenlists.clear();
	}

private:
	OmGarbage(){}
	~OmGarbage(){
		Delete();
	}

	std::vector<GLuint> mTextures;
	std::vector<GLuint> mGenlists;
	zi::mutex textureMutex_;
	zi::mutex meshMutex_;

	friend class zi::singleton<OmGarbage>;
};

#endif
