#pragma once

#include "common/common.h"

#include <cstddef>
#include <cstdio>

extern "C"
{
#include <jpeglib.h>
#include <jerror.h>
}


namespace om {
namespace jpeg {

void writeRGB(const uint32_t width, const uint32_t height,
              uint8_t const*const data, std::vector<JOCTET>& out_buffer);

void write8bit(const uint32_t width, const uint32_t height,
               uint8_t const*const data, std::vector<JOCTET>& out_buffer);

} // namespace jpeg
} // namespace om

