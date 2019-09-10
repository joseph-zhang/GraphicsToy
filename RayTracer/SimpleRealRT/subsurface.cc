#include <cstdlib>
#include <iostream>
#include <iostream>
#include "bvh.h"
#include "noise.h"
#include "block.h"
#include "aarect.h"
#include "sphere.h"
#include "camera.h"
#include "texture.h"
#include "material.h"
#include "hitable_list.h"
#include "constant_medium.h"

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

hitable* cornell_box_subsurface() {
    hitable** list = new hitable*[10];
    int i = 0;
    material* red = new lambertian( new constant_texture(vec3(0.65, 0.05, 0.05)) );
    material* white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)) );
    material* green = new lambertian( new constant_texture(vec3(0.12, 0.45, 0.15)) );
    material* light = new diffuse_light( new constant_texture(vec3(7, 7, 7)) );

    // cornell_box
    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new xz_rect(113, 443, 127, 432, 554, light); // use a bigger and dimmer light
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));

    // subsurface
    hitable* s1 = new sphere(vec3(360, 120, 270), 120, new dielectric(1.6));
    hitable* s2 = new sphere(vec3(180, 65, 140), 65, new dielectric(1.6));
    list[i++] = s1;
    list[i++] = s2;
    list[i++] = new constant_medium(s1, 0.08, new constant_texture(vec3(0.2, 0.4, 0.9))); // blue jade
    list[i++] = new constant_medium(s2, 0.18, new constant_texture(vec3(0.2, 0.8, 0.4))); // green jade
    return new hitable_list(list, i);
}

int main() {
  int nx = 1200;
  int ny = 800;
  // spp 1200
  int ns = 1200;

  std::cout << "P3\n" << nx << " " << ny << "\n255\n";
  hitable* world = cornell_box_subsurface();

  // camera info.
  // Note that we look to z direction this time.
  vec3 lookfrom(278, 278, -800);
  vec3 lookat(278, 278, 0);
  float dist_to_focus = 10.0;
  float aspect = float(nx) / float(ny);
  float vfov = 40;
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
