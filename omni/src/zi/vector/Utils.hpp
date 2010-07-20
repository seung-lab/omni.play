#ifndef _VECTOR_UTILS_HPP_
#define _VECTOR_UTILS_HPP_

#include "Vec.hpp"

namespace zi { namespace vector {

template <typename T>
struct RawMat {
  T m_[16];
  operator T* () { return m_; }
  RawMat(const Mat2<T> &m) {
    for (int k=0,i=0; i<2; i++)
      for (int j=0; j<2; j++, k++)
        m_[k] = m[i][j];
  }
  RawMat(const Mat3<T> &m) {
    for (int k=0,i=0; i<3; i++)
      for (int j=0; j<3; j++, k++)
        m_[k] = m[i][j];
  }
  RawMat(const Mat4<T> &m) {
    for (int k=0,i=0; i<4; i++)
      for (int j=0; j<4; j++, k++)
        m_[k] = m[i][j];
  }
};

} }

#endif
