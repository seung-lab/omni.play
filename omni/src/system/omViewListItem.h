#ifndef _OM_VIEWLISTITEM_H_
#define _OM_VIEWLISTITEM_H_

#include "omLayer.h"
#include "omGenericManager.h"

/**
 *  The view list item holds the layers for that specific view.
 *  
 *  Matt Wimer mwimer@mit.edu  1/26/2010
 */

class OmViewListItem : public OmManageableObject {

public:

	OmLayer& AddLayer ();
	void RemoveLayer (OmId layer);
	void RemoveLayer (OmLayer& layer);
	set<OmLayer> GetLayers ();

	void MoveLayerUp (OmId layer);
	void MoveLayerUp (OmLayer& layer);
	void MoveLayerDown (OmId layer);
	void MoveLayerDown (OmLayer& layer);

	int GetLayerCount ();

private:
	OmGenericManager <OmLayer> mLayerManager;

        //serialization
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int file_version);
};



BOOST_CLASS_VERSION(OmViewListItem, 0)

template<class Archive>
void
OmViewListItem::serialize(Archive & ar, const unsigned int file_version) {
        ar & mLayerManager;
}


#endif
