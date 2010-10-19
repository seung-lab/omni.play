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
	OmFilter2d(OmID);

	std::string GetName(){
		return "filter" + boost::lexical_cast<std::string>(GetID());
	}

	void SetAlpha(const double);
	double GetAlpha();

	OmID GetSegmentation();
	void SetSegmentation(const OmID id);

	OmID GetChannel();
	void SetChannel(const OmID id);

	bool setupVol();
	OmMipVolume* getVolume(){ return vol_; }

private:
	double mAlpha;
	OmID mChannel;
	OmID mSeg;
	OmMipVolume* vol_;

	friend QDataStream &operator<<(QDataStream&, const OmFilter2d&);
	friend QDataStream &operator>>(QDataStream&, OmFilter2d&);
};

#endif
