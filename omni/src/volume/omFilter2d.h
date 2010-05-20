#ifndef OM_FILTER2D_H
#define OM_FILTER2D_H

/*
 *	Filter Object
 *	
 *	Matthew Wimer - mwimer@mit.edu - 11/13/09
 */

#include "system/omManageableObject.h"
#include "view2d/omThreadedCachingTile.h"

class OmFilter2d : public OmManageableObject {

public:
	OmFilter2d (OmId segid, OmId chanid, OmId filterid, OmViewGroupState *);
	OmFilter2d(OmId);
	OmFilter2d();
	
	//accessors
	void SetAlpha(double);
	double GetAlpha();

	OmId GetSegmentation ();
	void SetSegmentation (OmId id);

	OmId GetChannel();
	void SetChannel (OmId id);

	OmThreadedCachingTile * GetCache (ViewType);
	
private:
	OmViewGroupState * mViewGroupState;
	double mAlpha;
	OmThreadedCachingTile * mCache;
	OmId mChannel;
	OmId mSeg;

	friend QDataStream &operator<<(QDataStream & out, const OmFilter2d & f );
	friend QDataStream &operator>>(QDataStream & in, OmFilter2d & f );
};

#endif
