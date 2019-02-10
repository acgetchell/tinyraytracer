//
// Created by Adam Getchell on 2019-02-09.
// Inspired by https://github.com/ssloy/tinyraytracer
//
#include <fstream>
#include <geometry.hpp>
#include <limits>

struct Sphere
{
  Vec3f center;
  float radius;

  Sphere(Vec3f const& c, float const& r) : center(c), radius(r) {}

  bool ray_intersect(Vec3f const& orig, Vec3f const& dir, float& t0) const
  {
    Vec3f L   = center - orig;
    float tca = L * dir;
    float d2  = L * L - tca * tca;
    if (d2 > radius * radius) return false;
    float thc = sqrt(radius * radius - d2);
    t0        = tca - thc;
    float t1  = tca + thc;
    if (t0 < 0) t0 = t1;
    if (t0 < 0) return false;
    return true;
  }
};

Vec3f cast_ray(Vec3f const& orig, Vec3f const& dir, Sphere const& sphere)
{
  float sphere_dist = std::numeric_limits<float>::max();
  if (!sphere.ray_intersect(orig, dir, sphere_dist))
  {
    return Vec3f(0.2, 0.7, 0.8);  // background color
  }
  return Vec3f(0.4, 0.4, 0.3);
}

void render(Sphere const& sphere)
{
  int const          width  = 1024;
  int const          height = 768;
  int const          fov    = M_PI / 2.;
  std::vector<Vec3f> framebuffer(width * height);

#pragma omp parallel for
  for (auto j = 0; j < height; j++)
  {
    for (auto i = 0; i < width; i++)
    {
      float x = (2 * (i + 0.5) / (float)width - 1) * tan(fov / 2.) * width /
                (float)height;
      float y   = -(2 * (j + 0.5) / (float)height - 1) * tan(fov / 2.);
      Vec3f dir = Vec3f(x, y, -1).normalize();
      framebuffer[i + j * width] = cast_ray(Vec3f(0, 0, 0), dir, sphere);
    }
  }

  std::ofstream ofs;  // save to file
  ofs.open("./out.ppm");
  ofs << "P6\n" << width << " " << height << "\n255\n";
  for (auto i = 0; i < height * width; ++i)
  {
    for (auto j = 0; j < 3; j++)
    {
      ofs << static_cast<char>(255 *
                               std::max(0.f, std::min(1.f, framebuffer[i][j])));
    }
  }
  ofs.close();
}

int main()
{
  Sphere sphere(Vec3f(-3, 0, -16), 2);
  render(sphere);

  return 0;
}