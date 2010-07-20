#ifndef _VECTOR_VEC4_HPP_
#define _VECTOR_VEC4_HPP_

#include "Common.hpp"
#include "Vec3.hpp"

namespace zi { namespace vector {

template <typename T>
class Vec4 {
public:

  Vec4();
  Vec4(const T &x);
  Vec4(const T &x, const T &y, const T &z, const T &w);
  Vec4(const Vec4 &v);
  Vec4(const Vec3<T> &v);
  Vec4(const Vec3<T> &v, const T &w);

  T         &operator[] (int i);
  const T   &operator[] (int i) const;

  operator T* () { return v_; }

  // Cast operators
  operator Vec3<T>() { return Vec3<T>(v_[0], v_[1], v_[2]); }
  operator Vec2<T>() { return Vec2<T>(v_[0], v_[1]); }

  // Unary operators
  Vec4 &operator += (const Vec4 &v);
  Vec4 &operator -= (const Vec4 &v);
  Vec4 &operator *= (const Vec4 &v);
  Vec4 &operator /= (const Vec4 &v);

  Vec4 &operator += (const T &x);
  Vec4 &operator -= (const T &x);
  Vec4 &operator *= (const T &x);
  Vec4 &operator /= (const T &x);

  // Comparison
  bool operator <  (const Vec4 &v) const;
  bool operator == (const Vec4 &v) const;
  bool operator != (const Vec4 &v) const;

  // Arithmetic
  Vec4 operator + (const Vec4 &v) const;
  Vec4 operator - (const Vec4 &v) const;
  Vec4 operator * (const Vec4 &v) const;
  Vec4 operator / (const Vec4 &v) const;

  Vec4 operator + (const T &x) const;
  Vec4 operator - (const T &x) const;
  Vec4 operator * (const T &x) const;
  Vec4 operator / (const T &x) const;

  Vec4 operator - () const;

  // Assignment
  Vec4 &operator = (const Vec4 &v);
  Vec4 &operator = (const T &x);

  // To String
  operator std::string ();

  T dot(const Vec4 &v) const;
  T len() const;
  T sqrlen() const;
  void normalize();
  Vec4 norm() const;

protected:
  T v_[4];
};

template <typename T> std::ostream &operator << (std::ostream &out, const Vec4<T> &v);
template <typename T> std::istream &operator >> (std::istream &in, Vec4<T> &v);

template <typename T, typename C> Vec4<T> operator + (const C &x, const Vec4<T> &v);
template <typename T, typename C> Vec4<T> operator - (const C &x, const Vec4<T> &v);
template <typename T, typename C> Vec4<T> operator * (const C &x, const Vec4<T> &v);
template <typename T, typename C> Vec4<T> operator / (const C &x, const Vec4<T> &v);

template <typename T> Vec4<T> cross(const Vec4<T> &a, const Vec4<T> &b, const Vec4<T> &c);

#ifdef _VT
# define _VT_SAVE _VT
# undef  _VT
#endif

#define _VT(...) template <typename T> __VA_ARGS__ Vec4<T>
#define SELF (*this)

// Constructors
_VT()::Vec4() {}
_VT()::Vec4(const T &x)
{ v_[0] = x; v_[1] = x; v_[2] = x; v_[3] = x; }
_VT()::Vec4(const T &x, const T &y, const T &z, const T &w)
{ v_[0] = x; v_[1] = y; v_[2] = z; v_[3] = w; }
_VT()::Vec4(const Vec4 &v)
{ v_[0] = v[0]; v_[1] = v[1]; v_[2] = v[2]; v_[3] = v[3]; }
_VT()::Vec4(const Vec3<T> &v)
{ v_[0] = v[0]; v_[1] = v[1]; v_[2] = v[2]; v_[3] = (T)0; }
_VT()::Vec4(const Vec3<T> &v, const T &w)
{ v_[0] = v[0]; v_[1] = v[1]; v_[2] = v[2]; v_[3] = w; }

#undef _VT
#define _VT(...) template <typename T> inline __VA_ARGS__ Vec4<T>

_VT(T &)::operator[] (int i) { return v_[i]; }
_VT(const T &)::operator[] (int i) const { return v_[i]; }

