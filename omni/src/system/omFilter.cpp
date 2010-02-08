
#include "system/omFilter.h"

#include "volume/omDrawOptions.h"
#include "common/omGl.h"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "volume/omVolume.h"
#include "view2d/omThreadedCachingTile.h"
#include "view2d/omView2d.h"
#include "system/omDebug.h"

#define DEBUG 0

#pragma mark -
#pragma mark OmFilter Class
/////////////////////////////////
///////
///////         OmFilter Class
///////

OmFilter::OmFilter(OmId segid, OmId chanid, OmId filterid)
{
	OmFilter *repair;

	//OmView2d *xy = new OmView2d(XY_VIEW, CHANNEL, chanid, 0, 0, NULL);

	repair = new OmFilter(filterid);
	mSeg = segid;
	mChannel = chanid;
	repair->GetCache(XY_VIEW);
}

OmFilter::OmFilter()
{
	mAlpha = 0.0;
	mSeg = 0;
	mChannel = 0;
}

OmFilter::OmFilter(OmId omId)
 : OmManageableObject(omId)
{

	mName = printf2str(256, "filter%05d", omId);

	//initially transparent.
	mAlpha = 0.0;
	mSeg = 0;
	mChannel = 0;
}

void OmFilter::SetAlpha(double alpha)
{
	mAlpha = alpha;
}

double OmFilter::GetAlpha()
{
	return mAlpha;
}

OmThreadedCachingTile *OmFilter::GetCache(ViewType viewtype)
{
	OmCachingThreadedCachingTile *fastCache = NULL;
	if (mSeg) {
		fastCache =
		    new OmCachingThreadedCachingTile(viewtype, SEGMENTATION, mSeg, &OmVolume::GetSegmentation(mSeg),
						     NULL);
	} else if (mChannel) {
		fastCache =
		    new OmCachingThreadedCachingTile(viewtype, CHANNEL, mChannel, &OmVolume::GetChannel(mChannel),
						     NULL);
	}

	if (fastCache) {
		mCache = fastCache->mCache;
		if (fastCache->mDelete)
			delete fastCache;
	} else
		return NULL;

	return mCache;
}

void OmFilter::SetSegmentation(OmId id)
{
	if (mSeg != id || 0 == id) {
		mCache = NULL;
	}
	try {
		OmVolume::GetSegmentation(id);
		mSeg = id;
	}
	catch(OmAccessException e) {
		mSeg = 0;
	}
}

void OmFilter::SetChannel(OmId id)
{
	if (mChannel != id) {
		mCache = NULL;
	}
	try {
		OmVolume::GetChannel(id);
		mChannel = id;
	}
	catch(OmAccessException e) {
		mChannel = 0;
	}
}

#pragma mark
#pragma mark Accessor Methods
/////////////////////////////////
///////         Accessor Methods

#pragma mark
#pragma mark Example Methods
/////////////////////////////////
///////         Example Methods

#pragma mark
#pragma mark Print Methods
/////////////////////////////////
///////          Print Methods

void OmFilter::Print()
{
	//debug("FIXME", << "\t\t" << mName << " (" << mId << ")" << endl;
}
