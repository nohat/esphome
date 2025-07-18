#pragma once

#include "esphome/core/helpers.h"
#include <vector>

namespace esphome {
namespace light {

/// Easing curve types for light transitions
enum class EasingType : uint8_t {
  // Standard easing curves
  LINEAR,
  EASE_IN_QUAD,
  EASE_OUT_QUAD,
  EASE_IN_OUT_QUAD,
  EASE_IN_CUBIC,
  EASE_OUT_CUBIC,
  EASE_IN_OUT_CUBIC,
  EASE_IN_QUART,
  EASE_OUT_QUART,
  EASE_IN_OUT_QUART,
  EASE_IN_QUINT,
  EASE_OUT_QUINT,
  EASE_IN_OUT_QUINT,
  EASE_IN_SINE,
  EASE_OUT_SINE,
  EASE_IN_OUT_SINE,
  EASE_IN_EXPO,
  EASE_OUT_EXPO,
  EASE_IN_OUT_EXPO,
  EASE_IN_CIRC,
  EASE_OUT_CIRC,
  EASE_IN_OUT_CIRC,
  EASE_IN_BACK,
  EASE_OUT_BACK,
  EASE_IN_OUT_BACK,
  EASE_IN_ELASTIC,
  EASE_OUT_ELASTIC,
  EASE_IN_OUT_ELASTIC,
  EASE_IN_BOUNCE,
  EASE_OUT_BOUNCE,
  EASE_IN_OUT_BOUNCE,
  
  // Professional lighting curves
  SMOOTH,  // Current ESPHome default (6x^5 - 15x^4 + 10x^3)
  THEATER_DIM,  // Theater-style dimming
  CANDLE_FLICKER,  // Candle-like dimming
  FADE_TO_BLACK,  // Slow start, fast finish for dramatic fade-to-black
  FAST_DIM_SLOW,  // Fast dim to 20%, then slow to 0%
  
  // Custom curve defined by points
  CUSTOM_POINTS,
};

/// Control point for custom curves
struct EasingPoint {
  float x;  // Input position (0.0 to 1.0)
  float y;  // Output value (0.0 to 1.0)
  
  EasingPoint(float x_pos, float y_val) : x(x_pos), y(y_val) {}
};

/// Easing curve evaluator
class EasingCurve {
 public:
  EasingCurve(EasingType type = EasingType::SMOOTH) : type_(type) {}
  
  /// Set curve type
  void set_type(EasingType type) { this->type_ = type; }
  
  /// Set custom control points for CUSTOM_POINTS type
  void set_points(const std::vector<EasingPoint> &points) { 
    this->custom_points_ = points;
    this->type_ = EasingType::CUSTOM_POINTS;
  }
  
  /// Evaluate the curve at position t (0.0 to 1.0)
  float evaluate(float t) const;
  
 protected:
  EasingType type_;
  std::vector<EasingPoint> custom_points_;
  
  // Standard easing function implementations
  static float ease_in_quad(float t) { return t * t; }
  static float ease_out_quad(float t) { return 1.0f - (1.0f - t) * (1.0f - t); }
  static float ease_in_out_quad(float t) {
    return t < 0.5f ? 2.0f * t * t : 1.0f - 2.0f * (1.0f - t) * (1.0f - t);
  }
  
  static float ease_in_cubic(float t) { return t * t * t; }
  static float ease_out_cubic(float t) { 
    float f = 1.0f - t;
    return 1.0f - f * f * f;
  }
  static float ease_in_out_cubic(float t) {
    return t < 0.5f ? 4.0f * t * t * t : 1.0f - 4.0f * (1.0f - t) * (1.0f - t) * (1.0f - t);
  }
  
  static float ease_in_quart(float t) { return t * t * t * t; }
  static float ease_out_quart(float t) {
    float f = 1.0f - t;
    return 1.0f - f * f * f * f;
  }
  static float ease_in_out_quart(float t) {
    return t < 0.5f ? 8.0f * t * t * t * t : 1.0f - 8.0f * (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t);
  }
  
  static float ease_in_quint(float t) { return t * t * t * t * t; }
  static float ease_out_quint(float t) {
    float f = 1.0f - t;
    return 1.0f - f * f * f * f * f;
  }
  static float ease_in_out_quint(float t) {
    return t < 0.5f ? 16.0f * t * t * t * t * t : 1.0f - 16.0f * (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t);
  }
  
  static float ease_in_sine(float t) { return 1.0f - cosf(t * M_PI_2); }
  static float ease_out_sine(float t) { return sinf(t * M_PI_2); }
  static float ease_in_out_sine(float t) { return -(cosf(M_PI * t) - 1.0f) / 2.0f; }
  
