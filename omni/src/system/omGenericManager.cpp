#include "omGenericManager.h"

/*
 *	Constructor initializes first id and default parent.
 */
template < class T > 
OmGenericManager<T>::OmGenericManager() 
	: mNextId(1) 
{ 
}

/*
 *	Destructor destroyes all managed objects.
 */
template < class T > 
OmGenericManager<T>::~OmGenericManager() 
{
	//delete all objects
	foreach( T* thing, mMap ){
		delete( thing );
	}
}

/*
 *	Get object with given OmId from manager
 */
template < class T > 
T&
OmGenericManager<T>::Get(OmId omId) 
{	
	//check volume with id exists
	if( !mMap.contains(omId) ){
		throw OmAccessException("Cannot get object with id: " + omId);
	}
	
	//else return ref to volume
	return *mMap[omId];
}

/*
 *	Add object to manager, tries to give it given OmId if specified
 */
template < class T > 
T& 
OmGenericManager<T>::Add() 
{
	//get next id, adv if not available or next was null id
	OmId omId = mNextId++;
	while( mMap.contains(omId) || 0 == omId ){
		omId = mNextId++;
	}
	
	//insert new managed object
	mMap[omId] = new T(omId);
	
	//enable/valid on insert
	mValidSet.insert(omId);
	mEnabledSet.insert(omId);
	
	//return pointer
	return Get(omId);
}

/*
 *	Remove object with given OmId from manager
 */
template < class T > 
void 
OmGenericManager<T>::Remove(OmId omId) 
{
	//check volume with id exists
	if(mMap.count(omId) == 0) {
		throw OmAccessException("Cannot remove object with id: " + omId); 
	}
	
	//disable/not valid on remove
	mValidSet.remove(omId);
	mEnabledSet.remove(omId);
	
	//erase from map before deleting object
	T *p_object = mMap[omId];
	mMap.remove(omId);
	delete p_object;

	//make sure mNextId is adjusted accordingly
	OmId maxId = 0;
	QHashIterator< OmId, T* > it(mMap);
	while( it.hasNext()) {
		it.next();
		if (it.key() > maxId) maxId=it.key();
	}
	maxId++;
	mNextId=maxId;
}

/////////////////////////////////
///////		 Valid

/*
 *	Check if object with given OmId is being managed
 */
template < class T > 
bool
OmGenericManager<T>::IsValid(OmId omId) const 
{
	return mMap.contains(omId);
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
OmGenericManager<T>::IsEnabled(OmId omId) const 
{
	if(!IsValid(omId))
		throw OmAccessException("Given id is not valid: " + omId); 
	
	return mEnabledSet.contains(omId);
}


/*
 *	Set object with given OmId to be enabled/disabled
 */
template < class T > 
void
OmGenericManager<T>::SetEnabled(OmId omId, bool enable) 
{
	if(!IsValid(omId))
		throw OmAccessException("Given id is not valid: " + omId); 
	
	if(enable)
		mEnabledSet.insert(omId);
	else
		mEnabledSet.remove(omId);
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
