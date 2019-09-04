#ifndef __INTERPH__
#define __INTERPH__

#include "vec3.h"

// trilinear interpolation of float value
inline float trilinear_interp(float c[2][2][2], float u, float v, float w) {
  float accum = 0;
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2; j++)
      for (int k = 0; k < 2; k++)
        accum += (i*u + (1-i)*(1-u))*
                 (j*v + (1-j)*(1-v))*
                 (k*w + (1-k)*(1-w))*c[i][j][k];
  return accum;
}

// trilinear perlin interpolation
inline float perlin_interp(vec3 c[2][2][2], float u, float v, float w) {
  float accum = 0;
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2; j++)
      for (int k = 0; k < 2; k++) {
        vec3 neighbor = vec3(i,j,k);
        vec3 weight_v = vec3(u,v,w) - neighbor;
        accum += (i*u + (1-i)*(1-u))*
                 (j*v + (1-j)*(1-v))*
                 (k*w + (1-k)*(1-w))*dot(c[i][j][k], weight_v);
      }
  return accum;
}

#endif
