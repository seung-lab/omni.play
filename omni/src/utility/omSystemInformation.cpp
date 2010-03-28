#include <boost/thread.hpp>
#include "omSystemInformation.h"
#include "common/omDebug.h"
#include <stdio.h>

unsigned int OmSystemInformation::get_num_cores()
{
	const unsigned int boost_num_cores = boost::thread::hardware_concurrency();

	if( boost_num_cores < 1  ||
	    boost_num_cores > 32 ){
		printf("boost did not find correct number of cores (found %d)...\n", boost_num_cores);
		return 2;
	}

	return boost_num_cores;
}



#ifdef __APPLE__



	#include <sys/types.h>
	#include <sys/sysctl.h>
	
	/*
	 * Determine physical RAM in OS X
	 * http://stackoverflow.com/questions/583736/determine-physical-mem-size-programmatically-on-osx
	 * http://developer.apple.com/mac/library/documentation/Darwin/Reference/ManPages/man3/sysctl.3.html
	 */
	unsigned int OmSystemInformation::get_total_system_memory_megs()
	{
		int mib[2];
		int64_t physical_memory;
		size_t length;

		// Get the Physical memory size
		mib[0] = CTL_HW;
		mib[1] = HW_MEMSIZE;
		length = sizeof(int64_t);
		sysctl(mib, 2, &physical_memory, &length, NULL, 0);

		// Convert to megabites
		return physical_memory / ( 1024 * 1024 );
	}



#else


	#include <sys/sysinfo.h>

	/**
	 * Determine physical RAM in Linux
	 * from http://stackoverflow.com/questions/349889/how-do-you-determine-the-amount-of-linux-system-ram-in-c
	 * http://linux.about.com/library/cmd/blcmdl2_intro.htm
	 */
	unsigned int OmSystemInformation::get_total_system_memory_megs()
	{
		struct sysinfo s_info;
		
		int error = sysinfo(&s_info);
		if( 0 != error ){
			printf("%s: code error = %d\n", __FUNCTION__, error);
		}
	 
		unsigned long system_memory = s_info.totalram;
		return  system_memory / 1024 / 1024;
	}




#endif



