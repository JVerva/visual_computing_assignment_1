#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "camera.hpp"
#include "geometry.hpp"
#include "shader.hpp"
#include <cstddef>
#include <cstdio>
#include <glad/glad.h>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <vector>

class Mesh {
public:
  Mesh(const std::vector<Vertex> &vertices_, const std::vector<uint> &indices_)
      : vertices(vertices_), indices(indices_) {}
  void setup() {
    // gen bind and fill vertex array, vertex buffer object and elemenr buffer
    // object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                 &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
                 &indices[0], GL_STATIC_DRAW);

    // specify how to read vertex buffer object in vertex array
    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    // normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, normal));
    // uvs
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, uv));
    glBindVertexArray(0);
  }

  void draw(Shader &shader) {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }

private:
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  GLuint vao, vbo, ebo;
};

class Renderer {
private:
  std::vector<Mesh> meshes;
  std::vector<Shader> shader_programs;
  Camera camera;

public:
  void setup(Camera camera_) {
    shader_programs.push_back(
        Shader("src/vert/vertex.vert", "src/frag/triangle.frag"));
    shader_programs[0].build();
    camera = camera_;
  }

  void add_mesh_to_scene(Mesh &mesh) {
    mesh.setup();
    meshes.push_back(mesh);
  }

  void render_scene() {
    glClear(GL_COLOR_BUFFER_BIT);
    if (shader_programs.empty()) {
      fprintf(stderr, "No shader programs built, please setup the rederer!");
      return;
    }
    Shader shader = shader_programs[0];
    shader.use();

    glm::mat4 model = glm::mat4(1);
    shader.set_matrix_4f("model", model);
    shader.set_matrix_4f("view", camera.getViewMatrix());
    shader.set_matrix_4f("projection", camera.getProjectionMartix());

    for (Mesh mesh : meshes) {
      mesh.draw(shader_programs[0]);
    }
  }
};

#endif
