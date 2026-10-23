
#include "camera.hpp"
#include "computer_vision.hpp"
#include "geometry.hpp"
#include "input_manager.hpp"
#include "renderer.hpp"
#include "texture_manager.hpp"
#include "themes.hpp"
#include "window.hpp"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <iostream>
#include <opencv2/core.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/detail/qualifier.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/string_cast.hpp>
#include <memory>
#include <opencv2/calib3d.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

void show_calibration_scenario(Camera &camera, Renderer &renderer,
                               int scenario_index, std::shared_ptr<Model> cube);

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

  std::string calibration_dir =
      std::string(PROJECT_DIR) + "/assets/images/calibration";
  std::vector<cv::Mat> calibration_images;
  load_directory_images(calibration_dir, calibration_images);
  cv::Size pattern_size = cv::Size(10, 7);

  GLuint background_texture;
  create_default_texture(background_texture);

  DirectionalLight light;
  light.direction = glm::normalize(glm::vec3(0.0, 0.0, -1.0));
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
  cube->add_mesh(cube_mesh);
  cube->scale(glm::vec3(1, 1, 0.1));
  cube->move(-glm::vec3(0.5, 0.5, 0.05));
  // add cube to scene
  renderer.add_model_to_scene(cube);

  double delta_t;
  double last_frame_t = glfwGetTime();
  while (!window.should_close()) {
    delta_t = last_frame_t - glfwGetTime();
    // camera
    if (input_manager.is_just_pressed(GLFW_KEY_C)) {
      if (!camera->calibrate(calibration_images, pattern_size)) {
        fprintf(stderr, "Failed calibration!\n");
      }
      for (int i = 0; i < camera->calibration_info.images.size(); i++) {
        show_calibration_scenario(*camera, renderer, i, cube);
        renderer.render_scene();
        window.swap_buffers();
        last_frame_t = glfwGetTime();
        std::getc(stdin);
      }
    }
    renderer.render_scene();
    input_manager.update();
    window.swap_buffers();
    window.poll_events();
    last_frame_t = glfwGetTime();
  }

  window.close();
}

void show_calibration_scenario(Camera &camera, Renderer &renderer,
                               int scenario_index,
                               std::shared_ptr<Model> cube) {
  if (scenario_index >= camera.calibration_info.images.size()) {
    return;
  }

  GLuint calibration_texture;
  create_default_texture(calibration_texture);
  cv::Mat fliped_img;
  cv::flip(camera.calibration_info.images[scenario_index], fliped_img, -1);

  write_image_to_texture(calibration_texture, fliped_img);
  renderer.set_back_ground_texture(calibration_texture);

  cv::Mat rvec = camera.calibration_info.rotation_vecs[scenario_index];
  cv::Mat tvec = camera.calibration_info.trans_vecs[scenario_index];
  /*
  glm::vec3 pos =
      glm::vec3((float)-tvec.at<double>(0), (float)-tvec.at<double>(1),
                (float)-tvec.at<double>(2));
  std::cout << "rvec:" << tvec << std::endl;
  std::cout << "pos:" << glm::to_string(pos) << std::endl;
  cube->set_position(pos);
  */

  cv::Mat R_cv;
  cv::Rodrigues(rvec, R_cv);

  // OpenCV camera axes -> OpenGL camera axes
  cv::Mat S = (cv::Mat_<double>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 1);

  // Camera-to-world rotation
  cv::Mat R_camera_gl = R_cv.t() * S;

  // Camera position in the calibration pattern's coordinates
  cv::Mat Cw = R_cv.t() * tvec;

  // Construct the GLM camera world transform
  glm::mat4 transform(1.0f);

  for (int row = 0; row < 3; ++row) {
    for (int col = 0; col < 3; ++col) {
      transform[col][row] =
          static_cast<float>(R_camera_gl.at<double>(row, col));
    }
  }

  transform[3] = glm::vec4(static_cast<float>(Cw.at<double>(0)),
                           static_cast<float>(Cw.at<double>(1)),
                           static_cast<float>(Cw.at<double>(2)), 1.0f);
  camera.transform = transform;
}
