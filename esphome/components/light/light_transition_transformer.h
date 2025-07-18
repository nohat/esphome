#pragma once

#include "light_transformer.h"
#include "light_color_values.h"

namespace esphome {
namespace light {

/// Transformer for smooth transitions between two light states over a finite duration
class LightTransitionTransformer : public FiniteTransformer {
 public:
  void start() override;
  optional<LightColorValues> apply() override;

  /// Set the hue transition path for color transitions
  void set_hue_path(HueTransitionPath path) { hue_path_ = path; }

 protected:
  /// Smooth sigmoid-like transition function (6x^5 - 15x^4 + 10x^3)
  static float smoothed_progress(float x) { return x * x * x * (x * (x * 6.0f - 15.0f) + 10.0f); }

  bool changing_color_mode_{false};
  LightColorValues end_values_{};
  LightColorValues intermediate_values_{};
  HueTransitionPath hue_path_{HUE_PATH_SHORTEST};
};

}  // namespace light
}  // namespace esphome
