#include <iostream>
#include <cstddef>
#include <fstream>
#include <vector>
#include <memory>
#include <cmath>
#include <chrono>
#include "bitmap.hpp"
#include "data_structures.hpp"
#include "types.hpp"
#include "acts_struct.hpp"

#include "../utils/global_file.hpp"
#include "../utils/chrono.hpp"

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

$ clang++-15 covfie_kwk_test.cpp bitmap.cpp -o exe -O3 -std=c++20 && ./exe
*/



void render_slice(float z_value = 0)
{
  acts_data a;
  a.read_acts_file();

  std::size_t image_width  = 1024;
  std::size_t image_height = 1024;

  bench::chrono_t chrono;
  chrono.Init();

  char* img = new char[image_width * image_height];

  // std::unique_ptr<char[]> img =
  //   std::make_unique<char[]>(image_width * image_height);

  std::chrono::high_resolution_clock::time_point t1 =
    std::chrono::high_resolution_clock::now();

  for (uint x = 0; x < image_width; ++x) {
    for (uint y = 0; y < image_height; ++y) {
      float fx = x / static_cast<float>(image_width);
      float fy = y / static_cast<float>(image_height);

      pt3D<float> asked{fx * 20000.f - 10000.f, fy * 20000.f - 10000.f, z_value};

      pt3D<float> p = a.at(asked);
      // Do something with p
      img[image_height * x + y] =
                static_cast<char>(std::lround(
                    255.f *
                    std::min(
                        std::sqrt(p[0] * p[0] + p[1] * p[1] + p[2] * p[2]), 1.0f
                    )
                ));
    }
  }

  double elapsed_time = chrono.ElapsedTime();

  std::chrono::high_resolution_clock::time_point t2 =
    std::chrono::high_resolution_clock::now();

  // Checking image integrity
  std::uint64_t int_chk = 0;
  for (std::size_t x = 0; x < image_width; ++x) {
    for (std::size_t y = 0; y < image_height; ++y) {
      int_chk += static_cast<std::uint8_t>(img[image_height * x + y]); //img[image_height * x + y]
    }
  }

  /*
  Structure du fichier (texte) :
  Autant de fois qu'il y a d'évènements (nouvelle ligne) :
    z_value | elapsed_time_us | check_string
  */
  write_f
  << z_value << " " 
  << static_cast<uint64_t>(elapsed_time * 1000000) << " " // microseconds
  << int_chk << "\n";
  
  std::cout << "---> z_value(" << z_value << ")";
  std::cout << " took " << static_cast<std::size_t>(elapsed_time*1000) << "ms\n";

  std::cout << "Rendering took "
            << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()
            << "us." << std::endl;

  render_bitmap(
        img,//.get(),
        image_width,
        image_height,
        "../img_out/2023-03-26_standalone_" + std::to_string(z_value/100) + ".bmp"
    );
    
  delete[] img;
}


int main()
{
  float offset = 15000;
  printer_t::head("HANDMADE STANDALONE (inline)");

  write_f.open("bench_acts_field_standalone.txt");
  write_f << "1\n";
  // for (int i = 31; i < 64; ++i) {
  for (float i = offset; i < 30000+offset; i += 6000) {
    render_slice(i);
  }
  write_f.close();

  return 0;
}
