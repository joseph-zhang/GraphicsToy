#ifndef __CAMERAH__
#define __CAMERAH__
/*
Class camera
 */

#include "ray.h"

vec3 random_in_unit_disk() {
  vec3 p;
  do {
    p = 2.0 * vec3(drand48(), drand48(), 0) - vec3(1, 1, 0);
  } while (dot(p, p) >= 1.0);
  return p;
}

class camera {
 public:
  camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov,
         float aspect, float aperture, float focus_dist, float t0, float t1) {
    time0 = t0; time1 = t1;
    lens_radius = aperture / 2.;
    // vertival field of view, the angle
    float theta = vfov * M_PI / 180;
    // unit half_height, when focus_dist = 1.0
    float half_height = tan(theta/2);
    float half_width = aspect * half_height;

    origin = lookfrom;
    // lookfrom and lookat together define w axis
    w = unit_vector(lookfrom - lookat);
    // vup defines camera roll
    u = unit_vector(cross(vup, w));
    // use cross product to get v axis
    v = cross(w, u);

    lower_left_corner = origin - (half_width*focus_dist)*u - (half_height*focus_dist)*v - focus_dist*w;
    // given a specific focus distance
    horizontal = 2 * (half_width*focus_dist) * u;
    vertical = 2 * (half_height*focus_dist) * v;
  }

  ray get_ray(float s, float t) {
    // defocus blur
    vec3 rd = lens_radius * random_in_unit_disk();
    vec3 offset = u * rd.x() + v * rd.y();
    // time sample during shutter open
    float time = time0 + drand48()*(time1 - time0);
    return ray(origin + offset,
               lower_left_corner + s*horizontal + t*vertical - (origin + offset),
               time);
  }

  vec3 origin;
  vec3 lower_left_corner;
  vec3 horizontal;
  vec3 vertical;
  vec3 u, v, w;
  float lens_radius;
  float time0, time1;
};

#endif
