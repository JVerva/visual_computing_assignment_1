#include "camera.hpp"
#include "computer_vision.hpp"
#include "window.hpp"
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>

void render(Window window);

int main() {
  Window window = Window("Calibration", 980, 620);
  window.init();
  while (!window.should_close()) {
    render(window);
    window.swap_buffers();
    window.poll_events();
  }
  window.close();
}

void render(Window window) {

  glClear(GL_COLOR_BUFFER_BIT);

  return;
}
