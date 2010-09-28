#ifndef _OM_DEBUG_H_
#define _OM_DEBUG_H_

#include <zi/logging.hpp>

#ifdef NDEBUG
# define debug(x, ...) ;
# else
# define debug(x, ...) ZiLOG(DEBUG, x) << zi::log_printf( __VA_ARGS__)
#endif

#define DEBUGV3(vec) vec.x, vec.y, vec.z

#endif


