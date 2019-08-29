#include <iostream>
#include "ray.h"

// function to test hit
bool hit_sphere(const vec3& center, float radius, const ray& r) {
  vec3 oc = r.origin() - center;
  float a = dot(r.direction(), r.direction());
  float b = 2.0 * dot(oc, r.direction());
  float c = dot(oc, oc) - radius*radius;
  float delta = b*b - 4*a*c;
  return (delta > 0);
}

vec3 color(const ray& r) {
  // place a sphere centered at (0,0,-1) and test hit
  if (hit_sphere(vec3(0,0,-1), 0.5, r)) {
    // if hit, set red color
    return vec3(1, 0, 0);
  }
  vec3 unit_direction = unit_vector(r.direction());
  //rescale to [0, 1]
  float t = 0.5 * (unit_direction.y() + 1.0);
  // color linear blend
  return (1.0-t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

int main() {
  int nx = 200;
  int ny = 100;
  std::cout << "P3\n" << nx << " " << ny << "\n255\n";

  // position of lower left corner of the screen
  vec3 lower_left_corner(-2.0, -1.0, -1.0);
  vec3 horizontal(4.0, 0.0, 0.0);
  vec3 vertical(0.0, 2.0, 0.0);
  //origin of the camera
  vec3 origin(0.0, 0.0, 0.0);

  for (int j = ny-1; j >= 0; j--) {
    for (int i = 0;i < nx; i++) {
      float u  = static_cast<float>(i) / static_cast<float>(nx);
      float v = static_cast<float>(j) / static_cast<float>(ny);
      ray r(origin, lower_left_corner + u*horizontal + v*vertical);
      vec3 colorx = color(r);
      int ir = static_cast<int>(255.99 * colorx[0]);
      int ig = static_cast<int>(255.99 * colorx[1]);
      int ib = static_cast<int>(255.99 * colorx[2]);

      std::cout << ir << " " << ig << " " << ib << "\n";
    }
  }
}