  static float ease_in_expo(float t) { return t == 0.0f ? 0.0f : powf(2.0f, 10.0f * (t - 1.0f)); }
  static float ease_out_expo(float t) { return t == 1.0f ? 1.0f : 1.0f - powf(2.0f, -10.0f * t); }
  static float ease_in_out_expo(float t) {
    if (t == 0.0f) return 0.0f;
    if (t == 1.0f) return 1.0f;
    return t < 0.5f ? powf(2.0f, 20.0f * t - 10.0f) / 2.0f 
                    : (2.0f - powf(2.0f, -20.0f * t + 10.0f)) / 2.0f;
  }
  
  static float ease_in_circ(float t) { return 1.0f - sqrtf(1.0f - t * t); }
  static float ease_out_circ(float t) { return sqrtf(1.0f - (t - 1.0f) * (t - 1.0f)); }
  static float ease_in_out_circ(float t) {
    return t < 0.5f ? (1.0f - sqrtf(1.0f - 4.0f * t * t)) / 2.0f
                    : (sqrtf(1.0f - 4.0f * (t - 1.0f) * (t - 1.0f)) + 1.0f) / 2.0f;
  }
  
  static float ease_in_back(float t) {
    const float c1 = 1.70158f;
    const float c3 = c1 + 1.0f;
    return c3 * t * t * t - c1 * t * t;
  }
  static float ease_out_back(float t) {
    const float c1 = 1.70158f;
    const float c3 = c1 + 1.0f;
    return 1.0f + c3 * powf(t - 1.0f, 3.0f) + c1 * powf(t - 1.0f, 2.0f);
  }
  static float ease_in_out_back(float t) {
    const float c1 = 1.70158f;
    const float c2 = c1 * 1.525f;
    return t < 0.5f ? (powf(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f
                    : (powf(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
  }
  
  static float ease_in_elastic(float t) {
    const float c4 = (2.0f * M_PI) / 3.0f;
    return t == 0.0f ? 0.0f : t == 1.0f ? 1.0f : -powf(2.0f, 10.0f * t - 10.0f) * sinf((t * 10.0f - 10.75f) * c4);
  }
  static float ease_out_elastic(float t) {
    const float c4 = (2.0f * M_PI) / 3.0f;
    return t == 0.0f ? 0.0f : t == 1.0f ? 1.0f : powf(2.0f, -10.0f * t) * sinf((t * 10.0f - 0.75f) * c4) + 1.0f;
  }
  static float ease_in_out_elastic(float t) {
    const float c5 = (2.0f * M_PI) / 4.5f;
    return t == 0.0f ? 0.0f : t == 1.0f ? 1.0f : t < 0.5f 
           ? -(powf(2.0f, 20.0f * t - 10.0f) * sinf((20.0f * t - 11.125f) * c5)) / 2.0f
           : (powf(2.0f, -20.0f * t + 10.0f) * sinf((20.0f * t - 11.125f) * c5)) / 2.0f + 1.0f;
  }
  
  static float ease_out_bounce(float t) {
    const float n1 = 7.5625f;
    const float d1 = 2.75f;
    if (t < 1.0f / d1) {
      return n1 * t * t;
    } else if (t < 2.0f / d1) {
      return n1 * (t -= 1.5f / d1) * t + 0.75f;
    } else if (t < 2.5f / d1) {
      return n1 * (t -= 2.25f / d1) * t + 0.9375f;
    } else {
      return n1 * (t -= 2.625f / d1) * t + 0.984375f;
    }
  }
  static float ease_in_bounce(float t) { return 1.0f - ease_out_bounce(1.0f - t); }
  static float ease_in_out_bounce(float t) {
    return t < 0.5f ? (1.0f - ease_out_bounce(1.0f - 2.0f * t)) / 2.0f
                    : (1.0f + ease_out_bounce(2.0f * t - 1.0f)) / 2.0f;
  }
  
  // ESPHome's current smooth curve (for backward compatibility)
  static float smooth(float t) { return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f); }
  
  // Professional lighting curves
  static float theater_dim(float t) {
    // Typical theater dimming: slower at beginning and end, faster in middle
    return 0.5f * (1.0f - cosf(M_PI * t));
  }
  
  static float candle_flicker(float t) {
    // Simulate candle-like dimming with slight randomness (simplified)
    return t + 0.1f * sinf(t * M_PI * 8.0f) * (1.0f - t);
  }
  
  static float fade_to_black(float t) {
    // Slow start, accelerating to dramatic black-out
    return t < 0.7f ? 0.2f * t / 0.7f : 0.2f + 0.8f * ease_in_cubic((t - 0.7f) / 0.3f);
  }
  
  static float fast_dim_slow(float t) {
    // Fast dim to 20%, then slow dim to 0%
    if (t <= 0.3f) {
      // Fast phase: 100% to 20% in first 30% of time
      return 1.0f - 0.8f * (t / 0.3f);
    } else {
      // Slow phase: 20% to 0% in remaining 70% of time
      float slow_progress = (t - 0.3f) / 0.7f;
      return 0.2f * (1.0f - ease_out_cubic(slow_progress));
    }
  }
  
  // Custom curve interpolation using control points
  float interpolate_custom_points(float t) const;
};

}  // namespace light
}  // namespace esphome