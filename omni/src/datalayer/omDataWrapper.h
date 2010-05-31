#ifndef OM_DATA_WRAPPER_H
#define OM_DATA_WRAPPER_H

#include "common/omGl.h"

class OmDataWrapper : boost::noncopyable
{
 public:
	OmDataWrapper()
		: mData(NULL)
		, isValid(false)
	{
	}

	OmDataWrapper( void * ptr )
		: mData(ptr)
		, isValid( true )
	{
	}

	~OmDataWrapper(){
		free(mData); // from malloc in hdf5...
	}

	char * getCharPtr(){ assert(isValid); return (char*)mData; }
	quint8 * getQuint8Ptr(){ assert(isValid); return (quint8*)mData; }
	uint32_t * getUInt32Ptr(){ assert(isValid); return (uint32_t *)mData; }
	GLuint * getGLuintPtr(){ assert(isValid); return (GLuint *)mData; }
	GLfloat * getGLfloatPtr(){ assert(isValid); return (GLfloat *)mData; }
	quint32 * getQuint32Ptr(){ assert(isValid); return (quint32 *)mData; }
	float * getFloatPtr(){ assert(isValid); return (float*)mData; }
	void * getPtr(){ assert(isValid); return mData; }

 private:	
	void * mData;
	const bool isValid;
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
