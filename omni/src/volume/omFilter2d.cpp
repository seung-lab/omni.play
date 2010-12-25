#include "volume/omFilter2d.h"

OmFilter2d::OmFilter2d()
	: mAlpha(0.0)
	, sdw_(SegmentationDataWrapper(1))
{}

OmFilter2d::OmFilter2d(const OmID omId)
	: OmManageableObject(omId)
	, mAlpha(0.0)
	, sdw_(SegmentationDataWrapper(1))
{}

void OmFilter2d::SetSegmentation(const OmID id){
	sdw_ = SegmentationDataWrapper(id);
}

void OmFilter2d::SetChannel(const OmID id){
	cdw_ = ChannelDataWrapper(id);
}

bool OmFilter2d::HasValidVol()
{
	if(sdw_.IsSegmentationValid()){
		return true;
	}

	if(cdw_.IsChannelValid()){
		return true;
	}

	return false;
}

OmMipVolume* OmFilter2d::GetMipVolume()
{
	if(sdw_.IsSegmentationValid()){
		return sdw_.GetSegmentationPtr();
	}

	if(cdw_.IsChannelValid()){
		return cdw_.GetChannelPtr();
	}

	return NULL;
}
