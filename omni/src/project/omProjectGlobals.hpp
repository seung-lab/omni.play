#ifndef OM_PROJECT_GLOBALS_HPP
#define OM_PROJECT_GLOBALS_HPP

#include "utility/omRandColorFile.hpp"

class OmProjectImpl;

class OmProjectGlobals {
private:
	OmProjectImpl* project_;

	OmRandColorFile randColorFile_;

public:
	OmProjectGlobals(OmProjectImpl* project)
		: project_(project)
	{
		randColorFile_.createOrLoad();
	}

	OmRandColorFile& RandColorFile(){
		return randColorFile_;
	}
};

#endif
