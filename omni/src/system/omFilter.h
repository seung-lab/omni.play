#ifndef OM_FILTER_H
#define OM_FILTER_H

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


class OmFilter : public OmManageableObject {

public:
	OmFilter(OmId);
	OmFilter();
	
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

	OmFilter (OmId segid, OmId chanid, OmId filterid);
	
	OmThreadedCachingTile * GetCache (ViewType);
	
	void Print();
	
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



#pragma mark 
#pragma mark Serialization
/////////////////////////////////
///////		 Serialization


BOOST_CLASS_VERSION(OmFilter, 0)

template<class Archive>
void 
OmFilter::serialize(Archive & ar, const unsigned int file_version) {
	ar & boost::serialization::base_object<OmManageableObject>(*this);
	
	ar & mAlpha;
	ar & mChannel;
	ar & mSeg;
}


#endif
