#include "project/omProject.h"
#include "volume/omFilter2d.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"

OmFilter2d::OmFilter2d()
	: mAlpha(0.0)
	, mChannel(0)
	, mSeg(1)
	, vol_(NULL) {}

OmFilter2d::OmFilter2d(OmId omId)
	: OmManageableObject(omId)
	, mAlpha(0.0)
	, mChannel(0)
	, mSeg(1)
	, vol_(NULL) {}

void OmFilter2d::SetAlpha(const double alpha){
	mAlpha = alpha;
}

double OmFilter2d::GetAlpha(){
	return mAlpha;
}

OmId OmFilter2d::GetSegmentation() {
	return mSeg;
}

void OmFilter2d::SetSegmentation(const OmId id)
{
	if(OmProject::IsSegmentationValid(id)){
		mSeg = id;
	} else{
		mSeg = 0;
	}
}

OmId OmFilter2d::GetChannel(){
	return mChannel;
}

void OmFilter2d::SetChannel(const OmId id)
{
	if(OmProject::IsChannelValid(id)){
		mChannel = id;
	} else{
		mChannel = 0;
	}
}

bool OmFilter2d::setupVol()
{
	if (OmProject::IsSegmentationValid(mSeg)) {
		vol_ = &OmProject::GetSegmentation(mSeg);
		return true;

	}

	if (OmProject::IsChannelValid(mChannel)) {
		vol_ = &OmProject::GetChannel(mChannel);
		return true;
	}

	return false;
}
