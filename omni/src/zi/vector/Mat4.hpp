#ifndef _VECTOR_MAT4_HPP_
#define _VECTOR_MAT4_HPP_

#include "Common.hpp"
#include "Vec3.hpp"
#include "Vec4.hpp"

namespace zi { namespace vector {

template <typename T>
class Mat4;

template <typename T>
class RawMat4 {
public:
  RawMat4(const Mat4<T> &m) {
    for (int i=0; i<4; i++)
      for (int j=0; j<4; j++)
        m_[(i<<2) | j] = m[i][j];
  }
  operator T* () {
    return m_;
  }
protected:
  T m_[16];
};

template <typename T>
class Mat4 {
public:

  Mat4();
  Mat4(const T &a0, const T &b0, const T &c0, const T &d0,
       const T &a1, const T &b1, const T &c1, const T &d1,
       const T &a2, const T &b2, const T &c2, const T &d2,
       const T &a3, const T &b3, const T &c3, const T &d3);
  Mat4(const Vec4<T> &a, const Vec4<T> &b, const Vec4<T> &c, const Vec4<T> &d);
  Mat4(const Mat4 &m);
  Mat4(const Vec4<T> &v);
  Mat4(const MatFill &mf);

  Vec4<T>        &operator[] (int i);
  const Vec4<T>  &operator[] (int i) const;

  operator T* () { return m_; }

  // Unary operators
  Mat4 &operator += (const Mat4 &m);
  Mat4 &operator -= (const Mat4 &m);
  Mat4 &operator *= (const Mat4 &m);
  Mat4 &operator /= (const Mat4 &m);

  Mat4 &operator += (const T &x);
  Mat4 &operator -= (const T &x);
  Mat4 &operator *= (const T &x);
  Mat4 &operator /= (const T &x);

  // Comparison
  bool operator == (const Mat4 &m) const;
  bool operator != (const Mat4 &m) const;

  // Arithmetic
  Mat4 operator + (const Mat4 &m) const;
  Mat4 operator - (const Mat4 &m) const;
  Mat4 operator * (const Mat4 &m) const;
  Mat4 operator / (const Mat4 &m) const;

  Mat4 operator + (const T &x) const;
  Mat4 operator - (const T &x) const;
  Mat4 operator * (const T &x) const;
  Mat4 operator / (const T &x) const;

  Mat4 operator - () const;

  // Assignment
  Mat4 &operator = (const Mat4 &m);
  Mat4 &operator = (const T &x);

