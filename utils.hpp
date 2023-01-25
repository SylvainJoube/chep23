#pragma once

#include <string>

namespace kwk
{
  inline std::string padTo(std::string str, const size_t num, const char paddingChar = ' ')
  {
    if (num > str.size())
      str.insert(0, num - str.size(), paddingChar);
    return str;
  }
}