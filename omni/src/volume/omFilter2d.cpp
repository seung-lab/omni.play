
#include "volume/omFilter.h"

#include "volume/omDrawOptions.h"
#include "common/omGl.h"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "volume/omVolume.h"
#include "view2d/omThreadedCachingTile.h"
#include "view2d/omView2d.h"

#define DEBUG 0



/////////////////////////////////
///////
///////		OmFilter Class
///////

OmFilter2d::OmFilter2d (OmId segid, OmId chanid, OmId filterid)
{
	OmFilter2d *repair = new OmFilter2d (filterid);
	mSeg = segid;
	mChannel = chanid;
	repair->GetCache (XY_VIEW);
}


OmFilter2d::OmFilter2d() {
	mAlpha = 0.0;
	mSeg = 0;
	mChannel = 0;
}

OmFilter2d::OmFilter2d(OmId omId) 
: OmManageableObject(omId) 
{
	mName = QString("filter%1").arg(omId).toStdString();
	
	//initially transparent.
	mAlpha = 0.0;
	mSeg = 0;
	mChannel = 0;
}


void OmFilter2d::SetAlpha (double alpha)
{
	mAlpha = alpha;
}

double OmFilter2d::GetAlpha ()
{
	return mAlpha;
}

OmThreadedCachingTile * OmFilter2d::GetCache (ViewType viewtype)
{
	OmCachingThreadedCachingTile *fastCache = NULL;
	if (mSeg) {
		fastCache = new OmCachingThreadedCachingTile (viewtype, SEGMENTATION, mSeg, &OmVolume::GetSegmentation(mSeg), NULL);
	} else if (mChannel) {
		fastCache = new OmCachingThreadedCachingTile (viewtype, CHANNEL, mChannel, &OmVolume::GetChannel(mChannel), NULL);
	}

	if (fastCache) {
		mCache = fastCache->mCache;
		if (fastCache->mDelete) delete fastCache;
	} else
		return NULL;

	return mCache;
}


void OmFilter2d::SetSegmentation (OmId id) {
	if (mSeg != id || 0 == id) {
		mCache = NULL;
	}
	try {
		//OmVolume::GetSegmentation (id);
		mSeg = id;
	} catch (OmAccessException e) {
		mSeg = 0;
	}
}

void OmFilter2d::SetChannel (OmId id) {
	if (mChannel != id) {
		mCache = NULL;
	}
	try {
		OmVolume::GetChannel (id);
		mChannel = id;
	} catch (OmAccessException e) {
		mChannel = 0;
	}
}
