#pragma once

#include "common/omStd.h"

namespace om {
namespace jpeg {

void writeRGB(const uint32_t width, const uint32_t height,
              uint8_t const*const data, const std::string& fileName);

void write8bit(const uint32_t width, const uint32_t height,
               uint8_t const*const data, const std::string& fileName);

} // namespace jpeg
} // namespace om

