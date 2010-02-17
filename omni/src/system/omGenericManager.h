#ifndef OM_GENERIC_MANAGER_H
#define OM_GENERIC_MANAGER_H

/*
 *	Templated generic manager for a objects that have an (OmId id) constructor.
 *
 *	Brett Warne - bwarne@mit.edu - 2/20/09
 */

#include "common/omStd.h"
#include "common/omException.h"
#include "common/omSerialization.h"
#include "omSystemTypes.h"



template < class T >
class OmGenericManager {
	
public:	

	OmGenericManager();
	~OmGenericManager();
	
	//managed accessors
	T& Add(OmId omId = NULL_OM_ID);
	T& Set(OmId omId, T* pObject);
	T& Get(OmId omId);
	void Remove(OmId omId);
	
	//state
	uint32_t Size();
	
	//valid
	bool IsValid(OmId omId) const;
	const OmIds& GetValidIds() const;
	void CallValid(void (T::*fxn)() );
	template< class U > void CallValid(void (T::*fxn)(U), U arg);

	//enabled
	bool IsEnabled(OmId omId) const;
	void SetEnabled(OmId omId, bool enable);
	void SetAllEnabled(bool enable);
	const OmIds& GetEnabledIds() const;
	void CallEnabled(void (T::*fxn)() );
	template< class U > void CallEnabled(void (T::*fxn)(U), U arg);	
	
	//selected
	bool IsSelected(OmId omId) const;
	void SetSelected(OmId omId, bool enable);
	void SetAllSelected(bool enable);
	const OmIds& GetSelectedIds() const;
	void CallSelected(void (T::*fxn)() );
	template< class U > void CallSelected(void (T::*fxn)(U), U arg);	
	
	//functions
	void Apply(void (*fxn)(T &) );

private:
	void Call( OmIds& set, void (T::*fxn)() );
	template< class U > void Call( OmIds& set, void (T::*fxn)(U), U arg);	
	
	OmId mNextId;
	map< OmId, T*> mMap;
	OmIds mValidSet;		//keys in map (fast iteration)
	OmIds mEnabledSet;		//enabled keys in map
	OmIds mSelectedSet;			//selected keys in map
	
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int file_version);
};





/*
 *	Constructor initializes first id and default parent.
 */
template < class T > 
OmGenericManager<T>::OmGenericManager() : mNextId(1) { 
	//cout << "OmGenericManager::OmGenericManager()" << endl; 
}



/*
 *	Destructor destroyes all managed objects.
 */
template < class T > 
OmGenericManager<T>::~OmGenericManager() {
	//cout << "OmGenericManager<T>::~OmGenericManager()" << endl;
	//delete all objects
	typename map< OmId, T* >::iterator it;
	for( it=mMap.begin(); it != mMap.end(); it++ ) {
		delete it->second;
	}
}



/*
 *	Get object with given OmId from manager
 */
template < class T > 
T&
OmGenericManager<T>::Get(OmId omId) {
	
	//check volume with id exists
	if(mMap.count(omId) == 0) {
		//if allowed to add
		//if(add) return Add(omId);
		//else throw
	        char omIdchar[25];
		snprintf(omIdchar,sizeof(omIdchar),"%i",omId);
		throw OmAccessException("Cannot get object with id: " + string(omIdchar)); 
	}
	
	//else return ref to volume
	return *mMap[omId];
}



/*
 *	Add object to manager, tries to give it given OmId if specified
 */
