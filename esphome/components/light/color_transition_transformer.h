#pragma once

#include "light_transformer.h"
#include "light_color_values.h"

namespace esphome {
namespace light {

/// Transformer for continuous color transitions (brightness, hue, saturation, etc.)
class ColorTransitionTransformer : public ContinuousTransformer {
 public:
  ColorTransitionTransformer(ColorTransitionType type, TransitionDirection direction, float speed);

  void start() override;
  optional<LightColorValues> apply() override;
  void stop() override;

  /// Set the hue transition path for hue transitions
  void set_hue_path(HueTransitionPath path) { hue_path_ = path; }

 private:
  ColorTransitionType type_;
  TransitionDirection direction_;
  float speed_;  ///< Units per second (brightness: 0-1/s, hue: degrees/s, etc.)
  float initial_value_;
  HueTransitionPath hue_path_{HUE_PATH_SHORTEST};  ///< Path for hue transitions

  uint32_t get_elapsed_time_() const;
};

}  // namespace light
}  // namespace esphome
