#include "brightness_curve.h"
#include <cmath>
#include <algorithm>

namespace esphome {
namespace light {

// Implementation of gamma_correct function (from helpers.cpp)
float gamma_correct(float value, float gamma) {
  if (value <= 0.0f)
    return 0.0f;
  if (gamma <= 0.0f)
    return value;
  return std::pow(value, gamma);
}

static const char *const TAG = "light.brightness_curve";

float BrightnessCurveProfile::transform(float input) const {
  return apply_brightness_curve(input, *this);
}

float apply_brightness_curve(float value, const BrightnessCurveProfile &profile) {
  // Clamp input to valid range
  value = std::max(0.0f, std::min(1.0f, value));
  
  switch (profile.type) {
    case BRIGHTNESS_CURVE_LINEAR:
      return value;
      
    case BRIGHTNESS_CURVE_GAMMA:
      return gamma_correct(value, profile.gamma_params.gamma);
      
    case BRIGHTNESS_CURVE_EXPONENTIAL:
      if (value <= 0.0f) return 0.0f;
      return std::pow(profile.exponential_params.exponent, value - 1.0f);
      
    case BRIGHTNESS_CURVE_LOGARITHMIC:
      if (value <= 0.0f) return 0.0f;
      return std::log(1.0f + value * (profile.logarithmic_params.base - 1.0f)) / std::log(profile.logarithmic_params.base);
      
    case BRIGHTNESS_CURVE_CUBIC:
      return value * value * value * profile.cubic_params.factor + value * (1.0f - profile.cubic_params.factor);
      
    case BRIGHTNESS_CURVE_CUSTOM:
      return apply_custom_curve(value, profile.custom_points);
      
    default:
      return value;
  }
}

float apply_custom_curve(float value, const std::vector<BrightnessCurvePoint> &points) {
  if (points.empty()) {
    return value;  // Fallback to linear if no points defined
  }
  
  // Handle edge cases
  if (value <= points.front().input) {
    return points.front().output;
  }
  if (value >= points.back().input) {
    return points.back().output;
  }
  
  // Find the two points to interpolate between
  for (size_t i = 0; i < points.size() - 1; i++) {
    const auto &p1 = points[i];
    const auto &p2 = points[i + 1];
    
    if (value >= p1.input && value <= p2.input) {
      // Linear interpolation between the two points
      float t = (value - p1.input) / (p2.input - p1.input);
      return p1.output + t * (p2.output - p1.output);
    }
  }
  
  // Fallback - should not reach here
  return value;
}

}  // namespace light
}  // namespace esphome