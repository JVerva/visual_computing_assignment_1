#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 uv;
};

static constexpr void get_cube_geometry(float side,
                                        std::vector<Vertex> &vertices,
                                        std::vector<glm::uint> &indices) {
  const float half_side = side * 0.5f;

  vertices = {
      // Front (+Z)
      {{-half_side, -half_side, half_side}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
      {{half_side, -half_side, half_side}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
      {{half_side, half_side, half_side}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
      {{-half_side, half_side, half_side}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
      // Back (-Z)
      {{half_side, -half_side, -half_side}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
      {{-half_side, -half_side, -half_side}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
      {{-half_side, half_side, -half_side}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
      {{half_side, half_side, -half_side}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
      // Left (-X)
      {{-half_side, -half_side, -half_side}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
      {{-half_side, -half_side, half_side}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
      {{-half_side, half_side, half_side}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
      {{-half_side, half_side, -half_side}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
      // Right (+X)
      {{half_side, -half_side, half_side}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
      {{half_side, -half_side, -half_side}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
      {{half_side, half_side, -half_side}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
      {{half_side, half_side, half_side}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
      // Top (+Y)
      {{-half_side, half_side, half_side}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
      {{half_side, half_side, half_side}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
      {{half_side, half_side, -half_side}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
      {{-half_side, half_side, -half_side}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
      // Bottom (-Y)
      {{-half_side, -half_side, -half_side}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
      {{half_side, -half_side, -half_side}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
      {{half_side, -half_side, half_side}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
      {{-half_side, -half_side, half_side}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
  };

  // Two counter-clockwise triangles per face, viewed from outside.
  indices = {
      0,  1,  2,  2,  3,  0,  4,  5,  6,  6,  7,  4,  8,  9,  10, 10, 11, 8,
      12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20,
  };
}

#endif // C
