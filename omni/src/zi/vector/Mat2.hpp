#ifndef _VECTOR_MAT2_HPP_
#define _VECTOR_MAT2_HPP_

#include "Common.hpp"

namespace zi { namespace vector {

#define Mat2f Mat2<float>  /* */
#define Mat2d Mat2<double> /* */
#define Mat2i Mat2<int>    /* */

template <typename T>
class Mat2 {
public:

  Mat2();
  Mat2(const T &a, const T &b, const T &c, const T &d);
  Mat2(const Vec2<T> &a, const Vec2<T> &b);
  Mat2(const Mat2 &m);

  Vec2<T>        &operator[] (int i);
  const Vec2<T>  &operator[] (int i) const;

  operator T** () { return m_; }

  // Unary operators
  Mat2 &operator += (const Mat2 &m);
  Mat2 &operator -= (const Mat2 &m);
  Mat2 &operator *= (const Mat2 &m);
  Mat2 &operator /= (const Mat2 &m);

  Mat2 &operator += (const T &x);
  Mat2 &operator -= (const T &x);
  Mat2 &operator *= (const T &x);
  Mat2 &operator /= (const T &x);

  // Comparison
  bool operator == (const Mat2 &m) const;
  bool operator != (const Mat2 &m) const;

  // Arithmetic
  Mat2 operator + (const Mat2 &m) const;
  Mat2 operator - (const Mat2 &m) const;
  Mat2 operator * (const Mat2 &m) const;
  Mat2 operator / (const Mat2 &m) const;

  Mat2 operator + (const T &x) const;
  Mat2 operator - (const T &x) const;
  Mat2 operator * (const T &x) const;
  Mat2 operator / (const T &x) const;

  Mat2 operator - () const;

  // Assignment
  Mat2 &operator = (const Mat2 &m);
  Mat2 &operator = (const T &x);

