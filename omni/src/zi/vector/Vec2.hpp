#ifndef _VECTOR_VEC2_HPP_
#define _VECTOR_VEC2_HPP_

#include "Common.hpp"

namespace zi { namespace vector {

template <typename T>
class Vec2 {
public:

  Vec2();
  Vec2(const T &x);
  Vec2(const T &x, const T &y);
  Vec2(const Vec2 &v);

  T         &operator[] (int i);
  const T   &operator[] (int i) const;
  T         &x();
  const T   &x() const;
  T         &y();
  const T   &y() const;

  operator T* () { return v_; }

  // Unary operators
  Vec2 &operator += (const Vec2 &v);
  Vec2 &operator -= (const Vec2 &v);
  Vec2 &operator *= (const Vec2 &v);
  Vec2 &operator /= (const Vec2 &v);

  Vec2 &operator += (const T &x);
  Vec2 &operator -= (const T &x);
  Vec2 &operator *= (const T &x);
  Vec2 &operator /= (const T &x);

  // Comparison
  bool operator <  (const Vec2 &v) const;
  bool operator == (const Vec2 &v) const;
  bool operator != (const Vec2 &v) const;

  // Arithmetic
  Vec2 operator + (const Vec2 &v) const;
  Vec2 operator - (const Vec2 &v) const;
  Vec2 operator * (const Vec2 &v) const;
  Vec2 operator / (const Vec2 &v) const;

  Vec2 operator + (const T &x) const;
  Vec2 operator - (const T &x) const;
  Vec2 operator * (const T &x) const;
  Vec2 operator / (const T &x) const;

  Vec2 operator - () const;

  // Assignment
  Vec2 &operator = (const Vec2 &v);
  Vec2 &operator = (const T &x);

  // To String
  operator std::string ();

  T dot(const Vec2 &v) const;
  T len()              const;
  T sqrlen()           const;
  Vec2 norm()          const;
  void normalize();

protected:
  T v_[2];
};

template <typename T> std::ostream &operator << (std::ostream &out, const Vec2<T> &v);
template <typename T> std::istream &operator >> (std::istream &in, Vec2<T> &v);

template <typename T, typename C> Vec2<T> operator + (const C &x, const Vec2<T> &v);
template <typename T, typename C> Vec2<T> operator - (const C &x, const Vec2<T> &v);
template <typename T, typename C> Vec2<T> operator * (const C &x, const Vec2<T> &v);
template <typename T, typename C> Vec2<T> operator / (const C &x, const Vec2<T> &v);

template <typename T> inline Vec2<T> cross(const Vec2<T> &v);

#ifdef _VT
# define _VT_SAVE _VT
# undef  _VT
#endif

#define _VT(...) template <typename T> __VA_ARGS__ Vec2<T>

// Constructors
_VT()::Vec2()                         {}
_VT()::Vec2(const T &x)               { v_[0] = x;    v_[1] = x;    }
_VT()::Vec2(const T &x, const T &y)   { v_[0] = x;    v_[1] = y;    }
_VT()::Vec2(const Vec2 &v)            { v_[0] = v[0]; v_[1] = v[1]; }

#undef _VT
#define _VT(...) template <typename T> inline __VA_ARGS__ Vec2<T>

_VT(T &)::operator[] (int i) { return v_[i]; }
_VT(const T &)::operator[] (int i) const { return v_[i]; }

_VT(T &)::x() { return v_[0]; }
_VT(const T &)::x() const { return v_[0]; }

_VT(T &)::y() { return v_[1]; }
_VT(const T &)::y() const { return v_[1]; }

  // Unary operators
_VT(Vec2<T> &)::operator += (const Vec2 &v) { v_[0] += v[0]; v_[1] += v[1]; return (*this); }
_VT(Vec2<T> &)::operator -= (const Vec2 &v) { v_[0] -= v[0]; v_[1] -= v[1]; return (*this); }
_VT(Vec2<T> &)::operator *= (const Vec2 &v) { v_[0] *= v[0]; v_[1] *= v[1]; return (*this); }
_VT(Vec2<T> &)::operator /= (const Vec2 &v) { v_[0] /= v[0]; v_[1] /= v[1]; return (*this); }

_VT(Vec2<T> &)::operator += (const T &x) { v_[0] += x; v_[1] += x; return (*this);  }
_VT(Vec2<T> &)::operator -= (const T &x) { v_[0] -= x; v_[1] -= x; return (*this);  }
_VT(Vec2<T> &)::operator *= (const T &x) { v_[0] *= x; v_[1] *= x; return (*this);  }
_VT(Vec2<T> &)::operator /= (const T &x) { v_[0] /= x; v_[1] /= x; return (*this);  }

_VT(bool)::operator <  (const Vec2 &v) const
{
  if (v_[0] < v[0]) return 1;
  if (v_[0] < v[0]) return 0;
  return (v_[1] < v[1]);
}
_VT(bool)::operator == (const Vec2 &v) const { return (v_[0] == v[0] && v_[1] == v[1]); }
_VT(bool)::operator != (const Vec2 &v) const { return (v_[0] != v[0] || v_[1] != v[1]); }

