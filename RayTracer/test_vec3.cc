#include <fstream>
#include <iostream>
#include "vec3.h"
using namespace std;

int main() {
  fstream file;
  file.open("ppm_demo.ppm", ios::out);
  streambuf *stream_buffer_cout = cout.rdbuf();
  streambuf *stream_buffer_file = file.rdbuf();
  cout.rdbuf(stream_buffer_file);

  int nx = 200;
  int ny = 100;
  cout << "P3\n" << nx << " " << ny << " " << "\n255\n";
  for (int j = ny-1; j >= 0; j--) {
    for (int i = 0; i < nx; i++) {
      vec3 colorx(static_cast<float>(i) / static_cast<float>(nx),
                  static_cast<float>(j) / static_cast<float>(ny),
                  0.2);
      int ir = static_cast<int>(255.99 * colorx.r());
      int ig = static_cast<int>(255.99 * colorx.g());
      int ib = static_cast<int>(255.99 * colorx.b());
      cout << ir << " " << ig << " " << ib << "\n";
    }
  }
  cout.rdbuf(stream_buffer_cout);
  return 0;
}