  // Unary operators
_VT(Vec4<T> &)::operator += (const Vec4 &v)
{ v_[0] += v[0]; v_[1] += v[1]; v_[2] += v[2]; v_[3] += v[3]; return SELF; }
_VT(Vec4<T> &)::operator -= (const Vec4 &v)
{ v_[0] -= v[0]; v_[1] -= v[1]; v_[2] -= v[2]; v_[3] -= v[3]; return SELF; }
_VT(Vec4<T> &)::operator *= (const Vec4 &v)
{ v_[0] *= v[0]; v_[1] *= v[1]; v_[2] *= v[2]; v_[3] *= v[3]; return SELF; }
_VT(Vec4<T> &)::operator /= (const Vec4 &v)
{ v_[0] /= v[0]; v_[1] /= v[1]; v_[2] /= v[2]; v_[3] /= v[3]; return SELF; }

_VT(Vec4<T> &)::operator += (const T &x) { v_[0] += x; v_[1] += x; v_[2] += x; v_[3] += x; return SELF;  }
_VT(Vec4<T> &)::operator -= (const T &x) { v_[0] -= x; v_[1] -= x; v_[2] -= x; v_[3] -= x; return SELF;  }
_VT(Vec4<T> &)::operator *= (const T &x) { v_[0] *= x; v_[1] *= x; v_[2] *= x; v_[3] *= x; return SELF;  }
_VT(Vec4<T> &)::operator /= (const T &x) { v_[0] /= x; v_[1] /= x; v_[2] /= x; v_[3] /= x; return SELF;  }

_VT(bool)::operator <  (const Vec4 &v) const
{
  if (v_[0] < v[0]) return 1;
  if (v_[0] > v[0]) return 0;
  if (v_[1] < v[1]) return 1;
  if (v_[1] > v[1]) return 0;
  if (v_[2] < v[2]) return 1;
  if (v_[2] > v[2]) return 0;
  if (v_[3] < v[3]) return 1;
  return 0; }
_VT(bool)::operator == (const Vec4 &v) const
{ return (v_[0] == v[0] && v_[1] == v[1] && v_[2] == v[2] && v_[3] == v[3]); }
_VT(bool)::operator != (const Vec4 &v) const
{ return (v_[0] != v[0] || v_[1] != v[1] || v_[2] != v[2] || v_[3] != v[3]); }

  // Arithmetic
_VT(Vec4<T>)::operator + (const Vec4 &v) const
{ Vec4<T> r; r[0] = v_[0] + v[0]; r[1] = v_[1] + v[1]; r[2] = v_[2] + v[2]; r[3] = v_[3] + v[3]; return r; }
_VT(Vec4<T>)::operator - (const Vec4 &v) const
{ Vec4<T> r; r[0] = v_[0] - v[0]; r[1] = v_[1] - v[1]; r[2] = v_[2] - v[2]; r[3] = v_[3] - v[3]; return r; }
_VT(Vec4<T>)::operator * (const Vec4 &v) const
{ Vec4<T> r; r[0] = v_[0] * v[0]; r[1] = v_[1] * v[1]; r[2] = v_[2] * v[2]; r[3] = v_[3] * v[3]; return r; }
_VT(Vec4<T>)::operator / (const Vec4 &v) const
{ Vec4<T> r; r[0] = v_[0] / v[0]; r[1] = v_[1] / v[1]; r[2] = v_[2] / v[2]; r[3] = v_[3] / v[3]; return r; }

_VT(Vec4<T>)::operator + (const T &x) const
{ Vec4<T> r; r[0] = v_[0] + x; r[1] = v_[1] + x; r[2] = v_[2] + x; r[3] = v_[3] + x; return r; }
_VT(Vec4<T>)::operator - (const T &x) const
{ Vec4<T> r; r[0] = v_[0] - x; r[1] = v_[1] - x; r[2] = v_[2] - x; r[3] = v_[3] - x; return r; }
_VT(Vec4<T>)::operator * (const T &x) const
{ Vec4<T> r; r[0] = v_[0] * x; r[1] = v_[1] * x; r[2] = v_[2] * x; r[3] = v_[3] * x; return r; }
_VT(Vec4<T>)::operator / (const T &x) const
{ Vec4<T> r; r[0] = v_[0] / x; r[1] = v_[1] / x; r[2] = v_[2] / x; r[3] = v_[3] / x; return r; }

_VT(Vec4<T>)::operator - () const
{ Vec4<T> r; r[0] = -v_[0]; r[1] = -v_[1]; r[2] = -v_[2]; r[3] = -v_[3]; return r; }

