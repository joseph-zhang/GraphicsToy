#ifndef __CAMERAH__
#define __CAMERAH__

/*
a tiny physically-based camera model
 */

#include <cmath>
#include "smatrix4.h"

static const float inchToMm = 25.4; // Inch to Millimeter
enum FitResolutionGate { kFill = 0, kOverscan };

class camera {
 public:
  camera(vec3 lookfrom, vec3 lookat, vec3 vup, float filmWidth, float filmHeight,
         int imageWidth, int imageHeight, FitResolutionGate fitFilm,
         float nearClipDist, float farClipDist, float focalLength) {
    origin = lookfrom;
    computeScreenPrams(filmWidth, filmHeight, imageWidth, imageHeight, fitFilm, nearClipDist, focalLength);
    computeCameraLookAt(lookfrom, lookat, vup);
    worldToCam = inverse(camToWorld);
    computePerspective(nearClipDist, farClipDist);
  }

  camera(smatrix4 CToWMatrix, float filmWidth, float filmHeight,
         int imageWidth, int imageHeight, FitResolutionGate fitFilm,
         float nearClipDist, float farClipDist, float focalLength) {
    computeScreenPrams(filmWidth, filmHeight, imageWidth, imageHeight, fitFilm, nearClipDist, focalLength);
    camToWorld = CToWMatrix;
    worldToCam = inverse(camToWorld);
    computePerspective(nearClipDist, farClipDist);

    u      = vec3(camToWorld[0][0], camToWorld[0][1], camToWorld[0][2]);
    v      = vec3(camToWorld[1][0], camToWorld[1][1], camToWorld[1][2]);
    w      = vec3(camToWorld[2][0], camToWorld[2][1], camToWorld[2][2]);
    origin = vec3(camToWorld[3][0], camToWorld[3][1], camToWorld[3][2]);
  }

  vec3 u, v, w;
  vec3 origin;
  float top, bottom, left, right;
  float hfov;
  smatrix4 camToWorld, worldToCam;
  smatrix4 perspProj;

 private:
  void computeScreenPrams(const float& filmW, const float& filmH,
                          const int& imgW, const int& imgH,
                          const FitResolutionGate& fitFilm,
                          const float& near, const float& fl) {
    float filmAspectRatio = filmW / filmH;
    float deviceAspectRatio = imgW / imgH;

    // we set canvas on near clipping plane
    top = ((filmH*inchToMm/2) / fl) * near;
    right = ((filmW*inchToMm/2) / fl) * near;

    // hfov (angle)
    float fov = 2 * 180/M_PI * atan((filmW*inchToMm/2) / fl);
    hfov = fov;

    float xscale = 1.;
    float yscale = 1.;

    switch(fitFilm) {
    default:
    case FitResolutionGate::kFill:
      if (filmAspectRatio > deviceAspectRatio) {
        xscale = deviceAspectRatio / filmAspectRatio;
      } else {
        yscale = filmAspectRatio / deviceAspectRatio;
      }
      break;
    case FitResolutionGate::kOverscan:
      if (filmAspectRatio > deviceAspectRatio) {
        yscale = filmAspectRatio / deviceAspectRatio;
      } else {
        xscale = deviceAspectRatio / filmAspectRatio;
      }
      break;
    }

    right *= xscale;
    top *= yscale;

    bottom = -top;
    left = -right;
  }

  void computeCameraLookAt(const vec3& from, const vec3& to, const vec3& tmp) {
    w = unit_vector(to - from);
    u = unit_vector(cross(tmp, w));
    v = cross(w, u);

    // posture
    camToWorld[0][0] = u.x(); camToWorld[0][1] = u.y(); camToWorld[0][2] = u.z(); camToWorld[0][3] = 0;
    camToWorld[1][0] = v.x(); camToWorld[1][0] = v.y(); camToWorld[1][2] = v.z(); camToWorld[1][3] = 0;
    camToWorld[2][0] = w.x(); camToWorld[2][1] = w.y(); camToWorld[2][2] = w.z(); camToWorld[2][3] = 0;
    // position
    camToWorld[3][0] = from.x();
    camToWorld[3][1] = from.y();
    camToWorld[3][2] = from.z();
    camToWorld[3][3] = 1.;
  }

  void computePerspective(const float& n, const float& f) {
    float r = right;
    float l = left;
    float t = top;
    float b = bottom;

    perspProj[0][0] = (2*n)/(r-l);
    perspProj[0][1] = 0;
    perspProj[0][2] = 0;
    perspProj[0][3] = 0;

    perspProj[1][0] = 0;
    perspProj[1][1] = (2*n)/(t-b);
    perspProj[1][2] = 0;
    perspProj[1][3] = 0;

    perspProj[2][0] = (r+l)/(r-l);
    perspProj[2][1] = (t+b)/(t-b);
    perspProj[2][2] = -(f+n)/(f-n);
    perspProj[2][3] = -1;

    perspProj[3][0] = 0;
    perspProj[3][1] = 0;
    perspProj[3][2] = -(2*f*n)/(f-n);
    perspProj[3][3] = 0;
  }
};

#endif
