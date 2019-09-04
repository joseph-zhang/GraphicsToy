#ifndef __PERLINH__
#define __PERLINH__

#include <cmath>
#include "vec3.h"
#include "interp.h"

/*
  class valuen.
  3D value noise.

  class perlin.
  3D perlin noise.
*/

// value noise
class valuen {
 public:
  float noise(const vec3& p) const {
    float u = p.x() - floor(p.x());
    float v = p.y() - floor(p.y());
    float w = p.z() - floor(p.z());

    // let weight a function, using heimite cubic: f(t) = 3t^2 - 2t^3
    u = u*u*(3-2*u);
    v = v*v*(3-2*v);
    w = w*w*(3-2*w);

    int i = floor(p.x());
    int j = floor(p.y());
    int k = floor(p.z());

    float c[2][2][2];
    for (int di = 0; di < 2; di++) {
      for (int dj = 0; dj < 2; dj++) {
        for (int dk = 0; dk < 2; dk++) {
          // color at 8 neighbors
          // use & and ^ to limit value in [0, 255], 256 values.
          c[di][dj][dk] = ranfloat[perm_x[(i+di) & 255] ^
                                   perm_y[(j+dj) & 255] ^
                                   perm_z[(k+dk) & 255]];
        }
      }
    }
    return trilinear_interp(c, u, v, w);
  }

  // use permutation table to save ram
  static float* ranfloat;
  static int* perm_x;
  static int* perm_y;
  static int* perm_z;
};

static float* value_generate() {
  float* p = new float[256];
  for (int i = 0; i < 256; i++) {
    p[i] = drand48();
  }
  return p;
}

// generate permute table
void permute(int *p, int n) {
  for(int i = n-1; i > 0; i--) {
    int target = int(drand48()*(i+1));
    // swap
    int tmp = p[i];
    p[i] = p[target];
    p[target] = tmp;
  }
  return;
}

static int* generate_perm() {
  int* p = new int[256];
  for (int i = 0; i < 256; i++)
    p[i] = i;
  permute(p, 256);
  return p;
}

float* valuen::ranfloat = value_generate();
int* valuen::perm_x = generate_perm();
int* valuen::perm_y = generate_perm();
int* valuen::perm_z = generate_perm();

// perline noise
class perlin {
 public:
  float noise(const vec3& p) const {
    float u = p.x() - floor(p.x());
    float v = p.y() - floor(p.y());
    float w = p.z() - floor(p.z());

    // let weight a function, using heimite cubic: f(t) = 3t^2 - 2t^3
    u = u*u*(3-2*u);
    v = v*v*(3-2*v);
    w = w*w*(3-2*w);

    int i = floor(p.x());
    int j = floor(p.y());
    int k = floor(p.z());

    vec3 c[2][2][2];
    for (int di = 0; di < 2; di++) {
      for (int dj = 0; dj < 2; dj++) {
        for (int dk = 0; dk < 2; dk++) {
          // color at 8 neighbors
          // use & and ^ to limit value in [0, 255], 256 values.
          c[di][dj][dk] = ranvec[perm_x[(i+di) & 255] ^
                                 perm_y[(j+dj) & 255] ^
                                 perm_z[(k+dk) & 255]];
        }
      }
    }
    return perlin_interp(c, u, v, w);
  }

  float turb(const vec3& p, int depth=7) const {
    float accum = 0;
    vec3 temp_p = p;
    float weight = 1.0;
    for (int i = 0; i < depth; i++) {
      accum += weight*noise(temp_p);
      weight *= 0.5;
      temp_p *= 2;
    }
    return fabs(accum);
  }

  // use permutation table to save ram
  static vec3* ranvec;
  static int* perm_x;
  static int* perm_y;
  static int* perm_z;
};

static vec3* perlin_generate() {
  vec3* p = new vec3[256];
  for (int i = 0; i < 256; i++) {
    p[i] = unit_vector( vec3(-1+2*drand48(), -1+2*drand48(), -1+2*drand48()) );
  }
  return p;
}

vec3* perlin::ranvec = perlin_generate();
int* perlin::perm_x = generate_perm();
int* perlin::perm_y = generate_perm();
int* perlin::perm_z = generate_perm();

#endif
