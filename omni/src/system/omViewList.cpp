/**
 * The list of views into the image stacks.
 *
 * Matt Wimer mwimer@mit.edu 1/27/2010
 */

#include "omViewList.h"


OmViewList* OmViewList::mspInstance = 0;

OmViewList::OmViewList() {
}

OmViewList::~OmViewList() {

}

OmViewList*
OmViewList::Instance() {
        if(NULL == mspInstance) {
                mspInstance = new OmViewList;
        }
        return mspInstance;
}

void
OmViewList::Delete() {
        if(mspInstance) delete mspInstance;
        mspInstance = NULL;
}


static OmId AddView ()
{
}

static void RemoveView (OmId view)
{
}

static set<OmId> GetViews ()
{
}

