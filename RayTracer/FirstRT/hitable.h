#ifndef __HITABLEH__
#define __HITABLEH__
/*
  Class hitable.
  abstract class "Hitable" for anything a ray might hit.
 */

#include "ray.h"
class material;

struct hit_record {
  float t;
  vec3 p;
  vec3 normal;
  material* mat_ptr;
};

class hitable {
 public:
  virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const = 0;
};

#endif
