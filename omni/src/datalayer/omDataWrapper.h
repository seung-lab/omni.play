#ifndef OM_DATA_WRAPPER_H
#define OM_DATA_WRAPPER_H

#include "common/omCommon.h"
#include "common/omGl.h"

class OmDataWrapper : boost::noncopyable
{
 public:
	OmDataWrapper()
		: mData(NULL)
		, isValid(false)
		, okToDelete(true)
	{
	}

	OmDataWrapper( void * ptr )
		: mData(ptr)
		, isValid( true )
		, okToDelete(true)
	{
	}

	~OmDataWrapper(){
		if(okToDelete){
			free(mData); // from malloc in hdf5...
		}
	}

	char * getCharPtr(){ assert(isValid); return (char*)mData; }
	quint8 * getQuint8Ptr(){ assert(isValid); return (quint8*)mData; }
	uint32_t * getUInt32Ptr(){ assert(isValid); return (uint32_t *)mData; }
	GLuint * getGLuintPtr(){ assert(isValid); return (GLuint *)mData; }
	GLfloat * getGLfloatPtr(){ assert(isValid); return (GLfloat *)mData; }
	quint32 * getQuint32Ptr(){ assert(isValid); return (quint32 *)mData; }
	float * getFloatPtr(){ assert(isValid); return (float*)mData; }
	void * getPtr(){ assert(isValid); return mData; }

 protected:	
	void * mData;
	const bool isValid;
	bool okToDelete;
};

typedef boost::shared_ptr<OmDataWrapper> OmDataWrapperPtr;

class OmDataWrapperMemmap : public OmDataWrapper
{
 public:
	OmDataWrapperMemmap()
		: OmDataWrapper()
		{
			okToDelete = false;
		}

	OmDataWrapperMemmap( void * ptr )
		: OmDataWrapper(ptr)
	{
		okToDelete = false;
	}

	~OmDataWrapperMemmap(){
	}
};


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
