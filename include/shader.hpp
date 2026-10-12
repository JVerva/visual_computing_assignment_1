#ifndef SHADER_HPP
#define SHADER_HPP

#include "utils.hpp"
#include <cstdio>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class Shader {
public:
  GLuint id;
  std::string vert_path;
  std::string frag_path;

  // reads compiles shaders and builds program
  Shader(const std::string &vertex_shader_path,
         const std::string &fragment_shader_path)
      : vert_path(vertex_shader_path), frag_path(fragment_shader_path) {}

  bool build() {
    std::string vert_src;
    std::string frag_src;
    bool success;

    // load and compile vertex shader
    success = read_file(vert_path, vert_src);
    if (!success) {
      fprintf(stderr, "Failed to open vetex shader file!");
      return false;
    }
    const char *c_vert_src = vert_src.c_str();

    GLuint vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &c_vert_src, NULL);
    glCompileShader(vertex_shader);
    // print shader compilation errors
    int c_success;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &c_success);
    if (!c_success) {
      glGetShaderInfoLog(vertex_shader, sizeof(info_log), NULL, info_log);
      fprintf(stderr, "Failed to compile vertex shader!\n %s", info_log);
      return false;
    }

    // load and compile fragment shader
    success = read_file(frag_path, frag_src);
    if (!success) {
      fprintf(stderr, "Failed to open vetex shader file!");
      return false;
    }
    const char *c_frag_src = frag_src.c_str();

    GLuint frag_shader;
    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, &c_frag_src, NULL);
    glCompileShader(frag_shader);
    // print shader compilation errors
    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &c_success);
    if (!c_success) {
      glGetShaderInfoLog(frag_shader, sizeof(info_log), NULL, info_log);
      fprintf(stderr, "Failed to compile fragment shader\n %s", info_log);
      return false;
    }

    // create shader program with vertex and fragment shader
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, frag_shader);
    glLinkProgram(shader_program);
    // print program linker errors
    glGetProgramiv(shader_program, GL_LINK_STATUS, &c_success);
    if (!c_success) {
      glGetProgramInfoLog(shader_program, sizeof(info_log), NULL, info_log);
      fprintf(stderr, "Failed to link shader program\n %s", info_log);
    }
    // delete shader objects
    glDeleteShader(vertex_shader);
    glDeleteShader(frag_shader);

    id = shader_program;
    return true;
  }

  void set_matrix_4f(const std::string &uniform_name, const glm::mat4 &matrix) {
    int mat_location = glGetUniformLocation(id, uniform_name.c_str());
    glUniformMatrix4fv(mat_location, 1, GL_FALSE, glm::value_ptr(matrix));
  }

  void use() { glUseProgram(id); }
};

#endif
