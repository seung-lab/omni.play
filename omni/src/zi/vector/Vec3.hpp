#ifndef _VECTOR_VEC3_HPP_
#define _VECTOR_VEC3_HPP_

#include "Common.hpp"
#include "Vec2.hpp"

namespace zi { namespace vector {

template <typename T>
class Vec3 {
public:

  Vec3();
  Vec3(const T &x);
  Vec3(const T &x, const T &y, const T &z);
  Vec3(const Vec3 &v);
  Vec3(const Vec2<T> &v, const T& z);

  T         &operator[] (int i);
  const T   &operator[] (int i) const;
  T         &x();
  const T   &x() const;
  T         &y();
  const T   &y() const;
  T         &z();
  const T   &z() const;

  operator T* () { return v_; }

  operator Vec2<T>() { return Vec2<T>(v_[0], v_[1]); }

  // Unary operators
  Vec3 &operator += (const Vec3 &v);
  Vec3 &operator -= (const Vec3 &v);
  Vec3 &operator *= (const Vec3 &v);
  Vec3 &operator /= (const Vec3 &v);

  Vec3 &operator += (const T &x);
  Vec3 &operator -= (const T &x);
  Vec3 &operator *= (const T &x);
  Vec3 &operator /= (const T &x);

  // Comparison
  bool operator <  (const Vec3 &v) const;
  bool operator == (const Vec3 &v) const;
  bool operator != (const Vec3 &v) const;

  // Arithmetic
  Vec3 operator + (const Vec3 &v) const;
  Vec3 operator - (const Vec3 &v) const;
  Vec3 operator * (const Vec3 &v) const;
  Vec3 operator / (const Vec3 &v) const;

  Vec3 operator + (const T &x) const;
  Vec3 operator - (const T &x) const;
  Vec3 operator * (const T &x) const;
  Vec3 operator / (const T &x) const;

  Vec3 operator - () const;

  // Assignment
  Vec3 &operator = (const Vec3 &v);
  Vec3 &operator = (const T &x);

  // To String
  operator std::string ();

  T dot(const Vec3 &v) const;
  T len() const;
  T sqrlen() const;
  void normalize();
  Vec3 norm() const;

protected:
  T v_[3];
};

template <typename T> std::ostream &operator << (std::ostream &out, const Vec3<T> &v);
template <typename T> std::istream &operator >> (std::istream &in, Vec3<T> &v);

template <typename T, typename C> Vec3<T> operator + (const C &x, const Vec3<T> &v);
template <typename T, typename C> Vec3<T> operator - (const C &x, const Vec3<T> &v);
template <typename T, typename C> Vec3<T> operator * (const C &x, const Vec3<T> &v);
template <typename T, typename C> Vec3<T> operator / (const C &x, const Vec3<T> &v);

template <typename T> Vec3<T> inline cross(const Vec3<T> &a, const Vec3<T> &b);


#ifdef _VT
# define _VT_SAVE _VT
# undef  _VT
#endif

#define _VT(...) template <typename T> __VA_ARGS__ Vec3<T>
#define SELF (*this)

// Constructors
_VT()::Vec3()                                   {}
_VT()::Vec3(const T &x)                         { v_[0] = x;    v_[1] = x;    v_[2] = x;   }
_VT()::Vec3(const T &x, const T &y, const T &z) { v_[0] = x;    v_[1] = y;    v_[2] = z;   }
_VT()::Vec3(const Vec3 &v)                      { v_[0] = v[0]; v_[1] = v[1]; v_[2] = v[2] ;}
_VT()::Vec3(const Vec2<T> &v, const T &z)       { v_[0] = v[0]; v_[1] = v[1]; v_[2] = z ;}

#undef _VT
#define _VT(...) template <typename T> inline __VA_ARGS__ Vec3<T>

_VT(T &)::operator[] (int i) { return v_[i]; }
_VT(const T &)::operator[] (int i) const { return v_[i]; }

_VT(T &)::x() { return v_[0]; }
_VT(const T &)::x() const { return v_[0]; }

_VT(T &)::y() { return v_[1]; }
_VT(const T &)::y() const { return v_[1]; }

_VT(T &)::z() { return v_[2]; }
_VT(const T &)::z() const { return v_[2]; }

  // Unary operators
_VT(Vec3<T> &)::operator += (const Vec3 &v) { v_[0] += v[0]; v_[1] += v[1]; v_[2] += v[2]; return SELF; }
_VT(Vec3<T> &)::operator -= (const Vec3 &v) { v_[0] -= v[0]; v_[1] -= v[1]; v_[2] -= v[2]; return SELF; }
_VT(Vec3<T> &)::operator *= (const Vec3 &v) { v_[0] *= v[0]; v_[1] *= v[1]; v_[2] *= v[2]; return SELF; }
_VT(Vec3<T> &)::operator /= (const Vec3 &v) { v_[0] /= v[0]; v_[1] /= v[1]; v_[2] /= v[2]; return SELF; }

_VT(Vec3<T> &)::operator += (const T &x) { v_[0] += x; v_[1] += x; v_[2] += x; return SELF;  }
_VT(Vec3<T> &)::operator -= (const T &x) { v_[0] -= x; v_[1] -= x; v_[2] -= x; return SELF;  }
_VT(Vec3<T> &)::operator *= (const T &x) { v_[0] *= x; v_[1] *= x; v_[2] *= x; return SELF;  }
_VT(Vec3<T> &)::operator /= (const T &x) { v_[0] /= x; v_[1] /= x; v_[2] /= x; return SELF;  }

_VT(bool)::operator <  (const Vec3 &v) const
{
  if (v_[0] < v[0]) return 1;
  if (v_[0] > v[0]) return 0;
  if (v_[1] < v[1]) return 1;
  if (v_[1] > v[1]) return 0;
  return (v_[2] < v[2]);
}
_VT(bool)::operator == (const Vec3 &v) const { return (v_[0] == v[0] && v_[1] == v[1] && v_[2] == v[2]); }
_VT(bool)::operator != (const Vec3 &v) const { return (v_[0] != v[0] || v_[1] != v[1] || v_[2] != v[2]); }

