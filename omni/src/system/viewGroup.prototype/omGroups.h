/// @cond DOXYGEN_IGNORE

#ifndef _OM_GROUPS_H_
#define _OM_GROUPS_H_

class OmGroups : OmManagableObject {
public:
        static OmGroups* Instance();
        static void Delete();

protected:
        // singleton constructor, copy constructor, assignment operator protected
        OmGroups();
        ~OmGroups();
        OmGroups(const OmGroups&);
        OmGroups& operator= (const OmGroups&);


private:
        //singleton
        static OmGroups* mspInstance;

        OmGenericManager< OmGroup > mGenericGroupManager;

};

//        ar & mGenericGroupManager;



#endif

/// @endcond
