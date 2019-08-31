#ifndef __CAMERAH__
#define __CAMERAH__
/*
Class camera
 */

#include "ray.h"

class camera {
 public:
  camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect) {
    vec3 u, v, w;

    // vertival field of view, the angle
    float theta = vfov * M_PI / 180;
    // assume the focal length = 1.0
    float half_height = tan(theta/2);
    float half_width = aspect * half_height;

    origin = lookfrom;
    // lookfrom and lookat together define w axis
    w = unit_vector(lookfrom - lookat);
    // vup defines camera roll
    u = unit_vector(cross(vup, w));
    // use cross product to get v axis
    v = cross(w, u);

    lower_left_corner = origin - half_width*u - half_height*v - w;
    horizontal = 2*half_width*u;
    vertical = 2*half_height*v;
  }

  ray get_ray(float u, float v) {
    return ray(origin, lower_left_corner + u*horizontal + v*vertical - origin);
  }

  vec3 origin;
  vec3 lower_left_corner;
  vec3 horizontal;
  vec3 vertical;
};

#endif
