#ifndef COMPUTER_VISION_HPP
#define COMPUTER_VISION_HPP

#include <opencv2/core/matx.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <string>
#include <vector>

// declarations
enum Filter { NONE, GRAYSCALE, GAUSSIAN_BLUR, SOBEL, SHARPEN };

bool load_directory_images(const std::string &path,
                           std::vector<cv::Mat> &images);

static bool find_refined_chessboard_corners(
    const cv::Mat &image, const cv::Size &pattern_size,
    std::vector<cv::Point2f> &corners,
    const cv::Size &refinement_win_size = cv::Size(11, 11),
    const cv::Size &refinement_zero_zone = cv::Size(-1, -1),
    const cv::TermCriteria &refinement_termination_criteria =
        cv::TermCriteria(30, 0.001));

static void obj_points_from_pattern(const cv::Size &size,
                                    std::vector<cv::Point3f> &obj_points);

bool calibrate_camera_from_chessboard_images(
    const std::vector<cv::Mat> &images, const cv::Size &pattern_size,
    cv::Mat &cam, std::vector<double> &dist_coefs,
    std::vector<cv::Mat> &rotation_vecs, std::vector<cv::Mat> &trans_vecs,
    std::vector<cv::Mat> &used_images, bool debug = false,
    const std::string &image_debug_path = std::string(PROJECT_DIR) +
                                          "/assets/images/debug/");

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
      cv::flip(frame, frame, -1);

      cv::Mat rgb_frame;
      cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    }
    return ret;
  }

  void set_filter(Filter filter_) { filter = filter_; }
};

#endif