  // Arithmetic
_VT(Vec3<T>)::operator + (const Vec3 &v) const
{ Vec3<T> r; r[0] = v_[0] + v[0]; r[1] = v_[1] + v[1]; r[2] = v_[2] + v[2]; return r; }
_VT(Vec3<T>)::operator - (const Vec3 &v) const
{ Vec3<T> r; r[0] = v_[0] - v[0]; r[1] = v_[1] - v[1]; r[2] = v_[2] - v[2]; return r; }
_VT(Vec3<T>)::operator * (const Vec3 &v) const
{ Vec3<T> r; r[0] = v_[0] * v[0]; r[1] = v_[1] * v[1]; r[2] = v_[2] * v[2]; return r; }
_VT(Vec3<T>)::operator / (const Vec3 &v) const
{ Vec3<T> r; r[0] = v_[0] / v[0]; r[1] = v_[1] / v[1]; r[2] = v_[2] / v[2]; return r; }

_VT(Vec3<T>)::operator + (const T &x) const
{ Vec3<T> r; r[0] = v_[0] + x; r[1] = v_[1] + x; r[2] = v_[2] + x; return r; }
_VT(Vec3<T>)::operator - (const T &x) const
{ Vec3<T> r; r[0] = v_[0] - x; r[1] = v_[1] - x; r[2] = v_[2] - x; return r; }
_VT(Vec3<T>)::operator * (const T &x) const
{ Vec3<T> r; r[0] = v_[0] * x; r[1] = v_[1] * x; r[2] = v_[2] * x; return r; }
_VT(Vec3<T>)::operator / (const T &x) const
{ Vec3<T> r; r[0] = v_[0] / x; r[1] = v_[1] / x; r[2] = v_[2] / x; return r; }

_VT(Vec3<T>)::operator - () const { Vec3<T> r; r[0] = -v_[0]; r[1] = -v_[1]; r[2] = -v_[2]; return r; }

  // Assignment
_VT(Vec3<T> &)::operator = (const Vec3 &v) { v_[0] = v[0]; v_[1] = v[1]; v_[2] = v[2]; return SELF;  }
_VT(Vec3<T> &)::operator = (const T &x)    { v_[0] = x   ; v_[1] = x;    v_[2] = x;    return SELF;  }

_VT(T)::dot(const Vec3 &v) const { return v_[0] * v[0] + v_[1] * v[1] + v_[2] * v[2];               }
_VT(T)::len()              const { return std::sqrt(v_[0] * v_[0] + v_[1] * v_[1] + v_[2] * v_[2]); }
_VT(T)::sqrlen()           const { return v_[0] * v_[0] + v_[1] * v_[1] + v_[2] * v_[2];            }
_VT(void)::normalize()     { T l = len(); v_[0] /= l; v_[1] /= l; v_[2] /= l;                 }
_VT(Vec3<T>)::norm()       const { return (SELF / len());                            }

template <typename T>
Vec3<T> inline cross(const Vec3<T> &a, const Vec3<T> &b)
{
  Vec3<T> r;
  r[0] = a[1] * b[2] - a[2] * b[1];
  r[1] = a[2] * b[0] - a[0] * b[2];
  r[2] = a[0] * b[1] - a[1] * b[0];
  return r;
}


#undef _VT
#define _VT(_TYPE_) template <typename T> _TYPE_

_VT(std::ostream &) operator << (std::ostream &out, const Vec3<T> &v)
{ out << "[" << v[0] << " " << v[1] << " " << v[2] << "]"; return out; }

_VT(std::istream &) operator >> (std::istream &in, Vec3<T> &v)
{ in >> v[0] >> v[1] >> v[2]; return in; }

#undef _VT
#define _VT(_TYPE_) template <typename T, typename C> _TYPE_

_VT(Vec3<T>) operator + (const C &x, const Vec3<T> &v)
{ Vec3<T> r; r[0] = (T)x + v[0]; r[1] = (T)x + v[1]; r[2] = (T)x + v[2]; return r; }

_VT(Vec3<T>) operator - (const C &x, const Vec3<T> &v)
{ Vec3<T> r; r[0] = (T)x - v[0]; r[1] = (T)x - v[1]; r[2] = (T)x - v[2]; return r; }

_VT(Vec3<T>) operator * (const C &x, const Vec3<T> &v)
{ Vec3<T> r; r[0] = (T)x * v[0]; r[1] = (T)x * v[1]; r[2] = (T)x * v[2]; return r; }

_VT(Vec3<T>) operator / (const C &x, const Vec3<T> &v)
{ Vec3<T> r; r[0] = (T)x / v[0]; r[1] = (T)x / v[1]; r[2] = (T)x / v[2]; return r; }

#undef _VT
#undef SELF

#ifdef _VT_SAVE
# define _VT _VT_SAVE
# undef  _VT_SAVE
#endif

#define Vec3f Vec3<float>  /* */
#define Vec3d Vec3<double> /* */
#define Vec3i Vec3<int>    /* */


} }

#endif
