#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>
#include <fstream>
#include <glm/glm.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core/mat.hpp>
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

static glm::mat4 cvt_glm_view(cv::Mat rvec, cv::Mat tvec) {
  cv::Mat R;
  cv::Rodrigues(rvec, R);
  // Build 4×4 model-view as glm matrix (column-major)
  return glm::mat4(
      R.at<double>(0, 0), -R.at<double>(1, 0), -R.at<double>(2, 0), 0,
      R.at<double>(0, 1), -R.at<double>(1, 1), -R.at<double>(2, 1), 0,
      R.at<double>(0, 2), -R.at<double>(1, 2), -R.at<double>(2, 2), 0,
      tvec.at<double>(0), -tvec.at<double>(1), -tvec.at<double>(2), 1);
}

#endif
