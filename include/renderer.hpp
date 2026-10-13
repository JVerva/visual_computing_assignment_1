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
#include <glm/gtc/matrix_transform.hpp>
#include <string>
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

class Model {
private:
  std::vector<Mesh> meshes;
  glm::mat4 model_transform = glm::mat4(1);

public:
  void add_mesh(Mesh &mesh) {
    mesh.setup();
    meshes.push_back(mesh);
  }

  void rotate(const glm::vec3 &angles) {
    model_transform =
        glm::rotate(model_transform, angles.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model_transform =
        glm::rotate(model_transform, angles.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model_transform =
        glm::rotate(model_transform, angles.z, glm::vec3(0.0f, 0.0f, 1.0f));
  }

  void scale(const glm::vec3 &scale_factor) {
    model_transform = glm::scale(model_transform, scale_factor);
  }

  void move(const glm::vec3 &translation) {
    model_transform = glm::translate(model_transform, translation);
  }

  void draw(Shader &shader) {
    shader.set_matrix_4f("model", model_transform);
    for (Mesh mesh : meshes) {
      mesh.draw(shader);
    }
  }
};

class Renderer {
private:
  std::vector<Model> models;
  Shader shader = Shader(std::string(PROJECT_DIR) + "/src/vert/vertex.vert",
                         std::string(PROJECT_DIR) + "/src/frag/triangle.frag");
  Camera camera;

public:
  Renderer(Camera camera_) : camera(camera_) {}

  bool setup() { return shader.build(); }

  void add_model_to_scene(Model &model) { models.push_back(model); }

  void render_scene() {
    glClear(GL_COLOR_BUFFER_BIT);
    if (!shader.is_built) {
      fprintf(stderr, "No shader programs built, please setup the rederer!\n");
      return;
    }
    shader.use();

    shader.set_matrix_4f("view", camera.get_view_matrix());
    shader.set_matrix_4f("projection", camera.get_projection_matrix());

    for (Model model : models) {
      model.draw(shader);
    }
  }
};

#endif
