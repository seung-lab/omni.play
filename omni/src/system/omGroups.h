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

        //serialization
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int file_version);
};


BOOST_CLASS_VERSION(OmGroups, 0)

template<class Archive>
void
OmGroups::serialize(Archive & ar, const unsigned int file_version) {

        ar & mGenericGroupManager;
}


#endif
