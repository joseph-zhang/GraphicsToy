#ifndef __CMEDH__
#define __CMEDH__

#include <cfloat>
#include "hitable.h"
#include "material.h"
#include "texture.h"

class constant_medium : public hitable {
 public:
  constant_medium(hitable* b, float d, texture* a) : boundary(b), density(d) {
    phase_function = new isotropic(a);
  }
  virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
  virtual bool bounding_box(float t0, float t1, aabb& box) const {
    return boundary->bounding_box(t0, t1, box);
  }

  hitable* boundary;
  float density;
  material* phase_function;
};

bool constant_medium::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
  hit_record rec1, rec2;
  if (boundary->hit(r, -FLT_MAX, FLT_MAX, rec1)) {
    if (boundary->hit(r, rec1.t+0.0001, FLT_MAX, rec2)) {
      if (rec1.t < t_min) rec1.t = t_min;
      if (rec2.t > t_max) rec2.t = t_max;
      if (rec1.t >= rec2.t) return false;
      if (rec1.t < 0) rec1.t = 0;

      float distance_inside_boundary = (rec2.t - rec1.t) * r.direction().length();
      float hit_distance = -(1/density) * log(drand48());

      // If that distanceis inside the volume
      if (hit_distance < distance_inside_boundary) {
        rec.t = rec1.t + hit_distance / r.direction().length();
        rec.p = r.point_at_parameter(rec.t);
        rec.normal = vec3(1,0,0); // no need (because of isotropic), any one is ok
        rec.mat_ptr = phase_function;
        return true;
      }
    }
  }
  return false;
}

#endif
