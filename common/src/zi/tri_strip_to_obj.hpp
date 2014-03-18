#pragma once
#ifndef TRI_STRIP_TO_OBJ_HPP
#define TRI_STRIP_TO_OBJ_HPP 1

#include "precomp.h"

namespace zi {

template <typename float_type, typename index_type>
void tri_strip_to_obj(const float_type* points, const std::size_t points_length,
                      const index_type* indices,
                      const std::size_t indices_length,
                      const index_type* strips,
                      const std::size_t strips_length) {
  for (auto i = 0; i < points_length; i += 6) {
    std::cout << "v " << points[i] << ' ' << points[i + 1] << ' '
              << points[i + 2] << '\n';
  }

  for (auto i = 3; i < points_length; i += 6) {
    std::cout << "n " << points[i] << ' ' << points[i + 1] << ' '
              << points[i + 2] << '\n';
  }

  for (auto i = 0; i < strips_length; i += 2) {
    bool even = true;
    for (auto j = strips[i]; j < strips[i + 1] - 1; ++j) {
      if (even) {
        std::cout << "f " << indices[j] << ' ' << indices[j + 1] << ' '
                  << indices[j + 1] << '\n';
      } else {
        std::cout << "f " << indices[j + 2] << ' ' << indices[j + 1] << ' '
                  << indices[j] << '\n';
      }
      even = !even;
    }
  }
}

}  // namesapce zi

#endif
