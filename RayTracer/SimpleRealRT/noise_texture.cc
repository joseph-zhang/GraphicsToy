#include <iostream>
#include <cstdlib>
#include <iostream>
#include "bvh.h"
#include "noise.h"
#include "sphere.h"
#include "camera.h"
#include "texture.h"
#include "material.h"
#include "hitable_list.h"

vec3 color(const ray& r, hitable* world, int iter) {
  hit_record rec;
  if (world->hit(r, 0.001, MAXFLOAT, rec)) {
    ray scattered;
    vec3 attenuation;
    if (iter < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
      return attenuation * color(scattered, world, iter+1);
    } else {
      return vec3(0., 0., 0.);
    }
  } else {
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0-t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
  }
}

hitable* procedural_texture_scene() {
  texture* value_texture = new value_noise_texture(3.0);
  texture* perlin_netting_texture = new perlin_noise_texture(5.0, 1);
  texture* perlin_marble_texture = new perlin_noise_texture(3.0, 2);
  hitable** list = new hitable*[3];
  list[0] = new sphere(vec3(0,-1000,0), 1000, new lambertian(value_texture));
  list[1] = new sphere(vec3(2,2,0), 2, new lambertian(perlin_netting_texture));
  list[2] = new sphere(vec3(-2,2,0), 2, new lambertian(perlin_marble_texture));
  return new hitable_list(list, 3);
}

int main() {
  int nx = 1200;
  int ny = 800;
  // spp 10
  int ns = 10;

  std::cout << "P3\n" << nx << " " << ny << "\n255\n";
  hitable* world = procedural_texture_scene();

  // camera info.
  vec3 lookfrom(10, 2, 10);
  vec3 lookat(0, 1, 0);
  float dist_to_focus = 10.0;
  float aspect = float(nx) / float(ny);
  float vfov = 30;
  float aperture = 0.0;
  camera cam(lookfrom, lookat, vec3(0,1,0), vfov,
             aspect, aperture, dist_to_focus, 0.0, 1.0);

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
