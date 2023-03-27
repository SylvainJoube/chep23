#pragma once

#include <iostream>
#include <cstddef>
#include <fstream>
#include <vector>
#include <memory>
#include <cmath>
#include "data_structures.hpp"
#include "../utils/printer.hpp"

struct acts_data
{
  inline bool should_display(bool modify = false);
  inline void read_acts_file();
  inline pt3D<float> at(const pt3D<float> &p);
  inline pt3D<float> at_affine(const pt3D<float> &p);
  inline pt3D<float> at_linear(const pt3D<float> &p);
  inline pt3D<float> at_strided(const pt3D<std::size_t> &c);
  inline pt3D<float> at_array(std::size_t c) { return data[c]; }

  inline void init_affine_matrix();

  static const std::size_t dims = 3;
  SCALAR_TYPE matrix[dims * (dims + 1)];
  std::size_t stride_sizes[dims];
  std::size_t read_numel;
  std::unique_ptr<pt3D<float>[]> data;
  uint S_DISPLAY_COUNT_AFFINE = 0;
  uint S_DISPLAY_COUNT_MAX = 0;
  kwk::matrix<float> m_transform_global;

  const bool ENABLE_DISPLAY = false;

};

#define ENABLE_DISPLAY false

#if ENABLE_DISPLAY
bool acts_data::should_display(bool modify)
{
  // if constexpr (ENABLE_DISPLAY) {}
  if (modify) ++S_DISPLAY_COUNT_AFFINE;
  if (S_DISPLAY_COUNT_AFFINE <= S_DISPLAY_COUNT_MAX) return true;
  else                                              return false;
}
#endif

void acts_data::read_acts_file()
{
  std::string fname = "../atlas.cvf";

  std::ifstream ifs(fname, std::ifstream::binary);

  if (!ifs.good())
  {
    std::cout << "Failed to open input file " << fname << "!";
    std::exit(1);
  }

  ifs.read(reinterpret_cast<char *>(&matrix), sizeof(matrix));
  ifs.read(reinterpret_cast<char *>(&stride_sizes), sizeof(stride_sizes));
  ifs.read(reinterpret_cast<char *>(&read_numel), sizeof(read_numel));

  data = std::make_unique<pt3D<float>[]>(read_numel);
  ifs.read(reinterpret_cast<char *>(data.get()), read_numel * sizeof(pt3D<float>));

  ifs.close();

  constexpr auto n = dims;
  constexpr auto m = dims + 1;

  init_affine_matrix();


  #if ENABLE_DISPLAY
  // float arr[dims][dims+1];
  std::cout << "Affine matrix:\n";
  for (std::size_t i = 0; i < n; ++i)
  {
    for (std::size_t j = 0; j < m; ++j)
    {
      auto e = matrix[i * m + j];
      std::cout << bench_t::pad_right(std::to_string(e), 20) << "";
    }
    std::cout << "\n";
  }
  std::cout << "\n";
  std::cout << "\n";

  kwk::matrix<SCALAR_TYPE> kmt(n, m, matrix);
  kmt.print();


  std::cout << "Dimension sizes:  ";
  for (std::size_t i = 0; i < dims; ++i)
  {
    std::cout << stride_sizes[i] << " ";
  }
  std::cout << "\n";
  #endif

  std::size_t excepted_numel = 0;
  for (std::size_t i = 0; i < dims; ++i)
  {
    if (i == 0)
      excepted_numel = stride_sizes[i];
    else
      excepted_numel *= stride_sizes[i];
  }

  if (excepted_numel != read_numel)
  {
    std::cout << "ERROR : excepted numel(" << excepted_numel << ") does not match read numel(" << read_numel << ")\n";
  }
  #if ENABLE_DISPLAY
  else
  {
    std::cout << "OK, matching expected and read numel: " << read_numel << " \n";
  }
  #endif

  // kwk::matrix<SCALAR_TYPE> mt(2, 3);
  // mt(0, 0) = 4;
  // std::cout << mt(0, 0) << "\n";

  // mt.print();
}





pt3D<float> acts_data::at(const pt3D<float> &p)
{
  return at_affine(p);
}

