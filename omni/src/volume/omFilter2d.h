#ifndef OM_FILTER2D_H
#define OM_FILTER2D_H

/*
 *	Filter Object
 *
 *	Matthew Wimer - mwimer@mit.edu - 11/13/09
 */

#include "system/omManageableObject.h"

class OmMipVolume;

class OmFilter2d : public OmManageableObject {

public:
	OmFilter2d();
	OmFilter2d(OmId);

	std::string GetName(){
		return "filter" + boost::lexical_cast<std::string>(GetId());
	}

	void SetAlpha(const double);
	double GetAlpha();

	OmId GetSegmentation();
	void SetSegmentation(const OmId id);

	OmId GetChannel();
	void SetChannel(const OmId id);

	bool setupVol();
	OmMipVolume* getVolume(){ return vol_; }

private:
	double mAlpha;
	OmId mChannel;
	OmId mSeg;
	OmMipVolume* vol_;

	friend QDataStream &operator<<(QDataStream&, const OmFilter2d&);
	friend QDataStream &operator>>(QDataStream&, OmFilter2d&);
};

#endif
