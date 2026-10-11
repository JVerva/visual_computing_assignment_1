#include "computer_vision.hpp"
#include <filesystem>

namespace fs = std::filesystem;

static void load_directory_images(const std::string &path,
                                  std::vector<cv::Mat> &images) {
  for (const fs::directory_entry &entry : fs::directory_iterator(path)) {
    images.push_back(cv::imread(entry.path()));
  }
}

static bool find_refined_chessboard_corners(
    const cv::Mat &image, const cv::Size &pattern_size,
    std::vector<cv::Point2f> &corners, const cv::Size &refinement_win_size,
    const cv::Size &refinement_zero_zone,
    const cv::TermCriteria &refinement_termination_criteria) {
  // turn to grayscale
  cv::Mat gray;
  cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
  // find the corners
  bool success = cv::findChessboardCorners(gray, pattern_size, corners);
  // refine them
  if (success) {
    cv::cornerSubPix(gray, corners, refinement_win_size, refinement_zero_zone,
                     refinement_termination_criteria);
  }

  return success;
}

static void obj_points_from_pattern(const cv::Size &size,
                                    std::vector<cv::Point3f> &obj_points) {
  for (int y = 0; y < size.height; y++) {
    for (int x = 0; x < size.width; x++) {
      obj_points.push_back(cv::Point3f(float(x), float(y), 0.0));
    }
  }
}

static bool calibrate_camera_from_chessboard_images(
    const std::vector<cv::Mat> &images, const cv::Size &pattern_size,
    cv::Mat &cam, std::vector<double> &dist_coefs,
    std::vector<cv::Mat> &rotation_vecs, std::vector<cv::Mat> &trans_vecs) {
  cv::Size img_size;
  if (images.empty() || pattern_size.area() <= 0) {
    return false;
  }
  img_size = images[0].size();
  bool success;
  std::vector<std::vector<cv::Point3f>> obj_points;
  std::vector<std::vector<cv::Point2f>> img_points;
  // get image points and obj points per image
  for (cv::Mat image : images) {
    std::vector<cv::Point2f> corners;
    success = find_refined_chessboard_corners(image, pattern_size, corners);
    // skip this image if cant find checkerboard points
    if (!success) {
      fprintf(stderr, "Coulden't find checkerboard corners for image!");
      continue;
    }
    img_points.push_back(corners);
    // get obj obj_points
    std::vector<cv::Point3f> points;
    obj_points_from_pattern(pattern_size, points);
    obj_points.push_back(points);
  }
  // get camera calibration outputs
  cv::calibrateCamera(obj_points, img_points, img_size, cam, dist_coefs,
                      rotation_vecs, trans_vecs);

  return success;
}