  static Mat2 ZERO       ();
  static Mat2 IDENTITY   (T x = (T)1);
  static Mat2 FILL       (T x = (T)1);

protected:
  Vec2<T> m_[2];
};

template <typename T>        Mat2<T> inv   (const Mat2<T> &m);
template <typename T> inline Mat2<T> adj   (const Mat2<T> &m);
template <typename T> inline Mat2<T> trans (const Mat2<T> &m);
template <typename T> inline T       trace (const Mat2<T> &m);
template <typename T> inline T       det   (const Mat2<T> &m);

template <typename T> std::ostream &operator << (std::ostream &out, const Mat2<T> &m);
template <typename T> std::istream &operator >> (std::istream &in, Mat2<T> &m);

template <typename T, typename C> Mat2<T> operator + (const C &x, const Mat2<T> &m);
template <typename T, typename C> Mat2<T> operator - (const C &x, const Mat2<T> &m);
template <typename T, typename C> Mat2<T> operator * (const C &x, const Mat2<T> &m);
template <typename T, typename C> Mat2<T> operator / (const C &x, const Mat2<T> &m);

template <typename T> Vec2<T> operator * (const Vec2<T> &v, const Mat2<T> &m);
template <typename T> Vec2<T> operator / (const Vec2<T> &v, const Mat2<T> &m);

// Constructors
// ----------------------------------------------------------------------------
template <typename T>
Mat2<T>::Mat2() {}

template <typename T>
Mat2<T>::Mat2(const T &a, const T &b, const T &c, const T &d)
{
  m_[0][0] = a; m_[0][1] = b;
  m_[1][0] = c; m_[1][1] = d;
}

template <typename T>
Mat2<T>::Mat2(const Vec2<T> &a, const Vec2<T> &b)
{
  m_[0] = a;
  m_[1] = b;
}

template <typename T>
Mat2<T>::Mat2(const Mat2 &m)
{
  m_[0] = m[0];
  m_[1] = m[1];
}

// Access
// ----------------------------------------------------------------------------
template <typename T>
inline Vec2<T> &Mat2<T>::operator[] (int i)
{
  return m_[i];
}

template <typename T>
inline const Vec2<T> &Mat2<T>::operator[] (int i) const
{
  return m_[i];
}

// Unary operators
// ----------------------------------------------------------------------------
template <typename T>
inline Mat2<T> &Mat2<T>::operator += (const Mat2 &m)
{
  m_[0] += m[0];
  m_[1] += m[1];
  return (*this);
}

template <typename T>
inline Mat2<T> &Mat2<T>::operator -= (const Mat2 &m)
{
  m_[0] -= m[0];
  m_[1] -= m[1];
  return (*this);
}

template <typename T>
inline Mat2<T> &Mat2<T>::operator *= (const Mat2 &m)
{
  (*this) = (*this) * m;
  return (*this);
}

template <typename T>
inline Mat2<T> &Mat2<T>::operator /= (const Mat2 &m)
{
  (*this) = (*this) * inv(m);
  return (*this);
}

template <typename T>
inline Mat2<T> &Mat2<T>::operator += (const T &x)
{
  m_[0] += x;
  m_[1] += x;
  return (*this);
}

template <typename T>
inline Mat2<T> &Mat2<T>::operator -= (const T &x)
{
  m_[0] -= x;
  m_[1] -= x;
  return (*this);
}

template <typename T>
inline Mat2<T> &Mat2<T>::operator *= (const T &x)
{
  m_[0] *= x;
  m_[1] *= x;
  return (*this);
}

template <typename T>
inline Mat2<T> &Mat2<T>::operator /= (const T &x)
{
  m_[0] /= x;
  m_[1] /= x;
  return (*this);
}

// Comparison
// ----------------------------------------------------------------------------
template <typename T>
inline bool Mat2<T>::operator == (const Mat2 &m) const
{
  return (m_[0] == m[0] && m_[1] == m[1]);
}

template <typename T>
inline bool Mat2<T>::operator != (const Mat2 &m) const
{
  return (m_[0] != m[0] || m_[1] != m[1]);
}

// Binary Operators With MAT
// ----------------------------------------------------------------------------
template <typename T>
inline Mat2<T> Mat2<T>::operator + (const Mat2 &m) const
{
  Mat2<T> r;
  r[0] = m_[0] + m[0];
  r[1] = m_[1] + m[1];
  return r;
}

template <typename T>
inline Mat2<T> Mat2<T>::operator - (const Mat2 &m) const
{
  Mat2<T> r;
  r[0] = m_[0] * m[0];
  r[1] = m_[1] * m[1];
  return r;
}

template <typename T>
inline Mat2<T> Mat2<T>::operator * (const Mat2 &m) const
{
  Mat2<T> r;
  r[0][0] = m_[0][0] * m.m_[0][0] + m_[0][1] * m.m_[1][0];
  r[0][1] = m_[0][0] * m.m_[0][1] + m_[0][1] * m.m_[1][1];
  r[1][0] = m_[1][0] * m.m_[0][0] + m_[1][1] * m.m_[1][0];
  r[1][1] = m_[1][0] * m.m_[0][1] + m_[1][1] * m.m_[1][1];
  return r;
}

template <typename T>
inline Mat2<T> Mat2<T>::operator / (const Mat2 &m) const
{
  Mat2<T> r = (*this) * inv(m);
  return r;
}

// Binary Operators With Scalar
// ----------------------------------------------------------------------------
template <typename T>
inline Mat2<T> Mat2<T>::operator + (const T &x) const
{
  Mat2<T> r;
  r[0] = m_[0] + x;
  r[1] = m_[1] + x;
  return r;
}

template <typename T>
inline Mat2<T> Mat2<T>::operator - (const T &x) const
{
  Mat2<T> r;
  r[0] = m_[0] - x;
  r[1] = m_[1] - x;
  return r;
}

template <typename T>
inline Mat2<T> Mat2<T>::operator * (const T &x) const
{
  Mat2<T> r;
  r[0] = m_[0] * x;
  r[1] = m_[1] * x;
  return r;
}

template <typename T>
inline Mat2<T> Mat2<T>::operator / (const T &x) const
{
  Mat2<T> r;
  r[0] = m_[0] / x;
  r[1] = m_[1] / x;
  return r;
}

// Negate
// ----------------------------------------------------------------------------
template <typename T>
inline Mat2<T> Mat2<T>::operator - () const
{
  Mat2<T> r;
  r[0] = -m_[0];
  r[1] = -m_[1];
  return r;
}

// Assignment
// ----------------------------------------------------------------------------
template <typename T>
inline Mat2<T> &Mat2<T>::operator = (const Mat2 &m)
{
  m_[0] = m[0];
  m_[1] = m[1];
  return (*this);
}

template <typename T>
inline Mat2<T> &Mat2<T>::operator = (const T &x)
{
  m_[0] = x;
  m_[1] = x;
  return (*this);
}

// Friend Operators with Scalar
// ----------------------------------------------------------------------------
template <typename T, typename C>
inline Mat2<T> operator + (const C &x, const Mat2<T> &m)
{
  Mat2<T> r;
  r[0] = (T)x + m[0];
  r[1] = (T)x + m[1];
  return r;
}

template <typename T, typename C>
inline Mat2<T> operator - (const C &x, const Mat2<T> &m)
{
  Mat2<T> r;
  r[0] = (T)x - m[0];
  r[1] = (T)x - m[1];
  return r;
}

template <typename T, typename C>
inline Mat2<T> operator * (const C &x, const Mat2<T> &m)
{
  Mat2<T> r;
  r[0] = (T)x * m[0];
  r[1] = (T)x * m[1];
  return r;
}

template <typename T, typename C>
inline Mat2<T> operator / (const C &x, const Mat2<T> &m)
{
  Mat2<T> r;
  r[0] = (T)x / m[0];
  r[1] = (T)x / m[1];
  return r;
}

// Friend Operators with Vec
// ----------------------------------------------------------------------------
template <typename T>
Vec2<T> operator * (const Vec2<T> &v, const Mat2<T> &m)
{
  Vec2<T> r;
  r[0] = m[0][0] * v[0] + m[0][1] * v[1];
  r[1] = m[1][0] * v[0] + m[1][1] * v[1];
  return r;
}

template <typename T>
Vec2<T> operator / (const Vec2<T> &v, const Mat2<T> &m)
{
  return v * inv(m);
}


// Matrix Operations
// ----------------------------------------------------------------------------
template <typename T> Mat2<T> inv(const Mat2<T> &m)
{
  T det;
  Mat2<T> r;

  r[0][0] =  m[1][1]; r[0][1] = -m[0][1];
  r[1][0] = -m[1][0]; r[1][1] =  m[0][0];

  det = m[0][0] * r[0][0] + m[0][1] * r[1][0];
  // TODO: zero ?
  r /= det;
  return r;
}

template <typename T> inline Mat2<T> adj(const Mat2<T> &m)
{
  Mat2<T> r;
  r[0][0] =  m[1][1]; r[0][1] = -m[1][0];
  r[1][0] = -m[0][1]; r[1][1] =  m[0][0];
  return r;
}

template <typename T> inline Mat2<T> trans(const Mat2<T> &m)
{
  Mat2<T> r;
  r[0][0] = m[0][0]; r[0][1] = m[1][0];
  r[1][0] = m[0][1]; r[1][1] = m[1][1];
  return r;
}

template <typename T> inline T trace(const Mat2<T> &m)
{
  return (m[0][0] + m[1][1]);
}

template <typename T> inline T det(const Mat2<T> &m)
{
  return (m[0][0] * m[1][1] - m[1][0] * m[0][1]);
}

// Friend Stream Operations
// ----------------------------------------------------------------------------
template <typename T>
std::ostream &operator << (std::ostream &out, const Mat2<T> &m)
{
  out << "["   << m[0]
      << "\n " << m[1] << "]";
  return out;
}

template <typename T>
std::istream &operator >> (std::istream &in, Mat2<T> &m)
{
  in >> m[0]
     >> m[1];
  return in;
}

// Static Producers
// ----------------------------------------------------------------------------
template <typename T>
Mat2<T> Mat2<T>::ZERO()
{
  Mat2<T> r;
  r[0] = 0;
  r[1] = 0;
  return r;
}

template <typename T>
Mat2<T> Mat2<T>::IDENTITY(T x)
{
  Mat2<T> r;
  r[0][0] = x; r[0][1] = 0;
  r[1][0] = 0; r[1][1] = x;
  return r;
}

template <typename T>
Mat2<T> Mat2<T>::FILL(T x)
{
  Mat2<T> r;
  r[0][0] = x; r[0][1] = x;
  r[1][0] = x; r[1][1] = x;
  return r;
}

} }

#endif
