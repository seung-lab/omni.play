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



#include "omPreferences.h"
#include "omStateManager.h"
#include "omKeyManager.h"
#include "omTagManager.h"
#include "volume/omVolume.h"

#include "common/omStd.h"
#include "common/omSerialization.h"

#include <vmmlib/vmmlib.h>
#include <vmmlib/serialization.h>
#include <pthread.h>

using namespace vmml;





class OmGarbage {

public:
	
	static OmGarbage* Instance();
	static void Delete();
	

	static void asOmTextureId(GLuint);
	static void safeCleanTextureIds ();
	
	static void Lock ();
	static vector<GLuint>& LockTextures ();
	static void Unlock ();
	static void UnlockTextures ();
	
protected:
	// singleton constructor, copy constructor, assignment operator protected
	OmGarbage();
	~OmGarbage();
	OmGarbage(const OmGarbage&);
	OmGarbage& operator= (const OmGarbage&);

	
private:
	//singleton
	static OmGarbage* mspInstance;
	

	//garbage
	vector <GLuint> mTextures;
	pthread_mutex_t mTextureMutex;
};







#pragma mark 
#pragma mark Serialization
/////////////////////////////////
///////		 Serialization




#endif
