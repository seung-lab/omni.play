#ifndef OM_GENERIC_MANAGER_H
#define OM_GENERIC_MANAGER_H

/*
 *	Templated generic manager for a objects that have an
 *   (OmId id) constructor.
 *
 *  NOT thread-safe
 *
 *	Brett Warne - bwarne@mit.edu - 2/20/09
 */

#include "common/omCommon.h"
#include "common/omException.h"

class OmChannel;
class OmSegmentation;
class OmFilter2d;
class OmGroup;

template <typename T>
class OmGenericManager {
private:
	static const uint32_t DEFAULT_MAP_SIZE = 10;

public:
	OmGenericManager()
		: mNextId(1)
		, mSize(DEFAULT_MAP_SIZE)
	{
		mMap.resize(DEFAULT_MAP_SIZE, NULL);
	}

	~OmGenericManager()
	{
		for(uint32_t i = 1; i < mSize; ++i){
			delete mMap[i];
		}
	}

	//managed accessors
	T& Add()
	{
		const OmId id = mNextId;
		mMap[id] = new T(id);
		findAndSetNextValidID();

		mValidSet.insert(id);
		mEnabledSet.insert(id);

		return *mMap[id];
	}

	T& Get( const OmId id) const
	{
		throwIfInvalidID(id);
		return *mMap[id];
	}

	void Remove( const OmId id)
	{
		throwIfInvalidID(id);

		mValidSet.erase(id);
		mEnabledSet.erase(id);

		delete mMap[id];
		mMap[id] = NULL;

		findAndSetNextValidID();
	}

	//valid
	bool IsValid( const OmId id) const{
		return !isIDinvalid(id);
	}

	const OmIDsSet& GetValidIds() const{
		return mValidSet;
	}

	//enabled
	bool IsEnabled( const OmId id) const
	{
		throwIfInvalidID(id);
		return mEnabledSet.count(id);
	}

	void SetEnabled( const OmId id, const bool enable)
	{
		throwIfInvalidID(id);

		if(enable) {
			mEnabledSet.insert(id);
		} else {
			mEnabledSet.erase(id);
		}
	}

	const OmIDsSet& GetEnabledIds() const{
		return mEnabledSet;
	}

private:
	OmId mNextId;
	uint32_t mSize;

	std::vector<T*> mMap;

	OmIDsSet mValidSet;	  // keys in map (fast iteration)
	OmIDsSet mEnabledSet; // enabled keys in map

	bool isIDinvalid(const OmId id) const {
		return id < 1 || id >= mSize || NULL == mMap[id];
	}

	void throwIfInvalidID(const OmId id) const {
		if(isIDinvalid(id)){
			assert(0 && "invalid ID");
			throw OmAccessException("Cannot get object with id: " + id);
		}
	}

	void findAndSetNextValidID()
	{
		// search to fill in holes in number map
		//  (holes could be present from object deletion...)
		for(uint32_t i = 1; i < mSize; ++i ){
			if( NULL == mMap[i] ){
				mNextId = i;
				return;
			}
		}

		mNextId = mSize;
		mSize *= 2;
		mMap.resize(mSize, NULL);
	}

	friend QDataStream &operator<<(QDataStream & out, const OmGenericManager<OmChannel> & );
	friend QDataStream &operator>>(QDataStream & in, OmGenericManager<OmChannel> & );

	friend QDataStream &operator<<(QDataStream & out, const OmGenericManager<OmSegmentation> & );
	friend QDataStream &operator>>(QDataStream & in, OmGenericManager<OmSegmentation> & );

	friend QDataStream &operator<<(QDataStream & out, const OmGenericManager<OmFilter2d> & );
	friend QDataStream &operator>>(QDataStream & in, OmGenericManager<OmFilter2d> & );

	friend QDataStream &operator<<(QDataStream & out, const OmGenericManager<OmGroup> & );
	friend QDataStream &operator>>(QDataStream & in, OmGenericManager<OmGroup> & );
};

#endif

