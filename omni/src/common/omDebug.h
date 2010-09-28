#ifndef _OM_DEBUG_H
#define _OM_DEBUG_H

#include <zi/logging.hpp>

#define debug(x, ...) ZiLOG(DEBUG, x) << zi::log_printf(__VA_ARGS__)

#define DEBUGV3(vec) vec.x, vec.y, vec.z

#endif


