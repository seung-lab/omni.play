#ifndef _VECTOR_MAT3_HPP_
#define _VECTOR_MAT3_HPP_

#include "Common.hpp"

namespace zi { namespace vector {

template <typename T>
class Mat3 {
public:

  Mat3();
  Mat3(const T &a, const T &b, const T &c,
       const T &d, const T &e, const T &f,
       const T &g, const T &h, const T &i);
  Mat3(const Vec3<T> &a, const Vec3<T> &b, const Vec3<T> &c);
  Mat3(const Mat3 &m);

  Vec3<T>        &operator[] (int i);
  const Vec3<T>  &operator[] (int i) const;

  operator T** () { return m_; }

  // Unary operators
  Mat3 &operator += (const Mat3 &m);
  Mat3 &operator -= (const Mat3 &m);
  Mat3 &operator *= (const Mat3 &m);
  Mat3 &operator /= (const Mat3 &m);

  Mat3 &operator += (const T &x);
  Mat3 &operator -= (const T &x);
  Mat3 &operator *= (const T &x);
  Mat3 &operator /= (const T &x);

  // Comparison
  bool operator == (const Mat3 &m) const;
  bool operator != (const Mat3 &m) const;

  // Arithmetic
  Mat3 operator + (const Mat3 &m) const;
  Mat3 operator - (const Mat3 &m) const;
  Mat3 operator * (const Mat3 &m) const;
  Mat3 operator / (const Mat3 &m) const;

  Mat3 operator + (const T &x) const;
  Mat3 operator - (const T &x) const;
  Mat3 operator * (const T &x) const;
  Mat3 operator / (const T &x) const;

  Mat3 operator - () const;

  // Assignment
  Mat3 &operator = (const Mat3 &m);
  Mat3 &operator = (const T &x);

  static Mat3 ZERO       ();
  static Mat3 IDENTITY   (T x = (T)1);
  static Mat3 FILL       (T x = (T)1);

