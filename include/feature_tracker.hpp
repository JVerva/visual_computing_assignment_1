#pragma once

#include <cstdio>
#include <opencv2/core/cvstd_wrapper.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/features.hpp>
#include <vector>

// how many matches we take and evaluate, needs to be 2 for D.Lowie testing
static const unsigned int K = 2;

class FeatureTracker {
private:
  cv::Mat target_image;
  cv::Mat match_image;
  cv::Ptr<cv::SIFT> detector;
  std::vector<cv::KeyPoint> target_key_points, input_key_points;
  cv::Mat target_descriptors, input_descriptors;
  cv::Ptr<cv::BFMatcher> matcher;
  std::vector<std::vector<cv::DMatch>> matches;
  std::vector<cv::DMatch> good_matches;
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
  }

  void find_good_matches(const cv::Mat &image, bool debug = false) {
    good_matches.clear();
    matches.clear();
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
      }
    }
    if (debug)
      cv::drawMatches(target_image, target_key_points, image, input_key_points,
                      good_matches, match_image);
  }

  cv::Mat get_match_image() { return match_image; }
};
