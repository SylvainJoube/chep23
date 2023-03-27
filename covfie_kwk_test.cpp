#include <iostream>
#include <cstddef>
// #include <fstream>
// #include <vector>
// #include <memory>
#include <cmath>
#include <chrono>
#include <iostream>
#include "bitmap.hpp"
// #include "utils.hpp"
#include "data_structures.hpp"
#include "types.hpp"
#include "acts_struct.hpp"
#include <kwk/kwk.hpp>

#include "utils/global_file.hpp"
#include "utils/chrono.hpp"

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

$ clang++-15 covfie_kwk_test.cpp bitmap.cpp acts_struct.cpp -o exe -O3 -std=c++20 -I"/home/data_not_sync/logiciels/kiwaku/include" && ./exe 

$ mencoder mf://*.bmp -mf fps=10:type=bmp -ovc x264 -x264encopts bitrate=1200:threads=2 -o outputfile.mkv
*/



void render_slice_kiwaku(float z_value = 0, bool use_inline = false)
{
  acts_data a;
  a.read_acts_file();

  uint const width  = 1024;
  uint const height = 1024;

  bench::chrono_t chrono;
  std::string chk_str;
  chrono.Init();

  auto img = kwk::table{kwk::type = kwk::int8, kwk::of_size(width, height)};

  std::chrono::high_resolution_clock::time_point t1 =
  std::chrono::high_resolution_clock::now();

  if (use_inline)
  {
    kwk::for_each_index([ fw = static_cast<float>(width)
                        , fh = static_cast<float>(height)
                        , z_value
                        , &a
                        ]
    (auto& e, auto x, auto y) 
    {
      float fx = x / fw;
      float fy = y / fh;

      pt3D<float> asked{fx * 20000.f - 10000.f, fy * 20000.f - 10000.f, z_value};

      pt3D<float> p = a.inline_at(asked);
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
  } else {
    kwk::for_each_index([ fw = static_cast<float>(width)
                        , fh = static_cast<float>(height)
                        , z_value
                        , &a
                        ]
    (auto& e, auto x, auto y) 
    {
      float fx = x / fw;
      float fy = y / fh;

      // Size: 201 * 201 * 301
      // Size*100: 20100 * 20100 * 30100
      // pt3D<float> asked{fx * 20000.f, fy * 20000.f, z_value};

      // pt3D<float> asked{fx * 200.f, fy * 200.f, z_value};
      // pt3D<float> p = a.at_linear(asked);

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
  }

  double elapsed_time = chrono.ElapsedTime();

  std::chrono::high_resolution_clock::time_point t2 =
    std::chrono::high_resolution_clock::now();

  // Checking image integrity
  std::uint64_t int_chk = 0;
  kwk::for_each([&int_chk](auto& e) 
  {
    int_chk += static_cast<std::uint8_t>(e);
  }
  , img);

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


  /*
  Issue Kiwaku : (02-02-2023)
  - Source doit accepter des containers.
  */

  std::cout << "Rendering took "
            << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()
            << "us." << std::endl;

  render_bitmap(img,"img_out/2023-03-26_jimg_" + std::to_string(z_value/1000) + ".bmp");
}

int main()
{
  bool use_inline;
  float offset = 15000;

  printer_t::head("KIWAKU INLINE");

  use_inline = true;
  write_f.open("bench_acts_field_kiwaku_inline.txt");
  write_f << "1\n";
  for (float i = offset; i < 30000+offset; i += 6000) {
    render_slice_kiwaku(i, use_inline);
  }
  write_f.close();

  printer_t::head("KIWAKU NOT INLINE");

  use_inline = false;
  write_f.open("bench_acts_field_kiwaku_noinline.txt");
  write_f << "1\n";
  for (float i = offset; i < 30000+offset; i += 6000) {
    render_slice_kiwaku(i, use_inline);
  }
  write_f.close();

  // pt3D<float> p = {};
  return 0;
}
