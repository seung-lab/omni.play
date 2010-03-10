#include <boost/thread.hpp>
#include "omSystemInformation.h"
#include "common/omDebug.h"
#include <sys/sysinfo.h>
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

/**
 * may not be Mac OS X compatible (sorry!)
 * from http://stackoverflow.com/questions/349889/how-do-you-determine-the-amount-of-linux-system-ram-in-c
 * http://linux.about.com/library/cmd/blcmdl2_intro.htm
 */
unsigned int OmSystemInformation::get_total_system_memory_megs()
{
	struct sysinfo s_info;
	
	int error = sysinfo(&s_info);
	printf("%s: code error = %d\n", __FUNCTION__, error);
 
	unsigned long system_memory = s_info.totalram;
	return  system_memory / 1024 / 1024;
}

