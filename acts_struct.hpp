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

  static const std::size_t dims = 3;
  SCALAR_TYPE matrix[dims * (dims + 1)];
  std::size_t dimensions[dims];
  std::size_t read_numel;
  
  data_t data;
  uint S_DISPLAY_COUNT_AFFINE = 0;
  uint S_DISPLAY_COUNT_MAX = 0;

  const bool ENABLE_DISPLAY = false;

};