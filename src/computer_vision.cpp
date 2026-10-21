#include "computer_vision.hpp"
#include <cstdio>
#include <filesystem>
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/imgcodecs.hpp>
#include <string>
#include <vector>

namespace fs = std::filesystem;

bool load_directory_images(const std::string &path,
                           std::vector<cv::Mat> &images) {
  if (!fs::exists(path) || !fs::is_directory(path)) {
    fprintf(stderr, "Path does not exist or is not a directory: %s\n",
            path.c_str());
    return false;
  }

  bool success = true;
  size_t initial_count = images.size();

  for (const fs::directory_entry &entry : fs::directory_iterator(path)) {
    if (!entry.is_regular_file()) {
      continue;
    }

    cv::Mat img = cv::imread(entry.path().string(), cv::IMREAD_COLOR_RGB);
    if (img.empty()) {
      fprintf(stderr, "Failed to load image file: %s\n",
              entry.path().string().c_str());
      success = false;
      continue;
    }

    images.push_back(img);
  }

  // Check if we actually loaded anything from the directory
  if (images.size() == initial_count) {
    fprintf(stderr, "No valid images found in directory: %s\n", path.c_str());
    return false;
  }

  return success;
}

bool find_refined_chessboard_corners(
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

void obj_points_from_pattern(const cv::Size &size,
                             std::vector<cv::Point3f> &obj_points) {
  for (int y = 0; y < size.height; y++) {
    for (int x = 0; x < size.width; x++) {
      obj_points.push_back(cv::Point3f(float(x), float(y), 0.0));
    }
  }
}

bool calibrate_camera_from_chessboard_images(
    const std::vector<cv::Mat> &images, const cv::Size &pattern_size,
    cv::Mat &cam, std::vector<double> &dist_coefs,
    std::vector<cv::Mat> &rotation_vecs, std::vector<cv::Mat> &trans_vecs,
    std::vector<cv::Mat> &used_images, bool debug,
    const std::string &debug_path) {
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
      fprintf(stderr, "Coulden't find checkerboard corners for image!\n");
      continue;
    }
    if (debug) {
      cv::Mat debug_image = image.clone();
      cv::drawChessboardCorners(debug_image, pattern_size, corners, success);
      cv::imwrite(debug_path + "chess_" + std::to_string(img_points.size()) +
                      ".jpg",
                  debug_image);
      fprintf(stdout, "Pattern found and debug image writen!\n");
    }
    used_images.push_back(image.clone());
    img_points.push_back(corners);
    // get obj obj_points
    std::vector<cv::Point3f> points;
    obj_points_from_pattern(pattern_size, points);
    obj_points.push_back(points);
  }
  if (img_points.empty() || img_points.size() != obj_points.size()) {
    return false;
  }
  // get camera calibration outputs
  cv::calibrateCamera(obj_points, img_points, img_size, cam, dist_coefs,
                      rotation_vecs, trans_vecs);

  return true;
}
