#include "utility/omSystemInformation.h"
#include <zi/system>

unsigned int OmSystemInformation::get_num_cores()
{
	return zi::System::getCPUCount();
}

unsigned int OmSystemInformation::get_total_system_memory_megs()
{
	return zi::System::getTotalMB();
}
