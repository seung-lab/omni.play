#ifndef _OM_COLORMAPS_H_
#define _OM_COLORMAPS_H_

#include "omColormap.h"

/**
 * 
 */

class OmColormaps : OmManagableObject {
public:
        static OmColormaps* Instance();
        static void Delete();


protected:
        // singleton constructor, copy constructor, assignment operator protected
        OmColormaps();
        ~OmColormaps();
        OmColormaps(const OmColormaps&);
        OmColormaps& operator= (const OmColormaps&);


private:
        //singleton
        static OmColormaps* mspInstance;

        OmGenericManager< OmColormap > mGenericColormapManager;

        //serialization
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int file_version);
};


BOOST_CLASS_VERSION(OmColormaps, 0)

template<class Archive>
void
OmColormaps::serialize(Archive & ar, const unsigned int file_version) {

        ar & mGenericColormapManager;
}


#endif
