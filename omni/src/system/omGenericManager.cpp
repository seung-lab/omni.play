#include "omGenericManager.h"

#define IS_ID_INVALID()  id < 1 || id >= mSize || NULL == mMap[id]

/*
 *	Constructor initializes first id and default parent.
 */
template < class T > 
OmGenericManager<T>::OmGenericManager() 
	: mNextId(1), mSize(10)
{ 
	mMap = std::vector<T*>(mSize, NULL);
}

/*
 *	Destructor destroyes all managed objects.
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
	findAndSetNextValudID();

	mMap[id] = new T(id);
	
	mValidSet.insert(id);
	mEnabledSet.insert(id);
	
	return *mMap[id];
}

// fill in holes in number map
template < class T > 
void
OmGenericManager<T>::findAndSetNextValudID()
{
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

	//make sure mNextId is adjusted accordingly
	findAndSetNextValudID();
}

/////////////////////////////////
///////		 Valid

/*
 *	Check if object with given OmId is being managed
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


/*
 *	Returns set of all valid OmIds (Ids of objects being managed).
 */
template < class T > 
const OmIds&
OmGenericManager<T>::GetValidIds() const 
{
	return mValidSet;
}

/////////////////////////////////
///////		 Enabled


/*
 *	Check if object with given OmId is enabled
 */
template < class T > 
bool
OmGenericManager<T>::IsEnabled(const OmId id) const 
{
	if( IS_ID_INVALID() ){
		throw OmAccessException("Given id is not valid: " + id); 
	}

	return mEnabledSet.contains(id);
}


/*
 *	Set object with given OmId to be enabled/disabled
 */
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

/*
 *	Returns set of all enabled ids
 */
template < class T > 
const OmIds&
OmGenericManager<T>::GetEnabledIds() const 
{
	return mEnabledSet;
}
