#ifndef OM_GENERIC_MANAGER_H
#define OM_GENERIC_MANAGER_H

/*
 *	Templated generic manager for a objects that have an (OmId id) constructor.
 *
 *	Brett Warne - bwarne@mit.edu - 2/20/09
 */

#include "common/omCommon.h"
#include "common/omException.h"

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
	T& Get(OmId omId);
	void Remove(OmId omId);
	
	//valid
	bool IsValid(OmId omId) const;
	const OmIds& GetValidIds() const;

	//enabled
	bool IsEnabled(OmId omId) const;
	void SetEnabled(OmId omId, bool enable);
	const OmIds& GetEnabledIds() const;

private:
	
	QHash< OmId, T* > mMap;

	OmId mNextId;
	OmIds mValidSet;		// keys in map (fast iteration)
	OmIds mEnabledSet;		// enabled keys in map

	friend QDataStream &operator<<(QDataStream & out, const OmGenericManager<OmChannel> & );
	friend QDataStream &operator>>(QDataStream & in, OmGenericManager<OmChannel> & );

	friend QDataStream &operator<<(QDataStream & out, const OmGenericManager<OmSegmentation> & );
	friend QDataStream &operator>>(QDataStream & in, OmGenericManager<OmSegmentation> & );

	friend QDataStream &operator<<(QDataStream & out, const OmGenericManager<OmFilter2d> & );
	friend QDataStream &operator>>(QDataStream & in, OmGenericManager<OmFilter2d> & );

};

#endif

