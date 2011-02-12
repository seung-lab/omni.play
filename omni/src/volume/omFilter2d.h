#ifndef OM_FILTER2D_H
#define OM_FILTER2D_H

/*
 *	Filter Object
 *
 *	Matthew Wimer - mwimer@mit.edu - 11/13/09
 */

#include "system/omManageableObject.h"
#include "utility/channelDataWrapper.hpp"
#include "utility/segmentationDataWrapper.hpp"

class OmMipVolume;

class OmFilter2d : public OmManageableObject {
public:
	OmFilter2d();
	OmFilter2d(const OmID);

	std::string GetName(){
		return "filter" + om::NumToStr(GetID());
	}

	void SetAlpha(const double alpha){
		mAlpha = alpha;
	}
	double GetAlpha(){
		return mAlpha;
	}

	void SetSegmentation(const OmID id);
	const SegmentationDataWrapper& GetSegmentationWrapper() const{
		return sdw_;
	}

	void SetChannel(const OmID id);
	const ChannelDataWrapper& GetChannelDataWrapper() const{
		return cdw_;
	}

	bool HasValidVol();
	OmMipVolume* GetMipVolume();

private:
	double mAlpha;

	ChannelDataWrapper cdw_;
	SegmentationDataWrapper sdw_;

	friend QDataStream &operator<<(QDataStream&, const OmFilter2d&);
	friend QDataStream &operator>>(QDataStream&, OmFilter2d&);
};

#endif
