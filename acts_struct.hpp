#pragma once

#include <iostream>
#include <cstddef>
#include <fstream>
#include <vector>
#include <memory>
#include <cmath>
#include "data_structures.hpp"

struct acts_data
{
  bool should_display(bool modify = false);
  void read_acts_file();
  pt3D<float> at(const pt3D<float> &p);
  pt3D<float> at_affine(const pt3D<float> &p);
  pt3D<float> at_linear(const pt3D<float> &p);
  pt3D<float> at_strided(const pt3D<std::size_t> &c);
  pt3D<float> at_array(std::size_t c) { return data[c]; }

  static const std::size_t dims = 3;
  SCALAR_TYPE matrix[dims * (dims + 1)];
  std::size_t stride_sizes[dims];
  std::size_t read_numel;
  std::unique_ptr<pt3D<float>[]> data;
  uint S_DISPLAY_COUNT_AFFINE = 0;
  uint S_DISPLAY_COUNT_MAX = 0;

  const bool ENABLE_DISPLAY = false;

};