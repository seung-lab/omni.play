#ifndef _OM_LAYER_H_
#define _OM_LAYER_H_

#include "omManageableObject.h"
#include "omColormap.h"

/**
 * These layers are similar to the layers found in your commmon paint
 * programs like photoshop or gimp.  
 *
 * Matt Wimer  mwimer@mit.edu  1/26/2010
 */

class OmLayer : public OmManageableObject {

public:
	void SetSourceVolume (OmId channel, OmId segmenatation);
	void SetFilter ();
	void SetColormap (OmId colormap);
	void SetColormap (OmColormap& colormap);
	OmColormap& GetColormap ();

	void SetOpacity (float opacity);
	float GetOpacity ();

	void SetVisible (bool visible);
	bool IsVisible ();

	void SetEditable (bool editable);
	bool IsEditable ();

	//void SetListPosition (int loc);
	//int GetListPosition ();

private:
	bool mIsVisible;
	float mOpacity;
	bool mIsEditable;
	int mListPosition;
	OmId mColormap;
	OmId mFilter;
	OmId mSourceChannel;
	OmId mSourceSegmentation;

        //serialization
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int file_version);
};

BOOST_CLASS_VERSION(OmLayer, 0)

template<class Archive>
void
OmLayer::serialize(Archive & ar, const unsigned int file_version) {

        ar & mIsVisible;
        ar & mOpacity;
        ar & mIsEditable;
        ar & mListPosition;
        ar & mColormap;
        ar & mFilter;
	ar & mSourceChannel;
	ar & mSourceSegmentation;
}


#endif

