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

#define TRACKER_FEATURES 2000
#define FEATURE_RATIO 0.8
const std::string TARGET_IMAGE_PATH =
    std::string(PROJECT_DIR) + "/assets/images/feature_matching/target.png";

enum Mode { FEATURE, POSE };

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
  const glm::vec3 default_camera_position = glm::vec3(0, 0, -10);
  camera->set_position(default_camera_position);

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

  // calibrate camera
  std::string calibration_dir =
      std::string(PROJECT_DIR) + "/assets/images/calibration";
  std::vector<cv::Mat> calibration_images;
  load_directory_images(calibration_dir, calibration_images);
  cv::Size pattern_size = cv::Size(10, 7);
  if (!camera->calibrate(calibration_images, pattern_size)) {
    fprintf(stderr, "Failed calibration!\n");
  }

  cv::Mat target_image = cv::imread(TARGET_IMAGE_PATH);
  FeatureTracker feature_tracker =
      FeatureTracker(TRACKER_FEATURES, target_image, 1, 1, FEATURE_RATIO);
  feature_tracker.set_camera_calibration(camera->calibration_info.intrinsic,
                                         camera->calibration_info.dist_coefs);
  GLuint background_texture;
  create_default_texture(background_texture);

  Mode mode = FEATURE;

  // light creation
  DirectionalLight light;
  light.direction = glm::normalize(glm::vec3(0.0, 0.0, 1.0));
  light.ambient = glm::vec3(0.2);
  light.diffuse = glm::vec3(0.5);
  light.specular = glm::vec3(1.0);
  renderer.set_scene_light(light);

  // cube creation
  // vertices
  std::vector<Vertex> cube_vertices;
  std::vector<uint> cube_indices;
  get_cube_geometry(1, cube_vertices, cube_indices);
  GLuint cube_diffuse;
  texture_manager.load_texture_from_color(gruvbox::orange, cube_diffuse);
  GLuint cube_specular;
  texture_manager.load_texture_from_color(gruvbox::fg, cube_specular);
  // material
  Material cube_material;
  cube_material.diffuse_texture = cube_diffuse;
  cube_material.specular_texture = cube_specular;
  cube_material.shininess = 32;
  // model
  Mesh cube_mesh = Mesh(cube_vertices, cube_indices, cube_material);
  std::shared_ptr<Model> cube = std::make_shared<Model>();
  const glm::vec3 default_cube_position = glm::vec3(0.5, 0.5, 0);
  cube->add_mesh(cube_mesh);
  cube->set_position(default_cube_position);
  cube->set_scale(glm::vec3(0.5, 0.5, 0.5));
  // add cube to scene
  renderer.add_model_to_scene(cube);

  double delta_t;
  double last_frame_t = glfwGetTime();
  while (!window.should_close()) {
    delta_t = last_frame_t - glfwGetTime();
    webcam.update_frame();
    feature_tracker.set_input_image(webcam.frame);
    feature_tracker.visualy_track(true);
    // get input
    if (input_manager.is_just_pressed(GLFW_KEY_1)) {
      mode = POSE;
    }
    if (input_manager.is_just_pressed(GLFW_KEY_0)) {
      mode = FEATURE;
    }
    // change background
    switch (mode) {
    case FEATURE: {
      cv::Mat background_image;
      cv::flip(feature_tracker.get_match_image(), background_image, 0);
      write_image_to_texture(background_texture, background_image);
    }
      camera->set_position(default_camera_position);
      break;
    case POSE: {
      cv::Mat background_image;
      cv::flip(webcam.frame, background_image, 0);
      write_image_to_texture(background_texture, background_image);
    }
      auto pose = feature_tracker.get_estimated_pose();
      auto rvec = get<0>(pose);
      auto tvec = get<1>(pose);
      if (rvec.empty() or tvec.empty()) {
        camera->set_position(default_camera_position);
        break;
      }
      glm::mat4 view = cvt_glm_view(rvec, tvec);
      camera->set_transform_from_view(view);
      break;
    }
    renderer.set_back_ground_texture(background_texture);

    renderer.render_scene();
    input_manager.update();
    window.swap_buffers();
    window.poll_events();
    last_frame_t = glfwGetTime();
  }

  window.close();
}
