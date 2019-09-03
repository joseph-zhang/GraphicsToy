#ifndef __MATERIAL__
#define __MATERIAL__

#include "ray.h"
#include "texture.h"
#include "hitable.h"

vec3 random_in_unit_sphere() {
  vec3 p;
  do {
    p = 2.0 * vec3(drand48(), drand48(), drand48()) - vec3(1,1,1);
  } while (p.squared_length() >= 1.0);
  return p;
}

vec3 reflect(const vec3& v, const vec3& n) {
  return v - 2 * dot(v, n) * n;
}

bool refract(const vec3& v, const vec3& n, float ni_ov_nt, vec3& refracted) {
  vec3 uv = unit_vector(v);
  float dt = dot(uv, n);
  // \cos^2\theta' = 1 - (\frac{n}{n'})^2 (1 - (\hat{i}\cdot \hat{n})^2)
  float discriminant = 1.0 - ni_ov_nt*ni_ov_nt * (1-dt*dt);
  if (discriminant > 0.) {
    refracted = ni_ov_nt * (uv - dt*n) - sqrt(discriminant)*n;
    return true;
  } else {
    return false;
  }
}

float schlick(float cosine, float ref_idx) {
  // we assume the \eta_1 is always air
  float f0 = (1-ref_idx) / (1+ref_idx);
  f0 = f0 * f0;
  return f0 + (1-f0) * pow((1-cosine), 5);
}

class material {
 public:
  virtual bool scatter(const ray& r_in, const hit_record& rec,
                       vec3& attenuation, ray& scattered) const = 0;
};

// Diffuse
class lambertian : public material {
 public:
  lambertian(texture* a) : albedo(a) {}
  virtual bool scatter(const ray& r_in, const hit_record& rec,
                       vec3& attenuation, ray& scattered) const {
    vec3 target = rec.p + rec.normal + random_in_unit_sphere();
    scattered = ray(rec.p, target - rec.p, r_in.time());
    attenuation = albedo->value(rec.u, rec.v, rec.p);
    return true;
  }

  texture* albedo;
};

// Metal
class metal : public material {
 public:
  metal(const vec3& a, float f) : albedo(a) {
    if (f < 1) fuzz = f; else fuzz = 1;
  }
  virtual bool scatter(const ray& r_in, const hit_record& rec,
                       vec3& attenuation, ray& scattered) const {
    vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
    scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere(), r_in.time());
    attenuation = albedo;
    return (dot(scattered.direction(), rec.normal) > 0);
  }

  vec3 albedo;
  // fuzzier parameter
  float fuzz;
};

// Dielectric
class dielectric : public material {
 public:
  dielectric(float ri) : ref_idx(ri) {}
  virtual bool scatter(const ray& r_in, const hit_record& rec,
                       vec3& attenuation, ray& scattered) const {
    vec3 outward_normal;
    float ni_ov_nt;
    // the glass surface absorbs nothing
    attenuation = vec3(1., 1., 1.);
    vec3 reflected = reflect(r_in.direction(), rec.normal);
    vec3 refracted;

    float reflect_prob;
    float cosine;

    // from dielectric material
    if (dot(r_in.direction(), rec.normal) > 0) {
      outward_normal = -rec.normal;
      ni_ov_nt = ref_idx;
      cosine = dot(r_in.direction(), rec.normal) / r_in.direction().length();
      cosine = sqrt(1 - ref_idx*ref_idx*(1-cosine*cosine));
    } else {
      // from air
      outward_normal = rec.normal;
      ni_ov_nt = 1.0 / ref_idx;
      cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
    }

    // schlick's approximation
    if (refract(r_in.direction(), outward_normal, ni_ov_nt, refracted)) {
      reflect_prob = schlick(cosine, ref_idx);
    } else {
      reflect_prob = 1.0;
    }

    if (drand48() < reflect_prob) {
      scattered = ray(rec.p, reflected, r_in.time());
    } else {
      scattered = ray(rec.p, refracted, r_in.time());
    }

    return true;
  }

  float ref_idx;
};

#endif
