#ifndef OM_GENERIC_MANAGER_H
#define OM_GENERIC_MANAGER_H

/*
 *	Templated generic manager for a objects that have an (OmId id) constructor.
 *
 *	Brett Warne - bwarne@mit.edu - 2/20/09
 */

#include "common/omCommon.h"

class OmChannel;
class OmSegmentation;
class OmFilter2d;

template < class T >
class OmGenericManager {
	
public:	
	OmGenericManager();
	~OmGenericManager();
	
	//managed accessors
	T& Add();
	T& Get( const OmId omId);
	void Remove( const OmId omId);
	
	//valid
	bool IsValid( const OmId omId) const;
	const OmIds& GetValidIds() const;

	//enabled
	bool IsEnabled( const OmId omId) const;
	void SetEnabled( const OmId omId, const bool enable);
	const OmIds& GetEnabledIds() const;

private:
	OmId mNextId;
	unsigned int mSize;

	std::vector< T* > mMap;

	OmIds mValidSet;		// keys in map (fast iteration)
	OmIds mEnabledSet;		// enabled keys in map

	void findAndSetNextValidID();


	friend QDataStream &operator<<(QDataStream & out, const OmGenericManager<OmChannel> & );
	friend QDataStream &operator>>(QDataStream & in, OmGenericManager<OmChannel> & );

	friend QDataStream &operator<<(QDataStream & out, const OmGenericManager<OmSegmentation> & );
	friend QDataStream &operator>>(QDataStream & in, OmGenericManager<OmSegmentation> & );

	friend QDataStream &operator<<(QDataStream & out, const OmGenericManager<OmFilter2d> & );
	friend QDataStream &operator>>(QDataStream & in, OmGenericManager<OmFilter2d> & );

};

#endif

