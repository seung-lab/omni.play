#ifndef OM_DATA_WRAPPER_H
#define OM_DATA_WRAPPER_H

#include "common/omGl.h"

class OmDataWrapper : boost::noncopyable
{
 public:
	OmDataWrapper( void * ptr )
		: mData(ptr)
	{
	}

	~OmDataWrapper(){
		free(mData); // from malloc in hdf5...
	}

	char * getCharPtr(){ return (char*)mData; }
	quint8 * getQuint8Ptr(){ return (quint8*)mData; }
	uint32_t * getUInt32Ptr(){ return (uint32_t *)mData; }
	GLuint * getGLuintPtr(){ return (GLuint *)mData; }
	GLfloat * getGLfloatPtr(){ return (GLfloat *)mData; }
	quint32 * getQuint32Ptr(){ return (quint32 *)mData; }
	float * getFloatPtr(){ return (float*)mData; }
	void * getPtr(){ return mData; }

 private:	
	void * mData;
};

typedef boost::shared_ptr<OmDataWrapper> OmDataWrapperPtr;

#endif

/*
typedef < typename T >
class OmDataWrapper
{
 public:
	OmDataWrapper( T* ptr )
		: mData(ptr)
	{
	}
	
	T* getData(){ return mData; }

 private:	
	T* mData;

};
*/
