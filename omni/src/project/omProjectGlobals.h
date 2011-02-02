#ifndef OM_PROJECT_GLOBALS_HPP
#define OM_PROJECT_GLOBALS_HPP

#include <boost/scoped_ptr.hpp>

class OmRandColorFile;
class OmView2dManagerImpl;

class OmProjectGlobals {
private:
	boost::scoped_ptr<OmRandColorFile> randColorFile_;
	boost::scoped_ptr<OmView2dManagerImpl> v2dManagerImpl_;

public:
	OmProjectGlobals();
	~OmProjectGlobals();

	inline OmRandColorFile& RandColorFile(){
		return *randColorFile_;
	}

	inline OmView2dManagerImpl& View2dManagerImpl(){
		return *v2dManagerImpl_;
	}
};

#endif
