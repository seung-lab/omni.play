#pragma once

#include "common/common.h"

namespace om {
namespace jpeg {

void writeRGB(const uint32_t width, const uint32_t height,
              uint8_t const*const data, const std::string& fnp);

void write8bit(const uint32_t width, const uint32_t height,
               uint8_t const*const data, const std::string& fnp);

} // namespace jpeg
} // namespace om

