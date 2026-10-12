#ifndef UTILS_HPP
#define UTILS_HPP

#include <fstream>
#include <sstream>
#include <string>

bool read_file(const std::string &path, std::string &file_content) {
  std::ifstream file(path);
  if (!file) {
    return false;
  }

  std::stringstream buff;
  buff << file.rdbuf();
  file_content = buff.str();

  return true;
}

#endif
