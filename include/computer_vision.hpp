#ifndef COMPUTER_VISION_HPP
#define COMPUTER_VISION_HPP

#include <cstdio>
#include <filesystem>
#include <opencv2/core/matx.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <string>
#include <vector>

namespace fs = std::filesystem;

enum Filter { NONE, GRAYSCALE, GAUSSIAN_BLUR, SOBEL, SHARPEN };

inline bool load_directory_images(const std::string &path,
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

  if (images.size() == initial_count) {
    fprintf(stderr, "No valid images found in directory: %s\n", path.c_str());
    return false;
  }

  return success;
}

inline bool find_refined_chessboard_corners(
    const cv::Mat &image, const cv::Size &pattern_size,
    std::vector<cv::Point2f> &corners,
    const cv::Size &refinement_win_size = cv::Size(5, 5),
    const cv::Size &refinement_zero_zone = cv::Size(-1, -1),
    const cv::TermCriteria &refinement_termination_criteria =
        cv::TermCriteria(30, 0.001)) {
  cv::Mat gray;
  cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
  bool success = cv::findChessboardCorners(gray, pattern_size, corners);

  if (success) {
    cv::cornerSubPix(gray, corners, refinement_win_size, refinement_zero_zone,
                     refinement_termination_criteria);
  }

  return success;
}

inline void obj_points_from_pattern(const cv::Size &size,
                                    std::vector<cv::Point3f> &obj_points) {
  for (int y = 0; y < size.height; y++) {
    for (int x = 0; x < size.width; x++) {
      obj_points.push_back(cv::Point3f(float(x), float(y), 0.0));
    }
  }
}

inline bool calibrate_camera_from_chessboard_images(
    const std::vector<cv::Mat> &images, const cv::Size &pattern_size,
    cv::Mat &cam, std::vector<double> &dist_coefs,
    std::vector<cv::Mat> &rotation_vecs, std::vector<cv::Mat> &trans_vecs,
    std::vector<cv::Mat> &used_images, bool debug = false,
    const std::string &image_debug_path = std::string(PROJECT_DIR) +
                                          "/assets/images/debug/") {
  if (images.empty() || pattern_size.area() <= 0) {
    return false;
  }

  cv::Size img_size = images[0].size();
  std::vector<std::vector<cv::Point3f>> obj_points;
  std::vector<std::vector<cv::Point2f>> img_points;

  for (const cv::Mat &image : images) {
    std::vector<cv::Point2f> corners;
    bool success =
        find_refined_chessboard_corners(image, pattern_size, corners);
    if (!success) {
      fprintf(stderr, "Coulden't find checkerboard corners for image!\n");
      continue;
    }
    if (debug) {
      cv::Mat debug_image = image.clone();
      cv::drawChessboardCorners(debug_image, pattern_size, corners, success);
      cv::imwrite(image_debug_path + "chess_" +
                      std::to_string(img_points.size()) + ".jpg",
                  debug_image);
    }
    used_images.push_back(image.clone());
    img_points.push_back(corners);

    std::vector<cv::Point3f> points;
    obj_points_from_pattern(pattern_size, points);
    obj_points.push_back(points);
  }

  if (img_points.empty() || img_points.size() != obj_points.size()) {
    return false;
  }

  cv::calibrateCamera(obj_points, img_points, img_size, cam, dist_coefs,
                      rotation_vecs, trans_vecs);
  return true;
}

static void transform_image(cv::Mat in_frame, cv::Mat &out_frame,
                            Filter filter) {
  // transfrom og frame
  cv::Mat s_x;
  cv::Mat s_y;
  switch (filter) {
  case Filter::NONE:
    out_frame = in_frame;
    break;
  case Filter::GRAYSCALE:
    // convert to grayscale
    cv::cvtColor(in_frame, out_frame, cv::COLOR_BGR2GRAY);
    break;
  case Filter::GAUSSIAN_BLUR:
    // convolute image with gaussian blur kernal
    cv::blur(in_frame, out_frame, cv::Size(5, 5));
    break;
  case Filter::SOBEL:
    // apply sobel x
    cv::Sobel(in_frame, out_frame, in_frame.depth(), 1, 1, 3, 3);
    break;
  case Filter::SHARPEN:
    cv::blur(in_frame, out_frame, cv::Size(3, 3));
    cv::addWeighted(in_frame, 1.5, out_frame, -0.5, 0.0, out_frame);
    break;

  default:
    out_frame = in_frame;
    break;
  }
}

class WebcamCapturePipeline {
private:
  cv::VideoCapture capture;
  Filter filter;

public:
  cv::Mat frame;
  bool setup() {
    // open the default webcam (0)
    capture = cv::VideoCapture(0);
    // Check if the video was opened successfully
    if (!capture.isOpened()) {
      fprintf(stderr, "Failed to open video capture\n");
      return false;
    }
    return true;
  }

  bool update_frame() {
    bool ret = capture.read(frame);
    if (ret && !frame.empty()) {
      transform_image(frame, frame, filter);
      cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    }
    return ret;
  }

  void set_filter(Filter filter_) { filter = filter_; }
};

#endif
