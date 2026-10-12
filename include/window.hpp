#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "themes.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string>

#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>

class Window {
private:
  GLFWwindow *window;
  std::string name;
  int height;
  int width;

  static void key_callback(GLFWwindow *window, int key, int scancode,
                           int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  static void frame_buffer_resize_callbak(GLFWwindow *window, int width,
                                          int height) {
    glViewport(0, 0, width, height);
  }

public:
  Window(const std::string &name_, int width_, int height_)
      : name(name_), width(width_), height(height_) {}

  bool init() {
    // Initialise GLFW
    if (!glfwInit()) {
      fprintf(stderr, "Failed to initialize GLFW\n");
      getchar();
      return false;
    }
    // set open GL context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Open a window and create its OpenGL context
    window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (window == NULL) {
      fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, "
                      "they are not 3.3 compatible. .\n");
      getchar();
      glfwTerminate();
      return false;
    }

    glfwMakeContextCurrent(window);
    // let glad load the context
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      fprintf(stderr, "Failed to initialize GLAD\n");
      return false;
    }
    glfwSetFramebufferSizeCallback(window, frame_buffer_resize_callbak);
    glfwSetKeyCallback(window, key_callback);
    glClearColor(gruvbox::bg[0], gruvbox::bg[1], gruvbox::bg[2], 1);

    return true;
  }

  void swap_buffers() { glfwSwapBuffers(window); }

  void poll_events() { glfwPollEvents(); }

  bool should_close() { return glfwWindowShouldClose(window); }

  void close() {
    glfwDestroyWindow(window);
    glfwTerminate();
  }
};

#endif
