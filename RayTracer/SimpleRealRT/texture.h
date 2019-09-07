#ifndef __TEXTUREH__
#define __TEXTUREH__

#include "noise.h"

class texture {
 public:
  virtual vec3 value(float u, float v, const vec3& p) const = 0;
};

// treat color as texture
class constant_texture : public texture {
 public:
  constant_texture() {}
  constant_texture(vec3 c) : color(c) {}
  virtual vec3 value(float u, float v, const vec3& p) const {
    return color;
  }
  vec3 color;
};

// checkerboard texture
class checker_texture : public texture {
 public:
  checker_texture() {}
  checker_texture(texture* text0, texture* text1): even(text0), odd(text1) {}
  virtual vec3 value(float u, float v, const vec3& p) const {
    float sines = sin(10*p.x())*sin(10*p.y())*sin(10*p.z());
    if (sines < 0.) {
      return odd->value(u, v, p);
    } else {
      return even->value(u, v, p);
    }
  }

  // can be constant texture or some other procedural texture
  texture* odd;
  texture* even;
};

class image_texture : public texture {
 public:
  image_texture() {}
  image_texture(unsigned char* image, int A, int B, int C)
    : data(image), nx(A), ny(B), nn(C) {}
  virtual vec3 value(float u, float v, const vec3& p) const;

  // image data as a big array
  unsigned char* data;
  int nx, ny, nn;
};

vec3 image_texture::value(float u, float v, const vec3& p) const {
  int i = u*nx;
  // note that the image coordinate and the global coordinate are different
  // here we minus 0.001 from y to locate at the last row
  int j = (1-v)*ny - 0.001;
  if (i < 0) i = 0;
  if (j < 0) j = 0;
  if (i > nx-1) i = nx-1;
  if (j > ny-1) j = ny-1;

  // convert 1-d array to the corresponding 2-d
  float r = int(data[nn*nx*j+0 + nn*i]) / 255.0;
  float g = int(data[nn*nx*j+1 + nn*i]) / 255.0;
  float b = int(data[nn*nx*j+2 + nn*i]) / 255.0;
  return vec3(r,g,b);
}

class value_noise_texture : public texture {
 public:
  value_noise_texture() {}
  value_noise_texture(float sc) : scale(sc) {}
  virtual vec3 value(float u, float v, const vec3& p) const {
    return vec3(1,1,1) * noise.noise(scale*p);
  }

  // frequency
  valuen noise;
  float scale;
};

class perlin_noise_texture : public texture {
 public:
  perlin_noise_texture() {}
  perlin_noise_texture(float sc, int type=0) : scale(sc), value_type(type) {}
  virtual vec3 value(float u, float v, const vec3& p) const {
    if (value_type == 0) {
      return vec3(1,1,1) * noise.noise(scale*p);
    } else if (value_type == 1){ // turbulence -> camouflage netting appearance
      return vec3(1,1,1) * noise.turb(scale*p);
    } else { // turbulence -> use it to adjust the phase of sin, giving a marble-like effect
      return vec3(1,1,1) * 0.5 * (1 + sin(scale*p.z() + 10*noise.turb(p)));
    }
  }

  perlin noise;
  float scale;
  int value_type;
};

#endif
