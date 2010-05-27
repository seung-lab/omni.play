#include "omGenericManager.h"

#define IS_ID_INVALID()  id < 1 || id >= mSize || NULL == mMap[id]

static const unsigned int DEFAULT_MAP_SIZE = 10;

template < class T > 
OmGenericManager<T>::OmGenericManager() 
	: mNextId(1)
	, mSize(DEFAULT_MAP_SIZE)
	, mMap( std::vector<T*>(DEFAULT_MAP_SIZE, NULL) )
{
}

/**
 *	Destroy all managed objects
 */
template < class T > 
OmGenericManager<T>::~OmGenericManager() 
{
	for( unsigned int i = 1; i < mSize; ++i){
		delete mMap[i];
	}
}

extern void myBacktrace(int);

template < class T > 
T&
OmGenericManager<T>::Get( const OmId id) 
{	
	if( IS_ID_INVALID() ){
		throw OmAccessException("Cannot get object with id: " + id);
	}

	//myBacktrace(0);
	
	// return ref
	return *mMap[id];
}

template < class T > 
T& 
OmGenericManager<T>::Add() 
{
	const OmId id = mNextId;
	findAndSetNextValidID();

	mMap[id] = new T(id);
	
	mValidSet.insert(id);
	mEnabledSet.insert(id);
	
	return *mMap[id];
}

template < class T > 
void
OmGenericManager<T>::findAndSetNextValidID()
{
	// search to fill in holes in number map 
	//  (holes could be present from object deletion...)
	for( unsigned int i = 1; i < mSize; ++i ){
		if( NULL == mMap[i] ){
			mNextId = i;
			return;
		}
	}

	mNextId = mSize;
	mSize *= 2;
	mMap.resize( mSize, NULL);
}

template < class T > 
void 
OmGenericManager<T>::Remove(const OmId id) 
{
	if( IS_ID_INVALID() ){
		throw OmAccessException("Cannot remove object with id: " + id); 
	}
	
	mValidSet.remove(id);
	mEnabledSet.remove(id);
	
	delete mMap[id];
	mMap[id] = NULL;

	findAndSetNextValidID();
}

/////////////////////////////////
///////		 Valid

/**
 * ID is of an object in the manager
 */
template < class T > 
bool
OmGenericManager<T>::IsValid( const OmId id) const 
{
	if( IS_ID_INVALID() ){
		return false;
	}

	return true;
}

/**
 *	Set of IDs for objects being managed
 */
template < class T > 
const OmIds&
OmGenericManager<T>::GetValidIds() const 
{
	return mValidSet;
}

/////////////////////////////////
///////		 Enabled

template < class T > 
bool
OmGenericManager<T>::IsEnabled(const OmId id) const 
{
	if( IS_ID_INVALID() ){
		throw OmAccessException("Given id is not valid: " + id); 
	}

	return mEnabledSet.contains(id);
}

template < class T > 
void
OmGenericManager<T>::SetEnabled(const OmId id, const bool enable) 
{
	if( IS_ID_INVALID() ){
		throw OmAccessException("Given id is not valid: " + id); 
	}
	
	if(enable) {
		mEnabledSet.insert(id);
	} else {
		mEnabledSet.remove(id);
	}
}

template < class T > 
const OmIds&
OmGenericManager<T>::GetEnabledIds() const 
{
	return mEnabledSet;
}