  // Arithmetic
_VT(Vec2<T>)::operator + (const Vec2 &v) const { Vec2<T> r; r[0] = v_[0] + v[0]; r[1] = v_[1] + v[1]; return r; }
_VT(Vec2<T>)::operator - (const Vec2 &v) const { Vec2<T> r; r[0] = v_[0] - v[0]; r[1] = v_[1] - v[1]; return r; }
_VT(Vec2<T>)::operator * (const Vec2 &v) const { Vec2<T> r; r[0] = v_[0] * v[0]; r[1] = v_[1] * v[1]; return r; }
_VT(Vec2<T>)::operator / (const Vec2 &v) const { Vec2<T> r; r[0] = v_[0] / v[0]; r[1] = v_[1] / v[1]; return r; }

_VT(Vec2<T>)::operator + (const T &x) const { Vec2<T> r; r[0] = v_[0] + x; r[1] = v_[1] + x; return r; }
_VT(Vec2<T>)::operator - (const T &x) const { Vec2<T> r; r[0] = v_[0] - x; r[1] = v_[1] - x; return r; }
_VT(Vec2<T>)::operator * (const T &x) const { Vec2<T> r; r[0] = v_[0] * x; r[1] = v_[1] * x; return r; }
_VT(Vec2<T>)::operator / (const T &x) const { Vec2<T> r; r[0] = v_[0] / x; r[1] = v_[1] / x; return r; }

_VT(Vec2<T>)::operator - () const { Vec2<T> r; r[0] = -v_[0]; r[1] = -v_[1]; return r; }

  // Assignment
_VT(Vec2<T> &)::operator = (const Vec2 &v) { v_[0] = v[0]; v_[1] = v[1]; return (*this);  }
_VT(Vec2<T> &)::operator = (const T &x)    { v_[0] = x   ; v_[1] = x;    return (*this);  }

_VT(T)::dot(const Vec2 &v) const { return v_[0] * v[0] + v_[1] * v[1];               }
_VT(T)::len()              const { return std::sqrt(v_[0] * v_[0] + v_[1] * v_[1]);  }
_VT(T)::sqrlen()           const { return v_[0] * v_[0] + v_[1] * v_[1];             }
_VT(void)::normalize()     { T l = len(); v_[0] /= l; v_[1] /= l;              }
_VT(Vec2<T>)::norm()       const { return ((*this) / len());                            }


template <typename T>
inline Vec2<T> cross(const Vec2<T> &v)
{
  Vec2<T> r;
  r[0] =  v[1];
  r[1] = -v[0];
  return r;
}


#undef _VT
#define _VT(_TYPE_) template <typename T> _TYPE_

_VT(std::ostream &) operator << (std::ostream &out, const Vec2<T> &v)
{ out << "[" << v[0] << " " << v[1] << "]"; return out; }

_VT(std::istream &) operator >> (std::istream &in, Vec2<T> &v)
{ in >> v[0] >> v[1]; return in; }

#undef _VT
#define _VT(_TYPE_) template <typename T, typename C> _TYPE_

_VT(Vec2<T>) operator + (const C &x, const Vec2<T> &v)
{ Vec2<T> r; r[0] = (T)x + v[0]; r[1] = (T)x + v[1]; return r; }

_VT(Vec2<T>) operator - (const C &x, const Vec2<T> &v)
{ Vec2<T> r; r[0] = (T)x - v[0]; r[1] = (T)x - v[1]; return r; }

_VT(Vec2<T>) operator * (const C &x, const Vec2<T> &v)
{ Vec2<T> r; r[0] = (T)x * v[0]; r[1] = (T)x * v[1]; return r; }

_VT(Vec2<T>) operator / (const C &x, const Vec2<T> &v)
{ Vec2<T> r; r[0] = (T)x / v[0]; r[1] = (T)x / v[1]; return r; }

#undef _VT

#ifdef _VT_SAVE
# define _VT _VT_SAVE
# undef  _VT_SAVE
#endif

#define Vec2f Vec2<float>  /* */
#define Vec2d Vec2<double> /* */
#define Vec2i Vec2<int>    /* */


} }

#endif
