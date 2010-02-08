#include <boost/thread.hpp>
#include "omNumCores.h"
#include "omDebug.h"

unsigned int OmNumCores::get_num_cores()
{
	const unsigned int boost_num_cores = boost::thread::hardware_concurrency();

	if( boost_num_cores < 1  ||
	    boost_num_cores > 32 ){
		assert("boost did not find correct number of cores...\n");
		return 2;
	}

	return boost_num_cores;
}
