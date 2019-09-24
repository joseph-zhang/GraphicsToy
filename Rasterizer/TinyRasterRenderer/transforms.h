#ifndef __TRANSFORMS__
#define __TRANSFORMS__

/*
Affine transformations and Projection transformations.
 */

#include <cmath>
#include "camera.h"

// affine coordinate transform
inline void convertToCamera(const vec3& vertexWorld, const camera& cam, vec3& vertexCamera) {
  smatrix4 transMatrix = cam.worldToCam;
  transMatrix.multPoint(vertexWorld, vertexCamera);
}

// projection transform
inline void convertToNDC(const vec3& vertexWorld, const camera& cam, vec3& vertexNDC) {
  vec3 vertexCamera;
  // world -> camera
  convertToCamera(vertexWorld, cam, vertexCamera);
  smatrix4 transMatrix = cam.perspProj;
  // camera -> screen -> NDC, range [-1, 1], a canonical volume
  transMatrix.multPoint(vertexCamera, vertexNDC);
}

inline vec3 NDCToRaster(const vec3& vertexNDC, const int& imageWidth, const int& imageHeight) {
  float raster_x = (vertexNDC.x() + 1)/2 * imageWidth;
  // in raster space, y is down so invert direction
  float raster_y = (-vertexNDC.y() + 1)/2 * imageHeight;
  float raster_z = vertexNDC.z();

  // continuous point under raster coordinate
  return vec3(raster_x, raster_y, raster_z);
}

#endif
