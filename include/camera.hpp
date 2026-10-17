#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "computer_vision.hpp"
#include <glm/detail/qualifier.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>
#include <opencv2/core/mat.hpp>
#include <vector>

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

  Camera() {}

  void rotate(const glm::vec3 &angles) {
    transform = glm::rotate(transform, angles.x, glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, angles.y, glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, angles.z, glm::vec3(0.0f, 0.0f, 1.0f));
  }

  void move(const glm::vec3 &translation) {
    transform = glm::translate(transform, translation);
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
    cv::Mat intrinsic;
    std::vector<double> dist_coefs;
    std::vector<cv::Mat> rotation_vecs;
    std::vector<cv::Mat> trans_vecs;
    if (!calibrate_camera_from_chessboard_images(images, pattern_size,
                                                 intrinsic, dist_coefs,
                                                 rotation_vecs, trans_vecs))
      return false;

    focal_length.x = (float)intrinsic.at<double>(0, 0);
    focal_length.y = (float)intrinsic.at<double>(1, 1);
    offset = glm::vec2((float)intrinsic.at<double>(0, 2),
                       (float)intrinsic.at<double>(1, 2));

    return true;
  }
};

#endif
