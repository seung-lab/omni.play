#ifndef OM_FILTER2D_H
#define OM_FILTER2D_H

/*
 *	Filter Object
 *	
 *	Matthew Wimer - mwimer@mit.edu - 11/13/09
 */

#include "system/omSystemTypes.h"
#include "system/omManageableObject.h"
#include "view2d/omThreadedCachingTile.h"

#include <vmmlib/vmmlib.h>
#include <vmmlib/serialization.h>
using namespace vmml;


class OmFilter2d : public OmManageableObject {

public:
	OmFilter2d(OmId);
	OmFilter2d();
	
	//accessors
	void SetAlpha(double);
	double GetAlpha();

	OmId GetSegmentation () {
		return mSeg;
	}
	void SetSegmentation (OmId id);
	OmId GetChannel () {
		return mChannel;
	}
	void SetChannel (OmId id);

	OmFilter2d (OmId segid, OmId chanid, OmId filterid);
	
	OmThreadedCachingTile * GetCache (ViewType);
	
private:
	//data members
	double mAlpha;
	OmThreadedCachingTile * mCache;
	OmId mChannel;
	OmId mSeg;
	
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int file_version);
};



/////////////////////////////////
///////		 Serialization


BOOST_CLASS_VERSION(OmFilter2d, 0)

template<class Archive>
void 
OmFilter2d::serialize(Archive & ar, const unsigned int file_version) {
	ar & boost::serialization::base_object<OmManageableObject>(*this);
	
	ar & mAlpha;
	ar & mChannel;
	ar & mSeg;
}


#endif
