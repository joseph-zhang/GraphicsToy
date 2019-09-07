#include <cstdlib>
#include <iostream>
#include <iostream>
#include "bvh.h"
#include "noise.h"
#include "aarect.h"
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
    vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
    if (iter < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
      return emitted + attenuation * color(scattered, world, iter+1);
    } else {
      return emitted;
    }
  } else {
    // no indirect environment light
    return vec3(0., 0., 0.);
  }
}

hitable* simple_light() {
  texture* pertext = new perlin_noise_texture(4, 2);
  hitable** list = new hitable*[4];
  list[0] =  new sphere(vec3(0,-1000, 0), 1000, new lambertian( pertext ));
  list[1] =  new sphere(vec3(0, 2, 0), 2, new lambertian( pertext ));
  // week red light
  list[2] =  new xz_rect(-1, 1, -1, 1, 5, new diffuse_light(new constant_texture(vec3(1,0,0))));
  // strong white light
  list[3] =  new xy_rect(3, 5, 1, 3, -2, new diffuse_light(new constant_texture(vec3(4,4,4))));
  return new hitable_list(list,4);
}

int main() {
  int nx = 1200;
  int ny = 800;
  // spp 100
  int ns = 100;

  std::cout << "P3\n" << nx << " " << ny << "\n255\n";
  hitable* world = simple_light();

  // camera info.
  vec3 lookfrom(18, 7, 10);
  vec3 lookat(0, 2, 0);
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
