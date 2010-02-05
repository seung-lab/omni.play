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

	static OmId AddView ();
	static void RemoveView (OmId view);
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

	OmGenericManager< OmViewListItem > mGenericViewListManager;
        
        //serialization
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int file_version);
};


BOOST_CLASS_VERSION(OmViewList, 0)

template<class Archive>
void 
OmViewList::serialize(Archive & ar, const unsigned int file_version) {
        
        ar & mGenericViewListManager;
}


#endif