  static Mat4 ZERO       ();
  static Mat4 IDENTITY   (T x = (T)1);
  static Mat4 TRANSLATION(const Vec3<T> &);
  static Mat4 SCALE      (const Vec3<T> &);
  static Mat4 FILL       (T x = (T)1);
  static Mat4 Rotation   (const Vec3<T> &axis, T theta);

protected:
  Vec4<T> m_[4];
};

template <typename T>        Mat4<T> inv   (const Mat4<T> &m);
template <typename T> inline Mat4<T> adj   (const Mat4<T> &m);
template <typename T> inline Mat4<T> trans (const Mat4<T> &m);
template <typename T> inline T       trace (const Mat4<T> &m);
template <typename T>        T       det   (const Mat4<T> &m);

template <typename T> std::ostream &operator << (std::ostream &out, const Mat4<T> &m);
template <typename T> std::istream &operator >> (std::istream &in, Mat4<T> &m);

template <typename T, typename C> Mat4<T> operator + (const C &x, const Mat4<T> &m);
template <typename T, typename C> Mat4<T> operator - (const C &x, const Mat4<T> &m);
template <typename T, typename C> Mat4<T> operator * (const C &x, const Mat4<T> &m);
template <typename T, typename C> Mat4<T> operator / (const C &x, const Mat4<T> &m);

template <typename T> Vec4<T> operator * (const Vec4<T> &v, const Mat4<T> &m);
template <typename T> Vec4<T> operator / (const Vec4<T> &v, const Mat4<T> &m);

// Constructors
// ----------------------------------------------------------------------------
template <typename T>
Mat4<T>::Mat4() {}

template <typename T>
Mat4<T>::  Mat4(const T &a0, const T &b0, const T &c0, const T &d0,
                const T &a1, const T &b1, const T &c1, const T &d1,
                const T &a2, const T &b2, const T &c2, const T &d2,
                const T &a3, const T &b3, const T &c3, const T &d3)
{
  m_[0][0] = a0; m_[0][1] = b0; m_[0][2] = c0; m_[0][3] = d0;
  m_[1][0] = a1; m_[1][1] = b1; m_[1][2] = c1; m_[1][3] = d1;
  m_[2][0] = a2; m_[2][1] = b2; m_[2][2] = c2; m_[2][3] = d2;
  m_[3][0] = a3; m_[3][1] = b3; m_[3][2] = c3; m_[3][3] = d3;
}

template <typename T>
Mat4<T>::Mat4(const Vec4<T> &a, const Vec4<T> &b,
              const Vec4<T> &c, const Vec4<T> &d)
{
  m_[0] = a;
  m_[1] = b;
  m_[2] = c;
  m_[3] = d;
}

template <typename T>
Mat4<T>::Mat4(const Mat4 &m)
{
  m_[0] = m[0];
  m_[1] = m[1];
  m_[2] = m[2];
  m_[3] = m[3];
}

template <typename T>
Mat4<T>::Mat4(const Vec4<T> &v)
{
  m_[0] = v;
  m_[1] = v;
  m_[2] = v;
  m_[3] = v;
}

// Access
// ----------------------------------------------------------------------------
template <typename T>
inline Vec4<T> &Mat4<T>::operator[] (int i)
{
  return m_[i];
}

template <typename T>
inline const Vec4<T> &Mat4<T>::operator[] (int i) const
{
  return m_[i];
}

// Unary operators
// ----------------------------------------------------------------------------
template <typename T>
inline Mat4<T> &Mat4<T>::operator += (const Mat4 &m)
{
  m_[0] += m[0];
  m_[1] += m[1];
  m_[2] += m[2];
  m_[3] += m[3];
  return (*this);
}

template <typename T>
inline Mat4<T> &Mat4<T>::operator -= (const Mat4 &m)
{
  m_[0] -= m[0];
  m_[1] -= m[1];
  m_[2] -= m[2];
  m_[3] -= m[3];
  return (*this);
}

template <typename T>
inline Mat4<T> &Mat4<T>::operator *= (const Mat4 &m)
{
  (*this) = (*this) * m;
  return (*this);
}

template <typename T>
inline Mat4<T> &Mat4<T>::operator /= (const Mat4 &m)
{
  (*this) = (*this) * inv(m);
  return (*this);
}

template <typename T>
inline Mat4<T> &Mat4<T>::operator += (const T &x)
{
  m_[0] += x;
  m_[1] += x;
  m_[2] += x;
  m_[3] += x;
  return (*this);
}

template <typename T>
inline Mat4<T> &Mat4<T>::operator -= (const T &x)
{
  m_[0] -= x;
  m_[1] -= x;
  m_[2] -= x;
  m_[3] -= x;
  return (*this);
}

template <typename T>
inline Mat4<T> &Mat4<T>::operator *= (const T &x)
{
  m_[0] *= x;
  m_[1] *= x;
  m_[2] *= x;
  m_[3] *= x;
  return (*this);
}

template <typename T>
inline Mat4<T> &Mat4<T>::operator /= (const T &x)
{
  m_[0] /= x;
  m_[1] /= x;
  m_[2] /= x;
  m_[3] /= x;
  return (*this);
}

// Comparison
// ----------------------------------------------------------------------------
template <typename T>
inline bool Mat4<T>::operator == (const Mat4 &m) const
{
  return (m_[0] == m[0] && m_[1] == m[1] && m_[2] == m[2] && m_[3] == m[3]);
}

template <typename T>
inline bool Mat4<T>::operator != (const Mat4 &m) const
{
  return (m_[0] != m[0] || m_[1] != m[1] || m_[2] != m[2] || m_[3] != m[3]);
}

// Binary Operators With MAT
// ----------------------------------------------------------------------------
template <typename T>
inline Mat4<T> Mat4<T>::operator + (const Mat4 &m) const
{
  Mat4<T> r;
  r[0] = m_[0] + m[0];
  r[1] = m_[1] + m[1];
  r[2] = m_[2] + m[2];
  r[3] = m_[3] + m[3];
  return r;
}

template <typename T>
inline Mat4<T> Mat4<T>::operator - (const Mat4 &m) const
{
  Mat4<T> r;
  r[0] = m_[0] * m[0];
  r[1] = m_[1] * m[1];
  r[2] = m_[2] * m[2];
  r[3] = m_[3] * m[3];
  return r;
}

template <typename T>
inline Mat4<T> Mat4<T>::operator * (const Mat4 &m) const
{
  Mat4<T> r;
  r[0] = m_[0][0] * m.m_[0] + m_[0][1] * m.m_[1] + m_[0][2] * m.m_[2] + m_[0][3] * m.m_[3];
  r[1] = m_[1][0] * m.m_[0] + m_[1][1] * m.m_[1] + m_[1][2] * m.m_[2] + m_[1][3] * m.m_[3];
  r[2] = m_[2][0] * m.m_[0] + m_[2][1] * m.m_[1] + m_[2][2] * m.m_[2] + m_[2][3] * m.m_[3];
  r[3] = m_[3][0] * m.m_[0] + m_[3][1] * m.m_[1] + m_[3][2] * m.m_[2] + m_[3][3] * m.m_[3];
  return r;
}

template <typename T>
inline Mat4<T> Mat4<T>::operator / (const Mat4 &m) const
{
  Mat4<T> r = (*this) * inv(m);
  return r;
}

// Binary Operators With Scalar
// ----------------------------------------------------------------------------
template <typename T>
inline Mat4<T> Mat4<T>::operator + (const T &x) const
{
  Mat4<T> r;
  r[0] = m_[0] + x;
  r[1] = m_[1] + x;
  r[2] = m_[2] + x;
  r[3] = m_[3] + x;
  return r;
}

template <typename T>
inline Mat4<T> Mat4<T>::operator - (const T &x) const
{
  Mat4<T> r;
  r[0] = m_[0] - x;
  r[1] = m_[1] - x;
  r[2] = m_[2] - x;
  r[3] = m_[3] - x;
  return r;
}

template <typename T>
inline Mat4<T> Mat4<T>::operator * (const T &x) const
{
  Mat4<T> r;
  r[0] = m_[0] * x;
  r[1] = m_[1] * x;
  r[2] = m_[2] * x;
  r[3] = m_[3] * x;
  return r;
}

template <typename T>
inline Mat4<T> Mat4<T>::operator / (const T &x) const
{
  Mat4<T> r;
  r[0] = m_[0] / x;
  r[1] = m_[1] / x;
  r[2] = m_[2] / x;
  r[3] = m_[3] / x;
  return r;
}

// Negate
// ----------------------------------------------------------------------------
template <typename T>
inline Mat4<T> Mat4<T>::operator - () const
{
  Mat4<T> r;
  r[0] = -m_[0];
  r[1] = -m_[1];
  r[2] = -m_[2];
  r[3] = -m_[3];
  return r;
}

// Assignment
// ----------------------------------------------------------------------------
template <typename T>
inline Mat4<T> &Mat4<T>::operator = (const Mat4 &m)
{
  m_[0] = m[0];
  m_[1] = m[1];
  m_[2] = m[2];
  m_[3] = m[3];
  return (*this);
}

template <typename T>
inline Mat4<T> &Mat4<T>::operator = (const T &x)
{
  m_[0] = x;
  m_[1] = x;
  m_[2] = x;
  m_[3] = x;
  return (*this);
}

// Friend Operators with Scalar
// ----------------------------------------------------------------------------
template <typename T, typename C>
inline Mat4<T> operator + (const C &x, const Mat4<T> &m)
{
  Mat4<T> r;
  r[0] = (T)x + m[0];
  r[1] = (T)x + m[1];
  r[2] = (T)x + m[2];
  r[3] = (T)x + m[3];
  return r;
}

template <typename T, typename C>
inline Mat4<T> operator - (const C &x, const Mat4<T> &m)
{
  Mat4<T> r;
  r[0] = (T)x - m[0];
  r[1] = (T)x - m[1];
  r[2] = (T)x - m[2];
  r[3] = (T)x - m[3];
  return r;
}

template <typename T, typename C>
inline Mat4<T> operator * (const C &x, const Mat4<T> &m)
{
  Mat4<T> r;
  r[0] = (T)x * m[0];
  r[1] = (T)x * m[1];
  r[2] = (T)x * m[2];
  r[3] = (T)x * m[3];
  return r;
}

template <typename T, typename C>
inline Mat4<T> operator / (const C &x, const Mat4<T> &m)
{
  Mat4<T> r;
  r[0] = (T)x / m[0];
  r[1] = (T)x / m[1];
  r[2] = (T)x / m[2];
  r[3] = (T)x / m[3];
  return r;
}

// Friend Operators with Vec
// ----------------------------------------------------------------------------
template <typename T>
Vec4<T> operator * (const Vec4<T> &v, const Mat4<T> &m)
{
  Vec4<T> r;
  r[0] = m[0].dot(v);
  r[1] = m[1].dot(v);
  r[2] = m[2].dot(v);
  r[3] = m[3].dot(v);
  return r;
}

template <typename T>
Vec4<T> operator / (const Vec4<T> &v, const Mat4<T> &m)
{
  return v * inv(m);
}


// Matrix Operations
// ----------------------------------------------------------------------------
template <typename T> Mat4<T> inv(const Mat4<T> &m)
{
  T det;
  Mat4<T> adjm, r;

  adjm = adj(m);
  det  = adjm[0].dot(m[0]);

  // TODO: zero ?
  if ((det > 0.000001) || (det < -0.000001)) {
    r  = trans(adjm);
    r /= det;
    return r;
  }

  return Mat4<T>::ZERO();
}

template <typename T> inline Mat4<T> adj(const Mat4<T> &m)
{
  Mat4<T> r;
  r[0] =  cross(m[1], m[2], m[3]);
  r[1] = -cross(m[0], m[2], m[3]);
  r[2] =  cross(m[0], m[1], m[3]);
  r[3] = -cross(m[0], m[1], m[2]);
  return r;
}

template <typename T> inline Mat4<T> trans(const Mat4<T> &m)
{
  Mat4<T> r;
  r[0][0] = m[0][0]; r[0][1] = m[1][0]; r[0][2] = m[2][0]; r[0][3] = m[3][0];
  r[1][0] = m[0][1]; r[1][1] = m[1][1]; r[1][2] = m[2][1]; r[1][3] = m[3][1];
  r[2][0] = m[0][2]; r[2][1] = m[1][2]; r[2][2] = m[2][2]; r[2][3] = m[3][2];
  r[3][0] = m[0][3]; r[3][1] = m[1][3]; r[3][2] = m[2][3]; r[3][3] = m[3][3];
  return r;
}

template <typename T> inline T trace(const Mat4<T> &m)
{
  return (m[0][0] + m[1][1] + m[2][2] + m[3][3]);
}

template <typename T> T det(const Mat4<T> &m)
{
  return m[0].dot(cross(m[1], m[2], m[3]));
}

// Friend Stream Operations
// ----------------------------------------------------------------------------
template <typename T>
std::ostream &operator << (std::ostream &out, const Mat4<T> &m)
{
  out << "["   << m[0]
      << "\n " << m[1]
      << "\n " << m[2]
      << "\n " << m[3] << "]";
  return out;
}

template <typename T>
std::istream &operator >> (std::istream &in, Mat4<T> &m)
{
  in >> m[0]
     >> m[1]
     >> m[2]
     >> m[3];
  return in;
}

// Static Producers
// ----------------------------------------------------------------------------
template <typename T>
Mat4<T> Mat4<T>::ZERO()
{
  Mat4<T> r;
  r[0] = 0;
  r[1] = 0;
  r[2] = 0;
  r[3] = 0;
  return r;
}

template <typename T>
Mat4<T> Mat4<T>::IDENTITY(T x)
{
  Mat4<T> r;
  r[0][0] = x; r[0][1] = 0; r[0][2] = 0; r[0][3] = 0;
  r[1][0] = 0; r[1][1] = x; r[1][2] = 0; r[1][3] = 0;
  r[2][0] = 0; r[2][1] = 0; r[2][2] = x; r[2][3] = 0;
  r[3][0] = 0; r[3][1] = 0; r[3][2] = 0; r[3][3] = x;
  return r;
}

template <typename T>
Mat4<T> Mat4<T>::TRANSLATION(const Vec3<T> &x)
{
  Mat4<T> r;
  r[0][0] = 1; r[0][1] = 0; r[0][2] = 0; r[0][3] = x[0];
  r[1][0] = 0; r[1][1] = 1; r[1][2] = 0; r[1][3] = x[1];
  r[2][0] = 0; r[2][1] = 0; r[2][2] = 1; r[2][3] = x[2];
  r[3][0] = 0; r[3][1] = 0; r[3][2] = 0; r[3][3] = 1;
  return r;
}

template <typename T>
Mat4<T> Mat4<T>::SCALE(const Vec3<T> &x)
{
  Mat4<T> r;
  r[0][0] = x[0]; r[0][1] = 0;    r[0][2] = 0;    r[0][3] = 0;
  r[1][0] = 0;    r[1][1] = x[1]; r[1][2] = 0;    r[1][3] = 0;
  r[2][0] = 0;    r[2][1] = 0;    r[2][2] = x[2]; r[2][3] = 0;
  r[3][0] = 0;    r[3][1] = 0;    r[3][2] = 0;    r[3][3] = 1;
  return r;
}

template <typename T>
Mat4<T> Mat4<T>::FILL(T x)
{
  Mat4<T> r;
  r[0] = x;
  r[1] = x;
  r[2] = x;
  r[3] = x;
  return r;
}

template <typename T>
Mat4<T> Mat4<T>::Rotation(const Vec3<T> &axis, T theta)
{
  T s = (T) std::sin(theta);
  T c = (T) std::cos(theta);
  Vec4<T> v(axis[0], axis[1], axis[2], (T)0);
  v.normalize();
  Mat4<T> r;
  r[0] = v * v[0] * ((T)1 - c) + Vec4<T>(c, -v[2]*s, v[1]*s, (T)0);
  r[1] = v * v[1] * ((T)1 - c) + Vec4<T>(v[2] * s, c, -v[0]*s, (T)0);
  r[2] = v * v[2] * ((T)1 - c) + Vec4<T>(-v[1]*s, v[0]*s, c, (T)0);
  r[3] = Vec4<T>((T)0, (T)0, (T)0, (T)1);
  return r;
}

typedef Mat4<float>  Mat4f;
typedef Mat4<double> Mat4d;
typedef Mat4<int>    Mat4i;


} }

#endif
