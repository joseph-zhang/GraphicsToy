#ifndef __VEC2H__
#define __VEC2H__
/*
The vec2 class.
To express an 2D vector.
 */

#include "vec3.h"

class vec2 {
 public:
  //constructor
  vec2() {}
  vec2(float e0, float e1) {
    vec = vec3(e0, e1, 0.);
  }
  vec2(vec3 v1) {
    vec = vec3(v1.x(), v1.y(), 0.);
  }

  //access methods
  inline float x() const { return vec.x(); }
  inline float y() const { return vec.y(); }

  inline const vec2& operator+() const { return *this; }
  inline vec2 operator-() const { return -vec; }
  inline float operator[](int i) const { return vec[i]; }
  inline float& operator[](int i) { return vec[i]; }

  inline vec2& operator+=(const vec2& v2);
  inline vec2& operator-=(const vec2& v2);
  inline vec2& operator*=(const vec2& v2);
  inline vec2& operator/=(const vec2& v2);

  inline vec2& operator*=(const float t);
  inline vec2& operator/=(const float t);

  inline float squared_length() const {
    return vec.squared_length();
  }

  inline float length() const {
    return sqrt(this->squared_length());
  }

  inline void make_unit_vector();

  vec3 vec;
};

inline std::istream& operator>>(std::istream& is, vec2& t) {
  is >> t.vec.e[0] >> t.vec.e[1];
  return is;
}

inline std::ostream& operator<<(std::ostream& os, vec2& t) {
  os << t.vec.e[0] << " " << t.vec.e[1];
  return os;
}

inline vec2 operator+(const vec2& v1, const vec2& v2) {
  return vec2(v1.vec + v2.vec);
}

inline vec2 operator-(const vec2& v1, const vec2& v2) {
  return vec2(v1.vec - v2.vec);
}

inline vec2 operator*(const vec2& v1, const vec2& v2) {
  return vec2(v1.vec * v2.vec);
}

inline vec2 operator/(const vec2& v1, const vec2& v2) {
  return vec2(v1.vec / v2.vec);
}

inline vec2 operator*(float t, const vec2& v) {
  return vec2(t * v.vec);
}

inline vec2 operator*(const vec2& v, float t) {
  return t * v;
}

inline vec2 operator/(const vec2& v, float t) {
  return vec2(v.vec / t);
}

inline float dot(const vec2& v1, const vec2& v2) {
  return dot(v1.vec, v2.vec);
}

inline vec2& vec2::operator+=(const vec2& v) {
  vec += v.vec;
  return *this;
}

inline vec2& vec2::operator-=(const vec2& v) {
  vec -= v.vec;
  return *this;
}

inline vec2& vec2::operator*=(const vec2& v) {
  vec *= v.vec;
  return *this;
}

inline vec2& vec2::operator/=(const vec2& v) {
  vec /= v.vec;
  return *this;
}

inline vec2& vec2::operator*=(const float t) {
  vec *= t;
  return *this;
}

inline vec2& vec2::operator/=(const float t) {
  vec /= t;
  return *this;
}

inline void vec2::make_unit_vector() {
  float k = 1.0 / this->length();
  this->operator*=(k);
}

inline vec2 unit_vector(vec2 v) {
  return unit_vector(v.vec);
}

#endif
