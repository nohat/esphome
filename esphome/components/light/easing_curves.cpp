#include "easing_curves.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace light {

float EasingCurve::evaluate(float t) const {
  // Clamp input to valid range
  t = clamp(t, 0.0f, 1.0f);
  
  switch (this->type_) {
    case EasingType::LINEAR:
      return t;
      
    case EasingType::EASE_IN_QUAD:
      return ease_in_quad(t);
    case EasingType::EASE_OUT_QUAD:
      return ease_out_quad(t);
    case EasingType::EASE_IN_OUT_QUAD:
      return ease_in_out_quad(t);
      
    case EasingType::EASE_IN_CUBIC:
      return ease_in_cubic(t);
    case EasingType::EASE_OUT_CUBIC:
      return ease_out_cubic(t);
    case EasingType::EASE_IN_OUT_CUBIC:
      return ease_in_out_cubic(t);
      
    case EasingType::EASE_IN_QUART:
      return ease_in_quart(t);
    case EasingType::EASE_OUT_QUART:
      return ease_out_quart(t);
    case EasingType::EASE_IN_OUT_QUART:
      return ease_in_out_quart(t);
      
    case EasingType::EASE_IN_QUINT:
      return ease_in_quint(t);
    case EasingType::EASE_OUT_QUINT:
      return ease_out_quint(t);
    case EasingType::EASE_IN_OUT_QUINT:
      return ease_in_out_quint(t);
      
    case EasingType::EASE_IN_SINE:
      return ease_in_sine(t);
    case EasingType::EASE_OUT_SINE:
      return ease_out_sine(t);
    case EasingType::EASE_IN_OUT_SINE:
      return ease_in_out_sine(t);
      
    case EasingType::EASE_IN_EXPO:
      return ease_in_expo(t);
    case EasingType::EASE_OUT_EXPO:
      return ease_out_expo(t);
    case EasingType::EASE_IN_OUT_EXPO:
      return ease_in_out_expo(t);
      
    case EasingType::EASE_IN_CIRC:
      return ease_in_circ(t);
    case EasingType::EASE_OUT_CIRC:
      return ease_out_circ(t);
    case EasingType::EASE_IN_OUT_CIRC:
      return ease_in_out_circ(t);
      
    case EasingType::EASE_IN_BACK:
      return ease_in_back(t);
    case EasingType::EASE_OUT_BACK:
      return ease_out_back(t);
    case EasingType::EASE_IN_OUT_BACK:
      return ease_in_out_back(t);
      
    case EasingType::EASE_IN_ELASTIC:
      return ease_in_elastic(t);
    case EasingType::EASE_OUT_ELASTIC:
      return ease_out_elastic(t);
    case EasingType::EASE_IN_OUT_ELASTIC:
      return ease_in_out_elastic(t);
      
    case EasingType::EASE_IN_BOUNCE:
      return ease_in_bounce(t);
    case EasingType::EASE_OUT_BOUNCE:
      return ease_out_bounce(t);
    case EasingType::EASE_IN_OUT_BOUNCE:
      return ease_in_out_bounce(t);
      
    case EasingType::SMOOTH:
      return smooth(t);
      
    case EasingType::THEATER_DIM:
      return theater_dim(t);
    case EasingType::CANDLE_FLICKER:
      return candle_flicker(t);
    case EasingType::FADE_TO_BLACK:
      return fade_to_black(t);
    case EasingType::FAST_DIM_SLOW:
      return fast_dim_slow(t);
      
    case EasingType::CUSTOM_POINTS:
      return interpolate_custom_points(t);
      
    default:
      return smooth(t);  // Default to current ESPHome behavior
  }
}

float EasingCurve::interpolate_custom_points(float t) const {
  if (this->custom_points_.empty()) {
    return t;  // Fallback to linear if no points defined
  }
  
  // If only one point, treat as constant
  if (this->custom_points_.size() == 1) {
    return this->custom_points_[0].y;
  }
  
  // Find the two points to interpolate between
  size_t i = 0;
  while (i + 1 < this->custom_points_.size() && this->custom_points_[i + 1].x < t) {
    i++;
  }
  
  // If t is before first point or at first point
  if (i == 0 && t <= this->custom_points_[0].x) {
    return this->custom_points_[0].y;
  }
  
  // If t is after last point
  if (i + 1 >= this->custom_points_.size()) {
    return this->custom_points_[this->custom_points_.size() - 1].y;
  }
  
  // Linear interpolation between points
  const EasingPoint &p1 = this->custom_points_[i];
  const EasingPoint &p2 = this->custom_points_[i + 1];
  
  if (p2.x == p1.x) {
    return p1.y;  // Avoid division by zero
  }
  
  float ratio = (t - p1.x) / (p2.x - p1.x);
  return p1.y + ratio * (p2.y - p1.y);
}

}  // namespace light
}  // namespace esphome