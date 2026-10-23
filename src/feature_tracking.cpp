#include "camera.hpp"
#include "computer_vision.hpp"
#include "feature_tracker.hpp"
#include "input_manager.hpp"
#include "renderer.hpp"
#include "texture_manager.hpp"
#include "window.hpp"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtc/constants.hpp>
#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <string>

#define TRACKER_FEATURES 1000
#define FEATURE_RATIO 0.7
const std::string TARGET_IMAGE_PATH =
    std::string(PROJECT_DIR) + "/assets/images/feature_matching/target.png";

int main() {
  // component initialization
  Window window = Window("Calibration", 640, 480);
  if (!window.init()) {
    fprintf(stderr, "Failed to initialize window!\n");
    return -1;
  }
  InputManager input_manager = InputManager(window);

  TextureManager texture_manager = TextureManager();

  std::shared_ptr<Camera> camera = std::make_shared<Camera>();

  Renderer renderer = Renderer(camera);
  if (!renderer.setup()) {
    fprintf(stderr, "Renderer setup failed!\n");
    return -1;
  }

  WebcamCapturePipeline webcam = WebcamCapturePipeline();
  if (!webcam.setup()) {
    fprintf(stderr, "Failed to initialize webcam capture!\n");
    return -1;
  }

  cv::Mat target_image = cv::imread(TARGET_IMAGE_PATH);
  cv::flip(target_image, target_image, -1);
  FeatureTracker feature_tracker =
      FeatureTracker(TRACKER_FEATURES, target_image, FEATURE_RATIO);

  GLuint background_texture;
  create_default_texture(background_texture);

  double delta_t;
  double last_frame_t = glfwGetTime();
  while (!window.should_close()) {
    delta_t = last_frame_t - glfwGetTime();
    webcam.update_frame();
    feature_tracker.find_good_matches(webcam.frame, true);
    write_image_to_texture(background_texture,
                           feature_tracker.get_match_image());
    renderer.set_back_ground_texture(background_texture);

    renderer.render_scene();
    input_manager.update();
    window.swap_buffers();
    window.poll_events();
    last_frame_t = glfwGetTime();
  }

  window.close();
}
