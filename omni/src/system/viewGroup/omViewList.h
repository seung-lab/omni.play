/// @cond DOXYGEN_IGNORE

#ifndef _OM_VIEWLIST_H__
#define _OM_VIEWLIST_H__

#include "omViewListItem.h"
#include "omGenericManager.h"

/**
 *      OmViewList is the list of the default and user defined views in the project.
 *
 *      Matt Wimer - mwimer@mit.edu - 1/26/2010
 */


class OmViewList : public OmManageableObject {

public:
        static OmViewList* Instance();
        static void Delete();

	static OmId AddBinder ();
	static void RemoveBinder (OmId view);
	static set<OmId> GetViews ();

protected:
        // singleton constructor, copy constructor, assignment operator protected
        OmViewList();
        ~OmViewList();
        OmViewList(const OmViewList&);
        OmViewList& operator= (const OmViewList&);
        

private:
        //singleton
        static OmViewList* mspInstance;

	OmGenericManager<OmViewListBinder> mGenericViewListManager;
};

//        ar & mGenericViewListManager;



#endif

/// @endcond
