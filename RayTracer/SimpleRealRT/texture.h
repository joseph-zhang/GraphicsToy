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
