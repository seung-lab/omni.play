#pragma once

#include "project/omProject.h"
#include "datalayer/fs/omFile.hpp"

namespace om {
namespace test {

class tempProject
{
private:
	std::string fnp_;
public:
	tempProject()
		: fnp_("/tmp/" + OmUUID().Str())
	{
		OmProject::New(fnp_);
	}
	~tempProject()
	{
		
	}

	/* data */
};

}} // namespace om::test::
