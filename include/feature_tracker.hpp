#pragma once

#include <cstdio>
#include <opencv2/core.hpp>
#include <opencv2/core/cvstd_wrapper.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/operations.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/features.hpp>
#include <opencv2/geometry/3d.hpp>
#include <opencv2/imgproc.hpp>
#include <tuple>
#include <vector>

// how many matches we take and evaluate, needs to be 2 for D.Lowie testing
static const unsigned int K = 2;

class FeatureTracker {
private:
  cv::Mat target_image;
  cv::Mat input_image;
  cv::Mat match_image;

  cv::Ptr<cv::SIFT> detector;
  cv::Ptr<cv::BFMatcher> matcher;
  std::vector<std::vector<cv::DMatch>> matches;
  std::vector<cv::DMatch> good_matches;
  float good_match_ratio;

  std::vector<cv::KeyPoint> target_key_points, input_key_points;
  std::vector<cv::Point2f> target_good_points, input_good_points;
  std::vector<cv::Point2f> target_inlier_points, input_inlier_points;
  cv::Mat target_descriptors, input_descriptors;

  cv::Mat homography;
  std::vector<cv::Point2f> edges;
  std::vector<cv::Point2f> projected_edges;

  cv::Size2f target_physical_size;

  std::vector<cv::Point3f> object_points;
  cv::Mat rvec;
  cv::Mat tvec;

  cv::Mat intrinsic;
  std::vector<double> distortion_coeffs;

  bool find_good_matches() {
    if (input_image.empty() or target_image.empty()) {
      return false;
    }

    clear_feature_data();
    detector->detectAndCompute(input_image, cv::noArray(), input_key_points,
                               input_descriptors);
    matcher->knnMatch(target_descriptors, input_descriptors, matches, K);
    // find good matches via D.Lowie
    for (auto match : matches) {
      if (match.size() != K) {
        fprintf(stderr, "Match has incorrect size: %lu (should be 2)\n",
                match.size());
        continue;
      }
      if (match[0].distance < good_match_ratio * match[1].distance) {
        good_matches.push_back(match[0]);
        target_good_points.push_back(target_key_points[match[0].queryIdx].pt);
        input_good_points.push_back(input_key_points[match[0].trainIdx].pt);
      }
    }
    return true;
  }

  void clear_feature_data() {
    good_matches.clear();
    matches.clear();
    target_good_points.clear();
    input_good_points.clear();
    input_inlier_points.clear();
    target_inlier_points.clear();
    object_points.clear();
    rvec.release();
    tvec.release();
  }

  bool find_homography() {
    bool ret = target_good_points.size() > 3 and input_good_points.size() > 3;
    cv::Mat inlier_mask;
    if (ret) {
      homography = cv::findHomography(target_good_points, input_good_points,
                                      cv::RANSAC, 3, inlier_mask);
      for (size_t k = 0; k < inlier_mask.rows; ++k) {
        if (!inlier_mask.at<uchar>(k, 0))
          continue;
        input_inlier_points.push_back(input_good_points[k]);
        target_inlier_points.push_back(target_good_points[k]);
      }
    } else {
      homography.release();
    }
    return ret;
  }

  bool draw_feature_bounding_box() {
    if (input_image.empty() or target_image.empty()) {
      return false;
    }

    cv::drawMatches(target_image, target_key_points, input_image,
                    input_key_points, good_matches, match_image);
    if (!homography.empty()) {
      project(edges, projected_edges);
      std::vector<cv::Point2i> pixel_edges;
      std::vector<cv::Point2i> pixel_points;
      for (int i = 0; i < edges.size(); i++) {
        cv::Point2i pixel_point;
        pixel_edges.push_back(cv::Point2i(edges[i]));
        pixel_point.x = int(projected_edges[i].x + target_image.size().width);
        pixel_point.y = int(projected_edges[i].y);
        pixel_points.push_back(pixel_point);
      }
      cv::polylines(match_image, pixel_points, true, cv::Scalar(0, 255, 0), 5);
      cv::polylines(match_image, pixel_edges, true, cv::Scalar(0, 255, 0), 5);
    }
    return true;
  }

  void project(const std::vector<cv::Point2f> &in_points,
               std::vector<cv::Point2f> &out_points) {
    cv::perspectiveTransform(in_points, out_points, homography);
  }

  bool estimate_pose() {
    if (input_inlier_points.size() < 6 or target_inlier_points.size() < 6 or
        distortion_coeffs.empty() or intrinsic.empty()) {
      return false;
    }
    // create object points, assuming z=0 at image plane
    for (const auto &point : target_inlier_points) {
      object_points.push_back(cv::Point3f(
          point.x * target_physical_size.width / target_image.cols,
          point.y * target_physical_size.height / target_image.rows, 0.0));
    }
    cv::solvePnPRansac(object_points, input_inlier_points, intrinsic,
                       distortion_coeffs, rvec, tvec);
    return true;
  }

public:
  FeatureTracker(int features, const cv::Mat &target_image,
                 float match_ratio = 0.8, float physical_width_m = 1,
                 float physical_height_m = 1) {
    detector = cv::SIFT::create(features);
    matcher = cv::BFMatcher::create();
    set_target(target_image, physical_width_m, physical_height_m);
    good_match_ratio = match_ratio;
  }

  void set_target(const cv::Mat &image, float physical_width_m,
                  float physical_height_m) {
    set_target_image(image);
    set_target_physical_size(physical_width_m, physical_height_m);
  }

  void set_target_image(const cv::Mat &image) {
    cv::cvtColor(image, target_image, cv::COLOR_BGR2GRAY);
    detector->detectAndCompute(target_image, cv::noArray(), target_key_points,
                               target_descriptors);
    edges = {cv::Point2f(0, 0), cv::Point2f(0, target_image.size().height),
             cv::Point2f(target_image.size().width, target_image.size().height),
             cv::Point2f(target_image.size().width, 0)};
  }

  void set_camera_calibration(const cv::Mat &intrinsic_,
                              const std::vector<double> &distortion_coeffs_) {
    intrinsic = intrinsic_;
    distortion_coeffs = distortion_coeffs_;
  }

  void set_target_physical_size(float width_m, float height_m) {
    target_physical_size = cv::Size2f(width_m, height_m);
  }

  void set_input_image(const cv::Mat &input_image_) {
    cv::cvtColor(input_image_, input_image, cv::COLOR_RGB2GRAY);
  }

  cv::Mat get_match_image() { return match_image; }

  std::tuple<cv::Mat, cv::Mat> get_estimated_pose() {
    return std::tuple<cv::Mat, cv::Mat>(rvec, tvec);
  }

  void visualy_track(bool debug = false) {
    find_good_matches();
    find_homography();
    if (debug) {
      draw_feature_bounding_box();
    }
    estimate_pose();
  }
};
