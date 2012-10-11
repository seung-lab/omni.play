#include <cstddef>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/assume_abstract.hpp>



class serializable_map: public std::map<uint32_t, std::size_t>
{
    friend std::ostream & operator<<(std::ostream &os, const serializable_map &gp);
    friend class boost::serialization::access;

    template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
        std::size_t s = this->size();
        ar & s;

        for ( serializable_map::const_iterator it = this->begin(); it != this->end(); ++it )
        {
            ar & it->first;
            ar & it->second;
        }
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int version)
    {
        std::size_t s;
        ar & s;

        for ( std::size_t i = 0; i < s; ++i )
        {
            int a, b;
            ar & a;
            ar & b;
            this->insert(std::make_pair(a,b));
        }
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()

};

template<class Type>
void save_it(const Type &s, const char * filename)
{
    std::ofstream ofs(filename);
    boost::archive::binary_oarchive oa(ofs);
    oa << s;
}

template<class Type>
void restore_it(Type &s, const char * filename)
{
    s.clear();
    std::ifstream ifs(filename);
    if ( ifs )
    {
        boost::archive::binary_iarchive ia(ifs);
        ia >> s;
    }
}

int main()
{
    serializable_map sm;


    sm[1] = 2;
    sm[2] = 3;
    sm[3] = 4;


    save_it(sm, "data.txt");


    sm.clear();

    restore_it(sm, "data.txt2");

    for ( serializable_map::const_iterator it = sm.begin(); it != sm.end(); ++it )
    {
        std::cout << it->first << ": " << it->second << "\n";
    }

}
