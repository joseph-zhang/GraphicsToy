#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "camera.h"
#include "triangle.h"
#include "transforms.h"

// To Convenience, we just define triangles in head file.
// This should be avoided in serious projects!
#include "cow.hpp"

int main(int argc, char** argv) {
  int ntris = 3156;
  int imageWidth = 1200;
  int imageHeight = 900;
  float filmWidth = 0.980; // inch
  float filmHeight = 0.735; // inch
  float nearClippingPlane = 1;
  float farClippingPlane = 1000;
  float focalLength = 20; // mm

  /*
  // option1: use lookAt function to define camera.
  vec3 lookfrom(24.5, 23., 22.17);
  vec3 lookat(30.7469, 28.685, 28.42);
  vec3 vup(0, 1, 0);
  camera cam(lookfrom, lookat, vup, filmWidth, filmHeight, imageWidth, imageHeight,
             FitResolutionGate::kFill, nearClippingPlane, farClippingPlane, focalLength);
  */

  // option2: define camera by using camera-to-world matrix directly.
  smatrix4 camToWorld = { 0.707106562,  0.,          -0.707106562,  0.,
                          -0.331294409, 0.883452228, -0.331294409,  0.,
                          0.624694961,  0.468521928, 0.624694961,   0.,
                          24.4924700,   24.0063653,  22.1749848,    1.};
  camera cam(camToWorld, filmWidth, filmHeight, imageWidth, imageHeight,
             FitResolutionGate::kFill, nearClippingPlane, farClippingPlane, focalLength);

  int sampleNum = imageWidth * imageHeight;
  vec3*  framebuffer = new vec3[sampleNum];
  float* depthbuffer = new float[sampleNum];
  // initialize frame-buffer and depth-buffer
  for (int i = 0; i < sampleNum; i++) framebuffer[i] = vec3(255., 255., 255.);
  for (int i = 0; i < sampleNum; i++) depthbuffer[i] = farClippingPlane;

  // for every triangles
  for (int i = 0; i < ntris; i++) {
    const vec3& v0 = vertices[nvertices[i*3]];
    const vec3& v1 = vertices[nvertices[i*3 + 1]];
    const vec3& v2 = vertices[nvertices[i*3 + 2]];

    vec3 v0Raster, v1Raster, v2Raster;
    convertToNDC(v0, cam, v0Raster);
    convertToNDC(v1, cam, v1Raster);
    convertToNDC(v2, cam, v2Raster);
    v0Raster = NDCToRaster(v0Raster, imageWidth, imageHeight);
    v1Raster = NDCToRaster(v1Raster, imageWidth, imageHeight);
    v2Raster = NDCToRaster(v2Raster, imageWidth, imageHeight);

    // prepare vertex attributes
    vec2 st0 = st[stindices[i*3]];
    vec2 st1 = st[stindices[i*3 + 1]];
    vec2 st2 = st[stindices[i*3 + 2]];

    triangle tric(v0Raster, v1Raster, v2Raster);
    // the whole triangle is out of screen (test by bbox)
    if (tric.xmin > imageWidth-1 || tric.xmax < 0 || tric.ymin > imageHeight-1 || tric.ymax < 0) continue;

    // be careful xmin/xmax/ymin/ymax can be negative.
    // get sampling region.
    int x0 = std::max(static_cast<int>(0), static_cast<int>(std::floor(tric.xmin)));
    int x1 = std::min(static_cast<int>(imageWidth)-1, static_cast<int>(std::floor(tric.xmax)));
    int y0 = std::max(static_cast<int>(0), static_cast<int>(std::floor(tric.ymin)));
    int y1 = std::min(static_cast<int>(imageHeight)-1, static_cast<int>(std::floor(tric.ymax)));

    // render triangle in (clipped) bbox
    for (int y = y0; y <= y1; y++) {
      for (int x = x0; x <= x1; x++) {
        vec3 samplePoint(x+0.5, y+0.5, 0.);
        float w0, w1, w2, depth;

        // coverage test
        if (!tric.barycen(samplePoint, w0, w1, w2, depth)) continue;

        // depth-buffer test
        if (depth < depthbuffer[y*imageWidth + x]) {
          depthbuffer[y*imageWidth + x] = depth;
          vec3 st = perspCorrInterp(tric, st0.vec, st1.vec, st2.vec, w0, w1, w2, depth);

          // treat the camera position as a kind of attribute, then interpolate it.
          vec3 v0Cam, v1Cam, v2Cam;
          convertToCamera(v0, cam, v0Cam);
          convertToCamera(v1, cam, v1Cam);
          convertToCamera(v2, cam, v2Cam);
          vec3 pt = perspCorrInterp(tric, v0Cam, v1Cam, v2Cam, w0, w1, w2, depth);

          vec3 normal = cross((v1Cam-v0Cam), (v2Cam-v0Cam));
          normal.make_unit_vector();
          vec3 viewDirection = -pt;
          viewDirection.make_unit_vector();
          float nDotView = std::max(0.f, dot(normal, viewDirection));

          const int M = 10;
          float checker = (fmod(st.x() * M, 1.0) > 0.5) ^ (fmod(st.y() * M, 1.0) < 0.5);
          float c = 0.3 * (1 - checker) + 0.7 * checker;
          nDotView *= c;
          framebuffer[y*imageWidth + x] = vec3(1,1,1) * static_cast<unsigned char>(nDotView * 255.99);
        }
      }
    }
  }

  std::ofstream ofs;
  ofs.open("./raster3d.ppm");
  ofs << "P3\n" << imageWidth << " " << imageHeight << "\n255\n";
  // save as ppm file
  for (int j = 0; j < imageHeight; j++) {
    for (int i = 0; i < imageWidth; i++) {
      ofs << framebuffer[j*imageWidth + i][0] << " "
          << framebuffer[j*imageWidth + i][1] << " "
          << framebuffer[j*imageWidth + i][2] << "\n";
    }
  }
  ofs.close();
  delete []framebuffer;
  delete []depthbuffer;

  return 0;
}
