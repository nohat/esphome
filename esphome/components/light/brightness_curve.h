#pragma once

#include <vector>

namespace esphome {
namespace light {

/// Enum for different brightness curve types
enum BrightnessCurveType : uint8_t {
  /// Linear 1:1 mapping (no transformation)
  LINEAR = 0,
  /// Gamma correction (power function)
  GAMMA = 1,
  /// Exponential curve
  EXPONENTIAL = 2,
  /// Logarithmic curve  
  LOGARITHMIC = 3,
  /// Cubic curve for smooth transitions
  CUBIC = 4,
  /// Custom curve defined by control points
  CUSTOM = 5,
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
  BrightnessCurveProfile() : type(LINEAR) {}
  
  /// Constructor for gamma curve
  explicit BrightnessCurveProfile(float gamma) : type(GAMMA) {
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