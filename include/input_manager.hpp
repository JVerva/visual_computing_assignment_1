#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#include "window.hpp"
#include <GLFW/glfw3.h>
#include <unordered_set>

class InputManager {
private:
  GLFWwindow *window;
  std::unordered_set<int> just_pressed_keys;
  std::unordered_set<int> pressed_keys;
  std::unordered_set<int> released_keys;
  static void key_callback(GLFWwindow *window, int key, int scancode,
                           int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    InputManager *self = (InputManager *)(glfwGetWindowUserPointer(window));
    if (self) {
      self->handle_key_press(key, scancode, action, mods);
    }
  }
  void handle_key_press(int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS)
      just_pressed_keys.insert(key);
    pressed_keys.insert(key);
    if (action == GLFW_RELEASE)
      pressed_keys.erase(key);
  }

public:
  InputManager(const Window &window_) {
    window = window_.window;
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, &InputManager::key_callback);
  }
  bool is_pressed(const int &key) { return pressed_keys.contains(key); };
  bool is_released(const int &key) { return released_keys.contains(key); };
  bool is_just_pressed(const int &key) {
    return just_pressed_keys.contains(key);
  };
  void update() {
    just_pressed_keys.clear();
    released_keys.clear();
  }
};

#endif // !INPUT_MANAGER_HPP
