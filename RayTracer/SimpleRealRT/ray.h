#ifndef __RAYH__
#define __RAYH__
/*
Class: Ray
 */

#include "vec3.h"

class ray {
 public:
  ray() {}
  ray(const vec3 &a, const vec3 &b, float ti = 0.0)
    : A(a), B(b), _time(ti) {}

  // access
  vec3 origin() const { return A; }
  vec3 direction() const { return B; }
  vec3 point_at_parameter(const float t) const { return A + t*B; }
  float time() const { return _time; }

  vec3 A;
  vec3 B;
  float _time;
};

#endif
