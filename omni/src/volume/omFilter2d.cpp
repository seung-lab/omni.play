#include "project/omProject.h"
#include "volume/omFilter2d.h"

#include "volume/omDrawOptions.h"
#include "common/omGl.h"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "volume/omVolume.h"
#include "view2d/omThreadedCachingTile.h"
#include "view2d/omCachingThreadedCachingTile.h"
#include "view2d/omView2d.h"
#include "system/viewGroup/omViewGroupState.h"

/////////////////////////////////
///////
///////		OmFilter Class
///////

OmFilter2d::OmFilter2d() {
	mAlpha = 0.0;
	mSeg = 1;
	mChannel = 0;
}

OmFilter2d::OmFilter2d(OmId omId)
  : OmManageableObject(omId) 
{
	mName = QString("filter%1").arg(omId);
	
	//initially transparent.
	mAlpha = 0.0;
	mSeg = 1;
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

OmThreadedCachingTile * OmFilter2d::GetCache (ViewType viewtype, OmViewGroupState * vgs)
{
	OmCachingThreadedCachingTile *fastCache = NULL;
	if (mSeg) {
		vgs->SetSegmentation(mSeg);
		fastCache = new OmCachingThreadedCachingTile (viewtype, SEGMENTATION, mSeg, &OmProject::GetSegmentation(mSeg), NULL, vgs);

	} else if (mChannel) {
		vgs->SetChannel(mChannel);
		fastCache = new OmCachingThreadedCachingTile (viewtype, CHANNEL, mChannel, &OmProject::GetChannel(mChannel), NULL, vgs);
	}

	if (fastCache) {
		mCache = fastCache->mCache;
		if (fastCache->mDelete) {
			delete fastCache;
		}
	} else {
		return NULL;
	}

	return mCache;
}

OmId OmFilter2d::GetSegmentation () {
	return mSeg;
}

void OmFilter2d::SetSegmentation (OmId id) {

	return;

	if (mSeg != id || 0 == id) {
		mCache = NULL;
	}
	try {
		mSeg = id;
	} catch (OmAccessException e) {
		mSeg = 0;
	}

	//	mViewGroupState->SetSegmentation( SegmentationDataWrapper(mSeg));
}

OmId OmFilter2d::GetChannel () {
	return mChannel;
}

void OmFilter2d::SetChannel (OmId id) {

	return;

	if (mChannel != id) {
		mCache = NULL;
	}
	try {
		OmProject::GetChannel (id);
		mChannel = id;
	} catch (OmAccessException e) {
		mChannel = 0;
	}

	//	mViewGroupState->SetChannel( ChannelDataWrapper( mChannel ));
}
