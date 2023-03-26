#pragma once

#include <iostream>
#include <cstddef>
#include <fstream>
#include <vector>
#include <memory>
#include <cmath>
#include "data_structures.hpp"
#include <kwk/kwk.hpp>

struct acts_data
{
  using data_t = kwk::make_table_t<kwk::as<pt3D<float>>, kwk::_3D>;

  bool should_display(bool modify = false);
  void read_acts_file();
  pt3D<float> at(const pt3D<float> &p) { return at_affine(p); }
  pt3D<float> at_affine(const pt3D<float> &p);
  pt3D<float> at_linear(const pt3D<float> &p);

  inline pt3D<float> inline_at(const pt3D<float> &p) { return inline_at_affine(p); }
  inline pt3D<float> inline_at_affine(const pt3D<float> &p);
  inline pt3D<float> inline_at_linear(const pt3D<float> &p);

  void display_pos(int x, int y, int z);

  static const std::size_t dims = 3;
  SCALAR_TYPE matrix[dims * (dims + 1)];
  std::size_t dimensions[dims];
  std::size_t read_numel;

  // kwk::matrix<float> m_transform; // Calculated by read_acts_file
  
  data_t data;
  uint S_DISPLAY_COUNT_AFFINE = 0;
  uint S_DISPLAY_COUNT_MAX = 0;

  const bool ENABLE_DISPLAY = false;

};


inline pt3D<float> acts_data::inline_at_affine(const pt3D<float> & p)
{

  // ==== AFFINE ====
  #if ENABLE_DISPLAY
  bool display = should_display(true);
  #endif

  std::size_t sd0 = dims;
  std::size_t sd1 = dims + 1;

  kwk::matrix<float> m_transformm(sd0, sd1, matrix);

  #if ENABLE_DISPLAY
  if (display) m_transform.print();
  #endif

  std::vector<float> kv(p.begin(),p.end());
  
  auto kres = m_transformm * kv;

  #if ENABLE_DISPLAY
  if (display) {
    for (uint i = 0; i < sd0; ++i) {
      std::cout << "kk(" << i << ") = " << kres.at(i) << "\n";
    }
  }
  #endif

  // ==== LINEAR ====
  return acts_data::inline_at_linear(pt3D<float>{ kres[0], kres[1], kres[2]} );
}

inline pt3D<float> acts_data::inline_at_linear(const pt3D<float> &p)
{
  return kwk::linear.interpolate(data, p[0], p[1], p[2]);
}