  // Assignment
_VT(Vec4<T> &)::operator = (const Vec4 &v)
{ v_[0] = v[0]; v_[1] = v[1]; v_[2] = v[2]; v_[3] = v[3]; return SELF;  }

_VT(Vec4<T> &)::operator = (const T &x)
{ v_[0] = x   ; v_[1] = x;    v_[2] = x;    v_[3] = x;    return SELF;  }

_VT(T)::dot(const Vec4<T> &v) const { return v_[0] * v[0] + v_[1] * v[1] + v_[2] * v[2] + v_[3] * v[3];                }
_VT(T)::len()              const { return std::sqrt(v_[0] * v_[0] + v_[1] * v_[1] + v_[2] * v_[2] + v_[3] * v_[3]); }
_VT(T)::sqrlen()           const { return v_[0] * v_[0] + v_[1] * v_[1] + v_[2] * v_[2] + v_[3] * v_[3];            }
_VT(void)::normalize()     { T l = len(); v_[0] /= l; v_[1] /= l; v_[2] /= l; v_[3] /= l;                     }
_VT(Vec4<T>)::norm()       const { return (SELF / len());                            }


template <typename T>
Vec4<T> cross(const Vec4<T> &a, const Vec4<T> &b, const Vec4<T> &c)
{
  Vec4<T> r;
  r[0] =  Vec3<T>(a[1], b[1], c[1]).dot(cross(Vec3<T>(a[2], b[2], c[2]),
                                              Vec3<T>(a[3], b[3], c[3])));
  r[1] = -Vec3<T>(a[0], b[0], c[0]).dot(cross(Vec3<T>(a[2], b[2], c[2]),
                                              Vec3<T>(a[3], b[3], c[3])));
  r[2] =  Vec3<T>(a[0], b[0], c[0]).dot(cross(Vec3<T>(a[1], b[1], c[1]),
                                              Vec3<T>(a[3], b[3], c[3])));
  r[3] = -Vec3<T>(a[0], b[0], c[0]).dot(cross(Vec3<T>(a[1], b[1], c[1]),
                                              Vec3<T>(a[2], b[2], c[2])));
  return r;
}


#undef _VT
#define _VT(_TYPE_) template <typename T> _TYPE_

_VT(std::ostream &) operator << (std::ostream &out, const Vec4<T> &v)
{ out << "[" << v[0] << " " << v[1] << " " << v[2] << " " << v[3] << "]"; return out; }

_VT(std::istream &) operator >> (std::istream &in, Vec4<T> &v)
{ in >> v[0] >> v[1] >> v[2] >> v[3]; return in; }

#undef _VT
#define _VT(_TYPE_) template <typename T, typename C> _TYPE_

_VT(Vec4<T>) operator + (const C &x, const Vec4<T> &v)
{ Vec4<T> r; r[0] = (T)x + v[0]; r[1] = (T)x + v[1]; r[2] = (T)x + v[2]; r[3] = (T)x + v[3]; return r; }

_VT(Vec4<T>) operator - (const C &x, const Vec4<T> &v)
{ Vec4<T> r; r[0] = (T)x - v[0]; r[1] = (T)x - v[1]; r[2] = (T)x - v[2]; r[3] = (T)x - v[3];  return r; }

_VT(Vec4<T>) operator * (const C &x, const Vec4<T> &v)
{ Vec4<T> r; r[0] = (T)x * v[0]; r[1] = (T)x * v[1]; r[2] = (T)x * v[2]; r[3] = (T)x * v[3];  return r; }

_VT(Vec4<T>) operator / (const C &x, const Vec4<T> &v)
{ Vec4<T> r; r[0] = (T)x / v[0]; r[1] = (T)x / v[1]; r[2] = (T)x / v[2]; r[3] = (T)x / v[3];  return r; }

#undef _VT
#undef SELF

#ifdef _VT_SAVE
# define _VT _VT_SAVE
# undef  _VT_SAVE
#endif

#define Vec4f Vec4<float>  /* */
#define Vec4d Vec4<double> /* */
#define Vec4i Vec4<int>    /* */


} }

#endif
