
/*
 *	Standard Serialization Headers
 *
 *	Brett Warne - bwarne@mit.edu - 2/6/09
 */


#ifndef OM_SERIALIZATION_H
#define OM_SERIALIZATION_H




#define OM_ARCHIVE_IN_CLASS boost::archive::binary_iarchive
#define OM_ARCHIVE_OUT_CLASS boost::archive::binary_oarchive


//archives
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

//utilties
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/version.hpp>

//containers
#include <boost/serialization/list.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/map.hpp>








/*
 * Serialize Boost::tuple
 *
 * Macro to generate serialization for tuples of length 1 to 6
 * http://uint32t.blogspot.com/2008/03/update-serializing-boosttuple-using.html
 */

#include <boost/tuple/tuple.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/preprocessor/repetition.hpp>

namespace boost { namespace serialization {
	
#define GENERATE_ELEMENT_SERIALIZE(z,which,unused) \
ar & boost::serialization::make_nvp("element",t.get< which >());
	
#define GENERATE_TUPLE_SERIALIZE(z,nargs,unused)                        \
template< typename Archive, BOOST_PP_ENUM_PARAMS(nargs,typename T) > \
void serialize(Archive & ar,                                        \
boost::tuple< BOOST_PP_ENUM_PARAMS(nargs,T) > & t,   \
const unsigned int version)                          \
{                                                                   \
BOOST_PP_REPEAT_FROM_TO(0,nargs,GENERATE_ELEMENT_SERIALIZE,~);    \
}
	
	
    BOOST_PP_REPEAT_FROM_TO(1,6,GENERATE_TUPLE_SERIALIZE,~);
	
}}






/*
 *	Convenience serialization calls.
 */
#include <fstream>


/*
 *	Templated archive read/in method.
 *	NOTE: Does not check fpath validity first.
 */
template< class T >
inline
void
archive_read(const std::string &fpath, T* t) {
	std::ifstream ifs(fpath.c_str());
	OM_ARCHIVE_IN_CLASS ia(ifs);
	ia >> *t;
}


/*
 *	Templated archive write/out method.
 */
template< class T >
inline
void
archive_write(const std::string &fpath, T* t) {
	std::ofstream ofs(fpath.c_str());
	OM_ARCHIVE_OUT_CLASS oa(ofs);
	oa << *t;
}





#endif
