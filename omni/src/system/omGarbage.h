#ifndef OM_GARBAGE_H
#define OM_GARBAGE_H

/*
 * 	Sweaps all the bad stuff under the rug until those who can
 * 	do the real work execute next. This is an alterative
 * 	garbage collection system. The texture throughput should
 * 	measured in time spent in the paint event in the call
 * 	to safeCleanTextureIds.
 *
 * 	Matt Wimer mwimer@mit.edu 10/21/09
 */

#include "common/omCommon.h"
#include "common/omGl.h"

#include <QMutex>

class OmGarbage : boost::noncopyable {

public:
	
	static OmGarbage* Instance();
	static void Delete();
	
	static void asOmTextureId(GLuint);
	static void safeCleanTextureIds ();
	
	static void Lock ();
	static vector<GLuint>& LockTextures ();
	static void Unlock ();
	static void UnlockTextures ();
	
private:
	OmGarbage();
	~OmGarbage();

	//singleton
	static OmGarbage* mspInstance;
	
	//garbage
	vector <GLuint> mTextures;
	QMutex mTextureMutex;
};

#endif
