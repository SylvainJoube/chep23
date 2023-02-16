#include "acts_struct.hpp"

#include <iostream>
#include <cstddef>
#include <fstream>
#include <vector>
#include <memory>
#include <cmath>
#include "data_structures.hpp"

#define ENABLE_DISPLAY false
constexpr bool DISPLAY_AFFINE_MATRIX = false;
constexpr bool DISPLAY_DIMENSIONS_SIZES = false;

/*
On a passé pas mal de temps à redevier l'ordre des coordonnées
parce qu'elles ne sont pas typées.
A partir du moment où Kiwaku est utilisé pour définir le format des tables,
il n'y a plus le souci de savoir quelles dimensions sont où.
*/

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
  std::string fname = "atlas.cvf";

  std::ifstream ifs(fname, std::ifstream::binary);

  if (!ifs.good())
  {
    std::cout << "Failed to open input file " << fname << "!";
    std::exit(1);
  }

  ifs.read(reinterpret_cast<char *>(&matrix), sizeof(matrix));
  ifs.read(reinterpret_cast<char *>(&dimensions), sizeof(dimensions));
  ifs.read(reinterpret_cast<char *>(&read_numel), sizeof(read_numel));

  data = data_t{kwk::of_size(dimensions[0], dimensions[1], dimensions[2])};
  ifs.read(reinterpret_cast<char *>(data.get_data()), read_numel * sizeof(pt3D<float>));

  ifs.close();

  constexpr auto n = dims;
  constexpr auto m = dims + 1;

  // float arr[dims][dims+1];
  if constexpr(DISPLAY_AFFINE_MATRIX) {
    std::cout << "Affine matrix:\n";
    for (std::size_t i = 0; i < n; ++i)
    {
      for (std::size_t j = 0; j < m; ++j)
      {
        auto e = matrix[i * m + j];
        std::cout << kwk::padTo(std::to_string(e), 20) << "";
      }
      std::cout << "\n";
    }
    std::cout << "\n";
    std::cout << "\n";
  }

  kwk::matrix<SCALAR_TYPE> kmt(n, m, matrix);
  // kmt.print();

  if constexpr (DISPLAY_DIMENSIONS_SIZES) {
    std::cout << "Dimension sizes:  ";
    for (std::size_t i = 0; i < dims; ++i)
    {
      std::cout << dimensions[i] << " ";
    }
    std::cout << "\n";
  }


  std::size_t excepted_numel = 0;
  for (std::size_t i = 0; i < dims; ++i)
  {
    if (i == 0)
      excepted_numel = dimensions[i];
    else
      excepted_numel *= dimensions[i];
  }

  if (excepted_numel != read_numel)
  {
    std::cout << "ERROR : excepted numel(" << excepted_numel << ") does not match read numel(" << read_numel << ")\n";
  }
  else
  {
    // Silent ok
    // std::cout << "OK, matching expected and read numel: " << read_numel << " \n";
  }

  // now useless test
  // kwk::matrix<SCALAR_TYPE> mt(2, 3);
  // mt(0, 0) = 4;
  // std::cout << mt(0, 0) << "\n";
  // mt.print();
}

void acts_data::display_pos(int x, int y, int z)
{
  std::vector<float> kv;
  kv.push_back(x);
  kv.push_back(y);
  kv.push_back(z);
  
  // std::cout << "kv filled.\n";
  // auto kres = m_transform * kv;
}






pt3D<float> acts_data::at_affine(const pt3D<float> & p)
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
  
  // std::cout << "kv filled.\n";
  auto kres = m_transformm * kv;

  // std::cout << "kv: ";
  // for (auto e : kv) {
  //   std::cout << e << " ";
  // }
  // // std::cout << std::endl;

  // std::cout << " - kr: ";
  // for (auto e : kres) {
  //   std::cout << e << " ";
  // }
  // std::cout << std::endl;

  // std::vector<float> popo = {1, 2, 3};
  // auto kaka = m_transform * popo;
  // std::cout << kaka[0] << " " << kaka[1] << " " << kaka[2] << "\n";
  
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
  return kwk::linear.interpolate(data, p[0], p[1], p[2]);
}


