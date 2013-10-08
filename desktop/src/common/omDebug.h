#pragma once

#include <zi/logging.hpp>

#define debugs(x) ZiLOG(DEBUG, x)
#define debug(x, ...) ZiLOG(DEBUG, x) << zi::log_printf(__VA_ARGS__)

#define DEBUGV3(vec) vec.x, vec.y, vec.z
