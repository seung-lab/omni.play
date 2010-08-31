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

#include <zi/mutex>
#include <zi/utility>

class OmGarbage : boost::noncopyable {

public:
	static void Delete();

	static void assignOmTextureId(const GLuint);
	static void safeCleanTextureIds();

	static void assignOmGenlistId(const GLuint);
	static void safeCleanGenlistIds();

private:
	OmGarbage(){}
	~OmGarbage();
	static inline OmGarbage& Instance(){
		return zi::Singleton<OmGarbage>::Instance();
	}

	std::vector<GLuint> mTextures;
	std::vector<GLuint> mGenlists;
	zi::Mutex mTextureMutex;
	zi::Mutex mGenlistMutex;

	friend class zi::Singleton<OmGarbage>;
};

#endif
