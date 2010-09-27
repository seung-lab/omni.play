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

#include "zi/omMutex.h"
#include "zi/omUtility.h"

class OmGarbage : boost::noncopyable {
public:

	static void Delete()
	{
		OmGarbage::safeCleanTextureIds();
		OmGarbage::safeCleanGenlistIds();
	}

	static void assignOmTextureId(const GLuint textureID)
	{
		zi::guard g(Instance().textureMutex_);
		Instance().mTextures.push_back(textureID);
	}

	static void safeCleanTextureIds()
	{
		zi::guard g(Instance().textureMutex_);

		glDeleteTextures(Instance().mTextures.size(),
						 &Instance().mTextures[0]);

		Instance().mTextures.clear();
	}

	static void assignOmGenlistId(const GLuint genlistID)
	{
		zi::guard g(Instance().meshMutex_);
		Instance().mGenlists.push_back(genlistID);
	}

	static void safeCleanGenlistIds()
	{
		zi::guard g(Instance().meshMutex_);

		FOR_EACH(iter, Instance().mGenlists){
			glDeleteLists((*iter), 1);
		}

		Instance().mGenlists.clear();
	}

private:
	OmGarbage(){}
	~OmGarbage(){
		Delete();
	}
	static inline OmGarbage& Instance(){
		return zi::singleton<OmGarbage>::instance();
	}

	std::vector<GLuint> mTextures;
	std::vector<GLuint> mGenlists;
	zi::mutex textureMutex_;
	zi::mutex meshMutex_;

	friend class zi::singleton<OmGarbage>;
};

#endif
