#pragma once

#include <cstdio>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/core/cvstd_wrapper.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/operations.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/features.hpp>
#include <opencv2/geometry/3d.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

// how many matches we take and evaluate, needs to be 2 for D.Lowie testing
static const unsigned int K = 2;

class FeatureTracker {
private:
  cv::Mat target_image;
  cv::Mat match_image;
  cv::Ptr<cv::SIFT> detector;
  std::vector<cv::KeyPoint> target_key_points, input_key_points;
  std::vector<cv::Point2f> target_good_points, input_good_points;
  cv::Mat target_descriptors, input_descriptors;
  cv::Ptr<cv::BFMatcher> matcher;
  std::vector<std::vector<cv::DMatch>> matches;
  std::vector<cv::DMatch> good_matches;
  cv::Mat homography;
  std::vector<cv::Point2f> edges;
  std::vector<cv::Point2f> projected_edges;
  float good_match_ratio;

public:
  FeatureTracker(int features, const cv::Mat &target_image,
                 float match_ratio = 0.8) {
    detector = cv::SIFT::create(features);
    matcher = cv::BFMatcher::create();
    set_target_image(target_image);
    good_match_ratio = match_ratio;
  }

  void set_target_image(const cv::Mat &image) {
    target_image = image;
    detector->detectAndCompute(target_image, cv::noArray(), target_key_points,
                               target_descriptors);
    edges = {cv::Point2f(0, 0), cv::Point2f(0, target_image.size().height),
             cv::Point2f(target_image.size().width, target_image.size().height),
             cv::Point2f(target_image.size().width, 0)};
  }

  void find_good_matches(const cv::Mat &image, bool debug = false) {
    good_matches.clear();
    matches.clear();
    target_good_points.clear();
    input_good_points.clear();
    detector->detectAndCompute(image, cv::noArray(), input_key_points,
                               input_descriptors);
    matcher->knnMatch(target_descriptors, input_descriptors, matches, K);
    // find good matches via D.Lowie
    for (auto match : matches) {
      if (match.size() != 2) {
        fprintf(stderr, "Found an incorrect number of matches!");
        continue;
      }
      if (match[0].distance < good_match_ratio * match[1].distance) {
        good_matches.push_back(match[0]);
        target_good_points.push_back(target_key_points[match[0].queryIdx].pt);
        input_good_points.push_back(input_key_points[match[0].trainIdx].pt);
      }
    }
    homography.release();
    if (target_good_points.size() > 3 and input_good_points.size() > 3)
      homography =
          cv::findHomography(target_good_points, input_good_points, cv::RANSAC);
    if (debug) {
      cv::drawMatches(target_image, target_key_points, image, input_key_points,
                      good_matches, match_image);
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
        cv::polylines(match_image, pixel_points, true, cv::Scalar(0, 255, 0),
                      5);
        cv::polylines(match_image, pixel_edges, true, cv::Scalar(0, 255, 0), 5);
      }
    }
  }

  cv::Mat get_match_image() { return match_image; }

  void project(const std::vector<cv::Point2f> &in_points,
               std::vector<cv::Point2f> &out_points) {
    cv::perspectiveTransform(in_points, out_points, homography);
  }
};
