#pragma once

#include <string>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>

// void log(const std::string& s)
// {
//   std::cout << s << "\n";
// }


struct printer_t
{
  static void head(const std::string& str)
  {
    std::string hd = "=================================";
    std::string hp = "-";
    
    std::string header = hp + " " + str + " " + hp;
    uint rpad_len = (hd.size() - header.size()) / 2;
    std::string rpad(rpad_len, '=');
    uint lpad_len = hd.size() - (rpad_len + header.length());
    std::string lpad(lpad_len, '=');

    std::cout << hd << "\n";
    std::cout << hd << "\n";
    std::cout << rpad << header << lpad << "\n";
    std::cout << hd << "\n";
    std::cout << hd << "\n\n";
  }

  static void err(const std::string& str) {
    error(str);
  }
  static void warn(const std::string& str) {
    warning(str);
  }


  static void error(const std::string& str) {
    std::cout << "!!!!!!!       ERROR   : " << str << "\n";
  }
  static void ok(const std::string& str) {
    std::cout << "OK      : " << str << "\n";
  }
  static void warning(const std::string& str) {
    std::cout << "!!!!!!!       WARNING : " << str << "\n";
  }
  
};

using print = printer_t;

struct bench_t
{
public:
  static std::string pad_right(std::string str, const std::size_t len, const char paddingChar = ' ')
  {
    if (len <= str.size()) return str;
    str.insert(0, len - str.size(), paddingChar);
    return str;
  }

  static std::string pad_right(int value, const std::size_t len, const char paddingChar = ' ')
  {
    std::string str = std::to_string(value);
    return pad_right(str, len, paddingChar);
  }
};