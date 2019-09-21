#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include "vec3.h"
#include "vec2.h"

inline float edgeFunction(const vec2& a, const vec2& b, const vec2& c) {
  vec2 ac = c - a;
  vec2 ab = b - a;
  return cross(ac, ab);
}

int main(int argc, char** argv) {
  // image resolution
  const int nx = 512;
  const int ny = 512;

  // define a right triangle under NDC coordinate, range [0, 1]
  // clock-wise -> v0, v1, v2
  vec2 v0(0.2, 0.2);
  vec2 v1(0.2, 0.8);
  vec2 v2(0.8, 0.2);

  // define the color of triangle vertices
  vec3 c0(1, 0, 0);
  vec3 c1(0, 1, 0);
  vec3 c2(0, 0, 1);

  // frame buffer to store image
  vec3* framebuffer = new vec3[nx * ny];

  float s = edgeFunction(v0, v1, v2);
  for (int j = ny-1; j >= 0; j--) {
    for (int i = 0; i < nx; i++) {
      // for every sample point on canvas
      vec2 p((i + 0.5)/nx, (ny-j-1 + 0.5)/ny);
      float w0 = edgeFunction(v1, v2, p);
      float w1 = edgeFunction(v2, v0, p);
      float w2 = edgeFunction(v0, v1, p);

      if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
        w0 /= s; w1 /= s; w2 /= s;
        // linear interpolation
        float r = w0 * c0[0] + w1 * c1[0] + w2 * c2[0];
        float g = w0 * c0[1] + w1 * c1[1] + w2 * c2[1];
        float b = w0 * c0[2] + w1 * c1[2] + w2 * c2[2];

        // set the pixel in the image to the triangle's color
        framebuffer[j*nx + i][0] = static_cast<unsigned char>(r*255.99);
        framebuffer[j*nx + i][1] = static_cast<unsigned char>(g*255.99);
        framebuffer[j*nx + i][2] = static_cast<unsigned char>(b*255.99);
      }
    }
  }

  std::ofstream ofs;
  ofs.open("./raster2d.ppm");
  ofs << "P3\n" << nx << " " << ny << "\n255\n";

  // save as ppm file
  for (int j = 0; j < ny; j++) {
    for (int i = 0; i < nx; i++) {
      ofs << framebuffer[j*nx + i][0] << " "
          << framebuffer[j*nx + i][1] << " "
          << framebuffer[j*nx + i][2] << "\n";
    }
  }

  ofs.close();
  delete []framebuffer;
  return 0;
}
