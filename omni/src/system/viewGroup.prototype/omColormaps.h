/// @cond DOXYGEN_IGNORE

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

};


	//        ar & mGenericColormapManager;



#endif

/// @endcond
