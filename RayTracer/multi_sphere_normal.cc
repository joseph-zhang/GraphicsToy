#include <iostream>
#include <cfloat>
#include "sphere.h"
#include "hitable_list.h"

vec3 color(const ray &r, hitable* world) {
  hit_record rec;
  if (world->hit(r, 0.0, MAXFLOAT, rec)) {
    return 0.5 * vec3(rec.normal.x()+1, rec.normal.y()+1, rec.normal.z()+1);
  } else {
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0-t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
  }
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

  hitable *list[2];
  // a small sphere located at the screen center
  list[0] = new sphere(vec3(0.,0.,-1.), 0.5);
  //a big sphere
  list[1] = new sphere(vec3(0.,-100.5,-1), 100.);
  hitable *world = new hitable_list(list, 2);

  for (int j = ny-1; j >= 0; j--) {
    for (int i = 0;i < nx; i++) {
      float u  = static_cast<float>(i) / static_cast<float>(nx);
      float v = static_cast<float>(j) / static_cast<float>(ny);
      ray r(origin, lower_left_corner + u*horizontal + v*vertical);

      vec3 colorx = color(r, world);
      int ir = static_cast<int>(255.99 * colorx[0]);
      int ig = static_cast<int>(255.99 * colorx[1]);
      int ib = static_cast<int>(255.99 * colorx[2]);

      std::cout << ir << " " << ig << " " << ib << "\n";
    }
  }
}
