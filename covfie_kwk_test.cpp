#include <iostream>
#include <cstddef>
// #include <fstream>
// #include <vector>
// #include <memory>
#include <cmath>
#include <chrono>
#include "bitmap.hpp"
#include "utils.hpp"
#include "data_structures.hpp"
#include "types.hpp"
#include "acts_struct.hpp"
#include <kwk/kwk.hpp>

// joube@ls-cassidi:~/shared/covfie-stephen$ cmake --build build -- -j $(nproc) && build/examples/cpu/render_slice_cpu --input atlas.cvf --output my_image.bmp --z 0


// using field_t1 =
// covfie::field : une table
// covfie::backend::affine : backend (owning data)
// covfie::backend::linear : interpolation linéaire des coordonnées
// covfie::backend::strided : qui a un stride comme nos tables et vues
// covfie::vector::ulong3 : en 3D
// covfie::backend::array<covfie::vector::float3>> : dont chaque cellule est une structure constituée de 3 floats

// using field_t1 = covfie::field<covfie::backend::affine<
//       covfie::backend::linear<covfie::backend::strided<
//       covfie::vector::ulong3,
//       covfie::backend::array<covfie::vector::float3>>>>>;

// using data_t = kwk::make_table_t<kwk::as<pt3D<float>>, kwk::_3D, kwk::affine_transform(kwk::linear)>;

/*
C'est un array<float3>
qui est strided avec 3 dimensions (ulong3)
qui a une interpolation linéaire des coordonnées
qui subit une transformation affine sur ses coordonnées
encapsulé par un covfie::field.

Lecture du fichier dans cet ordre :
- (dim est supposé connu, le nombre de dimensions du stride)
- lecture de la transformation affine ((dim)*(dim+1)*sizeof(float))
- lecture des tailles des (dim) dimensions
- lecture du nombre d'éléments total (devrait être dim1*dim2*...*dimN)
- puis ce sont les éléments les uns à la suite des autres

Commande pour relancer l'exemple :
cmake --build build -- -j $(nproc) && build/examples/cpu/render_slice_cpu --input atlas.cvf --output my_image.bmp --z 0

$ clang++-15 covfie_kwk_test.cpp bitmap.cpp data_structures.cpp utils.cpp acts_struct.cpp -o exe -O3 -std=c++20 && ./exe
*/



void render_slice_kiwaku(float z_value = 0)
{
  acts_data a;
  a.read_acts_file();

  uint const width  = 1024;
  uint const height = 1024;

  auto img = kwk::table{kwk::type = kwk::int8_, kwk::of_size(width, height)};

  std::chrono::high_resolution_clock::time_point t1 =
  std::chrono::high_resolution_clock::now();

  kwk::for_each_index([ fw = static_cast<float>(width)
                      , fh = static_cast<float>(height)
                      , z_value
                      , &a
                      ]
  (auto& e, auto pos) 
  {
    auto[x,y] = pos;
    float fx = x / fw;
    float fy = y / fh;

    pt3D<float> asked{fx * 20000.f - 10000.f, fy * 20000.f - 10000.f, z_value};

    pt3D<float> p = a.at(asked);
    // Do something with p
    e = static_cast<char>(std::lround(
                  255.f *
                  std::min(
                      std::sqrt(p[0] * p[0] + p[1] * p[1] + p[2] * p[2]), 1.0f
                  )
              ));
  }
  , img
  );

  /*
  Issue Kiwaku : (02-02-2023)
  - Source doit accepter des containers.
  */

  std::chrono::high_resolution_clock::time_point t2 =
    std::chrono::high_resolution_clock::now();
  std::cout << "Rendering took "
            << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()
            << "us." << std::endl;

  render_bitmap(img,"210_jimg_" + std::to_string(z_value/100) + ".bmp");
}


void render_slice_xy(float z_value = 0)
{
  acts_data a;
  a.read_acts_file();

  uint const width  = 1024;
  uint const height = 1024;

  auto img_kwk = kwk::table{kwk::type = kwk::int8_, kwk::of_size(width, height)};

  std::chrono::high_resolution_clock::time_point t1 =
  std::chrono::high_resolution_clock::now();

  float fw = static_cast<float>(width);
  float fh = static_cast<float>(height);
  auto img = img_kwk.get_data();
  for (std::size_t x = 0; x < width; ++x) {
    for (std::size_t y = 0; y < height; ++y) {
      std::size_t linear_pos = x + y * width;

      float fx = x / fw;
      float fy = y / fh;

      pt3D<float> asked{fx * 20000.f - 10000.f, fy * 20000.f - 10000.f, z_value};

      pt3D<float> p = a.at(asked);
      
      img[linear_pos] =
        static_cast<char> (std::lround
                          (
                            255.f *
                            std::min
                            (
                              std::sqrt(p[0] * p[0] + p[1] * p[1] + p[2] * p[2]), 1.0f
                            )
                          ));

    }
  }

  std::chrono::high_resolution_clock::time_point t2 =
    std::chrono::high_resolution_clock::now();
  std::cout << "Rendering took "
            << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()
            << "us." << std::endl;

  render_bitmap(img_kwk,"wh_jimg_" + std::to_string(z_value/100) + ".bmp");
}

void render_slice_yx(float z_value = 0)
{
  acts_data a;
  a.read_acts_file();

  uint const width  = 1024;
  uint const height = 1024;

  auto img_kwk = kwk::table{kwk::type = kwk::int8_, kwk::of_size(width, height)};

  std::chrono::high_resolution_clock::time_point t1 =
  std::chrono::high_resolution_clock::now();

  float fw = static_cast<float>(width);
  float fh = static_cast<float>(height);
  auto img = img_kwk.get_data();

  // for (std::size_t y = 0; y < height; ++y) {
  //   for (std::size_t x = 0; x < width; ++x) {

  for (std::size_t x = 0; x < width; ++x) {
    for (std::size_t y = 0; y < height; ++y) {
      std::size_t linear_pos = x * height + y;

      float fx = x / fw;
      float fy = y / fh;

      pt3D<float> asked{fx * 20000.f - 10000.f, fy * 20000.f - 10000.f, z_value};

      pt3D<float> p = a.at(asked);
      
      img[linear_pos] =
        static_cast<char> (std::lround
                          (
                            255.f *
                            std::min
                            (
                              std::sqrt(p[0] * p[0] + p[1] * p[1] + p[2] * p[2]), 1.0f
                            )
                          ));

    }
  }

  std::chrono::high_resolution_clock::time_point t2 =
    std::chrono::high_resolution_clock::now();
  std::cout << "Rendering took "
            << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()
            << "us." << std::endl;

  render_bitmap(img_kwk,"hw_jimg_" + std::to_string(z_value/100) + ".bmp");
}



int main()
{
  // for (int i = 31; i < 64; ++i) {
  //   display_pos(0, 0, i * 1000);
  //   render_slice(i * 1000);

  // }
  render_slice_yx(0);
  render_slice_xy(0);

  pt3D<float> p = {};
  return 0;
}
