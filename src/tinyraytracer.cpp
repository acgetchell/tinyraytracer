//
// Created by Adam Getchell on 2019-02-09.
// Inspired by https://github.com/ssloy/tinyraytracer
//
#include <fstream>
#include <geometry.hpp>
#include <limits>

struct Light
{
  Vec3f position;
  float intensity;
  Light(Vec3f const& p, float const& i) : position(p), intensity(i) {}
};

struct Material
{
  float refractive_index;
  Vec4f albedo;
  Vec3f diffuse_color;
  float specular_exponent;
  explicit Material(float const& r, Vec4f const& a, Vec3f const& color,
                    float const& spec)
      : refractive_index(r)
      , albedo(a)
      , diffuse_color(color)
      , specular_exponent(spec)
  {}
  Material()
      : refractive_index(1)
      , albedo(1, 0, 0, 0)
      , diffuse_color()
      , specular_exponent()
  {}
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

Vec3f reflect(Vec3f const& I, Vec3f const& N) { return I - N * 2.f * (I * N); }

Vec3f refract(Vec3f const& I, Vec3f const& N, float const& refractive_index)
{
  // Snell's law
  float cos_i = -std::max(-1.f, std::min(1.f, I * N));
  float eta_i = 1;
  float eta_t = refractive_index;
  Vec3f n     = N;
  if (cos_i < 0)
  {
    // if the ray is inside the object, swap the indices and invert the normal
    // to get the correct result
    cos_i = -cos_i;
    std::swap(eta_i, eta_t);
    n = -N;
  }
  float eta = eta_i / eta_t;
  float k   = 1 - eta * eta * (1 - cos_i * cos_i);
  return k < 0 ? Vec3f(0, 0, 0) : I * eta + n * (eta * cos_i - sqrtf(k));
}

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

  float checkerboard_dist = std::numeric_limits<float>::max();
  if (fabs(dir.y) > 1e-3f)
  {
    float d  = -(orig.y + 4) / dir.y;  // Checkerboard plane is y = -4
    Vec3f pt = orig + dir * d;
    if (d > 0 && fabs(pt.x) < 10 && pt.z < -10 && pt.z > -30 &&
        d < spheres_dist)
    {
      checkerboard_dist = d;
      hit               = pt;
      N                 = Vec3f(0, 1, 0);
      material.diffuse_color =
          (int(.5f * hit.x + 1000) + int(.5f * hit.z)) & 1  // NOLINT
              ? Vec3f(.3f, .3f, .3f)
              : Vec3f(.3f, .2f, .1f);
    }
  }
  return std::min(spheres_dist, checkerboard_dist) < 1000;
}

Vec3f cast_ray(Vec3f const& orig, Vec3f const& dir,
               std::vector<Sphere> const& spheres,
               std::vector<Light> const& lights, size_t depth = 0)
{
  Vec3f    point, N;
  Material material;
  if (depth > 4 || !scene_intersect(orig, dir, spheres, point, N, material))
  {
    return Vec3f(0.2f, 0.7f, 0.8f);  // background color
  }

  Vec3f reflect_dir = reflect(dir, N).normalize();
  Vec3f refract_dir = refract(dir, N, material.refractive_index).normalize();
  Vec3f reflect_orig =
      reflect_dir * N < 0
          ? point - N * 1e-3f
          : point + N * 1e-3f;  // offset the original point to avoid occlusion
                                // by the object itself
  Vec3f refract_orig =
      refract_dir * N < 0 ? point - N * 1e-3f : point + N * 1e-3f;
  Vec3f reflect_color =
      cast_ray(reflect_orig, reflect_dir, spheres, lights, depth + 1);
  Vec3f refract_color =
      cast_ray(refract_orig, refract_dir, spheres, lights, depth + 1);

  float diffuse_light_intensity  = 0;
  float specular_light_intensity = 0;
  for (auto light : lights)
  {
    Vec3f light_dir = (light.position - point).normalize();
    // Shadows: if our ray intersects another object, stop
    float light_distance = (light.position - point).norm();

    Vec3f shadow_orig = light_dir * N < 0
                            ? point - N * 1e-3f
                            : point + N * 1e-3f;  // checking if the point lies
                                                  // in the shadow of the light
    Vec3f    shadow_pt, shadow_N;
    Material tmp_material;
    if (scene_intersect(shadow_orig, light_dir, spheres, shadow_pt, shadow_N,
                        tmp_material) &&
        (shadow_pt - shadow_orig).norm() < light_distance)
      continue;

    diffuse_light_intensity += light.intensity * std::max(0.f, light_dir * N);
    specular_light_intensity +=
        powf(std::max(0.f, -reflect(-light_dir, N) * dir),
             material.specular_exponent) *
        light.intensity;
  }
  return material.diffuse_color * diffuse_light_intensity * material.albedo[0] +
         Vec3f(1., 1., 1.) * specular_light_intensity * material.albedo[1] +
         reflect_color * material.albedo[2] +
         refract_color *
             material
                 .albedo[3];  // sphere color, lighting, reflection, refraction
}

void render(std::vector<Sphere> const& spheres,
            std::vector<Light> const&  lights)
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
      Vec3f dir = Vec3f(x, y, -1).normalize();
      framebuffer[i + j * width] =
          cast_ray(Vec3f(0, 0, 0), dir, spheres, lights);
    }
  }

  std::ofstream ofs;  // save to file
  ofs.open("./out.ppm");
  ofs << "P6\n" << width << " " << height << "\n255\n";
  for (size_t i = 0; i < height * width; ++i)
  {
    Vec3f& c   = framebuffer[i];
    float  max = std::max(c[0], std::max(c[1], c[2]));
    if (max > 1) c = c * (1.f / max);
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
  Material ivory(1.0, Vec4f(0.6f, 0.3f, 0.1f, 0.0f), Vec3f(0.4f, 0.4f, 0.3f),
                 50.);
  Material glass(1.5, Vec4f(0.0, 0.5, 0.1f, 0.8f), Vec3f(0.6f, 0.7f, 0.8f),
                 125.);
  Material red_rubber(1.0, Vec4f(0.9f, 0.1f, 0.0f, 0.0f),
                      Vec3f(0.3f, 0.1f, 0.1f), 10.);
  Material mirror(1.0, Vec4f(0.0f, 10.0f, 0.8f, 0.0), Vec3f(1.0, 1.0, 1.0),
                  1425.);

  std::vector<Sphere> spheres;
  spheres.emplace_back(Sphere(Vec3f(-3, 0, -16), 2, ivory));
  spheres.emplace_back(Sphere(Vec3f(-1.0, -1.5, -12), 2, glass));
  spheres.emplace_back(Sphere(Vec3f(1.5, -0.5, -18), 3, red_rubber));
  spheres.emplace_back(Sphere(Vec3f(7, 5, -18), 4, mirror));

  std::vector<Light> lights;
  lights.emplace_back(Light(Vec3f(-20, 20, 20), 1.5));
  lights.emplace_back(Light(Vec3f(30, 50, -25), 1.8f));
  lights.emplace_back(Light(Vec3f(30, 20, 30), 1.7f));

  render(spheres, lights);

  return 0;
}