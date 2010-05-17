#include "project/omProject.h"
#include "volume/omFilter2d.h"

#include "volume/omDrawOptions.h"
#include "common/omGl.h"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "volume/omVolume.h"
#include "view2d/omThreadedCachingTile.h"
#include "view2d/omView2d.h"

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
	mName = QString("filter%1").arg(omId);
	
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
		fastCache = new OmCachingThreadedCachingTile (viewtype, SEGMENTATION, mSeg, &OmProject::GetSegmentation(mSeg), NULL);
	} else if (mChannel) {
		fastCache = new OmCachingThreadedCachingTile (viewtype, CHANNEL, mChannel, &OmProject::GetChannel(mChannel), NULL);
	}

	if (fastCache) {
		mCache = fastCache->mCache;
		if (fastCache->mDelete) delete fastCache;
	} else
		return NULL;

	return mCache;
}

OmId OmFilter2d::GetSegmentation () {
	return mSeg;
}

void OmFilter2d::SetSegmentation (OmId id) {
	if (mSeg != id || 0 == id) {
		mCache = NULL;
	}
	try {
		//OmProject::GetSegmentation (id);
		mSeg = id;
	} catch (OmAccessException e) {
		mSeg = 0;
	}
}

OmId OmFilter2d::GetChannel () {
	return mChannel;
}

void OmFilter2d::SetChannel (OmId id) {
	if (mChannel != id) {
		mCache = NULL;
	}
	try {
		OmProject::GetChannel (id);
		mChannel = id;
	} catch (OmAccessException e) {
		mChannel = 0;
	}
}
