#ifndef OM_SYSTEM_INFORMATION_H
#define OM_SYSTEM_INFORMATION_H

class OmSystemInformation 
{
 public:
	static unsigned int get_num_cores();
	static unsigned int get_total_system_memory_megs();
};
#endif
