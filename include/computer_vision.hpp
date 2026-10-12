#ifndef COMPUTER_VISION_HPP
#define COMPUTER_VISION_HPP

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

// declarations
static void load_directory_images(const std::string &path,
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

static bool calibrate_camera_from_chessboard_images(
    const std::vector<cv::Mat> &images, const cv::Size &pattern_size,
    cv::Mat &cam, std::vector<double> &dist_coefs,
    std::vector<cv::Mat> &rotation_vecs, std::vector<cv::Mat> &trans_vecs);

#endif
