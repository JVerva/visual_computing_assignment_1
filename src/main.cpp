#include "camera.hpp"
#include "geometry.hpp"
#include "renderer.hpp"
#include "window.hpp"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <glm/ext/vector_float3.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
void render(Window window);

int main() {
  Window window = Window("Calibration", 980, 620);
  window.init();

  Camera camera = Camera();

  Renderer renderer(camera);
  if (!renderer.setup()) {
    fprintf(stderr, "Renderer setup failed!\n");
  }

  // cube creation
  std::vector<Vertex> cube_vertices;
  std::vector<uint> cube_indices;
  get_cube_geometry(1, cube_vertices, cube_indices);
  Mesh cube_mesh = Mesh(cube_vertices, cube_indices);
  Model cube = Model();
  cube.add_mesh(cube_mesh);
  glm::vec3 translation = glm::vec3(1, 0, 0);
  cube.move(translation);
  renderer.add_model_to_scene(cube);

  while (!window.should_close()) {
    renderer.render_scene();
    window.swap_buffers();
    window.poll_events();
  }
  window.close();
}
