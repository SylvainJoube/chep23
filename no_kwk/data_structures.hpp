#pragma once

#include "types.hpp"
#include <vector>
#include <iostream>
#include "../utils/printer.hpp"

namespace kwk
{

  template <typename T = SCALAR_TYPE>
  struct matrix
  {
    std::size_t d0, d1;
    std::vector<T> elems;

    inline explicit matrix(std::size_t d0_, std::size_t d1_) : d0(d0_), d1(d1_)
    {
      elems.resize(d0 * d1);
    }    
    
    // If used, use init(std::size_t d0_, std::size_t d1_, T* array)
    inline explicit matrix() = default;
    
    inline void init(std::size_t d0_, std::size_t d1_, T* array)
    {
      d0 = d0_;
      d1 = d1_;
      elems.resize(d0 * d1);
      for (uint i = 0; i < d0*d1; ++i) elems[i] = array[i];
    }

    inline explicit matrix(std::size_t d0_, std::size_t d1_, T* array) : d0(d0_), d1(d1_)
    {
      elems.resize(d0 * d1);
      for (uint i = 0; i < d0*d1; ++i) elems[i] = array[i];
    }

    inline T &operator()(std::size_t x, std::size_t y)
    {
      // return elems[x+ y * d0]);
      return elems[x* d1 + y];
    }

    inline T operator()(std::size_t x, std::size_t y) const
    {
      return elems[x* d1 + y];
    }

    inline T & at(std::size_t x, std::size_t y)
    {
      // return elems[x]* d1 + y);
      return elems[x* d1 + y];
    }

    inline T at(std::size_t x, std::size_t y) const
    {
      return elems[x* d1 + y];
    }

    inline void print(/*std::ostream os,*/ uint pad_length = 20)
    {
      for (uint i = 0; i < d0; ++i)
      {
        for (uint j = 0; j < d1; ++j)
        {
          std::cout << bench_t::pad_right(std::to_string(this->operator()(i, j)), pad_length) << " ";
        }
        std::cout << "\n";
      }
    }

    template <typename U>
    inline std::vector<U> operator*(const std::vector<U> &v) const
    {
      auto N = d0;
      kwk::matrix<U> r(N + 1, 1);

      for (uint i = 0; i < N; ++i)
      {
        r.at(i, 0) = v[i];
      }

      r.at(N, 0) = 1.;

      auto res = operator*(r);

      std::vector<U> vres;
      vres.resize(N + 1);
      for (uint i = 0; i < N; ++i)
      {
        vres[i] = res(i, 0);
      }

      return vres;
    }

    // template <std::size_t P>matrix<N, P, T, I>
    //  M P T I
    //  returns a N * P matrix
    //  this (N*M) * other (M*P)
    //  (d0*d1) * (d1*d2)
    template <typename U>
    inline kwk::matrix<U> operator*(const kwk::matrix<U> &o) const
    {
      // matrix<N, P, T, I> r;
      std::size_t N = d0;
      std::size_t M = d1;
      std::size_t P = o.d1;
      if (d1 != o.d0)
        std::cout << ">>>>>>>>>> ERROR d1 != o.d0\n";

      kwk::matrix<U> r(N, P);

      for (uint i = 0; i < N; ++i)
      {
        for (uint j = 0; j < P; ++j)
        {
          T t = 0.;

          for (uint k = 0; k < M; ++k)
          {
            t += this->operator()(i, k) * o.operator()(k, j);
          }
          r(i, j) = t;
        }
      }
      // std::cout << "r size (" << N << ", " << P << ")\n";
      return r;
    }
  };
}


template<typename T>
using pt3D = std::array<T, 3>;

template<typename T>
inline std::ostream& operator<<(std::ostream &s, const pt3D<T> &p)
{
  return s << "(" << p[0] << "," << p[1] << "," << p[2] << ")";
}