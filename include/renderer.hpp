#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "camera.hpp"
#include "geometry.hpp"
#include "shader.hpp"
#include "themes.hpp"
#include <cstddef>
#include <cstdio>
#include <glad/glad.h>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>

struct Material {
  GLuint diffuse_texture;
  GLuint specular_texture;
  float shininess;
};

struct DirectionalLight {
  glm::vec3 direction;

  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
};

class Background {
public:
  GLuint texture;

  void setup() {
    // gen bind and fill vertex array, vertex buffer object and elemenr buffer
    // object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), vertices,
                 GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices,
                 GL_STATIC_DRAW);

    // specify how to read vertex buffer object in vertex array
    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2),
                          (void *)0);

    glBindVertexArray(0);
  }

  void draw(Shader &shader) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture);

    shader.set_int("background", 1);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(0);
  }

private:
  glm::vec2 vertices[4] = {glm::vec2(0.0, 0.0), glm::vec2(1.0, 0.0),
                           glm::vec2(1.0, 1.0), glm::vec2(0.0, 1.0)};
  GLuint indices[6] = {0, 1, 2, 2, 3, 0};

  GLuint vao, vbo, ebo;
};

class Mesh {
public:
  Mesh();
  Mesh(const std::vector<Vertex> &vertices_, const std::vector<uint> &indices_,
       const Material &material_)
      : vertices(vertices_), indices(indices_), material(material_) {}
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
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, material.diffuse_texture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, material.specular_texture);

    shader.set_int("material.diffuse", 1);
    shader.set_int("material.specular", 2);
    shader.set_float("material.shininess", material.shininess);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(0);
  }

private:
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;

  Material material;

  GLuint vao, vbo, ebo;
};

class Model {
private:
  std::vector<Mesh> meshes;
  glm::mat4 model_transform = glm::mat4(1);

public:
  void add_mesh(Mesh mesh) {
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
  DirectionalLight light;
  Background background = Background();
  bool is_rendering_background = false;
  Shader background_shader =
      Shader(std::string(PROJECT_DIR) + "/src/vert/back.vert",
             std::string(PROJECT_DIR) + "/src/frag/back.frag");
  Shader shader = Shader(std::string(PROJECT_DIR) + "/src/vert/vertex.vert",
                         std::string(PROJECT_DIR) + "/src/frag/fragment.frag");
  Camera camera;

  void render_models() {
    shader.use();

    shader.set_matrix_4f("view", camera.get_view_matrix());
    shader.set_matrix_4f("projection", camera.get_projection_matrix());
    shader.set_vec3("view_position", camera.camera_pos);

    shader.set_vec3("light.direction", light.direction);
    shader.set_vec3("light.ambient", light.ambient);
    shader.set_vec3("light.diffuse", light.diffuse);
    shader.set_vec3("light.specular", light.specular);

    for (Model model : models) {
      model.draw(shader);
    }
  }

  void render_background() {
    glDisable(GL_DEPTH_TEST);
    background_shader.use();
    background.draw(background_shader);
    glEnable(GL_DEPTH_TEST);
  }

public:
  Renderer(Camera camera_) : camera(camera_) {}

  bool setup() {

    glClearColor(red_channel(gruvbox::bg), green_channel(gruvbox::bg),
                 blue_channel(gruvbox::bg), 1);
    glEnable(GL_DEPTH_TEST);

    background.setup();

    return shader.build() && background_shader.build();
  }

  void add_model_to_scene(Model &model) { models.push_back(model); }

  void set_scene_light(DirectionalLight &light_) { light = light_; }

  void set_back_ground_texture(GLuint &id) {
    background.texture = id;
    is_rendering_background = true;
  }

  void remove_background_texture() { is_rendering_background = false; }

  void render_scene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!shader.is_built || !background_shader.is_built) {
      fprintf(
          stderr,
          "Shader programs not built properly, please setup the rederer!\n");
      return;
    }
    if (is_rendering_background) {
      render_background();
    }
    render_models();
  }
};

#endif
