#ifndef __SPHEREH__
#define __SPHEREH__
/*
hitable class sphere.
 */

#include <cmath>
#include "hitable.h"

class sphere: public hitable {
 public:
  sphere() {}
  sphere(vec3 cen, float r, material* m) : center(cen), radius(r), mat_ptr(m) {}
  virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const;
  virtual bool bounding_box(float t0, float t1, aabb& box) const;
  static void get_uv(const vec3& p, float& u, float& v);
  vec3 center;
  float radius;
  material* mat_ptr;
};

// t0, t1 are preserved here to keep the interface
bool sphere::bounding_box(float t0, float t1, aabb& box) const {
  box = aabb(center - vec3(radius, radius, radius),
             center + vec3(radius, radius, radius));
  return true;
}

void sphere::get_uv(const vec3& p, float& u, float& v) {
  float phi = atan2(p.z(), p.x());
  float theta = asin(p.y());
  u = 1 - (phi + M_PI) / (2*M_PI);
  v = (theta + M_PI/2) / (M_PI);
}

bool sphere::hit(const ray &r, float t_min, float t_max, hit_record &rec) const {
  vec3 oc = r.origin() - center;
  float a = dot(r.direction(), r.direction());
  float b = dot(oc, r.direction());
  float c = dot(oc, oc) - radius*radius;
  float discriminant = b*b - a*c;

  if (discriminant > 0) {
    float temp = (-b - sqrt(discriminant)) / a;
    // valid interval checking
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      get_uv((rec.p-center)/radius, rec.u, rec.v);
      rec.normal = (rec.p - center) / radius;
      rec.mat_ptr = mat_ptr;
      return true;
    }
    temp = (-b + sqrt(discriminant)) / a;
    // valid interval checking
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      get_uv((rec.p-center)/radius, rec.u, rec.v);
      rec.normal = (rec.p - center) / radius;
      rec.mat_ptr = mat_ptr;
      return true;
    }
  }
  return false;
}

class moving_sphere: public hitable {
 public:
  moving_sphere() {}
  moving_sphere(vec3 cen0, vec3 cen1, float t0, float t1, float r, material* m)
    : center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r), mat_ptr(m) {}

  virtual bool hit(const ray& r, float tmin, float max, hit_record& rec) const;
  virtual bool bounding_box(float t0, float t1, aabb& box) const;
  static void get_uv(const vec3& p, float& u, float& v);
  vec3 center(float time) const;

  vec3 center0, center1;
  float time0, time1;
  float radius;
  material* mat_ptr;
};

vec3 moving_sphere::center(float time) const {
  // assume linearly move
  return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}

bool moving_sphere::bounding_box(float t0, float t1, aabb& box) const {
  vec3 offset = vec3(radius, radius, radius);
  aabb box0(center(t0) - offset, center(t0) + offset);
  aabb box1(center(t1) - offset, center(t1) + offset);
  box = surrounding_box(box0, box1);
  return true;
}

void moving_sphere::get_uv(const vec3& p, float& u, float& v) {
  float phi = atan2(p.z(), p.x());
  float theta = asin(p.y());
  u = 1 - (phi + M_PI) / (2*M_PI);
  v = (theta + M_PI/2) / (M_PI);
}

bool moving_sphere::hit(const ray &r, float t_min, float t_max, hit_record& rec) const {
  // the ray is very fast
  vec3 oc = r.origin() - center(r.time());
  float a = dot(r.direction(), r.direction());
  float b = dot(oc, r.direction());
  float c = dot(oc, oc) - radius*radius;
  float discriminant = b*b - a*c;

  if (discriminant > 0) {
    float temp = (-b - sqrt(discriminant)) / a;
    // valid interval checking
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      get_uv((rec.p-center(r.time()))/radius, rec.u, rec.v);
      rec.normal = (rec.p - center(r.time())) / radius;
      rec.mat_ptr = mat_ptr;
      return true;
    }
    temp = (-b + sqrt(discriminant)) / a;
    // valid interval checking
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      get_uv((rec.p-center(r.time()))/radius, rec.u, rec.v);
      rec.normal = (rec.p - center(r.time())) / radius;
      rec.mat_ptr = mat_ptr;
      return true;
    }
  }
  return false;
}

#endif
