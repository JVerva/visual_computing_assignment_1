#include "camera.hpp"
#include "geometry.hpp"
#include "renderer.hpp"
#include "texture_manager.hpp"
#include "themes.hpp"
#include "window.hpp"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtc/constants.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

int main() {
  // component initialization
  Window window = Window("Calibration", 980, 620);
  if (!window.init()) {
    fprintf(stderr, "Failed to initialize window!\n");
    return -1;
  }

  TextureManager texture_manager;

  Camera camera = Camera();

  Renderer renderer(camera);
  if (!renderer.setup()) {
    fprintf(stderr, "Renderer setup failed!\n");
    return -1;
  }

  DirectionalLight light;
  light.direction = glm::normalize(glm::vec3(0, -1.0, 1.0));
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
  Model cube = Model();

  cube.add_mesh(cube_mesh);
  // test transformations
  glm::vec3 translation = glm::vec3(0, 0.5, 0);
  glm::vec3 scaling = glm::vec3(1, 1, 1);
  glm::vec3 rotation = glm::vec3(0, 0.3, 0);
  cube.move(translation);
  cube.rotate(rotation);
  cube.scale(scaling);
  // add cube to scene
  renderer.add_model_to_scene(cube);

  while (!window.should_close()) {
    renderer.render_scene();
    window.swap_buffers();
    window.poll_events();
  }

  window.close();
}
