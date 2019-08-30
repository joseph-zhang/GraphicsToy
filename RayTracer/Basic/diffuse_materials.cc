#include <cfloat>
#include <cstdlib>
#include <iostream>
#include "sphere.h"
#include "camera.h"
#include "hitable_list.h"

vec3 random_in_unit_sphere() {
  vec3 p;
  // find a sample in unit sphere
  do {
    // scale to [-1, 1]
    p = 2.0 * vec3(drand48(), drand48(), drand48()) - vec3(1., 1., 1.);
  } while (p.squared_length() >= 1.0);
  return p;
}

vec3 color(const ray &r, hitable* world) {
  hit_record rec;
  // get rid of the shadow acne, set t_min=0.001
  if (world->hit(r, 0.001, MAXFLOAT, rec)) {
    vec3 target = rec.p + rec.normal + random_in_unit_sphere();
    // reflection with half energy
    return 0.5 * color(ray(rec.p, target - rec.p), world);
  } else {
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0-t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
  }
}

int main() {
  int nx = 200;
  int ny = 100;
  int ns = 100;

  std::cout << "P3\n" << nx << " " << ny << "\n255\n";
  hitable *list[2];
  // a small sphere located at the screen center
  list[0] = new sphere(vec3(0.,0.,-1.), 0.5);
  //a big sphere
  list[1] = new sphere(vec3(0.,-100.5,-1), 100.);
  hitable *world = new hitable_list(list, 2);
  camera cam;

  for (int j = ny-1; j >= 0; j--) {
    for (int i = 0; i < nx; i++) {
      vec3 colorx(0, 0, 0);

      // for a given pixel, we have several samples,
      // and send rays through each of the samples.
      for(int s = 0; s < ns; s++) {
        float u  = static_cast<float>(i + drand48()) / static_cast<float>(nx);
        float v = static_cast<float>(j + drand48()) / static_cast<float>(ny);
        ray r = cam.get_ray(u, v);
        colorx += color(r, world);
      }
      colorx /= static_cast<float>(ns);

      // Gamma correction with gamma=2
      colorx = vec3( sqrt(colorx[0]), sqrt(colorx[1]), sqrt(colorx[2]) );
      int ir = static_cast<int>(255.99 * colorx[0]);
      int ig = static_cast<int>(255.99 * colorx[1]);
      int ib = static_cast<int>(255.99 * colorx[2]);

      std::cout << ir << " " << ig << " " << ib << "\n";
    }
  }
}
