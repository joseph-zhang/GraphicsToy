#ifndef __TRIANGLEH__
#define __TRIANGLEH__

#include <algorithm>
#include <cmath>
#include "vec2.h"
#include "vec3.h"

inline float min3(const float &a, const float &b, const float &c) {
  return std::min(a, std::min(b, c));
}

inline float max3(const float &a, const float &b, const float &c) {
  return std::max(a, std::max(b, c));
}

float edgeFunction(const vec3& a, const vec3& b, const vec3& c) {
  vec2 v0 = vec2(a); vec2 v1 = vec2(b); vec2 v2 = vec2(c);
  vec2 ac = v2 - v0;
  vec2 ab = v1 - v0;
  return cross(ac, ab);
}

class triangle {
 public:
  triangle() {}
  triangle(vec3 v0, vec3 v1, vec3 v2) {
    // Precompute reciprocal of vertex z-coordinate for later interpolation
    v0Raster = vec3(v0.x(), v0.y(), 1/v0.z());
    v1Raster = vec3(v1.x(), v1.y(), 1/v1.z());
    v2Raster = vec3(v2.x(), v2.y(), 1/v2.z());

    xmin = min3(v0Raster.x(), v1Raster.x(), v2Raster.x());
    ymin = min3(v0Raster.y(), v1Raster.y(), v2Raster.y());
    xmax = max3(v0Raster.x(), v1Raster.x(), v2Raster.x());
    ymax = max3(v0Raster.y(), v1Raster.y(), v2Raster.y());
  }

  // compute the barycentric coordinate of a sample point
  bool barycen(const vec3& samplePoint, float& w0, float& w1, float& w2, float& depth) {
    w0 = edgeFunction(v1Raster, v2Raster, samplePoint);
    w1 = edgeFunction(v2Raster, v0Raster, samplePoint);
    w2 = edgeFunction(v0Raster, v1Raster, samplePoint);
    float s = edgeFunction(v0Raster, v1Raster, v2Raster);

    // samplePoint in the triangle
    if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
      w0 /= s; w1 /= s; w2 /= s;
      // interpolation of 1/z using barycentric coordinates
      float oneOverDepth = v0Raster.z() * w0 + v1Raster.z() * w1 + v2Raster.z() * w2;
      depth = 1 / oneOverDepth;
      return true;
    }

    // samplePoint outside the triangle
    return false;
  }

  vec3 v0Raster, v1Raster, v2Raster;
  // bbox corner of triangle
  float xmin, xmax, ymin, ymax;
};

vec3 perspCorrInterp(const triangle& tric, const vec3& attr0, const vec3& attr1, const vec3& attr2,
                     const float& w0, const float& w1, const float& w2, const float& pdepth) {
  // first we divide attributes by their z-coordinate
  // (though we use a multiplication here because of the precomputation)
  vec3 a0 = attr0 * tric.v0Raster.z();
  vec3 a1 = attr1 * tric.v1Raster.z();
  vec3 a2 = attr2 * tric.v2Raster.z();

  vec3 interp_res = a0 * w0 + a1 * w1 + a2 * w2;
  // after interpolation, multipy its own depth to get attribute result
  interp_res *= pdepth;
  return interp_res;
}

#endif
