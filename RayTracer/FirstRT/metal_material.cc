#include <cfloat>
#include <cstdlib>
#include <iostream>
#include "sphere.h"
#include "material.h"
#include "camera_naive.h"
#include "hitable_list.h"

vec3 color(const ray& r, hitable* world, int iter) {
  hit_record rec;
  if (world->hit(r, 0.001, MAXFLOAT, rec)) {
    ray scattered;
    vec3 attenuation;
    if (iter < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
      // reflection
      return attenuation * color(scattered, world, iter+1);
    } else {
      return vec3(0, 0, 0);
    }
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
  hitable *list[4];
  // a small lambertian sphere located at the screen center
  list[0] = new sphere(vec3(0., 0., -2.), 0.5, new lambertian(vec3(0.8, 0.3, 0.3)));
  // a big lambertian sphere
  list[1] = new sphere(vec3(0.,-100.5, -2.), 100., new lambertian(vec3(0.8, 0.8, 0.0)));
  // a metal sphere
  list[2] = new sphere(vec3(1., 0., -2.), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.3));
  // another metal sphere with different albedo
  list[3] = new sphere(vec3(-1., 0., -2.), 0.5, new metal(vec3(0.8, 0.8, 0.8), 0.8));
  hitable *world = new hitable_list(list, 4);

  // the camera setting the same as Basic: camera cam;
  // keep the camera horizontally.
  camera cam(vec3(0.,0.,0.), vec3(0.,0.,-1.), vec3(0.,1.,0.), 90, 2.);

  for (int j = ny-1; j >= 0; j--) {
    for (int i = 0; i < nx; i++) {
      vec3 colorx(0, 0, 0);

      // for a given pixel, we have several samples,
      // and send rays through each of the samples.
      for(int s = 0; s < ns; s++) {
        float u  = static_cast<float>(i + drand48()) / static_cast<float>(nx);
        float v = static_cast<float>(j + drand48()) / static_cast<float>(ny);
        ray r = cam.get_ray(u, v);
        colorx += color(r, world, 0);
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
