#pragma once

#include <vector>
#include <cstdint>

namespace esphome {
namespace light {

/// Enum for different brightness curve types
enum BrightnessCurveType {
  /// Linear 1:1 mapping (no transformation)
  BRIGHTNESS_CURVE_LINEAR = 0,
  /// Gamma correction (power function)
  BRIGHTNESS_CURVE_GAMMA = 1,
  /// Exponential curve
  BRIGHTNESS_CURVE_EXPONENTIAL = 2,
  /// Logarithmic curve  
  BRIGHTNESS_CURVE_LOGARITHMIC = 3,
  /// Cubic curve for smooth transitions
  BRIGHTNESS_CURVE_CUBIC = 4,
  /// Custom curve defined by control points
  BRIGHTNESS_CURVE_CUSTOM = 5,
};

/// Struct representing a control point for custom curves
struct BrightnessCurvePoint {
  float input;   // Input brightness value (0.0 - 1.0)
  float output;  // Output brightness value (0.0 - 1.0)
  
  BrightnessCurvePoint() : input(0.0f), output(0.0f) {}
  BrightnessCurvePoint(float in, float out) : input(in), output(out) {}
};

/// Struct representing a brightness curve profile configuration
struct BrightnessCurveProfile {
  /// Type of curve
  BrightnessCurveType type;
  
  /// Parameters for different curve types
  union {
    struct {
      float gamma;     // For GAMMA type
    } gamma_params;
    
    struct {
      float exponent;  // For EXPONENTIAL type
    } exponential_params;
    
    struct {
      float base;      // For LOGARITHMIC type
    } logarithmic_params;
    
    struct {
      float factor;    // For CUBIC type
    } cubic_params;
  };
  
  /// Control points for custom curves
  std::vector<BrightnessCurvePoint> custom_points;
  
  /// Default constructor - creates a linear curve
  BrightnessCurveProfile() : type(BRIGHTNESS_CURVE_LINEAR) {}
  
  /// Constructor for gamma curve
  explicit BrightnessCurveProfile(float gamma) : type(BRIGHTNESS_CURVE_GAMMA) {
    gamma_params.gamma = gamma;
  }
  
  /// Apply the curve transformation to a brightness value
  float transform(float input) const;
};

/// Apply brightness curve transformation
float apply_brightness_curve(float value, const BrightnessCurveProfile &profile);

/// Apply custom curve interpolation
float apply_custom_curve(float value, const std::vector<BrightnessCurvePoint> &points);

}  // namespace light
}  // namespace esphome