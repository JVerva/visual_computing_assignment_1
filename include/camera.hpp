#ifndef CAMERA_HPP
#define CAMERA_HPP

#define GLM_ENABLE_EXPERIMENTAL
#include "computer_vision.hpp"
#include "utils.hpp"
#include <glm/detail/qualifier.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/matrix.hpp>
#include <iostream>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/matx.hpp>
#include <vector>

struct CalibrationInfo {
  std::vector<cv::Mat> images;
  cv::Mat intrinsic;
  std::vector<double> dist_coefs;
  std::vector<cv::Mat> rotation_vecs;
  std::vector<cv::Mat> trans_vecs;
};

class Camera {
private:
  // view matrix vars
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  // projection matrix vars
  glm::vec2 resolution = glm::vec2(640, 480);
  glm::vec2 offset = glm::vec2(resolution.x / 2, resolution.y / 2);
  glm::vec2 focal_length = glm::vec2(772.55);
  float near_plane_distance = 0.1;
  float far_plane_distance = 100.0;

public:
  glm::mat4 transform = glm::mat4(1);
  CalibrationInfo calibration_info;

  Camera() {}

  void rotate(const glm::vec3 &rotation) {
    float magnitude = glm::length(rotation);
    glm::vec3 axis = glm::normalize(rotation);
    transform = glm::rotate(transform, magnitude, axis);
  }

  void move(const glm::vec3 &translation) {
    transform = glm::translate(transform, translation);
  }

  void set_position(const glm::vec3 &translation) {
    transform[3][0] = 0;
    transform[3][1] = 0;
    transform[3][2] = 0;
    move(translation);
  }

  void set_rotation(const glm::vec3 &rotation) {
    transform[0][0] = 1;
    transform[0][1] = 0;
    transform[0][2] = 0;
    transform[1][0] = 0;
    transform[1][1] = 1;
    transform[1][2] = 0;
    transform[2][0] = 0;
    transform[2][1] = 0;
    transform[2][2] = 1;
    rotate(rotation);
  }

  glm::vec3 get_position() {
    return glm::vec3(transform[3][0], transform[3][1], transform[3][2]);
  }

  glm::mat4 get_view_matrix() { return glm::inverse(transform); }

  glm::mat4 get_projection_matrix() {
    float left = -offset.x * near_plane_distance / focal_length.x;
    float right =
        (resolution.x - offset.x) * near_plane_distance / focal_length.x;
    float bottom =
        (-resolution.y + offset.y) * near_plane_distance / focal_length.y;
    float top = offset.y * near_plane_distance / focal_length.y;

    return glm::frustum(left, right, bottom, top, near_plane_distance,
                        far_plane_distance);
  }

  bool calibrate(std::vector<cv::Mat> &images, cv::Size &pattern_size) {
    if (!calibrate_camera_from_chessboard_images(
            images, pattern_size, calibration_info.intrinsic,
            calibration_info.dist_coefs, calibration_info.rotation_vecs,
            calibration_info.trans_vecs, calibration_info.images, true))
      return false;
    focal_length =
        glm::vec2((float)calibration_info.intrinsic.at<double>(0, 0),
                  (float)calibration_info.intrinsic.at<double>(1, 1));

    offset = glm::vec2(
        resolution.x - (float)calibration_info.intrinsic.at<double>(0, 2),
        resolution.y - (float)calibration_info.intrinsic.at<double>(1, 2));
    std::cout << glm::to_string(offset) << std::endl;
    return true;
  }
};

#endif
