#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
private:
  // view matrix vars
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  // projection matrix vars
  glm::float64 aspectRatio = 16.0f / 9.0f;
  glm::float64 closePlaneDistance = 0.1f;
  glm::float64 farPlaneDistance = 100.0f;

public:
  glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, 3.0f);
  glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::float64 fov = 45.0f;

  Camera() {}
  Camera(glm::vec3 cameraPos_, glm::vec3 cameraTarget_, glm::vec3 upDirection_,
         glm::float64 fov_, glm::float64 aspectRatio_,
         glm::float64 closePlaneDistance_, glm::float64 farPlaneDistance_) {
    cameraPos = cameraPos_;
    cameraTarget = cameraTarget_;
    up = upDirection_;
    fov = fov_;
    closePlaneDistance = closePlaneDistance_;
    farPlaneDistance = farPlaneDistance_;
  }

  void setFOV(glm::float64 fov_) { fov = fov_; }

  glm::vec3 getCameraDirection() {
    return glm::normalize(cameraPos - cameraTarget);
  }

  glm::vec3 getCameraRight() {
    return glm::normalize(glm::cross(up, getCameraDirection()));
  }

  glm::vec3 getCameraUp() {
    return glm::normalize(glm::cross(getCameraDirection(), getCameraRight()));
  }

  glm::vec3 getCameraFront() { return -getCameraDirection(); }

  glm::mat4 getViewMatrix() {
    return glm::lookAt(cameraPos, cameraPos + getCameraFront(), getCameraUp());
  }

  glm::mat4 getProjectionMartix() {
    return glm::perspective(fov, aspectRatio, closePlaneDistance,
                            farPlaneDistance);
  }
};
