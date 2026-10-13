#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
private:
  // view matrix vars
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  // projection matrix vars
  glm::float64 aspect_ratio = 16.0f / 9.0f;
  glm::float64 close_plane_distance = 0.1f;
  glm::float64 far_plane_distance = 100.0f;

public:
  glm::vec3 camera_pos = glm::vec3(0.0f, 3.0f, 3.0f);
  glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::float64 fov = 45.0f;

  Camera() {}
  Camera(glm::vec3 camera_pos_, glm::vec3 camera_target_,
         glm::vec3 up_direction_, glm::float64 fov_, glm::float64 aspect_ratio_,
         glm::float64 close_plane_distance_, glm::float64 far_plane_distance_) {
    camera_pos = camera_pos_;
    camera_target = camera_target_;
    up = up_direction_;
    fov = fov_;
    close_plane_distance = close_plane_distance_;
    far_plane_distance = far_plane_distance_;
  }

  void set_fov(glm::float64 fov_) { fov = fov_; }

  glm::vec3 get_camera_direction() {
    return glm::normalize(camera_pos - camera_target);
  }

  glm::vec3 get_camera_right() {
    return glm::normalize(glm::cross(up, get_camera_direction()));
  }

  glm::vec3 get_camera_up() {
    return glm::normalize(
        glm::cross(get_camera_direction(), get_camera_right()));
  }

  glm::vec3 get_camera_front() { return -get_camera_direction(); }

  glm::mat4 get_view_matrix() {
    return glm::lookAt(camera_pos, camera_pos + get_camera_front(),
                       get_camera_up());
  }

  glm::mat4 get_projection_matrix() {
    return glm::perspective(fov, aspect_ratio, close_plane_distance,
                            far_plane_distance);
  }
};

#endif
