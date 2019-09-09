#ifndef __HITABLEH__
#define __HITABLEH__
/*
  Class hitable.
  abstract class "Hitable" for anything a ray might hit.
 */

#include <cfloat>
#include "ray.h"
#include "aabb.h"
class material;

struct hit_record {
  float t;
  float u;
  float v;
  vec3 p;
  vec3 normal;
  material* mat_ptr;
};

class hitable {
 public:
  virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
  virtual bool bounding_box(float t0, float t1, aabb& box) const = 0;
};

class flip_normals : public hitable {
 public:
  flip_normals(hitable* p) : ptr(p) {}
  virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    if (ptr->hit(r, t_min, t_max, rec)) {
      rec.normal = -rec.normal;
      return true;
    } else {
      return false;
    }
  }

  virtual bool bounding_box(float t0, float t1, aabb& box) const {
    return ptr->bounding_box(t0, t1, box);
  }

  hitable* ptr;
};

class translate : public hitable {
 public:
  translate(hitable* p, const vec3& displacement) : ptr(p), offset(displacement) {}
  virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
  virtual bool bounding_box(float t0, float t1, aabb& box) const;
  hitable* ptr;
  vec3 offset;
};

bool translate::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
  // move the rays in the opposite direction
  ray moved_r(r.origin() - offset, r.direction(), r.time());
  if (ptr->hit(moved_r, t_min, t_max, rec)) {
    rec.p += offset;
    return true;
  } else {
    return false;
  }
}

bool translate::bounding_box(float t0, float t1, aabb& box) const {
  if (ptr->bounding_box(t0, t1, box)) {
    // the bounding box is actually moved
    box = aabb(box.min()+offset, box.max()+offset);
    return true;
  } else {
    return false;
  }
}

class rotate_y : public hitable {
 public:
  rotate_y(hitable* p, float angle);
  virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
  virtual bool bounding_box(float t0, float t1, aabb& box) const {
    box = bbox;
    return hasbox;
  }

  hitable* ptr;
  float sin_theta;
  float cos_theta;
  bool hasbox;
  aabb bbox;
};

// find bounding box
rotate_y::rotate_y(hitable* p, float angle) : ptr(p) {
  float radians = (M_PI / 180.) * angle;
  sin_theta = sin(radians);
  cos_theta = cos(radians);
  hasbox = ptr->bounding_box(0, 1, bbox);
  vec3 min(FLT_MAX, FLT_MAX, FLT_MAX);
  vec3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

  // rotate bounding box's 8 points
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      for (int k = 0; k < 2; k++) {
        float x = i*bbox.max().x() + (1-i)*bbox.min().x();
        float y = j*bbox.max().y() + (1-j)*bbox.min().y();
        float z = k*bbox.max().z() + (1-k)*bbox.min().z();
        // the bounding box is actually rotated
        float newx = cos_theta*x + sin_theta*z;
        float newz = -sin_theta*x + cos_theta*z;

        // find the two corners of bbox
        vec3 tester(newx, y, newz);
        for (int c = 0; c < 3; c++) {
          if (tester[c] > max[c]) max[c] = tester[c];
          if (tester[c] < min[c]) min[c] = tester[c];
        }
      }
    }
  }
  bbox = aabb(min, max);
}

bool rotate_y::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
  vec3 origin = r.origin();
  vec3 direction = r.direction();
  // rotate ray in opposite direction
  origin[0] = cos_theta*r.origin()[0] - sin_theta*r.origin()[2];
  origin[2] = sin_theta*r.origin()[0] + cos_theta*r.origin()[2];
  direction[0] = cos_theta*r.direction()[0] - sin_theta*r.direction()[2];
  direction[2] = sin_theta*r.direction()[0] + cos_theta*r.direction()[2];
  ray rotated_r(origin, direction, r.time());

  if (ptr->hit(rotated_r, t_min, t_max, rec)) {
    vec3 p = rec.p;
    vec3 normal = rec.normal;
    p[0] = cos_theta*rec.p[0] + sin_theta*rec.p[2];
    p[2] = -sin_theta*rec.p[0] + cos_theta*rec.p[2];

    // note that normal transform is covariant,
    // but rotation matrix is orthogonal.
    normal[0] = cos_theta*rec.normal[0] + sin_theta*rec.normal[2];
    normal[2] = -sin_theta*rec.normal[0] + cos_theta*rec.normal[2];

    rec.p = p;
    rec.normal = normal;
    return true;
  } else {
    return false;
  }
}

#endif