  void normalize();

protected:
  Vec3<T> m_[3];
};

template <typename T>        Mat3<T> inv   (const Mat3<T> &m);
template <typename T> inline Mat3<T> adj   (const Mat3<T> &m);
template <typename T> inline Mat3<T> trans (const Mat3<T> &m);
template <typename T> inline T       trace (const Mat3<T> &m);
template <typename T> inline T       det   (const Mat3<T> &m);

template <typename T> std::ostream &operator << (std::ostream &out, const Mat3<T> &m);
template <typename T> std::istream &operator >> (std::istream &in, Mat3<T> &m);

template <typename T, typename C> Mat3<T> operator + (const C &x, const Mat3<T> &m);
template <typename T, typename C> Mat3<T> operator - (const C &x, const Mat3<T> &m);
template <typename T, typename C> Mat3<T> operator * (const C &x, const Mat3<T> &m);
template <typename T, typename C> Mat3<T> operator / (const C &x, const Mat3<T> &m);

template <typename T> Vec3<T> operator * (const Vec3<T> &v, const Mat3<T> &m);
template <typename T> Vec3<T> operator / (const Vec3<T> &v, const Mat3<T> &m);

// Constructors
// ----------------------------------------------------------------------------
template <typename T>
Mat3<T>::Mat3() {}

template <typename T>
Mat3<T>::Mat3(const T &a, const T &b, const T &c,
              const T &d, const T &e, const T &f,
              const T &g, const T &h, const T &i)
{
  m_[0][0] = a; m_[0][1] = b; m_[0][2] = c;
  m_[1][0] = d; m_[1][1] = e; m_[1][2] = f;
  m_[2][0] = g; m_[2][1] = h; m_[2][2] = i;
}

template <typename T>
Mat3<T>::Mat3(const Vec3<T> &a, const Vec3<T> &b, const Vec3<T> &c)
{
  m_[0] = a;
  m_[1] = b;
  m_[2] = c;
}

template <typename T>
Mat3<T>::Mat3(const Mat3 &m)
{
  m_[0] = m[0];
  m_[1] = m[1];
  m_[2] = m[2];
}

// Access
// ----------------------------------------------------------------------------
template <typename T>
inline Vec3<T> &Mat3<T>::operator[] (int i)
{
  return m_[i];
}

template <typename T>
inline const Vec3<T> &Mat3<T>::operator[] (int i) const
{
  return m_[i];
}

// Unary operators
// ----------------------------------------------------------------------------
template <typename T>
inline Mat3<T> &Mat3<T>::operator += (const Mat3 &m)
{
  m_[0] += m[0];
  m_[1] += m[1];
  m_[2] += m[2];
  return (*this);
}

template <typename T>
inline Mat3<T> &Mat3<T>::operator -= (const Mat3 &m)
{
  m_[0] -= m[0];
  m_[1] -= m[1];
  m_[2] -= m[2];
  return (*this);
}

template <typename T>
inline Mat3<T> &Mat3<T>::operator *= (const Mat3 &m)
{
  (*this) = (*this) * m;
  return (*this);
}

template <typename T>
inline Mat3<T> &Mat3<T>::operator /= (const Mat3 &m)
{
  (*this) = (*this) * inv(m);
  return (*this);
}

template <typename T>
inline Mat3<T> &Mat3<T>::operator += (const T &x)
{
  m_[0] += x;
  m_[1] += x;
  m_[2] += x;
  return (*this);
}

template <typename T>
inline Mat3<T> &Mat3<T>::operator -= (const T &x)
{
  m_[0] -= x;
  m_[1] -= x;
  m_[2] -= x;
  return (*this);
}

template <typename T>
inline Mat3<T> &Mat3<T>::operator *= (const T &x)
{
  m_[0] *= x;
  m_[1] *= x;
  m_[2] *= x;
  return (*this);
}

template <typename T>
inline Mat3<T> &Mat3<T>::operator /= (const T &x)
{
  m_[0] /= x;
  m_[1] /= x;
  m_[2] /= x;
  return (*this);
}

// Comparison
// ----------------------------------------------------------------------------
template <typename T>
inline bool Mat3<T>::operator == (const Mat3 &m) const
{
  return (m_[0] == m[0] && m_[1] == m[1] && m_[2] == m[2]);
}

template <typename T>
inline bool Mat3<T>::operator != (const Mat3 &m) const
{
  return (m_[0] != m[0] || m_[1] != m[1] || m_[2] != m[2]);
}

// Binary Operators With MAT
// ----------------------------------------------------------------------------
template <typename T>
inline Mat3<T> Mat3<T>::operator + (const Mat3 &m) const
{
  Mat3<T> r;
  r[0] = m_[0] + m[0];
  r[1] = m_[1] + m[1];
  r[2] = m_[2] + m[2];
  return r;
}

template <typename T>
inline Mat3<T> Mat3<T>::operator - (const Mat3 &m) const
{
  Mat3<T> r;
  r[0] = m_[0] * m[0];
  r[1] = m_[1] * m[1];
  r[2] = m_[2] * m[2];
  return r;
}

template <typename T>
inline Mat3<T> Mat3<T>::operator * (const Mat3 &m) const
{
  Mat3<T> r;
  r[0][0] = m_[0][0] * m.m_[0][0] + m_[0][1] * m.m_[1][0] + m_[0][2] * m.m_[2][0];
  r[0][1] = m_[0][0] * m.m_[0][1] + m_[0][1] * m.m_[1][1] + m_[0][2] * m.m_[2][1];
  r[0][2] = m_[0][0] * m.m_[0][2] + m_[0][1] * m.m_[1][2] + m_[0][2] * m.m_[2][2];
  r[1][0] = m_[1][0] * m.m_[0][0] + m_[1][1] * m.m_[1][0] + m_[1][2] * m.m_[2][0];
  r[1][1] = m_[1][0] * m.m_[0][1] + m_[1][1] * m.m_[1][1] + m_[1][2] * m.m_[2][1];
  r[1][2] = m_[1][0] * m.m_[0][2] + m_[1][1] * m.m_[1][2] + m_[1][2] * m.m_[2][2];
  r[2][0] = m_[2][0] * m.m_[0][0] + m_[2][1] * m.m_[1][0] + m_[2][2] * m.m_[2][0];
  r[2][1] = m_[2][0] * m.m_[0][1] + m_[2][1] * m.m_[1][1] + m_[2][2] * m.m_[2][1];
  r[2][2] = m_[2][0] * m.m_[0][2] + m_[2][1] * m.m_[1][2] + m_[2][2] * m.m_[2][2];
  return r;
}

template <typename T>
inline Mat3<T> Mat3<T>::operator / (const Mat3 &m) const
{
  Mat3<T> r = (*this) * inv(m);
  return r;
}

// Binary Operators With Scalar
// ----------------------------------------------------------------------------
template <typename T>
inline Mat3<T> Mat3<T>::operator + (const T &x) const
{
  Mat3<T> r;
  r[0] = m_[0] + x;
  r[1] = m_[1] + x;
  r[2] = m_[2] + x;
  return r;
}

template <typename T>
inline Mat3<T> Mat3<T>::operator - (const T &x) const
{
  Mat3<T> r;
  r[0] = m_[0] - x;
  r[1] = m_[1] - x;
  r[2] = m_[2] - x;
  return r;
}

template <typename T>
inline Mat3<T> Mat3<T>::operator * (const T &x) const
{
  Mat3<T> r;
  r[0] = m_[0] * x;
  r[1] = m_[1] * x;
  r[2] = m_[2] * x;
  return r;
}

template <typename T>
inline Mat3<T> Mat3<T>::operator / (const T &x) const
{
  Mat3<T> r;
  r[0] = m_[0] / x;
  r[1] = m_[1] / x;
  r[2] = m_[2] / x;
  return r;
}

// Negate
// ----------------------------------------------------------------------------
template <typename T>
inline Mat3<T> Mat3<T>::operator - () const
{
  Mat3<T> r;
  r[0] = -m_[0];
  r[1] = -m_[1];
  r[2] = -m_[2];
  return r;
}

// Assignment
// ----------------------------------------------------------------------------
template <typename T>
inline Mat3<T> &Mat3<T>::operator = (const Mat3 &m)
{
  m_[0] = m[0];
  m_[1] = m[1];
  m_[2] = m[2];
  return (*this);
}

template <typename T>
inline Mat3<T> &Mat3<T>::operator = (const T &x)
{
  m_[0] = x;
  m_[1] = x;
  m_[2] = x;
  return (*this);
}

// Friend Operators with Scalar
// ----------------------------------------------------------------------------
template <typename T, typename C>
inline Mat3<T> operator + (const C &x, const Mat3<T> &m)
{
  Mat3<T> r;
  r[0] = (T)x + m[0];
  r[1] = (T)x + m[1];
  r[2] = (T)x + m[2];
  return r;
}

template <typename T, typename C>
inline Mat3<T> operator - (const C &x, const Mat3<T> &m)
{
  Mat3<T> r;
  r[0] = (T)x - m[0];
  r[1] = (T)x - m[1];
  r[2] = (T)x - m[2];
  return r;
}

template <typename T, typename C>
inline Mat3<T> operator * (const C &x, const Mat3<T> &m)
{
  Mat3<T> r;
  r[0] = (T)x * m[0];
  r[1] = (T)x * m[1];
  r[2] = (T)x * m[2];
  return r;
}

template <typename T, typename C>
inline Mat3<T> operator / (const C &x, const Mat3<T> &m)
{
  Mat3<T> r;
  r[0] = (T)x / m[0];
  r[1] = (T)x / m[1];
  r[2] = (T)x / m[2];
  return r;
}

// Friend Operators with Vec
// ----------------------------------------------------------------------------
template <typename T>
Vec3<T> operator * (const Vec3<T> &v, const Mat3<T> &m)
{
  Vec3<T> r;
  r[0] = m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2];
  r[1] = m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2];
  r[2] = m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2];
  return r;
}

