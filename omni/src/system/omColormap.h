#ifndef _OM_COLORMAP_H_
#define _OM_COLORMAP_H_

#include <QColor>
#include "segment/omSegmentTypes.h"

class OmColormap : public OmManageableObject {
public:

	void Load (char * file);
	void Save (char * file);
	QColor GetColor (SEGMENT_DATA_TYPE id);
	void SetColor (SEGMENT_DATA_TYPE id, QColor color);

private:
	set <OmColormapperRule> mColormapperRules;

        //serialization
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int file_version);

};


BOOST_CLASS_VERSION(OmColormap, 0)

template<class Archive>
void
OmColormap::serialize(Archive & ar, const unsigned int file_version) {
	ar & mColormapperRules;
}


#endif
