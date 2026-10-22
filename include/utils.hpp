#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>
#include <fstream>
#include <glm/glm.hpp>
#include <opencv2/core/matx.hpp>
#include <sstream>
#include <string>

static bool read_file(const std::string &path, std::string &file_content) {
  std::ifstream file(path);
  if (!file) {
    return false;
  }

  std::stringstream buff;
  buff << file.rdbuf();
  file_content = buff.str();

  return true;
}

static constexpr float red_channel(std::uint32_t color) {
  return static_cast<float>((color >> 16) & 0xff) / 255.0f;
}

static constexpr float green_channel(std::uint32_t color) {
  return static_cast<float>((color >> 8) & 0xff) / 255.0f;
}

static constexpr float blue_channel(std::uint32_t color) {
  return static_cast<float>(color & 0xff) / 255.0f;
}

static constexpr std::uint8_t i_red_channel(std::uint32_t color) {
  return static_cast<std::uint8_t>((color >> 16) & 0xff);
}

static constexpr std::uint8_t i_green_channel(std::uint32_t color) {
  return static_cast<std::uint8_t>((color >> 8) & 0xff);
}

static constexpr std::uint8_t i_blue_channel(std::uint32_t color) {
  return static_cast<std::uint8_t>(color & 0xff);
}

static glm::vec3 cv_to_glm_vec3(cv::Vec3f vec) {
  return glm::vec3(vec[0], -vec[1], -vec[2]);
}

#endif
