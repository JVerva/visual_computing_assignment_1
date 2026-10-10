#include <filesystem>
#include <glm/detail/qualifier.hpp>
#include <glm/glm.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

namespace fs = std::filesystem;

void load_calibration_images(const std::string &path,
                             std::vector<cv::Mat> &images);

int main() {
  std::vector<cv::Mat> cal_images;
  load_calibration_images("assets/images", cal_images);

  return 0;
}

void load_calibration_images(const std::string &path,
                             std::vector<cv::Mat> &images) {
  for (const fs::directory_entry &entry : fs::directory_iterator(path)) {
    images.push_back(cv::imread(entry.path()));
  }
}
