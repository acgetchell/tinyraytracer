//
// Created by Adam Getchell on 2019-02-09.
// Inspired by https://github.com/ssloy/tinyraytracer
//
#include <fstream>
#include <geometry.hpp>
#include <limits>

void render()
{
  const int          width  = 1024;
  const int          height = 768;
  std::vector<Vec3f> framebuffer(width * height);

  for (size_t j = 0; j < height; j++)
  {
    for (size_t i = 0; i < width; i++)
    {
      framebuffer[i + j * width] =
          Vec3f(j / float(height), i / float(width), 0);
    }
  }

  std::ofstream ofs;  // save to file
  ofs.open("./out.ppm");
  ofs << "P6\n" << width << " " << height << "\n255\n";
  for (size_t i = 0; i < height * width; ++i)
  {
    for (size_t j = 0; j < 3; j++)
    { ofs << (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j]))); }
  }
  ofs.close();
}

int main()
{
  render();

  return 0;
}