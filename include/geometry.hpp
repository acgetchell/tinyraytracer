//
// Created by Adam Getchell on 2019-02-09.
// Inspired by https://github.com/ssloy/tinyraytracer
//

#ifndef TINYRAYTRACER_GEOMETRY_HPP
#define TINYRAYTRACER_GEOMETRY_HPP

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

template <size_t DIM, typename T>
struct vec
{
  vec()
  {
    for (auto i = DIM; i--; data_[i] = T{})
      ;
  }
  T& operator[](size_t const i)
  {
    assert(i < DIM);
    return data_[i];
  }
  T const& operator[](size_t const i) const
  {
    assert(i < DIM);
    return data_[i];
  }

 private:
  T data_[DIM];
};

using Vec2f = vec<2, float>;
using Vec3f = vec<3, float>;
using Vec3i = vec<3, int>;
using Vec4f = vec<4, float>;

template <typename T>
struct vec<2, T>
{
  vec() : x{T{}}, y{T{}} {}
  vec(T X, T Y) : x{X}, y{Y} {}
  template <class U>
  vec<2, T>(vec<2, U> const& v);
  T& operator[](size_t const i)
  {
    assert(i < 2);
    return i <= 0 ? x : y;
  }
  T const& operator[](size_t const i) const
  {
    assert(i < 2);
    return i <= 0 ? x : y;
  }

  T x, y;
};

template <typename T>
struct vec<3, T>
{
  vec() : x{T{}}, y{T{}}, z{T{}} {}
  vec(T X, T Y, T Z) : x{X}, y{Y}, z{Z} {}
  T& operator[](size_t const i)
  {
    assert(i < 3);
    return i <= 0 ? x : (1 == i ? y : z);
  }
  T const& operator[](size_t const i) const
  {
    assert(i < 3);
    return i <= 0 ? x : (1 == i ? y : z);
  }
  float      norm() { return std::sqrt(x * x + y * y + z * z); }
  vec<3, T>& normalize(T l = 1)
  {
    *this = (*this) * (l / norm());
    return *this;
  }

  T x, y, z;
};

template <typename T>
struct vec<4, T>
{
  vec() : x{T{}}, y{T{}}, z{T{}}, w{T{}} {}
  vec(T X, T Y, T Z, T W) : x{X}, y{Y}, z{Z}, w{W} {}
  T& operator[](size_t const i)
  {
    assert(i < 4);
    return i <= 0 ? x : (1 == i ? y : (2 == i ? z : w));
  }
  T const& operator[](size_t const i) const
  {
    assert(i < 4);
    return i <= 0 ? x : (1 == i ? y : (2 == i ? z : w));
  }

  T x, y, z, w;
};

template <size_t DIM, typename T>
T operator*(vec<DIM, T> const& lhs, vec<DIM, T> const& rhs)
{
  T ret = T();
  for (auto i = DIM; i--; ret += lhs[i] * rhs[i])
    ;
  return ret;
}

template <size_t DIM, typename T>
vec<DIM, T> operator+(vec<DIM, T> lhs, vec<DIM, T> const& rhs)
{
  for (auto i = DIM; i--; lhs[i] += rhs[i])
    ;
  return lhs;
}

template <size_t DIM, typename T>
vec<DIM, T> operator-(vec<DIM, T> lhs, vec<DIM, T> const& rhs)
{
  for (auto i = DIM; i--; lhs[i] -= rhs[i])
    ;
  return lhs;
}

template <size_t DIM, typename T, typename U>
vec<DIM, T> operator*(vec<DIM, T> const& lhs, U const& rhs)
{
  vec<DIM, T> ret;
  for (auto i = DIM; i--; ret[i] = lhs[i] * rhs)
    ;
  return ret;
}

template <size_t DIM, typename T>
vec<DIM, T> operator-(vec<DIM, T> const& lhs)
{
  return lhs * T(-1);
}

template <typename T>
vec<3, T> cross(vec<3, T> v1, vec<3, T> v2)
{
  return vec<3, T>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z,
                   v1.x * v2.y - v1.y * v2.x);
}

template <size_t DIM, typename T>
std::ostream& operator<<(std::ostream& out, vec<DIM, T> const& v)
{
  for (auto i = 0; i < DIM; i++) out << v[i] << " ";
  return out;
}

#endif  // TINYRAYTRACER_GEOMETRY_HPP
