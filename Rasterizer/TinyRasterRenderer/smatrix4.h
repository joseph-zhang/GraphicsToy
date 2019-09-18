#ifndef __SMATRIX4H__
#define __SMATRIX4H__
/*
  A tiny and simple 4D Square Matrix class.
  (Using Row Major Vector by default.)
 */

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstdio>
#include <cmath>

#include "vec2.h"
#include "vec3.h"

class smatrix4 {
 public:
  smatrix4() {}
  smatrix4(float a, float b, float c, float d,
           float e, float f, float g, float h,
           float i, float j, float k, float l,
           float m, float n, float o, float p) {
    mat[0][0] = a; mat[0][1] = b; mat[0][2] = c; mat[0][3] = d;
    mat[1][0] = e; mat[1][1] = f; mat[1][2] = g; mat[1][3] = h;
    mat[2][0] = i; mat[2][1] = j; mat[2][2] = k; mat[2][3] = l;
    mat[3][0] = m; mat[3][1] = n; mat[3][2] = o; mat[3][3] = p;
  }

  const float* operator[](int i) const { return mat[i]; }
  float* operator[](int i) { return mat[i]; }
  // get the element at i row j column, both start from 0.
  float at(const int i, const int j) const { return mat[i][j]; }

  smatrix4 operator*(const smatrix4& m2) const;

  smatrix4& transpose() {
    smatrix4 tmp;
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        tmp[i][j] = mat[j][i];
      }
    }
    *this = tmp;
    return *this;
  }

  // matrix as transformation
  void multPoint(const vec3& src, vec3& dst) const;
  void multDir(const vec3& src, vec3& dst) const;

  // matrix invert
  const smatrix4& invert();

  float mat[4][4] = {{1,0,0,0},
                     {0,1,0,0},
                     {0,0,1,0},
                     {0,0,0,1}};
};

// naive 4x4 matrix multiply
void multiply(const smatrix4& a, const smatrix4& b, smatrix4& c) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      c[i][j] = a[i][0]*b[0][j] + a[i][1]*b[1][j] +
                a[i][2]*b[2][j] + a[i][3]*b[3][j];
    }
  }
}

smatrix4 smatrix4::operator*(const smatrix4& m2) const {
  smatrix4 tmp;
  multiply(*this, m2, tmp);
  return tmp;
}

smatrix4 make_transposed(const smatrix4& m) {
  smatrix4 tmp;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      tmp[i][j] = m.at(j, i);
    }
  }
  return tmp;
}

// Point-Matrix multiplication, using Row Major Vector.
// Points are implicitly be considered as having homogeneous coordinates.
void smatrix4::multPoint(const vec3& src, vec3& dst) const {
  float a, b, c, w;
  a = src[0]*mat[0][0] + src[1]*mat[1][0] + src[2]*mat[2][0] + mat[3][0];
  b = src[0]*mat[0][1] + src[1]*mat[1][1] + src[2]*mat[2][1] + mat[3][1];
  c = src[0]*mat[0][2] + src[1]*mat[1][2] + src[2]*mat[2][2] + mat[3][2];
  w = src[0]*mat[0][3] + src[1]*mat[1][3] + src[2]*mat[2][3] + mat[3][3];

  dst.e[0] = a/w; dst.e[1] = b/w; dst.e[2] = c/w;
}

// Direction-Matrix multiplication, using Row Major Vector.
void smatrix4::multDir(const vec3& src, vec3& dst) const {
  float a, b, c;
  a = src[0]*mat[0][0] + src[1]*mat[1][0] + src[2]*mat[2][0];
  b = src[0]*mat[0][1] + src[1]*mat[1][1] + src[2]*mat[2][1];
  c = src[0]*mat[0][2] + src[1]*mat[1][2] + src[2]*mat[2][2];

  dst.e[0] = a; dst.e[1] = b; dst.e[2] = c;
}

smatrix4 inverse(const smatrix4& m) {
  int i, j, k;
  smatrix4 s;
  smatrix4 t(m);

  for (i = 0; i < 3; i++) {
    int pivot = i;
    float pivotsize = t[i][i];
    if (pivotsize < 0) pivotsize = -pivotsize;

    for (j = i+1; j < 4; j++) {
      float tmp = t[j][i];
      if (tmp < 0) tmp = -tmp;
      if (tmp > pivotsize) {
        pivot = j;
        pivotsize = tmp;
      }
    }

    if (pivotsize == 0) {
      // meet singular matrix
      return smatrix4();
    }

    if (pivot != i) {
      for (j = 0; j < 4; j++) {
        float tmp;

        tmp = t[i][j];
        t[i][j] = t[pivot][j];
        t[pivot][j] = tmp;

        tmp = s[i][j];
        s[i][j] = s[pivot][j];
        s[pivot][j] = tmp;
      }
    }

    for (j = i+1; j < 4; j++) {
      float f = t[j][i] / t[i][i];
      for (k = 0; k < 4; k++) {
        t[j][k] -= f * t[i][k];
        s[j][k] -= f * s[i][k];
      }
    }
  }

  // Backward substitution
  for (i=3; i >= 0; --i) {
    float f;
    if ((f = t[i][i]) == 0) {
      // singular matrix
      return smatrix4();
    }

    for (j = 0; j < 4; j++) {
      t[i][j] /= f;
      s[i][j] /= f;
    }

    for (j = 0; j < i; j++) {
      f = t[j][i];
      for (k = 0; k < 4; k++) {
        t[j][k] -= f * t[i][k];
        s[j][k] -= f * s[i][k];
      }
    }
  }
  return s;
}

const smatrix4& smatrix4::invert() {
  *this = inverse(*this);
  return *this;
}

std::ostream& operator<<(std::ostream &os, const smatrix4& m) {
  std::ios_base::fmtflags oldFlags = os.flags();
  int width = 12; // total with of the displayed number
  os.precision(5); // control the number of displayed decimals
  os.setf (std::ios_base::fixed);

  os << "[" << std::setw (width) << m[0][0] <<
    " " << std::setw (width) << m[0][1] <<
    " " << std::setw (width) << m[0][2] <<
    " " << std::setw (width) << m[0][3] << "\n" <<

    " " << std::setw (width) << m[1][0] <<
    " " << std::setw (width) << m[1][1] <<
    " " << std::setw (width) << m[1][2] <<
    " " << std::setw (width) << m[1][3] << "\n" <<

    " " << std::setw (width) << m[2][0] <<
    " " << std::setw (width) << m[2][1] <<
    " " << std::setw (width) << m[2][2] <<
    " " << std::setw (width) << m[2][3] << "\n" <<

    " " << std::setw (width) << m[3][0] <<
    " " << std::setw (width) << m[3][1] <<
    " " << std::setw (width) << m[3][2] <<
    " " << std::setw (width) << m[3][3] << "]";

  os.flags (oldFlags);
  return os;
}

#endif
