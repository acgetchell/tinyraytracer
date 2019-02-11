//
// Created by Adam Getchell on 2019-02-09.
// Inspired by https://github.com/ssloy/tinyraytracer
//
#include <fstream>
#include <geometry.hpp>
#include <limits>

struct Material
{
  Vec3f diffuse_color;
  explicit Material(Vec3f const& color) : diffuse_color(color) {}
  Material() : diffuse_color() {}
};

struct Sphere
{
  Vec3f    center;
  float    radius;
  Material material;

  Sphere(Vec3f const& c, float const& r, Material const& m)
      : center(c), radius(r), material(m)
  {}

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
    return t0 >= 0;
  }
};

bool scene_intersect(Vec3f const& orig, Vec3f const& dir,
                     std::vector<Sphere> const& spheres, Vec3f& hit, Vec3f& N,
                     Material& material)
{
  auto spheres_dist = std::numeric_limits<float>::max();
  for (auto sphere : spheres)
  {
    float dist_i;
    if (sphere.ray_intersect(orig, dir, dist_i) && dist_i < spheres_dist)
    {
      spheres_dist = dist_i;
      hit          = orig + dir * dist_i;
      N            = (hit - sphere.center).normalize();
      material     = sphere.material;
    }
  }
  return spheres_dist < 1000;
}

Vec3f cast_ray(Vec3f const& orig, Vec3f const& dir,
               std::vector<Sphere> const& spheres)
{
  Vec3f    point, N;
  Material material;
  if (!scene_intersect(orig, dir, spheres, point, N, material))

  //  float sphere_dist = std::numeric_limits<float>::max();
  //  if (!sphere.ray_intersect(orig, dir, sphere_dist))
  {
    return Vec3f(0.2f, 0.7f, 0.8f);  // background color
  }
  //  return Vec3f(0.4f, 0.4f, 0.3f);
  return material.diffuse_color;
}

void render(std::vector<Sphere> const& spheres)
{
  int const          width  = 1024;
  int const          height = 768;
  auto               fov    = static_cast<int const>(M_PI / 2.);
  std::vector<Vec3f> framebuffer(width * height);

#pragma omp parallel for
  for (size_t j = 0; j < height; j++)
  {
    for (size_t i = 0; i < width; i++)
    {
      auto x = (2 * (i + 0.5f) / static_cast<float>(width) - 1) *
               tan(fov / 2.f) * width / static_cast<float>(height);
      auto y =
          -(2 * (j + 0.5f) / static_cast<float>(height) - 1) * tan(fov / 2.f);
      Vec3f dir                  = Vec3f(x, y, -1).normalize();
      framebuffer[i + j * width] = cast_ray(Vec3f(0, 0, 0), dir, spheres);
    }
  }

  std::ofstream ofs;  // save to file
  ofs.open("./out.ppm");
  ofs << "P6\n" << width << " " << height << "\n255\n";
  for (size_t i = 0; i < height * width; ++i)
  {
    for (size_t j = 0; j < 3; j++)
    {
      ofs << static_cast<char>(255 *
                               std::max(0.f, std::min(1.f, framebuffer[i][j])));
    }
  }
  ofs.close();
}

int main()
{
  //  Sphere sphere(Vec3f(-3, 0, -16), 2);
  //  render(sphere);

  Material ivory(Vec3f(0.4f, 0.4f, 0.3f));
  Material red_rubber(Vec3f(0.3f, 0.1f, 0.1f));

  std::vector<Sphere> spheres;
  spheres.emplace_back(Sphere(Vec3f(-3, 0, -16), 2, ivory));
  spheres.emplace_back(Sphere(Vec3f(-1.0, -1.5, -12), 2, red_rubber));
  spheres.emplace_back(Sphere(Vec3f(1.5, -0.5, -18), 3, red_rubber));
  spheres.emplace_back(Sphere(Vec3f(7, 5, -18), 4, ivory));

  render(spheres);

  return 0;
}