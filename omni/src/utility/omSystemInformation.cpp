#include "omSystemInformation.h"
#include "common/omDebug.h"
#include <stdio.h>
#include "src/zi/base/system.h"

unsigned int OmSystemInformation::get_num_cores()
{
	return zi::System::getCPUCount();
}

#ifdef __APPLE__
	#include <sys/types.h>
	#include <sys/sysctl.h>
	
	/**
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
		return physical_memory / 1024 / 1024;
	}
#elif WIN32
	
	unsigned int OmSystemInformation::get_total_system_memory_megs()
	{
		return 5000;
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
			printf("%s: getting sysinfo struct: code error = %d\n", __FUNCTION__, error);
		}
	 
		unsigned long system_memory_bytes = s_info.totalram;
		return system_memory_bytes / 1024 / 1024;
	}
#endif
