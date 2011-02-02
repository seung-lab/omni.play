#include "project/omProjectGlobals.h"
#include "utility/omRandColorFile.hpp"
#include "view2d/omView2dManagerImpl.hpp"

OmProjectGlobals::OmProjectGlobals()
	: randColorFile_(new OmRandColorFile())
	, v2dManagerImpl_(new OmView2dManagerImpl())
{
	randColorFile_->createOrLoad();
}

OmProjectGlobals::~OmProjectGlobals()
{}