template <typename T>
Vec3<T> operator / (const Vec3<T> &v, const Mat3<T> &m)
{
  return v * inv(m);
}


// Matrix Operations
// ----------------------------------------------------------------------------
template <typename T> Mat3<T> inv(const Mat3<T> &m)
{
  T det;
  Mat3<T> adjm, r;

  adjm = adj(m);
  det  = adjm[0].dot(m[0]);

  // TODO: zero ?
  r  = trans(adjm);
  r /= det;
  return r;
}

template <typename T> inline Mat3<T> adj(const Mat3<T> &m)
{
  Mat3<T> r;
  r[0] = cross(m[1], m[2]);
  r[1] = cross(m[2], m[0]);
  r[2] = cross(m[0], m[1]);
  return r;
}

template <typename T> inline Mat3<T> trans(const Mat3<T> &m)
{
  Mat3<T> r;
  r[0][0] = m[0][0]; r[0][1] = m[1][0]; r[0][2] = m[2][0];
  r[1][0] = m[0][1]; r[1][1] = m[1][1]; r[1][2] = m[2][1];
  r[2][0] = m[0][2]; r[2][1] = m[1][2]; r[2][2] = m[2][2];
  return r;
}

template <typename T> inline T trace(const Mat3<T> &m)
{
  return (m[0][0] + m[1][1] + m[2][2]);
}

template <typename T> inline T det(const Mat3<T> &m)
{
  return m[0].dot(m[1].cross(m[2]));
}

// Friend Stream Operations
// ----------------------------------------------------------------------------
template <typename T>
std::ostream &operator << (std::ostream &out, const Mat3<T> &m)
{
  out << "["   << m[0]
      << "\n " << m[1]
      << "\n " << m[2] << "]";
  return out;
}

template <typename T>
std::istream &operator >> (std::istream &in, Mat3<T> &m)
{
  in >> m[0]
     >> m[1]
     >> m[2];
  return in;
}

// Static Producers
// ----------------------------------------------------------------------------
template <typename T>
Mat3<T> Mat3<T>::ZERO()
{
  Mat3<T> r;
  r[0] = 0;
  r[1] = 0;
  r[2] = 0;
  return r;
}

template <typename T>
Mat3<T> Mat3<T>::IDENTITY(T x)
{
  Mat3<T> r;
  r[0][0] = x; r[0][1] = 0; r[0][2] = 0;
  r[1][0] = 0; r[1][1] = x; r[1][2] = 0;
  r[2][0] = 0; r[1][1] = 0; r[2][2] = x;
  return r;
}

template <typename T>
Mat3<T> Mat3<T>::FILL(T x)
{
  Mat3<T> r;
  r[0][0] = x; r[0][1] = x; r[0][2] = x;
  r[1][0] = x; r[1][1] = x; r[1][2] = x;
  r[2][0] = x; r[1][1] = x; r[2][2] = x;
  return r;
}

template <typename T>
void Mat3<T>::normalize()
{
  T sqrSize = m_[0].sqrlen() + m_[1].sqrlen() + m_[2].sqrlen();
  (*this) = (*this) / std::sqrt(sqrSize);
}

typedef Mat3<float>  Mat3f;
typedef Mat3<double> Mat3d;
typedef Mat3<int>    Mat3i;


} }

#endif
