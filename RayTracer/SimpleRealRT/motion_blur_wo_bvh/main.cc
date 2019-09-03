#include <iostream>
#include <cstdlib>
#include <iostream>
#include "sphere.h"
#include "camera.h"
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

hitable* random_scene() {
  int n = 500;
  hitable** list = new hitable*[n+1];
  // a very big sphere as ground
  list[0] = new sphere(vec3(0,-1000,0), 1000, new lambertian(vec3(0.5,0.5,0.5)));

  int i = 1;
  for (int a = -10; a < 10; a++) {
    for (int b = -10; b < 10; b++) {
      float choose_mat = drand48();
      vec3 center(a+0.9*drand48(), 0.2, b+0.9*drand48());
      if ((center-vec3(-4.0, 0.2, 0.0)).length() > 0.9 &&
          (center-vec3(0.0, 0.2, 0.0)).length() > 0.9 &&
          (center-vec3(4.0, 0.2, 0.0)).length() > 0.9) {
        if (choose_mat < 0.8) {
          list[i++] = new moving_sphere(center, center+vec3(0,0.5*drand48(),0),
                                        0.0, 1.0, 0.2,
                                        new lambertian(vec3(drand48()*drand48(),
                                                            drand48()*drand48(),
                                                            drand48()*drand48())));
        } else if (choose_mat < 0.95) {
          list[i++] = new sphere(center, 0.2,
                                 new metal(vec3(0.5*(1+drand48()),
                                                0.5*(1+drand48()),
                                                0.5*(1+drand48())),
                                           0.5*drand48()));
        } else {
          list[i++] = new sphere(center, 0.2,
                                 new dielectric(1.5));
        }
      }
    }
  }

  list[i++] = new sphere(vec3(0,1,0), 1.0, new dielectric(1.5));
  list[i++] = new sphere(vec3(4,1,0), 1.0, new lambertian(vec3(0.3, 0.5, 0.2)));
  list[i++] = new sphere(vec3(-4,1,0), 1.0, new metal(vec3(0.7,0.6,0.5), 0.3));

  return new hitable_list(list, i);
}

int main() {
  int nx = 1200;
  int ny = 800;
  int ns = 10;
  std::cout << "P3\n" << nx << " " << ny << "\n255\n";
  hitable* world = random_scene();

  // camera info.
  vec3 lookfrom(-13, 2, 3);
  vec3 lookat(0, 0, 0);
  float dist_to_focus = 10.0;
  float aspect = float(nx) / float(ny);
  float vfov = 20;
  float aperture = 0.1;
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
