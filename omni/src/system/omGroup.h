#ifndef _OM_GROUP_H_
#define _OM_GROUP_H_

class OmGroup : public OmManagableObject {
public:


private:

        //serialization
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int file_version);

};


BOOST_CLASS_VERSION(OmGroup, 0)

template<class Archive>
void
OmGroup::serialize(Archive & ar, const unsigned int file_version) {

}


#endif