pt3D<float> acts_data::at_affine(const pt3D<float> & p)
{

  // ==== AFFINE ====
  #if ENABLE_DISPLAY
  bool display = should_display(true);
  #endif

  // std::size_t sd0 = dims;
  // std::size_t sd1 = dims + 1;

  // kwk::matrix<float> m_transform(sd0, sd1, matrix);

  #if ENABLE_DISPLAY
  if (display) m_transform.print();
  #endif

  std::vector<float> kv(p.begin(),p.end());
  
  // std::cout << "kv filled.\n";
  auto kres = m_transform_global * kv;
  // std::cout << "multiplicaton done.\n";

  #if ENABLE_DISPLAY
  if (display) {
    for (uint i = 0; i < sd0; ++i) {
      std::cout << "kk(" << i << ") = " << kres.at(i) << "\n";
    }
  }
  #endif

  // ==== LINEAR ====

  // auto val = m_backend.at(nc); // linear backend
  return acts_data::at_linear(pt3D<float>{ kres[0], kres[1], kres[2]} );
}

pt3D<float> acts_data::at_linear(const pt3D<float> &p)
{
  std::size_t i = static_cast<std::size_t>(p[0]);
  std::size_t j = static_cast<std::size_t>(p[1]);
  std::size_t k = static_cast<std::size_t>(p[2]);

  float a = p[0] - i;
  float b = p[1] - j;
  float c = p[2] - k;

  float ra = static_cast<float>(1.) - a;
  float rb = static_cast<float>(1.) - b;
  float rc = static_cast<float>(1.) - c;

  #if ENABLE_DISPLAY
  bool display = should_display();
  #endif

  pt3D<float> pc[8];

  #if ENABLE_DISPLAY
  if (display) std::cout << "Linear: ";
  #endif

  for (std::size_t n = 0; n < 8; ++n) {

    std::size_t p0 = i + ((n & 4) ? 1 : 0);
    std::size_t p1 = j + ((n & 2) ? 1 : 0);
    std::size_t p2 = k + ((n & 1) ? 1 : 0);
    pt3D<std::size_t> pp = {p0, p1, p2};

    #if ENABLE_DISPLAY
    if (display) std::cout << pp << " ";
    #endif

    pc[n] = at_strided(pp);
  }

  #if ENABLE_DISPLAY
  if (display) std::cout << "  :  = ";
  #endif

  pt3D<float> rv; // 3D
  for (std::size_t q = 0; q < 3; ++q) // 3D
  {
    rv[q] = ra * rb * rc * pc[0][q] + ra * rb * c * pc[1][q] +
            ra * b * rc * pc[2][q] + ra * b * c * pc[3][q] +
            a * rb * rc * pc[4][q] + a * rb * c * pc[5][q] +
            a * b * rc * pc[6][q] + a * b * c * pc[7][q];
  }
  
  #if ENABLE_DISPLAY
  if (display) std::cout << "(" << rv[0] << "," << rv[1] << "," << rv[2] << ")\n";
  #endif

  return rv;
}

pt3D<float> acts_data::at_strided(const pt3D<std::size_t> &c)
{
  float idx = 0;

  #if ENABLE_DISPLAY
  bool display = should_display();
  #endif

  for (std::size_t k = 0; k < dims; ++k)
  {
      float tmp = c[k];

      for (std::size_t l = k + 1; l < dims; ++l) {
          tmp *= stride_sizes[l];
      }

      idx += tmp;
  }

  #if ENABLE_DISPLAY
  if (display) std::cout << "Strided access at " << idx << "\n";
  #endif

  auto value = at_array(idx);

  #if ENABLE_DISPLAY
  if (display) std::cout << "Value = " << value << "\n";
  #endif

  return value;
}

inline void acts_data::init_affine_matrix()
{
  // ==== AFFINE ====
  #if ENABLE_DISPLAY
  bool display = should_display(true);
  #endif

  std::size_t sd0 = dims;
  std::size_t sd1 = dims + 1;

  //kwk::matrix<float> 
  m_transform_global.init(sd0, sd1, matrix);

  #if ENABLE_DISPLAY
  if (display) m_transform_global.print();
  #endif
}