#ifndef BI_MAP_H
#define BI_MAP_H

/*
 *	Brett Warne - bwarne@mit.edu - 4/15/09
 */


#include "common/omSerialization.h"

#include <map>


template < typename T,  typename U  >
class BiMap {

public:
	size_t count( const T &t_key) {
		return mTUMap.count(t_key);
	}
	
	size_t count( const U &u_key) {
		return mUTMap.count(u_key);
	}
	
	U* operator[]( const T &t_key) {
		//if T doesn't exist, return null
		if(0 == count(t_key))
			return NULL;
		
		//else return mapped value
		return &mTUMap[t_key];
	}
	
	T* operator[]( const U &u_key )  {
		//if T doesn't exist, return null
		if(0 == count(u_key))
			return NULL;
		
		//else return mapped value
		return &mUTMap[u_key];
	}
	
	
	void unmap(const T &t_key) {
		//if T is already mapped to a value
		if(count(t_key)) {
			//erase old U to T map
			mUTMap.erase(mTUMap[t_key]);
			//erase old T to U map
			mTUMap.erase(t_key);
		}
	}
	
	void unmap(const U &u_key) {
		//if U is already mapped to a value
		if(count(u_key)) {
			//erase old T to U map
			mTUMap.erase(mUTMap[u_key]);
			//erase old U to T map
			mUTMap.erase(u_key);
		}
	}		
	
	
	
	//create new mapping
	void map( const U &u_key, const T &t_key ) {
		map( t_key, u_key);
	}
	
	//create new mapping
	void map(const T &t_key, const U &u_key) {
		//unmap keys incase already mapped
		unmap(t_key);
		unmap(u_key);
		
		//create new bimapping
		mTUMap[t_key] = u_key;
		mUTMap[u_key] = t_key;
	}

	
private:
	std::map<T, U> mTUMap;
	std::map<U, T> mUTMap;
	
	
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int file_version);
};




#pragma mark 
#pragma mark Serialization
/////////////////////////////////
///////		 Serialization


template< typename T, typename U > 
template<class Archive>
inline void 
BiMap<T,U>::serialize( Archive & ar, const unsigned int file_version ) {
	ar & mTUMap;
	ar & mUTMap;
}



#endif