template < class T > 
T& 
OmGenericManager<T>::Add(OmId omId) {
	//if no id specified
	if(NULL_OM_ID == omId) {
		//get next id, adv if not available or next was null id
		omId = mNextId++;
		while(!mMap.count(omId) && (NULL_OM_ID == omId)) {
			omId = mNextId++;
		}
	} else {
		//make sure given id is available
		if(mMap.count(omId) != 0) {
			//if ignoring duplicates
			//if(ignore) return Get(omId);
		        char omIdchar[25];
		        snprintf(omIdchar,sizeof(omIdchar),"%i",omId);
			throw OmAccessException("Object with given id already exists: " + string(omIdchar)); 
		}
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
 *	Set object in manager
 */
template < class T > 
T& 
OmGenericManager<T>::Set(OmId omId, T* pObject) {
	//if no id specified
	if(NULL_OM_ID == omId) {
		throw OmAccessException("Object must already have an id specified.");
	} else {
		//make sure given id is available
		if(mMap.count(omId) != 0) {
	                char omIdchar[25];
		        snprintf(omIdchar,sizeof(omIdchar),"%i",omId);		        
			throw OmAccessException("Object with given id already exists: " + string(omIdchar)); 
		}
	}
	
	//set new managed object
	mMap[omId] = pObject;
	
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
OmGenericManager<T>::Remove(OmId omId) {
	//check volume with id exists
	if(mMap.count(omId) == 0) {
		throw OmAccessException("Cannot remove object with id: " + omId); 
	}
	
	//disable/not valid on remove
	mValidSet.erase(omId);
	mEnabledSet.erase(omId);
	mSelectedSet.erase(omId);
	
	//erase from map before deleting object
	T *p_object = mMap[omId];
	mMap.erase(omId);
	delete p_object;
}




/*
 *	Get the size of objects managed
 */
template < class T >
uint32_t 
OmGenericManager<T>::Size() { 
	return mMap.size();
}





#pragma mark 
#pragma mark Valid
/////////////////////////////////
///////		 Valid

/*
 *	Check if object with given OmId is being managed
 */
template < class T > 
bool
OmGenericManager<T>::IsValid(OmId omId) const {
	return mMap.count(omId);
}


/*
 *	Returns set of all valid OmIds (Ids of objects being managed).
 */
template < class T > 
const OmIds&
OmGenericManager<T>::GetValidIds() const {
	return mValidSet;
}


template < class T >
void
OmGenericManager<T>::CallValid( void (T::*fxn)() ) {
	Call( mValidSet, fxn );
}

template < class T >
template < class U > 
void
OmGenericManager<T>::CallValid(void (T::*fxn)(U), U arg) {
	Call< U >( mValidSet, fxn, arg);
}





#pragma mark 
#pragma mark Enabled
/////////////////////////////////
///////		 Enabled


/*
 *	Check if object with given OmId is enabled
 */
template < class T > 
bool
OmGenericManager<T>::IsEnabled(OmId omId) const {
	if(!IsValid(omId))
		throw OmAccessException("Given id is not valid: " + omId); 
	
	return mEnabledSet.count(omId);
}


/*
 *	Set object with given OmId to be enabled/disabled
 */
template < class T > 
void
OmGenericManager<T>::SetEnabled(OmId omId, bool enable) {
	if(!IsValid(omId))
		throw OmAccessException("Given id is not valid: " + omId); 
	
	if(enable)
		mEnabledSet.insert(omId);
	else
		mEnabledSet.erase(omId);
}


/*
 *	Sets all enabled ids
 */
template < class T > 
void
OmGenericManager<T>::SetAllEnabled(bool selected) {
	if(selected)
		mEnabledSet = mValidSet;
	else
		mEnabledSet.clear();
}

/*
 *	Returns set of all enabled ids
 */
template < class T > 
const OmIds&
OmGenericManager<T>::GetEnabledIds() const {
	return mEnabledSet;
}


template < class T >
void
OmGenericManager<T>::CallEnabled( void (T::*fxn)() ) {
	Call( mEnabledSet, fxn );
}

template < class T >
template < class U > 
void
OmGenericManager<T>::CallEnabled(void (T::*fxn)(U), U arg) {
	Call< U >( mEnabledSet, fxn, arg);
}








#pragma mark 
#pragma mark Selected
/////////////////////////////////
///////		 Selected

/*
 *	Check if object with given OmId is selected
 */
template < class T > 
bool
OmGenericManager<T>::IsSelected(OmId omId) const {
	if(!IsValid(omId))
		throw OmAccessException("Given id is not valid: " + omId); 
	
	return mSelectedSet.count(omId);
}


/*
 *	Set object with given OmId to be selected/unselected
 */
template < class T > 
void
OmGenericManager<T>::SetSelected(OmId omId, bool selected) {
	
	if(!IsValid(omId))
		throw OmAccessException("Given id is not valid: " + omId); 
	
	if(selected)
		mSelectedSet.insert(omId);
	else
		mSelectedSet.erase(omId);
}


/*
 *	Sets all selected ids
 */
template < class T > 
void
OmGenericManager<T>::SetAllSelected(bool selected) {
	if(selected)
		mSelectedSet = mValidSet;
	else
		mSelectedSet.clear();
}


/*
 *	Returns set of all selected ids
 */
template < class T > 
const OmIds&
OmGenericManager<T>::GetSelectedIds() const {
	return mSelectedSet;
}




template < class T >
void
OmGenericManager<T>::CallSelected( void (T::*fxn)() ) {
	Call( mSelectedSet, fxn );
}

template < class T >
template < class U > 
void
OmGenericManager<T>::CallSelected(void (T::*fxn)(U), U arg) {
	Call< U >( mSelectedSet, fxn, arg);
}













#pragma mark 
#pragma mark Function Calls
/////////////////////////////////
///////		 Function Calls

/*
 *	Apply global function of form void fxn(T &) to each managed T
 */
template < class T >
void 
OmGenericManager<T>::Apply(void (*fxn)(T &) ) {
	typename map< OmId, T* >::iterator it;

	for( it=mMap.begin(); it != mMap.end(); it++ )
		(*fxn)( *(it->second) );
}


/*
 *	Call member function of T with one argument of type U
 */
/*
template < class T >
template < class U > 
void
OmGenericManager<T>::Call(void (T::*fxn)(U), U arg) {
	//specify to treat ::iterator as a dependent qualified type (rather than a static member)
	//http://pages.cs.wisc.edu/~driscoll/typename.html
	typename map< OmId, T* >::iterator it;
	
	for( it=mMap.begin(); it != mMap.end(); it++ )
		(it->second->*fxn)(arg);
}
*/




template < class T >
void
OmGenericManager<T>::Call( OmIds &set, void (T::*fxn)() ) {
	
	//for each enabled
	OmIds::iterator it;
	for( it=set.begin(); it != set.end(); it++ )
		(mMap[*it]->*fxn)();
}



template < class T >
template < class U > 
void
OmGenericManager<T>::Call( OmIds &set, void (T::*fxn)(U), U arg) {
	
	//for each enabled
	OmIds::iterator it;
	for( it=set.begin(); it != set.end(); it++ )
		(mMap[*it]->*fxn)(arg);
	
}











#pragma mark 
#pragma mark Serialization
/////////////////////////////////
///////		 Serialization


template<class T> 
template<class Archive>
inline void 
OmGenericManager<T>::serialize( Archive & ar, const unsigned int file_version ) {
	ar & mNextId; 
	ar & mMap;
	ar & mValidSet;
	ar & mEnabledSet;
	ar & mSelectedSet;
}



/*
namespace boost { 
	namespace serialization {

		template<class Archive, class T>
		inline void 
		serialize( Archive & ar, OmGenericManager<T> & t, const unsigned int file_version ) {
			ar & t.mNextId; 
			ar & t.mMap;
		}
		
	} //serialization
} //boost
*/


#endif

